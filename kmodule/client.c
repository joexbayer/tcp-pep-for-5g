#include <pep/common.h>
#include <pep/tcp.h>

void pep_client_receive_work(struct work_struct *work)
{
        int ret;
        int ret_forward;
        struct pep_tunnel* tun = container_of(work, struct pep_tunnel, c2e);

        unsigned char *buffer = kzalloc(PEP_MAX_TCP_BUFFER_SIZE, GFP_KERNEL);
        if (!buffer) {
                printk(KERN_INFO "[PEP] pep_endpoint_receive_work: out of memory \n");
                return;
        }

        printk(KERN_INFO "[PEP] pep_client_receive_work: reading data from client in tunnel %d starting work.\n", tun->id);
        ret = pep_tcp_receive(tun->client.sock, buffer, PEP_MAX_TCP_BUFFER_SIZE);
        if(ret > 0){
                ret_forward = pep_tcp_send(tun->endpoint.sock, buffer, ret);
                printk(KERN_INFO "[PEP] pep_client_receive_work: Tunnel %d forwarded %d/%d bytes to endpoint.\n", tun->id, ret, ret_forward);
        }

        kfree(buffer);
}

void pep_client_data_ready(struct sock* sk)
{
        struct pep_tunnel* tunnel = sk->sk_user_data;

        printk(KERN_INFO "[PEP] pep_client_data_ready: Incoming data from client in tunnel %d starting work.\n", tunnel->id);
        
        if(!queue_work(tunnel->server->forward_c2e_wq, &tunnel->c2e))
                printk(KERN_INFO "[PEP] pep_client_data_ready: Work already in queue. Tunnel %d\n", tunnel->id);

        default_data_ready(sk);
}
