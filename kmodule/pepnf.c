
#include <pep/nf.h>

u32 pep_syn_pkt(struct sk_buff* skb)
{
        int len = skb->data_len;
        int headlen = skb_headlen(skb);
        int datalen = skb_end_offset(skb) + len;
        int ret  = -1;
        u8 buffer[datalen];
                
        if(len == 0)
                return NF_ACCEPT;

        ret = skb_copy_bits(skb, headlen, buffer, len);
        if(ret <= 0)
                goto leave;
        
        if(!tlv_validate(buffer))
                goto leave;

        /* To be continued */

leave:
        return NF_ACCEPT;
}

u32 pep_nf_hook(void* priv, struct sk_buff* skb, const struct nf_hook_state* state)
{
        const struct iphdr* iph;
        const struct tcphdr* tcph;

        if(!skb)
                return NF_ACCEPT;
        
        iph = ip_hdr(skb);
        if(iph->protocol == IPPROTO_TCP){
                tcph = tcp_hdr(skb);
                if(tcph->dest == htons(8181) && tcph->syn && !tcph->ack){
                        return pep_syn_pkt(skb);
                }
        }

        return NF_ACCEPT;
}

static const struct nf_hook_ops pep_nf_hook_ops[] = {
    {
        .hook     = pep_nf_hook,
        .pf       = NFPROTO_IPV4,
        .hooknum  = NF_INET_PRE_ROUTING,
        .priority = -500,
    },
};

u8 pep_nf_register()
{
        nf_register_net_hooks(&init_net, pep_nf_hook_ops, ARRAY_SIZE(pep_nf_hook_ops));
}