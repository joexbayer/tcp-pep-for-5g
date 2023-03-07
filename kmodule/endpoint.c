#include <pep/common.h>
#include <pep/endpoint.h>
#include <pep/tcp.h>

void pep_endpoint_receive_work(struct work_struct *work)
{       
        int ret = 1;
        int ret_forward;
        struct pep_tunnel* tun = container_of(work, struct pep_tunnel, e2c);

        unsigned char *buffer = kzalloc(PEP_MAX_TCP_BUFFER_SIZE, GFP_KERNEL);
        if (!buffer) {
                printk(KERN_INFO "[PEP] pep_endpoint_receive_work: out of memory \n");
                return;
        }

        while(ret > 0 && !pep_tunnel_is_disconnected(tun) && tun->endpoint.sock != NULL)
        {
                printk(KERN_INFO "[PEP] pep_endpoint_receive_work: reading data from endpoint in tunnel %d starting work.\n", tun->id);
                ret = pep_tcp_receive(tun->endpoint.sock, buffer, PEP_MAX_TCP_BUFFER_SIZE);
                if(ret > 0){
                        ret_forward = pep_tcp_send(tun->client.sock, buffer, ret);
                        printk(KERN_INFO "[PEP] pep_endpoint_receive_work: Tunnel %d forwarded %d/%d bytes to client.\n", tun->id, ret, ret_forward);
                } else {
                        if(pep_tunnel_is_disconnected(tun)){
                                pep_tunnel_close(tun);
                                return;
                        }
                }
        }

        kfree(buffer);

}

void pep_endpoint_data_ready(struct sock* sk)
{
        struct pep_tunnel* tunnel = sk->sk_user_data;

        printk(KERN_INFO "[PEP] pep_endpoint_data_ready: Incoming data from endpoint in tunnel %d\n", tunnel->id);
        
        if(!queue_work(tunnel->server->forward_e2c_wq, &tunnel->e2c))
                printk(KERN_INFO "[PEP] pep_endpoint_data_ready: Work already in queue. Tunnel %d\n", tunnel->id);

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

        printk(KERN_INFO "[PEP] pep_endpoint_connect: Connecting to endpoint %d:%d\n", ip, port);
        ret = kernel_connect(sock, (struct sockaddr*)&daddr, addr_len, 0);
	if (ret < 0) {
		sock_release(sock);
		printk(KERN_INFO "[PEP] pep_endpoint_connect: failed to connect to endpoint.\n");
                return NULL;
	}

        return sock;
}