#ifndef D0D9EB81_F5DB_441A_A89F_9D74A2A67438
#define D0D9EB81_F5DB_441A_A89F_9D74A2A67438

#include <linux/workqueue.h>

struct pep_state {
        struct socket* server_socket;
        
        struct workqueue_struct* accept_wq;
        struct workqueue_struct* forward_c2e_wq;
        struct workqueue_struct* forward_e2c_wq;

        struct work_struct* accept_work;
};

#endif /* D0D9EB81_F5DB_441A_A89F_9D74A2A67438 */
