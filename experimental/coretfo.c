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

#define __TLV_VERSION 1
#define __TLV_MAGIC 0xbeef


struct __tlv_header {
    unsigned char version;
    unsigned char len;
    unsigned short magic;
};

struct __tlv {
  unsigned char type;
    unsigned char length;
    unsigned short value;
    unsigned int optional;
};

int __tlv_print(void* buffer)
{   
    int i;
    char* tlv = (char*) buffer;
    struct __tlv_header* header = (struct __tlv_header*) tlv;
    struct __tlv* option;
    tlv += sizeof(struct __tlv_header);

    if(header->magic != __TLV_MAGIC)
        return -1;

    printk("********** TLV **********\n");
    printk("Header: \n");
    printk("\tVersion %d\n\tOptions: %d\n\tMagic 0x%x\n", header->version, header->len, header->magic);  
    for (i = 0; i < header->len; i++)
    {
        printk("TLV Option %d\n", i+1);
        option = (struct __tlv*) tlv;
        printk("\tType %x\n\tLength: %d\n\tvalue %d\n", option->type, option->length, option->value);
        if(option->optional != 0)
            printk("\tOptional: %d\n", option->optional);

        tlv += sizeof(struct __tlv);
    }

    return 0;
}

#include "pep_errors.h"

#define DRIVER_AUTHOR "Joe Bayer <joeba@uio.no>" 
#define DRIVER_DESC "Kernel module for a Interactive Traffic PEP"

#define MAX_BUF_SIZE     1451 * 7

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
struct workqueue_struct * my_workqueue;
LIST_HEAD(pep_tunnel_list);

void pkt_hex_dump(struct sk_buff *skb)
{
    size_t len;
    int rowsize = 16;
    int i, l, linelen, remaining;
    int li = 0;
    uint8_t *data, ch; 

    printk("Packet hex dump: %d\n", skb->len);
    data = (uint8_t *) skb_mac_header(skb);

    if (skb_is_nonlinear(skb)) {
        len = skb->data_len;
    } else {
        len = skb->len;
    }

    remaining = skb->len+14;
    len = skb->len+14;
    for (i = 0; i < len; i += rowsize) {
        printk("%06d\t", li);

        linelen = min(remaining, rowsize);
        remaining -= rowsize;

        for (l = 0; l < linelen; l++) {
            ch = data[l];
            printk(KERN_CONT "%02X ", (uint32_t) ch);
        }

        data += linelen;
        li += 10; 

        printk(KERN_CONT "\n");
    }
}

static void pepdna_tcp_listen_data_ready(struct sock *sk)
{
        void (*ready)(struct sock *sk);
        const struct iphdr *iph;
        const struct tcphdr *tcph;
        char* data;
        unsigned char* tail_ptr;
        char* tcp_head_off;
        unsigned char *buffer;
        int i;
        int headerlen;
        unsigned int size;
        int headlen;

        read_lock_bh(&sk->sk_callback_lock);
        ready = sk->sk_user_data;
        if (!ready) { /* check for teardown race */
                ready = sk->sk_data_ready;
        }


        struct sk_buff* skb = skb_peek(&sk->sk_receive_queue);
        if(skb == NULL)
            printk(KERN_INFO "[PEP] NO SKB\n");
        else{
            size = skb_end_offset(skb) + skb->data_len;
            headerlen = skb_headroom(skb);
            iph = ip_hdr(skb);
            if (iph->protocol == IPPROTO_TCP) {
                tcph = tcp_hdr(skb);
                /* Check for packets with ONLY SYN flag set */
                if (tcph->dest == htons(8181) ) {

                    tail_ptr = skb_tail_pointer(skb);

                    tcp_head_off = (char *) ((unsigned char*) tcph + (tcph->doff * 4));

                    //printk(KERN_INFO "[PEP] data: %x, tail: %x, diff: %d\n TCP: %x, data2: %x\n", skb->data, tail_ptr, tail_ptr-skb->data, tcph, tcp_head_off);

                    buffer = kzalloc(size, GFP_KERNEL);

                    printk(KERN_INFO "[PEP] skb size: %d, size: %d, syn: %d, ack: %d\n", skb->len, size, tcph->syn, tcph->ack);

                    headlen = skb_headlen(skb);

                    if (skb_copy_bits(skb, headlen, buffer, skb->data_len))
                        printk(KERN_INFO "[PEP] SKB COPY BITS BUG\n");  

                    __tlv_print(buffer);
                    //printk(KERN_INFO  "[PEP] msg: %s\n", buffer);
                

                    data = tail_ptr;

                    kfree(buffer);
                    //data -= skb->data_len;
                    //printk(KERN_INFO "[PEP] FOR SERVER %d, %s\n", skb->data_len, data);
                    //pkt_hex_dump(skb);
                }
            }
        }

        read_unlock_bh(&sk->sk_callback_lock);
        ready(sk);
}

