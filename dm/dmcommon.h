/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2015 PIVA SOFTWARE (www.pivasoftware.com)
 *		Author: Imen Bhiri <imen.bhiri@pivasoftware.com>
 *		Author: Feten Besbes <feten.besbes@pivasoftware.com>
 */

#ifndef __DM_COMMON_H
#define __DM_COMMON_H
#include <sys/types.h>
#include <libubox/blobmsg_json.h>
#include <json-c/json.h>
#include <unistd.h>
#include "dmcwmp.h"
#define NVRAM_FILE "/proc/nvram/WpaKey"
#define MAX_DHCP_LEASES 256
#define MAX_PROC_ROUTING 256
#define ROUTING_FILE "/proc/net/route"
#define ARP_FILE "/proc/net/arp"
#define DHCP_LEASES_FILE "/tmp/dhcp.leases"
#define DMMAP "dmmap"
#define DHCPSTATICADDRESS_DISABLED_CHADDR "00:00:00:00:00:01"
#define DM_ASSERT(X, Y) \
do { \
	if(!(X)) { \
		Y; \
		return -1; \
	} \
} while(0)

#define dmstrappendstr(dest, src) \
do { \
	int len = strlen(src); \
	memcpy(dest, src, len); \
	dest += len; \
} while(0)

#define dmstrappendchr(dest, c) \
do { \
	*dest = c; \
	dest += 1; \
} while(0)

#define dmstrappendend(dest) \
do { \
	*dest = '\0'; \
} while(0)


