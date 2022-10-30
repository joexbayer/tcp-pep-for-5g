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

#include <linux/types.h>
#include <linux/net.h>
#include <linux/in.h>
#include <net/sock.h>
#include <net/ip.h>
#include <net/tcp.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>   

#include "pep_errors.h"
#include "../lib/include/tlv.h"

#define DRIVER_AUTHOR "Joe Bayer <joeba@uio.no>" 
#define DRIVER_DESC "Kernel module for a non-interactive Traffic PEP"

struct pep_state {
    struct socket* server_socket;
};
struct pep_state server_state;

static inline int pep_socket_options(struct socket* sock)
{
    int qlen = 5;
    int val = 1;
    sockptr_t qlenval;
    sockptr_t delayval;

    qlenval = KERNEL_SOCKPTR(&qlen);
    delayval = KERNEL_SOCKPTR(&val);

    sock_setsockopt(sock, SOL_TCP, TCP_FASTOPEN, qlenval, sizeof(qlen));
    sock_setsockopt(sock, SOL_TCP, TCP_NODELAY, delayval, sizeof(val));

    return 0;
}

static int __init init_core(void)
{
    struct socket* sock = NULL;
    struct sock* sk = NULL;
    struct sockaddr_in saddr;
    int addr_len = sizeof(saddr);
    int ret = 0;

    ret = sock_create_kern(&init_net, PF_INET, SOCK_STREAM, IPPROTO_TCP, &sock);
    if(ret){
        printk(KERN_INFO "[PEP] init_core: Error creating socket\n");
        return -EPEP_GENERIC;
    }

    sk = sock->sk;
    sk->sk_reuse = 1;

    /* pep server connection info */
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = INADDR_ANY;
    saddr.sin_port = (__force u16)htons(8181);

    pep_socket_options(sock);
    
    ret = kernel_bind(sock, (struct sockaddr*)&saddr, addr_len);
    // CHECK ERROR
    ret = kernel_listen(sock, 5);

    server_state.server_socket = sock;

    return 0; /* Module loaded successfully. */
}


static void __exit exit_core(void)
{
    sock_release(server_state.server_socket);
}   

module_init(init_core);
module_exit(exit_core);
/*
* Get rid of taint message by declaring code as GPL.
*/
MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR); /* Who wrote this module? */
MODULE_DESCRIPTION(DRIVER_DESC); /* What does this module do */