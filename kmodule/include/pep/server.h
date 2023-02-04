#ifndef D0D9EB81_F5DB_441A_A89F_9D74A2A67438
#define D0D9EB81_F5DB_441A_A89F_9D74A2A67438

#include <pep/common.h>

struct pep_state {
        struct socket* server_socket;
        
        struct workqueue_struct* accept_wq;
        struct workqueue_struct* forward_c2e_wq;
        struct workqueue_struct* forward_e2c_wq;

        struct work_struct accept_work;

        struct list_head tunnels;
};

void pep_server_accept_work(struct work_struct *work);
void pep_listen_data_ready(struct sock* sk);
void pep_server_init(struct pep_state* server, u16 port);

#endif /* D0D9EB81_F5DB_441A_A89F_9D74A2A67438 */
