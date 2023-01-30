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

#include "include/common.h"

#include <pep/server.h>
#include <pep/errors.h>
#include <pep/nf.h>

#define DRIVER_AUTHOR "Joe Bayer <joeba@uio.no>" 
#define DRIVER_DESC "Kernel module for a non-interactive Traffic PEP"


struct pep_connection {
        unsigned int ip;
        unsigned short port;
        struct socket* sock;
};

struct pep_tunnel {
        struct pep_connection client;
        struct pep_connection endpoint;

        int state;
};

struct pep_server server_state;

/* Linked list over all tunnels */
LIST_HEAD(pep_tunnels);


struct pep_connection* pep_new_connection(unsigned int ip, unsigned short port)
{
        struct pep_connection* conn = kzalloc(sizeof(struct pep_connection), GFP_KERNEL);
        conn->ip = ip;
        conn->port = port;

        return conn; 
}

inline struct pep_tunnel* pep_new_tunnel()
{
        return (struct pep_tunnel*) kzalloc(sizeof(struct pep_tunnel), GFP_KERNEL);
}

struct socket* pep_connect_endpoint(u32 ip, u16 port)
{
        struct socket* sock = NULL;
        struct sock* sk = NULL;
        struct sockaddr_in daddr;
        int addr_len = sizeof(daddr);
        int ret = 0;

        ret = sock_create_kern(&init_net, PF_INET, SOCK_STREAM, IPPROTO_TCP, &sock);
        if(ret){
                printk(KERN_INFO "[PEP] pep_connect_endpoint: Error creating socket\n");
                return NULL;
        }

        sk = sock->sk;
        sk->sk_reuse = 1;
        
        /* use our own data ready function */
        write_lock_bh(&sk->sk_callback_lock);
        sk->sk_user_data = sk->sk_data_ready;
        //sk->sk_data_ready = pep_listen_data_ready;
        write_unlock_bh(&sk->sk_callback_lock);

        /* Add tcp options? */

        /* pep endpoint connection info */
        daddr.sin_family = AF_INET;
        daddr.sin_addr.s_addr = ip;
        daddr.sin_port = (__force u16)port;

        ret = kernel_connect(sock, (struct sockaddr*)&daddr, addr_len, 0);
	if (rc < 0) {
		sock_release(sock);
		printk(KERN_INFO "[PEP] pep_connect_endpoint: failed to connect to endpoint.\n");
                return NULL;
	}

        return sock;
}

int pep_tcp_receive(struct socket *sock, u8 buffer, u32 size)
{
	struct msghdr msg = {
		.msg_flags = MSG_DONTWAIT,
	};

	struct kvec vec;
	int rc = 0;

	vec.iov_base = buffer;
	vec.iov_len  = size;

read_again:
	printk(KERN_INFO "[PEP] kernel_recvmsg: calling recvmsg \n");
	rc = kernel_recvmsg(sock, &msg, &vec, 1, vec.iov_len, MSG_DONTWAIT);
	if (rc > 0)
	{
		tlv_print(buffer);
		return rc;
	}

	printk(KERN_INFO "[PEP] pep_tcp_receive: no data!\n");

	if(rc == -EAGAIN || rc == -ERESTARTSYS)
	{
		goto read_again;
	}

	return rc;
}

void pep_server_accept_work(struct work_struct *work)
{
        int rc = 1;
        struct socket* client = NULL;

        while(rc > 0) {
                rc = kernel_accept(server_state.server_socket, &client, O_NONBLOCK);
                if(rc < 0)
                        return;

                /* allocate buffer memory */
                unsigned char *buffer = kzalloc(1500, GFP_KERNEL);
                if (!buffer) {
                        printk(KERN_INFO "[PEP] pep_server_accept_work: out of memory \n");
                        return;
                }

		pep_tcp_receive(client, buffer, 1500);
                printk(KERN_INFO "[PEP]: Accepted new connection!\n");
                
                /* TODO: Create new tunnel with client and endpoint */

                sock_release(client);
                kfree(buffer);
        }
}

static void pep_listen_data_ready(struct sock* sk)
{
        void (*ready)(struct sock *sk);


        read_lock_bh(&sk->sk_callback_lock);
        ready = sk->sk_user_data;

        struct sk_buff* skb = skb_peek(&sk->sk_receive_queue);

        printk(KERN_INFO "[PEP]: Packet received! %d\n", skb == NULL);

        /* Queue accept work */
        if(sk->sk_state == TCP_LISTEN){
                queue_work(server->accpet_wq, &con->accept_work)
                printk(KERN_INFO "[PEP]: START work\n");  
        }

        read_unlock_bh(&sk->sk_callback_lock);

        ready(sk);
}

static inline int pep_setsockopt(struct socket* sock, int option, int value)
{
        sockptr_t valuelen;
        valuelen = KERNEL_SOCKPTR(&value);

        sock->ops->setsockopt(sock, SOL_TCP, option, valuelen, sizeof(value));
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
        
        /* use our own data ready function */
        write_lock_bh(&sk->sk_callback_lock);
        sk->sk_user_data = sk->sk_data_ready;
        sk->sk_data_ready = pep_listen_data_ready;
        write_unlock_bh(&sk->sk_callback_lock);

        /* pep server connection info */
        saddr.sin_family = AF_INET;
        saddr.sin_addr.s_addr = INADDR_ANY;
        saddr.sin_port = (__force u16)htons(8181);

        pep_setsockopt(sock, TCP_FASTOPEN, 5);
        pep_setsockopt(sock, TCP_NODELAY, 1);
        
        ret = kernel_bind(sock, (struct sockaddr*)&saddr, addr_len);
        // CHECK ERROR
        ret = kernel_listen(sock, 5);

        server_state.server_socket = sock;
        server_state.accept_wq = alloc_workqueue("accept_wq", WQ_HIGHPRI|WQ_UNBOUND, 0);
        server_state.forward_c2e_wq = alloc_workqueue("c2e_wq", WQ_HIGHPRI|WQ_UNBOUND, 0);
        server_state.forward_e2c_wq = alloc_workqueue("e2c_wq", WQ_HIGHPRI|WQ_UNBOUND, 0);

        INIT_WORK(&server_state.accept_work, pep_server_accept_work);

        ppe_nf_register();

        printk("[PEP] Server has started.\n");

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
