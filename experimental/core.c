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

/* socket - net includes */
#include <linux/types.h>
#include <linux/net.h>	  
#include <linux/in.h>	   
#include <net/sock.h>	   

#define DRIVER_AUTHOR "Joe Bayer <joeba@uio.no>" 
#define DRIVER_DESC "Kernel module for a Interactive Traffic PEP"

struct pep_accept_work {
    struct work_struct task;
    struct socket* sock;
};
static struct pep_accept_work* conn;
static atomic_t run = ATOMIC_INIT(1);

/**
 * @brief Main work accepting new connections.
 * 
 * @param work work_struct
 */
void pep_accept_work(struct work_struct* work)
{
    struct pep_accept_work* conn = container_of(work, struct pep_accept_work, task);
    struct socket* server_sock = conn->sock;
    struct socket* client_sock;
    int ret;

    /* Main loop running til module is closed. */
    while(atomic_read(&run))
    {
        ret = kernel_accept(server_sock, &client_sock, 0);
        if(ret < 0){
            printk(KERN_INFO "[PEP] Error accepting new connection.\n");
            continue;
        }

        printk(KENR_INFO "[PEP] New client connection accepted.\n");

        release_sock(client_sock);
    }

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

    skb = skb_peek(&sk->sk_receive_queue);
    
    /* Check syn packet and connect to endpoint */

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

    INIT_WORK(&conn->task, workqueue_test);
    schedule_work(&conn->task);
    
    printk(KERN_INFO "[PEP] Initilized!\n");
    return 0;
}

/**
 * @brief Exit fuction for kernel module.
 * 
 */
static void __exit exit_core(void)
{
    atomic_set(&run, 0);

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
