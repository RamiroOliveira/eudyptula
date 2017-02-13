#include <linux/ip.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/skbuff.h>
#include <linux/tcp.h>

#define EUDYPTULA_ID "2a86d024c0e5"
static struct ts_config *ts;

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ramiro Oliveira <roliveir@synopsys.com>");
MODULE_DESCRIPTION("Eudyptula Netfilter");

static unsigned int hook_func(void *priv, struct sk_buff *skb,
			const struct nf_hook_state *state)
{
	if (!skb)
		return NF_ACCEPT;

	if (skb_find_text(skb, 0, INT_MAX, ts) != UINT_MAX)
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
	ts = textsearch_prepare("kmp", EUDYPTULA_ID, strlen(EUDYPTULA_ID),
						GFP_KERNEL, TS_AUTOLOAD);
	if (IS_ERR(ts))
		return PTR_ERR(ts);

	nf_register_hook(&nf_eudyptula);

	return 0;
}

static void __exit exit_eudyptula_nf(void)
{
	textsearch_destroy(ts);
	nf_unregister_hook(&nf_eudyptula);
}

module_init(init_eudyptula_nf);
module_exit(exit_eudyptula_nf);
