#ifndef __LAN_INTERFACES_H
#define __LAN_INTERFACES_H

struct linterfargs
{
	char *linterf;
	struct uci_section *port_sec;
};

struct ethrnet_ifaces_s
{
	char *eths[16];
	int eths_size;
};

bool check_laninterfaces(struct dmctx *dmctx, void *data);
extern DMLEAF tlaninterface_lanParam[];
extern DMLEAF tlaninterface_wlanParam[];
extern DMLEAF tLANInterfacesParam[];
extern DMOBJ tLANInterfacesObj[];

int browselaninterface_lanInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);
int browselaninterface_wlanInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);

int get_lan_eth_int_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_eth_name(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_wlan_conf_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_lan_ethernet_interface_number(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_lan_wlan_configuration_number(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);

int set_lan_eth_int_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_wlan_conf_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);

#endif
