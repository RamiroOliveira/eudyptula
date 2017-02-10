#include <linux/ip.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/skbuff.h>
#include <linux/tcp.h>
#include <uapi/linux/tcp.h>
#include <uapi/linux/netfilter.h>

#define EUDYPTULA_ID "2a86d024c0e5"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ramiro Oliveira <roliveir@synopsys.com>");
MODULE_DESCRIPTION("Eudyptula Netfilter");

static unsigned int hook_func(void *priv, struct sk_buff *skb,
			const struct nf_hook_state *state)
{
	struct tcphdr *tcp_header;
	unsigned char *user_data;

	if (!skb)
		return NF_ACCEPT;

	tcp_header = tcp_hdr(skb);

	/* Calculate pointers for begin and end of TCP packet data */
	user_data = (unsigned char *)((unsigned char *)tcp_header +
		(tcp_header->doff * 4));

	if (strstr(user_data, EUDYPTULA_ID) != NULL)
		pr_debug("%s\n", EUDYPTULA_ID);

	return NF_ACCEPT;
}

static struct nf_hook_ops nf_eudyptula = {
	.hook		= hook_func,
	.hooknum	= NF_INET_PRE_ROUTING,
	.pf		= PF_INET,
	.priority	= NF_IP_PRI_FIRST,
};

static int __init init_eudyptula_nf(void)
{
	nf_register_hook(&nf_eudyptula);

	return 0;
}

static void __exit exit_eudyptula_nf(void)
{
	nf_unregister_hook(&nf_eudyptula);
}

module_init(init_eudyptula_nf);
module_exit(exit_eudyptula_nf);
