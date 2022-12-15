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


struct pep_connection {
        unsigned int ip;
        unsigned short sport;
        struct socket* sock;
};

struct pep_tunnel {
        struct pep_connection client;
        struct pep_connection endpoint;

        int state;
};

struct pep_state {
                struct socket* server_socket;
} server_state;

LIST_HEAD(pep_tunnels);
struct work_struct task;

int pep_tcp_receive(struct socket *sock)
{
	struct msghdr msg = {
		.msg_flags = MSG_DONTWAIT,
	};

	struct kvec vec;
	int rc = 0;

	/* allocate buffer memory */
	unsigned char *buffer = kzalloc(100, GFP_KERNEL);
	if (!buffer) {
		return -ENOMEM;
	}

	vec.iov_base = buffer;
	vec.iov_len  = 100;

read_again:
	printk(KERN_INFO "[PEP] kernel_recvmsg: calling recvmsg \n");
	rc = kernel_recvmsg(sock, &msg, &vec, 1, vec.iov_len, MSG_DONTWAIT);
	if (rc > 0)
	{
		tlv_print(buffer);
		return rc;
	}

	printk(KERN_INFO "[PEP] From client: no data!\n");

	if(rc == -EAGAIN || rc == -ERESTARTSYS)
	{
		goto read_again;
	}

	kfree(buffer);

	return rc;
}

void pep_server_accept_work(struct work_struct *work)
{
        int rc;
        struct socket* client = NULL;

        while(1) {
                rc = kernel_accept(server_state.server_socket, &client, O_NONBLOCK);
                if(rc < 0)
                        return;

		pep_tcp_receive(client);
                printk(KERN_INFO "[PEP]: Accepted new connection!\n");
                sock_release(client);
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
                INIT_WORK(&task, pep_server_accept_work);
                schedule_work(&task);
                printk(KERN_INFO "[PEP]: START work\n");  
        }

        read_unlock_bh(&sk->sk_callback_lock);

        ready(sk);
}

static inline unsigned int pep_syn_pkt(struct sk_buff* skb)
{
        int len = skb->data_len;
        int headlen = skb_headlen(skb);
        int datalen = skb_end_offset(skb) + len;
        int ret  = -1;
        u8 buffer[datalen];
                
        if(len == 0)
                return NF_ACCEPT;

        ret = skb_copy_bits(skb, headlen, buffer, len);
        if(ret <= 0)
                goto leave;
        
        if(!tlv_validate(buffer))
                goto leave;

        /* To be continued */

leave:
        return NF_ACCEPT;
}

static unsigned int pep_nf_hook(void* priv, struct sk_buff* skb, const struct nf_hook_state* state)
{
        const struct iphdr* iph;
        const struct tcphdr* tcph;

        if(!skb)
                return NF_ACCEPT;
        
        iph = ip_hdr(skb);
        if(iph->protocol == IPPROTO_TCP){
                tcph = tcp_hdr(skb);
                if(tcph->dest == htons(8181) && tcph->syn && !tcph->ack){
                        return pep_syn_pkt(skb);
                }
        }

        return NF_ACCEPT;

}

static inline int pep_socket_options(struct socket* sock)
{
        int qlen = 5;
        int val = 1;
        sockptr_t qlenval;
        sockptr_t delayval;

        qlenval = KERNEL_SOCKPTR(&qlen);
        delayval = KERNEL_SOCKPTR(&val);

        sock->ops->setsockopt(sock, SOL_TCP, TCP_FASTOPEN, qlenval, sizeof(qlen));
        sock->ops->setsockopt(sock, SOL_TCP, TCP_NODELAY, delayval, sizeof(val));

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

        pep_socket_options(sock);
        
        ret = kernel_bind(sock, (struct sockaddr*)&saddr, addr_len);
        // CHECK ERROR
        ret = kernel_listen(sock, 5);

        server_state.server_socket = sock;

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
