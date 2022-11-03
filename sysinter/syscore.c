#include <linux/module.h>
#include <linux/kallsyms.h>

MODULE_LICENSE("GPL");
char *sym_name = "sys_call_table";

#include <linux/kprobes.h>
static struct kprobe kp = {
    .symbol_name = "kallsyms_lookup_name"
};

typedef asmlinkage long (*sys_call_ptr_t)(const struct pt_regs *);
static sys_call_ptr_t *sys_call_table;
typedef asmlinkage long (*custom_open) (const char __user *filename, int flags, umode_t mode);

custom_open old_open;

static asmlinkage long my_open(const char __user *filename, int flags, umode_t mode)
{
    pr_info("%s\n",__func__);
        return old_open(filename, flags, mode);
}

static void disable_page_protection(void) {

    unsigned long value;
    asm volatile("mov %%cr0,%0" : "=r" (value));
    if (value & 0x00010000) {
            value &= ~0x00010000;
            asm volatile("mov %0,%%cr0": : "r" (value));
    }
}


static void enable_page_protection(void) {

    unsigned long value;
    asm volatile("mov %%cr0,%0" : "=r" (value));
    if (!(value & 0x00010000)) {
            value |= 0x00010000;
            asm volatile("mov %0,%%cr0": : "r" (value));
    }
}


static int __init hello_init(void)
{
    typedef unsigned long (*kallsyms_lookup_name_t)(const char *name);
    kallsyms_lookup_name_t kallsyms_lookup_name;
    register_kprobe(&kp);
    kallsyms_lookup_name = (kallsyms_lookup_name_t) kp.addr;
    unregister_kprobe(&kp);


    sys_call_table = (sys_call_ptr_t *)kallsyms_lookup_name(sym_name);
    old_open = (custom_open)sys_call_table[__NR_open];
    
    disable_page_protection(); 
    sys_call_table[__NR_open] = (sys_call_ptr_t)my_open;
    disable_page_protection();

    return 0;
}

static void __exit hello_exit(void)
{
    disable_page_protection();
    sys_call_table[__NR_open] = (sys_call_ptr_t)old_open;
    enable_page_protection(); 
}

module_init(hello_init);
module_exit(hello_exit);
