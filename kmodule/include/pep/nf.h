#ifndef E2852838_145B_4FC0_A208_95CF34125D4D
#define E2852838_145B_4FC0_A208_95CF34125D4D

#include "common.h"

u32 pep_syn_pkt(struct sk_buff* skb);
u32 pep_nf_hook(void* priv, struct sk_buff* skb, const struct nf_hook_state* state);

#endif /* E2852838_145B_4FC0_A208_95CF34125D4D */
