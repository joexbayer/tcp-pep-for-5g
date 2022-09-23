/**
 * @file core.c
 * @author Joe Bayer <joeba@uio.no>
 * @brief core module file interactive traffic PEP.
 * @version 0.1
 * @date 2022-09-22
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <linux/module.h> 
#include <linux/kernel.h> 
#include <linux/init.h> 
#include <linux/sched.h>
#include <linux/workqueue.h>
#include <linux/slab.h>
#include <linux/list.h>

/* socket - net includes */
#include <linux/types.h>
#include <linux/net.h>
#include <linux/in.h>
#include <net/sock.h>
#include <net/ip.h>
#include <net/tcp.h>

#include "pep_errors.h"

#define DRIVER_AUTHOR "Joe Bayer <joeba@uio.no>" 
#define DRIVER_DESC "Kernel module for a Interactive Traffic PEP"

struct pep_accept_work {
    struct work_struct task;
    struct socket* sock;
    atomic_t run;
};

struct pep_tunnel {
    struct work_struct task;
    struct list_head list;

    struct socket* lsock;
    struct socket* rsock;

    atomic_t run;

    unsigned short sport;
};

static struct pep_accept_work* conn;
LIST_HEAD(pep_tunnel_list);

/**
 * @brief Main work accepting new connections.
 * 
 * @param work work_struct
 */
void pep_accept_work_fn(struct work_struct* work)
{
    struct pep_accept_work* conn = container_of(work, struct pep_accept_work, task);
    struct socket* server_sock = conn->sock;
    struct socket* client_sock;
    struct pep_tunnel* iter;

    struct inet_sock *inet;
    int ret;
    size_t i;

    /* Main loop running till module is closed. */
    while(atomic_read(&conn->run))
    {
        ret = kernel_accept(server_sock, &client_sock, 0);
        if(ret < 0){
            printk(KERN_INFO "[PEP] Error accepting new connection.\n");
            continue;
        }

        printk(KERN_INFO "[PEP] New client connection accepted.\n");

        /* Extract source port to match correct connection. */
        inet = inet_sk(client_sock->sk);
        u16 sport = ntohs(inet->inet_sport);

        list_for_each_entry(iter, &pep_tunnel_list, list) {
            
            if(iter->sport == sport){
                iter->rsock = client_sock;
                INIT_WORK(&tunnels[i]->task, pep_tunnel_work_fn);
                schedule_work(&tunnels[i]->task);
                break;
            }
        }

        //sock_release(client_sock);       
    }
}

void pep_tunnel_work_fn(struct work_struct* work)
{
    struct pep_tunnel* tunnel = container_of(work, struct pep_tunnel, task);

    sock_release(tunnel->lsock);
    sock_release(tunnel->rsock);
    atomic_set(&tunnel->run, 0);
}

static int pep_new_tunnel_connection(u32 ip, u16 dport, u16 sport)
{
    struct socket* lsock;
    struct sockaddr_in laddr;
    struct pep_tunnel* tunnel;
    int ret;
    size_t i;

    ret = sock_create_kern(&init_net, PF_INET, SOCK_STREAM, IPPROTO_TCP, &lsock);
    if(ret < 0) {
        // we have a problem
    }

    laddr.sin_family = AF_INET;
    laddr.sin_addr.s_addr = ip;
    laddr.sin_port = (__force u16)htons(dport);
    
    ret = kernel_connect(lsock, (struct sockaddr*)&laddr, sizeof(laddr), 0);
    if(ret < 0) {
        // we have a problem
    }

    printk(KERN_INFO "[PEP] Connection established to endpoint.\n");
    
    tunnel = kmalloc(sizeof(struct pep_tunnel), GFP_KERNEL);
    tunnel->lsock = lsock;
    tunnel->run = ATOMIC_INIT(1);
    tunnel->sport = sport;
    INIT_LIST_HEAD(&tunnel->list);

    list_add(&tunnel->list, &pep_tunnel_list);

    return 0;
}

/**
 * @brief Intercepts all packets that are about to be accepted
 * but the server socket. Should connect to endpoint before forwarding packet.
 * 
 * @param sk struct sock for accessing latest skb.
 */
static void pre_accept_callback(struct sock* sk)
{
    void (*ready)(struct sock *sk);
    struct sk_buff* skb = NULL;
    struct iphdr *ip_header;
    struct tcphdr *tcp_header;
    int ret;

    skb = skb_peek(&sk->sk_receive_queue);

    ip_header = (struct iphdr *)ip_hdr(skb);
    if(ip_header->protocol == IPPROTO_TCP)
    {
        tcp_header= (struct tcphdr *)tcp_hdr(skb);
        /* Check syn packet with connection data? and connect to endpoint */
        // CHECK BASED ON DATA SIZE?

        /**
         * Now, for the time being this will be a hardcoded connection to the server running on the same
         * machine. 2130706433 = 127.0.0.1
         */                         
        ret = pep_new_tunnel_connection(htonl(2130706433), 8182, tcp_header->dest);
        if(ret < 0){
            // we have a problem.
        }
    }
    

    read_lock_bh(&sk->sk_callback_lock);
    ready = sk->sk_user_data;
    read_unlock_bh(&sk->sk_callback_lock);
    
    if (ready)
        ready(sk);

    return;
}

/**
 * @brief init funcion for kernel module
 * 
 * @return int 
 */
static int __init init_core(void)
{
    struct socket* sock = NULL;
    struct sock* sk = NULL;
    struct sockaddr_in saddr;
    int addr_len = sizeof(saddr);
    int ret = 0;

    ret = sock_create_kern(&init_net, PF_INET, SOCK_STREAM, IPPROTO_TCP, &sock);
    if(ret){
        printk(KERN_INFO "[PEP] Error creating socket\n");
    }

    sk = sock->sk;
    sk->sk_reuse = 1;

    /* Overwrite data_ready pointer so we can intercept a packet before accept. */
    write_lock_bh(&sk->sk_callback_lock);
    sk->sk_user_data = sk->sk_data_ready; // Save ready function pointer
    sk->sk_data_ready = pre_accept_callback; // Overwrite ready pointer
    write_unlock_bh(&sk->sk_callback_lock);

    /* pep server connection info */
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = INADDR_ANY;
    saddr.sin_port = (__force u16)htons(8181);

    ret = kernel_bind(sock, (struct sockaddr*)&saddr, addr_len);
    // CHECK ERROR
    ret = kernel_listen(sock, 5);

    conn = kzalloc(sizeof(struct pep_accept_work), GFP_ATOMIC);
    conn->sock = sock;
    conn->run = ATOMIC_INIT(1);

    INIT_WORK(&conn->task, pep_accept_work_fn);
    schedule_work(&conn->task);
    
    printk(KERN_INFO "[PEP] Initilized!\n");
    return 0;
}

/**
 * @brief Exit function for kernel module.
 * 
 */
static void __exit exit_core(void)
{
    atomic_set(&conn->run, 0);
    sock_release(conn->sock);
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
