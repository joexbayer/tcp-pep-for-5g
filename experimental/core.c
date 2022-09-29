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
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>


#include "pep_errors.h"

#define DRIVER_AUTHOR "Joe Bayer <joeba@uio.no>" 
#define DRIVER_DESC "Kernel module for a Interactive Traffic PEP"

struct pep_accept_work {
    struct work_struct task;
    struct socket* sock;
    atomic_t run;
};

struct pep_tunnel_work {
    struct work_struct task;
    struct list_head list;

    struct socket* lsock;
    struct socket* rsock;

    atomic_t run;

    u16 sport;
};

static struct pep_accept_work* conn;
LIST_HEAD(pep_tunnel_list);

unsigned int inet_addr(char *str)
{
    int a,b,c,d;
    char arr[4];
    sscanf(str,"%d.%d.%d.%d",&a,&b,&c,&d);
    arr[0] = a; arr[1] = b; arr[2] = c; arr[3] = d;
    return *(unsigned int*)arr;
}

struct pep_connect_work {
    struct work_struct task;
    u16 sport;
    struct sk_buff* skb;
};

void pep_tunnel_work_fn(struct work_struct* work)
{
    struct pep_tunnel_work* tunnel = container_of(work, struct pep_tunnel_work, task);

    printk(KERN_INFO "[PEP] Tunnel running!\n");

    while (atomic_read(&tunnel->run))
    {
        /* code */
    }
    
    /* Each tunnel is responsible for cleaning up itself. */
    sock_release(tunnel->lsock);
    sock_release(tunnel->rsock);
    kfree(tunnel);
}

static struct pep_tunnel_work* pep_find_work(u16 sport)
{
    struct pep_tunnel_work* iter;
    list_for_each_entry(iter, &pep_tunnel_list, list) {
        
        printk(KERN_INFO "[PEP] pep_find_work: searching for %d -> %d\n", iter->sport, sport);
        if(iter->sport == sport){
            return iter;
        }
    }

    return NULL;
}

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
    struct pep_tunnel_work* iter;

    struct inet_sock *inet;
    int ret;
    u16 sport, dport;
    
    while(atomic_read(&conn->run))
    {
        ret = kernel_accept(server_sock, &client_sock, SOCK_NONBLOCK);
        if(ret < 0){
            //printk(KERN_INFO "[PEP] pep_accept_work_fn: Error accepting new connection!.\n");
            continue;
        }

        printk(KERN_INFO "[PEP] pep_accept_work_fn: New client connection accepted.\n");

        /* Extract source port to match correct connection. */
        inet = inet_sk(client_sock->sk);
        sport = inet->inet_dport;

        iter = pep_find_work(sport);
        if(iter == NULL) {
            printk(KERN_INFO "[PEP] pep_accept_work_fn: Could not find matching endpoint connection!\n");
            sock_release(client_sock);
            continue;
        }

        iter->rsock = client_sock;
        INIT_WORK(&iter->task, pep_tunnel_work_fn);
        schedule_work(&iter->task);
        printk(KERN_INFO "[PEP] Tunnel is scheduled!\n");
    }    
}

static void pep_new_tunnel_connection(struct work_struct* work)
{
    struct pep_connect_work* conn = container_of(work, struct pep_connect_work, task);

    struct socket* lsock;
    struct sockaddr_in laddr;
    struct pep_tunnel_work* tunnel;
    int ret;

    ret = sock_create_kern(&init_net, AF_INET, SOCK_STREAM, IPPROTO_TCP, &lsock);
    if(ret < 0) {
        printk(KERN_INFO "[PEP] pep_new_tunnel_connection: Error creating new kern sock!\n");
        return;
    }

    printk(KERN_INFO "[PEP] pep_new_tunnel_connection: Created a new endpoint socket.\n");

    laddr.sin_family = AF_INET;
    laddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    laddr.sin_port = (__force u16)htons(8182);

    ret = kernel_connect(lsock, (struct sockaddr*)&laddr, sizeof(laddr), 0);
    if(ret < 0) {
        // we have a problem
    }

    tunnel = kmalloc(sizeof(struct pep_tunnel_work), GFP_KERNEL);
    tunnel->lsock = lsock;
    tunnel->rsock = NULL;
    tunnel->run = ((atomic_t) { (1) });
    tunnel->sport = conn->sport;
    INIT_LIST_HEAD(&tunnel->list);

    list_add(&tunnel->list, &pep_tunnel_list);

    netif_receive_skb(conn->skb);

    printk(KERN_INFO "[PEP] pep_new_tunnel_connection: Connection established to endpoint.\n");
    return;
}

