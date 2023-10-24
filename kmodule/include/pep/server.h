#ifndef D0D9EB81_F5DB_441A_A89F_9D74A2A67438
#define D0D9EB81_F5DB_441A_A89F_9D74A2A67438

#include <pep/common.h>

/* default server operators */
struct pep_state_ops {
        int (*init)(struct pep_state* server, u16 port);
        int (*start)(struct pep_state* server);
        int (*stop)(struct pep_state* server);
};

/* default server work operators */
struct pep_state_work_ops {
        /* work for accepting new connections */
        void (*accept)(struct work_struct *work);
        /* work for forwarding data from client to endpoint */
        void (*forward_c2e)(struct work_struct *work);
        /* work for forwarding data from endpoint to client */
        void (*forward_e2c)(struct work_struct *work);
};

struct pep_state {
        struct socket* server_socket;

        struct pep_state_ops ops;
        struct pep_state_work_ops work_ops;

        atomic_t state;
        
        struct workqueue_struct* accept_wq;
        struct workqueue_struct* forward_c2e_wq;
        struct workqueue_struct* forward_e2c_wq;

        struct work_struct accept_work;

        struct list_head tunnels;
        unsigned int total_tunnels;
};

struct pep_state* pep_new_server(void);
void pep_server_clean(struct pep_state* server);

void pep_server_accept_work(struct work_struct *work);
void pep_listen_data_ready(struct sock* sk);
int pep_server_init(struct pep_state* server, u16 port);

#endif /* D0D9EB81_F5DB_441A_A89F_9D74A2A67438 */
