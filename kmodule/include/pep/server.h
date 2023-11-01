#ifndef D0D9EB81_F5DB_441A_A89F_9D74A2A67438
#define D0D9EB81_F5DB_441A_A89F_9D74A2A67438

#include <pep/common.h>

/* forward declaration */
struct pep_state;

/* server operators */
struct pep_state_ops {
        int (*init)(struct pep_state* server, u16 port);
        int (*start)(struct pep_state* server);
        void (*stop)(struct pep_state* server);
};

/* server work operators */
struct pep_state_work_ops {
        /* work for accepting new connections */
        void (*accept)(struct work_struct *work);
        /* work for forwarding data from client to endpoint */
        void (*forward_c2e)(struct work_struct *work);
        /* work for forwarding data from endpoint to client */
        void (*forward_e2c)(struct work_struct *work);
};

/* server callbacks */
struct pep_socket_callbacks {
        void (*server_data_ready)(struct sock* sk);
        void (*client_data_ready)(struct sock* sk);
        void (*endpoint_data_ready)(struct sock* sk);
};

struct pep_state {
        struct socket* socket;

        struct pep_state_ops* ops;
        struct pep_state_work_ops* work_ops;
        struct pep_socket_callbacks* callbacks;

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

int pep_config_server(struct pep_state* server, struct pep_state_work_ops* work_ops);

void pep_server_accept_work(struct work_struct *work);
void pep_listen_data_ready(struct sock* sk);
int pep_server_init(struct pep_state* server, u16 port);

#endif /* D0D9EB81_F5DB_441A_A89F_9D74A2A67438 */
