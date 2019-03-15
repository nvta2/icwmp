#ifndef __SE_IPACCCFG_H
#define __SE_IPACCCFG_H

struct ipaccargs
{
	struct uci_section *ipaccsection;
};

struct pforwardrgs
{
	struct uci_section *forwardsection;
};

struct zones
{
	struct uci_section *zonesection;
};

struct zoneforwards
{
	struct uci_section *zonefwdsection;
};


int entry_method_root_X_INTENO_SE_IpAccCfg(struct dmctx *ctx);
#endif