#define DMCMD(CMD, N, ...) \
do { \
	int mpp = dmcmd(CMD, N, ## __VA_ARGS__); \
	if (mpp) close (mpp); \
} while (0)

#define IPPING_PATH "/usr/share/icwmp/functions/ipping_launch"
#define IPPING_STOP DMCMD("/bin/sh", 2, IPPING_PATH, "stop");
#define DOWNLOAD_DIAGNOSTIC_PATH "/usr/share/icwmp/functions/download_launch"
#define DOWNLOAD_DUMP_FILE "/tmp/download_dump"
#define DOWNLOAD_DIAGNOSTIC_STOP DMCMD("/bin/sh", 2, DOWNLOAD_DIAGNOSTIC_PATH, "stop");
#define UPLOAD_DIAGNOSTIC_PATH "/usr/share/icwmp/functions/upload_launch"
#define UPLOAD_DUMP_FILE "/tmp/upload_dump"
#define UPLOAD_DIAGNOSTIC_STOP DMCMD("/bin/sh", 2, UPLOAD_DIAGNOSTIC_PATH, "stop");
#define NSLOOKUP_PATH "/usr/share/icwmp/functions/nslookup_launch"
#define NSLOOKUP_LOG_FILE "/tmp/nslookup.log"
#define NSLOOKUP_STOP DMCMD("/bin/sh", 2, NSLOOKUP_PATH, "stop");
#define TRACEROUTE_PATH "/usr/share/icwmp/functions/traceroute_launch"
#define TRACEROUTE_STOP DMCMD("/bin/sh", 2, TRACEROUTE_PATH, "stop");
#define UDPECHO_PATH "/usr/share/icwmp/functions/udpecho_launch"
#define UDPECHO_STOP DMCMD("/bin/sh", 2, UDPECHO_PATH, "stop");
#define SERVERSELECTION_PATH "/usr/share/icwmp/functions/serverselection_launch"
#define SERVERSELECTION_STOP DMCMD("/bin/sh", 2, SERVERSELECTION_PATH, "stop");

#define sysfs_foreach_file(path,dir,ent) \
        if ((dir = opendir(path)) == NULL) return 0; \
        while ((ent = readdir (dir)) != NULL) \


enum notification_enum {
	notification_none,
	notification_passive,
	notification_active,
	notification_passive_lw,
	notification_ppassive_passive_lw,
	notification_aactive_lw,
	notification_passive_active_lw,
	__MAX_notification
};

enum strstructered_enum {
	STRUCTERED_SAME,
	STRUCTERED_PART,
	STRUCTERED_NULL
};

struct proc_routing {
	char *iface;
	char *flags;
	char *refcnt;
	char *use;
	char *metric;
	char *mtu;
	char *window;
	char *irtt;
	char destination[16];
	char gateway[16];
	char mask[16];
};

struct routingfwdargs
{
	char *permission;
	struct uci_section *routefwdsection;
	struct proc_routing *proute;
	int type;
};

struct dmmap_dup
{
	struct list_head list;
	struct uci_section *config_section;
	struct uci_section *dmmap_section;
	void* additional_attribute;
};

struct dmmap_sect {
	struct list_head list;
	char *section_name;
	char *instance;
};

struct dm_args
{
	struct uci_section *section;
	struct uci_section *dmmap_section;
	char *name;
};

struct sysfs_dmsection {
	struct list_head list;
	char *sysfs_folder_path;
	char *sysfs_folder_name;
	struct uci_section *dm;
};

void compress_spaces(char *str);
char *cut_fx(char *str, char *delimiter, int occurence);
pid_t get_pid(char *pname);
int check_file(char *path);
char *cidr2netmask(int bits);
void remove_substring(char *s, const char *str_remove);
bool is_strword_in_optionvalue(char *optionvalue, char *str);
int get_interface_enable_ubus(char *iface, char *refparam, struct dmctx *ctx, char **value);
int set_interface_enable_ubus(char *iface, char *refparam, struct dmctx *ctx, int action, char *value);
int get_interface_firewall_enabled(char *iface, char *refparam, struct dmctx *ctx, char **value);
struct uci_section *create_firewall_zone_config(char *fwl, char *iface, char *input, char *forward, char *output);
int set_interface_firewall_enabled(char *iface, char *refparam, struct dmctx *ctx, int action, char *value);
int dmcmd(char *cmd, int n, ...);
int dmcmd_read(int pipe, char *buffer, int size);
void dmcmd_read_alloc(int pipe, char **value);
int dmcmd_no_wait(char *cmd, int n, ...);
int ipcalc(char *ip_str, char *mask_str, char *start_str, char *end_str, char *ipstart_str, char *ipend_str);
int ipcalc_rev_start(char *ip_str, char *mask_str, char *ipstart_str, char *start_str);
int ipcalc_rev_end(char *ip_str, char *mask_str, char *start_str, char *ipend_str, char *end_str);
int network_get_ipaddr(char **value, char *iface);
void remove_vid_interfaces_from_ifname(char *vid, char *ifname, char *new_ifname);
void update_section_option_list(char *config, char *section, char *option, char *option_2,char *val, char *val_2, char *name);
void update_section_list_icwmpd(char *config, char *section, char *option, int number, char *filter, char *option1, char *val1,  char *option2, char *val2);
void update_section_list(char *config, char *section, char *option, int number, char *filter, char *option1, char *val1,  char *option2, char *val2);
char *get_nvram_wpakey();
int reset_wlan(struct uci_section *s);
int get_cfg_layer2idx(char *pack, char *section_type, char *option, int shift);
int wan_remove_dev_interface(struct uci_section *interface_setion, char *dev);
int filter_lan_device_interface(struct uci_section *s);
void remove_vlan_from_bridge_interface(char *bridge_key, struct uci_section *vb);
void update_remove_vlan_from_bridge_interface(char *bridge_key, struct uci_section *vb);
int filter_lan_ip_interface(struct uci_section *ss, void *v);
void remove_interface_from_ifname(char *iface, char *ifname, char *new_ifname);
int max_array(int a[], int size);
int check_ifname_is_vlan(char *ifname);
int set_uci_dhcpserver_option(struct dmctx *ctx, struct uci_section *s, char *option, char *value);
int update_uci_dhcpserver_option(struct dmctx *ctx, struct uci_section *s, char *option, char * new_option, char *value);
void parse_proc_route_line(char *line, struct proc_routing *proute);
int strstructered(char *str1, char *str2);
int dmcommon_check_notification_value(char *value);
void hex_to_ip(char *address, char *ret);
void ip_to_hex(char *address, char *ret);
void free_dmmap_config_dup_list(struct list_head *dup_list);
void synchronize_specific_config_sections_with_dmmap(char *package, char *section_type, char *dmmap_package, struct list_head *dup_list);
void synchronize_specific_config_sections_with_dmmap_eq(char *package, char *section_type, char *dmmap_package,char* option_name, char* option_value, struct list_head *dup_list);
void synchronize_specific_config_sections_with_dmmap_eq_no_delete(char *package, char *section_type, char *dmmap_package,char* option_name, char* option_value, struct list_head *dup_list);
void synchronize_specific_config_sections_with_dmmap_cont(char *package, char *section_type, char *dmmap_package,char* option_name, char* option_value, struct list_head *dup_list);
bool synchronize_multi_config_sections_with_dmmap_eq(char *package, char *section_type, char *dmmap_package, char* dmmap_section, char* option_name, char* option_value, void* additional_attribute, struct list_head *dup_list);
bool synchronize_multi_config_sections_with_dmmap_eq_diff(char *package, char *section_type, char *dmmap_package, char* dmmap_section, char* option_name, char* option_value, char* opt_diff_name, char* opt_diff_value, void* additional_attribute, struct list_head *dup_list);
void synchronize_system_folders_with_dmmap_opt(char *sysfsrep, char *dmmap_package, char *dmmap_section, char *opt_name, char* inst_opt, struct list_head *dup_list);
void get_dmmap_section_of_config_section(char* dmmap_package, char* section_type, char *section_name, struct uci_section **dmmap_section);
void get_dmmap_section_of_config_section_eq(char* dmmap_package, char* section_type, char *opt, char* value, struct uci_section **dmmap_section);
void get_config_section_of_dmmap_section(char* package, char* section_type, char *section_name, struct uci_section **config_section);
void check_create_dmmap_package(char *dmmap_package);
int is_section_unnamed(char *section_name);
void delete_sections_save_next_sections(char* dmmap_package, char *section_type, char *instancename, char *section_name, int instance, struct list_head *dup_list);
void update_dmmap_sections(struct list_head *dup_list, char *instancename, char* dmmap_package, char *section_type);
unsigned char isdigit_str(char *str);
char *dm_strword(char *src, char *str);
char **strsplit(const char* str, const char* delim, size_t* numtokens);
char *get_macaddr(char *ifname);
char *get_device(char *ifname);
int is_elt_exit_in_str_list(char *str_list, char *elt);
void add_elt_to_str_list(char **str_list, char *elt);
void remove_elt_from_str_list(char **iface_list, char *ifname);
struct uci_section *get_dup_section_in_dmmap_eq(char *dmmap_package, char* section_type, char*sect_name, char *opt_name, char* opt_value);
int is_array_elt_exist(char **str_array, char *str, int length);
int get_shift_time_time(int shift_time, char *local_time, int size);
int get_shift_time_shift(char *local_time, char *shift);
int get_stats_from_ifconfig_command(char *device, char *direction, char *option);
int command_exec_output_to_array(char *cmd, char **output, int *length);
char* int_period_to_date_time_format(int time);
struct uci_section *is_dmmap_section_exist(char* package, char* section);
struct uci_section *is_dmmap_section_exist_eq(char* package, char* section, char* opt, char* value);
int isfileexist(char *filepath);
char* readFileContent(char *filepath);
#endif