static unsigned int pep_nf_hook(void *priv, struct sk_buff *skb, const struct nf_hook_state *state)
{
    const struct iphdr *iph;
    const struct tcphdr *tcph;
    struct pep_connect_work* new_conn;
    struct pep_tunnel_work* tunnel;
    char* data;
    unsigned char* tail_ptr;
    char* tcp_head_off;
    unsigned char *buffer;
    int i;
    int headerlen = skb_headroom(skb);
    unsigned int size = skb_end_offset(skb) + skb->data_len;

    int headlen;

    if (!skb)
        return NF_ACCEPT;

    iph = ip_hdr(skb);
    if (iph->protocol == IPPROTO_TCP) {
        tcph = tcp_hdr(skb);
        /* Check for packets with ONLY SYN flag set */
        if (tcph->dest == htons(8181) ) {

            tail_ptr = skb_tail_pointer(skb);

            tcp_head_off = (char *) ((unsigned char*) tcph + (tcph->doff * 4));

            //printk(KERN_INFO "[PEP] data: %x, tail: %x, diff: %d\n TCP: %x, data2: %x\n", skb->data, tail_ptr, tail_ptr-skb->data, tcph, tcp_head_off);

            buffer = kzalloc(size, GFP_KERNEL);

            printk(KERN_INFO "[PEP] skb size: %d, size: %d, syn: %d, ack: %d\n", skb->len, size, tcph->syn, tcph->ack);

            headlen = skb_headlen(skb);

            if (skb_copy_bits(skb, headlen, buffer, skb->data_len) <= 0)
                printk(KERN_INFO "[PEP] SKB COPY BITS BUG\n");  

            __tlv_print(buffer);
            //printk(KERN_INFO  "[PEP] msg: %s\n", buffer);
        

            data = tail_ptr;

            kfree(buffer);
            //data -= skb->data_len;
            //printk(KERN_INFO "[PEP] FOR SERVER %d, %s\n", skb->data_len, data);
            //pkt_hex_dump(skb);
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

    int qlen = 5;
    int val = 1;
    sockptr_t optval;
    sockptr_t optval2;

    ret = sock_create_kern(&init_net, PF_INET, SOCK_STREAM, IPPROTO_TCP, &sock);
    if(ret){
        printk(KERN_INFO "[PEP] init_core: Error creating socket\n");
    }

    sk = sock->sk;
    sk->sk_reuse = 1;

     write_lock_bh(&sk->sk_callback_lock);
    sk->sk_user_data  = sk->sk_data_ready;
    //sk->sk_data_ready = pepdna_tcp_listen_data_ready;
    write_unlock_bh(&sk->sk_callback_lock);

    /* pep server connection info */
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = INADDR_ANY;
    saddr.sin_port = (__force u16)htons(8181);

    optval = KERNEL_SOCKPTR(&qlen);
    optval2 = KERNEL_SOCKPTR(&val);
    sock_setsockopt(sock, SOL_TCP, TCP_FASTOPEN, optval, sizeof(qlen));
    sock_setsockopt(sock, SOL_TCP, TCP_NODELAY, optval2, sizeof(val));

    ret = kernel_bind(sock, (struct sockaddr*)&saddr, addr_len);
    // CHECK ERROR
    ret = kernel_listen(sock, 5);

    conn = kzalloc(sizeof(struct pep_accept_work), GFP_ATOMIC);
    conn->sock = sock;
    conn->run = ((atomic_t) { (1) });

    nf_register_net_hooks(&init_net, pep_nf_hook_ops, ARRAY_SIZE(pep_nf_hook_ops));

    //INIT_WORK(&conn->task, pep_accept_work_fn);
    //schedule_work(&conn->task);
 
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
