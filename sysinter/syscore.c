#include <linux/init.h>          // module_{init,exit}()
#include <linux/module.h>        // THIS_MODULE, MODULE_VERSION, ...
#include <linux/kernel.h>        // printk(), pr_*()
#include <asm/special_insns.h>   // {read,write}_cr0()
#include <asm/processor-flags.h> // X86_CR0_WP
#include <asm/unistd.h>          // __NR_*

#ifdef pr_fmt
#undef pr_fmt
#endif
#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

typedef long (*sys_call_ptr_t)(const struct pt_regs *);

static sys_call_ptr_t *real_sys_call_table;
static sys_call_ptr_t original_read;

static unsigned long sys_call_table_addr;
module_param(sys_call_table_addr, ulong, 0);
MODULE_PARM_DESC(sys_call_table_addr, "Address of sys_call_table");

static void write_cr0_unsafe(unsigned long val)
{
    asm volatile("mov %0,%%cr0": "+r" (val) : : "memory");
}

static long myread(const struct pt_regs *regs)
{
    pr_info("read(%ld, 0x%lx, %lx)\n", regs->di, regs->si, regs->dx);
    return original_read(regs);
}

static int __init modinit(void)
{
    unsigned long old_cr0;

    real_sys_call_table = (typeof(real_sys_call_table))sys_call_table_addr;

    pr_info("init\n");

    old_cr0 = read_cr0();
    write_cr0_unsafe(old_cr0 & ~(X86_CR0_WP));

    original_read = real_sys_call_table[__NR_read];
    real_sys_call_table[__NR_read] = myread;

    write_cr0_unsafe(old_cr0);
    pr_info("init done\n");
    return 0;
}

static void __exit modexit(void)
{
    unsigned long old_cr0;

    pr_info("exit\n");
    old_cr0 = read_cr0();
    write_cr0_unsafe(old_cr0 & ~(X86_CR0_WP));

    real_sys_call_table[__NR_read] = original_read;

    write_cr0_unsafe(old_cr0);
    pr_info("goodbye\n");
}

module_init(modinit);
module_exit(modexit);

MODULE_VERSION("0.1");
MODULE_DESCRIPTION("Syscall intercepting loadable kernel module.");
MODULE_AUTHOR("Joe Bayer");
MODULE_LICENSE("GPL");
