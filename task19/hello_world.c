#include <linux/ip.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/skbuff.h>
#include <linux/udp.h>
#include <linux/tcp.h>
#include <uapi/linux/tcp.h>
#include <uapi/linux/netfilter.h>

#define PTCP_WATCH_PORT1     443  /* HTTP port */
#define PTCP_WATCH_PORT2     80  /* HTTP port */
#define EUDYPTULA_ID_SIZE 13
#define EUDYPTULA_ID "2a86d024c0e5"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ramiro Oliveira <roliveir@synopsys.com>");
MODULE_DESCRIPTION("Eudyptula Netfilter");

unsigned int hook_func(void *priv, struct sk_buff *skb,
			const struct nf_hook_state *state)
{
	struct iphdr *iph;          /* IPv4 header */
	struct tcphdr *tcph;        /* TCP header */
	u16 sport, dport;           /* Source and destination ports */
	u32 saddr, daddr;           /* Source and destination addresses */
	unsigned char *user_data;   /* TCP data begin pointer */
	unsigned char *tail;        /* TCP data end pointer */
	unsigned char *it;          /* TCP data iterator */

	/* Network packet is empty, seems like some problem occurred. Skip it */
	if (!skb)
		return NF_ACCEPT;

	iph = ip_hdr(skb);          /* get IP header */

	/* Skip if it's not TCP packet */
	if (iph->protocol != IPPROTO_TCP)
		return NF_ACCEPT;

	tcph = tcp_hdr(skb);        /* get TCP header */

	/* Convert network endianness to host endiannes */
	saddr = ntohl(iph->saddr);
	daddr = ntohl(iph->daddr);
	sport = ntohs(tcph->source);
	dport = ntohs(tcph->dest);

	/* Watch only port of interest */
	if ((sport != PTCP_WATCH_PORT1) && (sport != PTCP_WATCH_PORT2))
		return NF_ACCEPT;

	/* Calculate pointers for begin and end of TCP packet data */
	user_data = (unsigned char *)((unsigned char *)tcph + (tcph->doff * 4));
	tail = skb_tail_pointer(skb);

	/* ----- Print all needed information from received TCP packet ------ */
	/* Show only HTTP packets */
	if (user_data[0] != 'H' || user_data[1] != 'T' || user_data[2] != 'T' || user_data[3] != 'P')
		return NF_ACCEPT;


	/* Print packet route */
	pr_debug("print_tcp: %pI4h:%d -> %pI4h:%d\n", &saddr, sport, &daddr, dport);

	/* Print TCP packet data (payload) */
	pr_debug("print_tcp: data:\n");
	for (it = user_data; it != tail; ++it) {
		char c = *(char *)it;

	if (c == '\0')
		break;

	printk("%c", c);
	}	
	printk("\n\n");

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
