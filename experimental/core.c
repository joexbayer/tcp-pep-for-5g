/*
* core.c − Demonstrates module documentation.
* Simple loadable kernel module template.
* @see https://tldp.org/LDP/lkmpg/2.6/lkmpg.pdf
*/
#include <linux/module.h> 
#include <linux/kernel.h> 
#include <linux/init.h> 
#include <linux/sched.h>
#include <linux/workqueue.h>
#include <linux/slab.h>

#define DRIVER_AUTHOR "Joe Bayer <joeba@uio.no>" 
#define DRIVER_DESC "Loadable kernel module"


/**
  * PEPDNA uses netfilter hook to connect directly to actual destination.
  * 
  */


struct pep_connection {
    struct work_struct task;
    int lsocket;
    int rsocket;
};
static struct pep_connection* conn;


void workqueue_test(struct work_struct* work)
{
    struct pep_connection* conn = container_of(work, struct pep_connection, task);
    printk(KERN_INFO "[PEP] Testing workqueue function %d %d\n", conn->lsocket, conn->rsocket);
}

static int __init init_core(void)
{

    conn = kzalloc(sizeof(struct pep_connection), GFP_ATOMIC);
    conn->lsocket = 1;
    conn->rsocket = 2;

    INIT_WORK(&conn->task, workqueue_test);
    schedule_work(&conn->task);
    

    printk(KERN_INFO "[PEP] Initilized!\n");
    return 0;
}


static void __exit exit_core(void)
{
    kfree(conn);
    printk(KERN_INFO "[PEP] exited.\n");
}

module_init(init_core);
module_exit(exit_core);
/*
* Get rid of taint message by declaring code as GPL.
*/
MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR); /* Who wrote this module? */
MODULE_DESCRIPTION(DRIVER_DESC); /* What does this module do */
