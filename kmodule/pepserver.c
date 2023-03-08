#include <pep/server.h>
#include <pep/endpoint.h>
#include <pep/client.h>
#include <pep/errors.h>
#include <pep/tcp.h>

enum pep_states  {
        PEP_SERVER_RUNNING,
        PEP_SERVER_STOPPED
};

/**
 * @brief Responsible for accepting a new client and connection to the desired endpoint.
 * 
 * @param work 
 */
void pep_server_accept_work(struct work_struct *work)
{
		int rc = 0;
		int ret;
		struct socket* client = NULL;
		struct socket* endpoint = NULL;
		struct pep_tunnel* tunnel;
		struct tlv* tlv;

		struct sock* sk;

		struct pep_state* server = container_of(work, struct pep_state, accept_work);
		unsigned char *buffer;

		if(atomic_read(&server->state) != PEP_SERVER_RUNNING)
			return;

		int buffsize = 98304;
		sockptr_t valuelen;
        valuelen = KERNEL_SOCKPTR(&buffsize);

		while(rc == 0) {
				rc = kernel_accept(server->server_socket, &client, O_NONBLOCK);
				if(rc < 0)
						return;

				printk(KERN_INFO "[PEP] pep_server_accept_work: Accepted new connection!\n");
				/* allocate buffer memory */
				buffer = kzalloc(PEP_MAX_TCP_BUFFER_SIZE, GFP_KERNEL);
				if (!buffer) {
						printk(KERN_INFO "[PEP] pep_server_accept_work: out of memory \n");
						sock_release(client);
						return;
				}

				/**
				 * It might be smart to create a new work for the endpoint connection?
				 * as we dont want the accept work to become a potential bottleneck.
				 * 
				 * But work queueing could also become a overhead.
				 */

				ret = pep_tcp_receive(client, buffer, PEP_MAX_TCP_BUFFER_SIZE);
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
				if(tlv == NULL || tlv->length != 6){
						printk(KERN_INFO "[PEP] pep_server_accept_work: invalid tlv option. \n");
						sock_release(client);
						return; 
				}

				/**
				 * Connect to TLV specified endpoint 
				 * tlv->optional contains ip, tlv->value contains port.
				 * Both in correct byte order.
				 */
				endpoint = pep_endpoint_connect(tlv->optional, tlv->value);
				if(NULL == endpoint)
				{
					printk(KERN_INFO "[PEP] pep_server_accept_work: Connection to endpoint failed.\n");
					sock_release(client);
					return; 
				}

				/* Setup new pep tunnel */
				tunnel = pep_new_tunnel();
				tunnel->client.sock = client;
				tunnel->endpoint.sock = endpoint;
				tunnel->state = 0;
				tunnel->total_client = 0;
				tunnel->total_endpoint = 0;
				tunnel->server = server;	
				INIT_WORK(&tunnel->c2e, &pep_client_receive_work);
				INIT_WORK(&tunnel->e2c, &pep_endpoint_receive_work);

				/* Configure sockets | TCP Options? send - recb buf size */
				pep_configue_sk(endpoint, &pep_endpoint_data_ready, tunnel);
				pep_configue_sk(client, &pep_client_data_ready, tunnel);

				/* Configure snd & rev buffers, can use SO_RCVBUFFORCE and SO_SNDBUFFORCE to overwrite limit*/
				sock_setsockopt(endpoint, SOL_SOCKET, SO_RCVBUF, valuelen, sizeof(buffsize));
				sock_setsockopt(endpoint, SOL_SOCKET, SO_SNDBUF, valuelen, sizeof(buffsize));

				sock_setsockopt(client, SOL_SOCKET, SO_SNDBUF, valuelen, sizeof(buffsize));
				sock_setsockopt(client, SOL_SOCKET, SO_RCVBUF, valuelen, sizeof(buffsize));


				/* Add tunnel to linked list of all pep tunnels. */
				list_add(&tunnel->list, &server->tunnels);
				tunnel->id = server->total_tunnels;
				server->total_tunnels++;

				/* check if more data than the initial TLV was sent.  */
				if(ret > 12){
					buffer += 12;
					/* again: should probably be moved out to own work. */
					pep_tcp_send(endpoint, buffer, ret-12);
				}

				queue_work(tunnel->server->forward_c2e_wq, &tunnel->c2e);

				printk(KERN_INFO "[PEP] pep_server_accept_work: New tunnel %d was created.\n", tunnel->id);
				/* Done? */
				kfree(buffer);
				return;
				break;
		}
}

void pep_server_clean(struct pep_state* server)
{
	atomic_set(&server->state, PEP_SERVER_STOPPED);
	cancel_work_sync(&server->accept_work);

	struct pep_tunnel* iter;
	list_for_each_entry(iter, &server->tunnels, list) {
        list_del(&iter->list);

		cancel_work_sync(&iter->c2e);
		cancel_work_sync(&iter->e2c);

		sock_release(iter->client.sock);
		sock_release(iter->endpoint.sock);

		kfree(iter);
    }

	sock_release(server->server_socket);
}

void pep_listen_data_ready(struct sock* sk)
{
		printk(KERN_INFO "[PEP] pep_listen_data_ready: New connection request incomming!\n");
		struct pep_state* server;
		struct sk_buff* skb = skb_peek(&sk->sk_receive_queue);

		read_lock_bh(&sk->sk_callback_lock);

		server = sk->sk_user_data;

		/* Queue accept work */
		if(sk->sk_state == TCP_LISTEN){
				queue_work(server->accept_wq, &server->accept_work);
		}

		read_unlock_bh(&sk->sk_callback_lock);

		default_data_ready(sk);
}

void pep_server_cleanup(struct pep_state* server)
{
	/* Destroy workqueues and sockets*/

	/* cleanup all tunnels */
}

int pep_server_init(struct pep_state* server, u16 port)
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

		server->state = ((atomic_t){(PEP_SERVER_RUNNING)});
		
		/* use our own data ready function */
		write_lock_bh(&sk->sk_callback_lock);
		default_data_ready = sk->sk_data_ready;
		sk->sk_user_data = server;
		sk->sk_data_ready = &pep_listen_data_ready;
		write_unlock_bh(&sk->sk_callback_lock);

		/* pep server connection info */
		saddr.sin_family = AF_INET;
		saddr.sin_addr.s_addr = INADDR_ANY;
		saddr.sin_port = (__force u16)htons(port);

		pep_setsockopt(sock, TCP_FASTOPEN, 5);
		pep_setsockopt(sock, TCP_NODELAY, 1);
		
		ret = kernel_bind(sock, (struct sockaddr*)&saddr, addr_len);
		if(ret < 0){
			printk(KERN_INFO "[PEP] init_core: Error binding socket\n");
			return -EPEP_GENERIC;
		}

		ret = kernel_listen(sock, 5);
		if(ret < 0){
			printk(KERN_INFO "[PEP] init_core: Error listen socket\n");
			return -EPEP_GENERIC;
		}

		server->server_socket = sock;
		server->accept_wq = alloc_workqueue("accept_wq", WQ_HIGHPRI|WQ_UNBOUND, 0);
		server->forward_c2e_wq = alloc_workqueue("c2e_wq", WQ_HIGHPRI|WQ_UNBOUND, 0);
		server->forward_e2c_wq = alloc_workqueue("e2c_wq", WQ_HIGHPRI|WQ_UNBOUND, 0);

		INIT_WORK(&server->accept_work, &pep_server_accept_work);
		INIT_LIST_HEAD(&server->tunnels);
		server->total_tunnels = 0;

		printk("[PEP] Server has started.\n");

		return 0;
}