static unsigned int pep_nf_hook(void *priv, struct sk_buff *skb, const struct nf_hook_state *state)
{
    const struct iphdr *iph;
    const struct tcphdr *tcph;
    struct pep_connect_work* new_conn;
    struct pep_tunnel_work* tunnel;

    if (!skb)
        return NF_ACCEPT;

    iph = ip_hdr(skb);
    if (iph->protocol == IPPROTO_TCP) {
        tcph = tcp_hdr(skb);
        /* Check for packets with ONLY SYN flag set */
        if (tcph->syn == 1 && tcph->ack == 0 && tcph->rst == 0 && tcph->dest == htons(8181) ) {
            printk(KERN_INFO "[PEP] SYN FOR SERVER\n");

            tunnel = pep_find_work(tcph->source);
            if(tunnel == NULL)
            {
                new_conn = kmalloc(sizeof(struct pep_connect_work), GFP_KERNEL);
                new_conn->sport = tcph->source;
                new_conn->skb = (skb) ? skb_copy(skb, GFP_ATOMIC) : NULL;

                INIT_WORK(&new_conn->task, pep_new_tunnel_connection);
                schedule_work(&new_conn->task);

                consume_skb(skb);
                return NF_STOLEN;
            }
        }
    }

    return NF_ACCEPT;
}
static const struct nf_hook_ops pep_nf_hook_ops[] = {
        {
                .hook     = pep_nf_hook,
                .pf       = NFPROTO_IPV4,
                .hooknum  = NF_INET_PRE_ROUTING,
                .priority = -500,
        },
};

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
        printk(KERN_INFO "[PEP] init_core: Error creating socket\n");
    }

    sk = sock->sk;
    sk->sk_reuse = 1;

    /* pep server connection info */
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = INADDR_ANY;
    saddr.sin_port = (__force u16)htons(8181);

    ret = kernel_bind(sock, (struct sockaddr*)&saddr, addr_len);
    // CHECK ERROR
    ret = kernel_listen(sock, 5);

    conn = kzalloc(sizeof(struct pep_accept_work), GFP_ATOMIC);
    conn->sock = sock;
    conn->run = ((atomic_t) { (1) });

    nf_register_net_hooks(&init_net, pep_nf_hook_ops, ARRAY_SIZE(pep_nf_hook_ops));

    INIT_WORK(&conn->task, pep_accept_work_fn);
    schedule_work(&conn->task);


    
    printk(KERN_INFO "[PEP] init_core: Initilized!\n");
    return 0;
}

/**
 * @brief Exit function for kernel module.
 * 
 */
static void __exit exit_core(void)
{
    struct pep_tunnel_work* iter;
exit_loop:
    list_for_each_entry(iter, &pep_tunnel_list, list) {
        list_del(&iter->list);
        atomic_set(&iter->run, 0);
        goto exit_loop;
    }

    atomic_set(&conn->run, 0);
    sock_release(conn->sock);
    kfree(conn);
    printk(KERN_INFO "[PEP] exit_core: exited.\n");
}

module_init(init_core);
module_exit(exit_core);
/*
* Get rid of taint message by declaring code as GPL.
*/
MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR); /* Who wrote this module? */
MODULE_DESCRIPTION(DRIVER_DESC); /* What does this module do */



/*
    SOCKET OPTIONS

    TCP_CONGESTION

    strcpy(buf, "reno");
    len = strlen(buf);
    if (setsockopt(sock, IPPROTO_TCP, TCP_CONGESTION, buf, len) != 0)
    {
        perror("setsockopt");
        return -1;
    }


    SOCKET_BUFFER_SIZES
    SO_RCVBUF
    SO_SNDBUF
    SO_RCVLOWAT

    SO_LINGER?

    The minimum size is 512 bytes and the maximum size is 1048576 bytes.
    https://www.ibm.com/docs/en/ztpf/1.1.0.15?topic=apis-setsockopt-set-options-associated-socket

    int         recvBuff = 3072;
    setsockopt(sock, SOL_SOCKET, SO_RCVBUF, &recvBuff, sizeof(recvBuff)


    QUESTIONS
    Race condition with linked lists?


    LINKS
    https://linux-kernel-labs.github.io/refs/heads/master/labs/networking.html

    https://www.ietf.org/proceedings/45/I-D/draft-ietf-pilc-pep-00.txt
    https://www.rfc-editor.org/rfc/rfc3135

*/
