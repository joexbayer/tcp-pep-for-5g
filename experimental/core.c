/*
* core.c − Demonstrates module documentation.
* Simple loadable kernel module template.
* @see https://tldp.org/LDP/lkmpg/2.6/lkmpg.pdf
*/
#include <linux/module.h> 
#include <linux/kernel.h> 
#include <linux/init.h> 

#define DRIVER_AUTHOR "Joe Bayer <joeba@uio.no>" 
#define DRIVER_DESC "Loadable kernel module"


static int __init init_core(void)
{
    printk(KERN_INFO "Hello, world 4\n");
    return 0;
}


static void __exit exit_core(void)
{
    printk(KERN_INFO "Goodbye, world 4\n");
}

module_init(init_core);
module_exit(exit_core);
/*
* Get rid of taint message by declaring code as GPL.
*/
MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR); /* Who wrote this module? */
MODULE_DESCRIPTION(DRIVER_DESC); /* What does this module do */
