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

        struct work_struct c2e;
        struct work_struct e2c;

        struct list_head list;

        int state;
};

struct pep_state server_state;


void (*default_data_ready)(struct sock *sk);

/* Linked list over all tunnels */
LIST_HEAD(pep_tunnels);

struct pep_tunnel* pep_new_tunnel(void)
{
        return (struct pep_tunnel*) kzalloc(sizeof(struct pep_tunnel), GFP_KERNEL);
}

static inline int pep_setsockopt(struct socket* sock, int option, int value)
{
        sockptr_t valuelen;
        valuelen = KERNEL_SOCKPTR(&value);

        sock->ops->setsockopt(sock, SOL_TCP, option, valuelen, sizeof(value));
        return 0;
}

void pep_client_receive_work(struct work_struct *work)
{
        struct pep_tunnel* tun = container_of(work, struct pep_tunnel, c2e);
}

void pep_endpoint_receive_work(struct work_struct *work)
{       
        int ret;
        struct pep_tunnel* tun = container_of(work, struct pep_tunnel, e2c);

        unsigned char *buffer = kzalloc(1500, GFP_KERNEL);
        if (!buffer) {
                printk(KERN_INFO "[PEP] pep_endpoint_receive_work: out of memory \n");
                return;
        }

        ret = pep_tcp_receive(tun->endpoint.sock, buffer, 1500);
        if(ret > 0){
                ret = pep_tcp_send(tun->client.sock, buffer, ret);
        }

}

static void pep_endpoint_data_ready(struct sock* sk)
{

        default_data_ready(sk);
}

static void pep_client_data_ready(struct sock* sk)
{
        default_data_ready(sk);
}


struct socket* pep_endpoint_connect(u32 ip, u16 port)
{
        struct socket* sock = NULL;
        struct sock* sk = NULL;
        struct sockaddr_in daddr;
        int addr_len = sizeof(daddr);
        int ret = 0;

        ret = sock_create_kern(&init_net, PF_INET, SOCK_STREAM, IPPROTO_TCP, &sock);
        if(ret){
                printk(KERN_INFO "[PEP] pep_endpoint_connect: Error creating socket\n");
                return NULL;
        }

        /* Add tcp options? */

        /* pep endpoint connection info */
        daddr.sin_family = AF_INET;
        daddr.sin_addr.s_addr = ip;
        daddr.sin_port = (__force u16)port;

        ret = kernel_connect(sock, (struct sockaddr*)&daddr, addr_len, 0);
	if (ret < 0) {
		sock_release(sock);
		printk(KERN_INFO "[PEP] pep_endpoint_connect: failed to connect to endpoint.\n");
                return NULL;
	}

        return sock;
}

void pep_configue_sk(struct socket* sock, void (*data_ready)(struct sock*), struct pep_tunnel* tunnel)
{
        struct sock* sk;

        sk = sock->sk;
        sk->sk_reuse = 1;
        write_lock_bh(&sk->sk_callback_lock);
        sk->sk_user_data = tunnel; // TODO: set to tunnel
        sk->sk_data_ready = data_ready;
        write_unlock_bh(&sk->sk_callback_lock);
}

/**
 * @brief Responsible for accepting a new client and connection to the desired endpoint.
 * 
 * @param work 
 */
void pep_server_accept_work(struct work_struct *work)
{
        int rc = 1;
        int ret;
        struct socket* client = NULL;
        struct socket* endpoint = NULL;
        struct pep_tunnel* tunnel;
        struct tlv* tlv;

        while(rc == 0) {
                rc = kernel_accept(server_state.server_socket, &client, O_NONBLOCK);
                if(rc < 0)
                        return;

                printk(KERN_INFO "[PEP] pep_server_accept_work: Accepted new connection!\n");
                /* allocate buffer memory */
                unsigned char *buffer = kzalloc(1500, GFP_KERNEL);
                if (!buffer) {
                        printk(KERN_INFO "[PEP] pep_server_accept_work: out of memory \n");
                        sock_release(client);
                        return;
                }

                /**
                 * It might be smart to create a new work for the endpoint connection
                 * as we dont want the accept work to become a potential bottleneck.
                 * 
                 * But work queueing could also become a overhead.
                 */

		ret = pep_tcp_receive(client, buffer, 1500);
                if(ret <= 0)
                        return;
                
                /* Validate that the first packet sent has a valid tlv header. */
                if(!tlv_validate(buffer)){
                        printk(KERN_INFO "[PEP] pep_server_accept_work: TLV validate error. \n");
                        sock_release(client);
                        return;
                }

                /* Get connect tlv options from tlv buffer */
                tlv = tlv_get_option(TLV_CONNECT, buffer);
                if(tlv == NULL || tlv->length == 6){
                        printk(KERN_INFO "[PEP] pep_server_accept_work: no CONNECT tlv option. \n");
                        sock_release(client);
                        return; 
                }

                /**
                 * Connect to TLV specified endpoint 
                 * tlv->optional contains ip, tlv->value contains port.
                 * Both in correct byte order.
                 */
                endpoint = pep_endpoint_connect(tlv->optional, tlv->value);

                /* Setup new pep tunnel */
                tunnel = pep_new_tunnel();
                tunnel->client.sock = client;
                tunnel->endpoint.sock = endpoint;
                tunnel->state = 0;
                INIT_WORK(&tunnel->c2e, &pep_client_receive_work);
                INIT_WORK(&tunnel->e2c, &pep_endpoint_receive_work);

                /* Configure sockets | TCP Options? send - recb buf size */
                pep_configue_sk(endpoint, &pep_endpoint_data_ready, tunnel);
                pep_configue_sk(client, &pep_client_data_ready, tunnel);                        
                
                /* Add tunnel to linked list of all pep tunnels. */
                list_add(&tunnel->list, &pep_tunnels);
                /* Done? */

                kfree(buffer);
        }
}

static void pep_listen_data_ready(struct sock* sk)
{
        
        struct sk_buff* skb = skb_peek(&sk->sk_receive_queue);

        read_lock_bh(&sk->sk_callback_lock);

        printk(KERN_INFO "[PEP]: Packet received! %d\n", skb == NULL);

        /* Queue accept work */
        if(sk->sk_state == TCP_LISTEN){
                queue_work(server_state.accept_wq, &server_state.accept_work);
                printk(KERN_INFO "[PEP]: START work\n");  
        }

        read_unlock_bh(&sk->sk_callback_lock);

        default_data_ready(sk);
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
        default_data_ready = sk->sk_data_ready;
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

        INIT_WORK(&server_state.accept_work, &pep_server_accept_work);

        //ppe_nf_register();

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
