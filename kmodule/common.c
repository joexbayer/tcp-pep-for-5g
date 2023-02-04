#include <pep/common.h>

void (*default_data_ready)(struct sock *sk);

struct pep_tunnel* pep_new_tunnel(void)
{
        return (struct pep_tunnel*) kzalloc(sizeof(struct pep_tunnel), GFP_KERNEL);
}

int pep_setsockopt(struct socket* sock, int option, int value)
{
        sockptr_t valuelen;
        valuelen = KERNEL_SOCKPTR(&value);

        sock->ops->setsockopt(sock, SOL_TCP, option, valuelen, sizeof(value));
        return 0;
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
