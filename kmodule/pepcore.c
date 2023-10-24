/**
 * @file pepcore.c
 * @author Joe Bayer <joeba@uio.no>
 * @brief Core kernel module file traffic PEP.
 * @version 0.1
 * @date 2022-09-22
 * @see PEPDNA by kr1stj0n
 * (https://github.com/kr1stj0n/pep-dna)
 * @copyright Copyright (c) 2022
 * 
 */

#include <linux/module.h>
#include <linux/sched.h>
#include <linux/workqueue.h>

#include <pep/common.h>

#include <pep/server.h>
#include <pep/errors.h>
#include <pep/nf.h>
#include <pep/tcp.h>
#include <pep/endpoint.h>
#include <pep/client.h>

#define DRIVER_AUTHOR "Joe Bayer <joeba@uio.no>" 
#define DRIVER_DESC "Kernel module for a Traffic PEP"

struct pep_state* server;

static int __init init_core(void)
{
        server = pep_new_server();

        server->ops->init(server, 8181);

        return 0; /* Module loaded successfully. */
}


static void __exit exit_core(void)
{
        pep_server_clean(server);
}   

module_init(init_core);
module_exit(exit_core);
/*
* Get rid of taint message by declaring code as GPL.
*/
MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR); /* Who wrote this module? */
MODULE_DESCRIPTION(DRIVER_DESC); /* What does this module do */
