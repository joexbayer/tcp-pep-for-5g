#ifndef C7C875A3_F654_4F90_A02F_1138DEFED801
#define C7C875A3_F654_4F90_A02F_1138DEFED801

#include <linux/types.h>
#include <linux/net.h>
#include <linux/in.h>
//#include <linux/sock.h>
#include <net/sock.h>
#include <net/ip.h>
#include <net/tcp.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h> 
#include <linux/atomic.h>

#include <linux/workqueue.h>

#include <pep/server.h>

#include <tlv.h>

#define PEP_MAX_TCP_BUFFER_SIZE 1500

enum pep_tunnel_states {
        NEW,
        RUNNING,
        STOPPED
};

struct pep_connection {
        unsigned int ip;
        unsigned short port;
        struct socket* sock;
};

struct pep_tunnel {
        unsigned int id;

        struct pep_connection client;
        struct pep_connection endpoint;

        int total_client;
        int total_endpoint;

        struct work_struct c2e;
        struct work_struct e2c;

        struct list_head list;

        struct pep_state* server;

        int state;

        int recv_callbacks;
        int packets_fowarded;

};

extern void (*default_data_ready)(struct sock *sk);

struct pep_tunnel* pep_new_tunnel(void);
int pep_setsockopt(struct socket* sock, int option, int value);
void pep_configue_sk(struct socket* sock, void (*data_ready)(struct sock*), struct pep_tunnel* tunnel);

int pep_tunnel_is_disconnected(struct pep_tunnel* tunnel);
void pep_tunnel_close(struct pep_tunnel* tunnel);

#endif /* C7C875A3_F654_4F90_A02F_1138DEFED801 */
