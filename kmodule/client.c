#include <pep/common.h>

void pep_client_receive_work(struct work_struct *work)
{
        struct pep_tunnel* tun = container_of(work, struct pep_tunnel, c2e);
}

void pep_client_data_ready(struct sock* sk)
{
        default_data_ready(sk);
}
