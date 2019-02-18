/*
*      This program is free software: you can redistribute it and/or modify
*      it under the terms of the GNU General Public License as published by
*      the Free Software Foundation, either version 2 of the License, or
*      (at your option) any later version.
*
*      Copyright (C) 2019 iopsys Software Solutions AB
*		Author: Amin Ben Ramdhane <amin.benramdhane@pivasoftware.com>
*/

#include "dmuci.h"
#include "dmubus.h"
#include "dmcwmp.h"
#include "dmcommon.h"
#include "dmjson.h"
#include "dns.h"


DMOBJ tDNSObj[] = {
{"Client", &DMREAD, NULL, NULL, NULL, NULL, NULL, NULL, tClientObj, tClientParams, NULL},
{"Relay", &DMREAD, NULL, NULL, NULL, NULL, NULL, NULL, tRelayObj, tRelayParams, NULL},
{"Diagnostics", &DMREAD, NULL, NULL, NULL, NULL, NULL, NULL, tDiagnosticsObj, NULL, NULL},
{0}
};

DMLEAF tDNSParams[] = {
{"SupportedRecordTypes", &DMREAD, DMT_STRING, get_dns_supported_record_types, NULL, NULL, NULL},
{0}
};

DMLEAF tClientParams[] = {
{"Enable", &DMWRITE, DMT_BOOL, get_client_enable, set_client_enable, NULL, NULL},
{"Status", &DMREAD, DMT_STRING, get_client_status, NULL, NULL, NULL},
{"ServerNumberOfEntries", &DMREAD, DMT_UNINT, get_client_server_number_of_entries, NULL, NULL, NULL},
{0}
};

DMOBJ tClientObj[] = {
{"Server", &DMWRITE, add_client_server, delete_client_server, NULL, browseServerInst, NULL, NULL, NULL, tServerParams, NULL},
{0}
};

DMLEAF tServerParams[] = {
{"Enable", &DMWRITE, DMT_BOOL, get_server_enable, set_server_enable, NULL, NULL},
{"Status", &DMREAD, DMT_STRING, get_server_status, NULL, NULL, NULL},
{"Alias", &DMWRITE, DMT_STRING, get_server_alias, set_server_alias, NULL, NULL},
{"DNSServer", &DMWRITE, DMT_STRING, get_server_dns_server, set_server_dns_server, NULL, NULL},
{"Interface", &DMWRITE, DMT_STRING, get_server_interface, set_server_interface, NULL, NULL},
{"Type", &DMREAD, DMT_STRING, get_server_type, NULL, NULL, NULL},
{0}
};

DMLEAF tRelayParams[] = {
{"Enable", &DMWRITE, DMT_BOOL, get_relay_enable, set_relay_enable, NULL, NULL},
{"Status", &DMREAD, DMT_STRING, get_relay_status, NULL, NULL, NULL},
{"ForwardNumberOfEntries", &DMREAD, DMT_UNINT, get_relay_forward_number_of_entries, NULL, NULL, NULL},
{0}
};

DMOBJ tRelayObj[] = {
{"Forwarding", &DMWRITE, add_relay_forwarding, delete_relay_forwarding, NULL, browseRelayForwardingInst, NULL, NULL, NULL, tForwardingParams, NULL},
{0}
};

DMLEAF tForwardingParams[] = {
{"Enable", &DMWRITE, DMT_BOOL, get_forwarding_enable, set_forwarding_enable, NULL, NULL},
{"Status", &DMREAD, DMT_STRING, get_forwarding_status, NULL, NULL, NULL},
{"Alias", &DMWRITE, DMT_STRING, get_forwarding_alias, set_forwarding_alias, NULL, NULL},
{"DNSServer", &DMWRITE, DMT_STRING, get_forwarding_dns_server, set_forwarding_dns_server, NULL, NULL},
{"Interface", &DMWRITE, DMT_STRING, get_forwarding_interface, set_forwarding_interface, NULL, NULL},
{"Type", &DMREAD, DMT_STRING, get_forwarding_type, NULL, NULL, NULL},
{0}
};

DMOBJ tDiagnosticsObj[] = {
{"NSLookupDiagnostics", &DMREAD, NULL, NULL, NULL, NULL, NULL, NULL, tNSLookupDiagnosticsObj, tNSLookupDiagnosticsParams, NULL},
{0}
};

DMLEAF tNSLookupDiagnosticsParams[] = {
{"DiagnosticsState", &DMWRITE, DMT_STRING, get_nslookupdiagnostics_diagnostics_state, set_nslookupdiagnostics_diagnostics_state, NULL, NULL},
{"Interface", &DMWRITE, DMT_STRING, get_nslookupdiagnostics_interface, set_nslookupdiagnostics_interface, NULL, NULL},
{"HostName", &DMWRITE, DMT_STRING, get_nslookupdiagnostics_host_name, set_nslookupdiagnostics_host_name, NULL, NULL},
{"DNSServer", &DMWRITE, DMT_STRING, get_nslookupdiagnostics_d_n_s_server, set_nslookupdiagnostics_d_n_s_server, NULL, NULL},
{"Timeout", &DMWRITE, DMT_UNINT, get_nslookupdiagnostics_timeout, set_nslookupdiagnostics_timeout, NULL, NULL},
{"NumberOfRepetitions", &DMWRITE, DMT_UNINT, get_nslookupdiagnostics_number_of_repetitions, set_nslookupdiagnostics_number_of_repetitions, NULL, NULL},
{"SuccessCount", &DMREAD, DMT_UNINT, get_nslookupdiagnostics_success_count, NULL, NULL, NULL},
{"ResultNumberOfEntries", &DMREAD, DMT_UNINT, get_nslookupdiagnostics_result_number_of_entries, NULL, NULL, NULL},
{0}
};

DMOBJ tNSLookupDiagnosticsObj[] = {
{"Result", &DMREAD, NULL, NULL, NULL, browseResultInst, NULL, NULL, NULL, tResultParams, NULL},
{0}
};

DMLEAF tResultParams[] = {
{"Status", &DMREAD, DMT_STRING, get_result_status, NULL, NULL, NULL},
{"AnswerType", &DMREAD, DMT_STRING, get_result_answer_type, NULL, NULL, NULL},
{"HostNameReturned", &DMREAD, DMT_STRING, get_result_host_name_returned, NULL, NULL, NULL},
{"IPAddresses", &DMREAD, DMT_STRING, get_result_i_p_addresses, NULL, NULL, NULL},
{"DNSServerIP", &DMREAD, DMT_STRING, get_result_d_n_s_server_i_p, NULL, NULL, NULL},
{"ResponseTime", &DMREAD, DMT_UNINT, get_result_response_time, NULL, NULL, NULL},
{0}
};

/********************************
 * init function
 ********************************/
inline int init_nslookup_results(struct NSLookupResult *args, char *Status, char *AnswerType, char *HostNameReturned, char *IPAddresses, char *DNSServerIP, char *ResponseTime)
{
	args->Status = Status;
	args->AnswerType = AnswerType;
	args->HostNameReturned = HostNameReturned;
	args->IPAddresses = IPAddresses;
	args->DNSServerIP = DNSServerIP;
	args->ResponseTime = ResponseTime;
	return 0;
}

/****************************************************************************************************************************/
static inline char *nslookup_get(char *option, char *def)
{
	char *tmp;
	dmuci_get_varstate_string("cwmp", "@nslookupdiagnostic[0]", option, &tmp);
	if(tmp && tmp[0] == '\0')
		return dmstrdup(def);
	else
		return tmp;
}

static unsigned char is_dns_server_in_dmmap(char *chk_ip, char *chk_interface)
{
	struct uci_section *s = NULL;
	char *ip, *interface;

	uci_path_foreach_sections(icwmpd, "dmmap_dns", "dns_server", s) {
		dmuci_get_value_by_section_string(s, "ip", &ip);
		dmuci_get_value_by_section_string(s, "interface", &interface);
		if (strcmp(interface, chk_interface) == 0 && strcmp(ip, chk_ip) == 0) {
			return 1;
		}
	}
	return 0;
}

static int dmmap_synchronizeDNSClientRelayServer(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	json_object *jobj, *arrobj;
	struct uci_list *v;
	struct uci_element *e;
	struct uci_section *s = NULL, *sdns = NULL, *stmp, *ss;
	char *ipdns, *str, *vip = NULL, *viface, *name;
	int j, found;

	check_create_dmmap_package("dmmap_dns");
	uci_path_foreach_sections_safe(icwmpd, "dmmap_dns", "dns_server", stmp, s) {
		dmuci_get_value_by_section_string(s, "ip", &vip);
		dmuci_get_value_by_section_string(s, "interface", &viface);
		found = 0;
		uci_foreach_sections("network", "interface", ss) {
			if (strcmp(section_name(ss), viface) != 0)
				continue;
			dmuci_get_value_by_section_list(ss, "dns", &v);
			if (v != NULL) {
				uci_foreach_element(v, e) {
					if (strcmp(e->name, vip) == 0) {
						found = 1;
						break;
					}
				}
			}
			if (found)
				break;
			dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", section_name(ss), String}}, 1, &jobj);
			dmjson_foreach_value_in_array(jobj, arrobj, ipdns, j, 1, "dns-server") {
				if (strcmp(ipdns, vip) == 0) {
					found = 1;
					break;
				}
			}
			if (found)
				break;
		}
		if (!found)
			dmuci_delete_by_section(s, NULL, NULL);
	}

	uci_foreach_sections("network", "interface", s) {
		dmuci_get_value_by_section_list(s, "dns", &v);
		if (v != NULL) {
			uci_foreach_element(v, e) {
				if (is_dns_server_in_dmmap(e->name, section_name(s)))
					continue;
				dmuci_add_section_icwmpd("dmmap_dns", "dns_server", &sdns, &name);
				dmuci_set_value_by_section(sdns, "ip", e->name);
				dmuci_set_value_by_section(sdns, "interface", section_name(s));
				dmuci_set_value_by_section(sdns, "enable", "1");
			}
		}
		dmuci_get_value_by_section_string(s, "peerdns", &str);
		if (str[0] == '0')
			continue;
		dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", section_name(s), String}}, 1, &jobj);
		dmjson_foreach_value_in_array(jobj, arrobj, ipdns, j, 1, "dns-server") {
			if (ipdns[0] == '\0' || is_dns_server_in_dmmap(ipdns, section_name(s)))
				continue;
			dmuci_add_section_icwmpd("dmmap_dns", "dns_server", &sdns, &name);
			dmuci_set_value_by_section(sdns, "ip", ipdns);
			dmuci_set_value_by_section(sdns, "interface", section_name(s));
			dmuci_set_value_by_section(sdns, "enable", "1");
			dmuci_set_value_by_section(sdns, "peerdns", "1");
		}
	}
	return 0;
}

/******************************** Browse Functions ****************************************/
int browseServerInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	struct uci_section *s = NULL;
	char *instance, *instnbr = NULL;

	dmmap_synchronizeDNSClientRelayServer(dmctx, NULL, NULL, NULL);
	uci_path_foreach_sections(icwmpd, "dmmap_dns", "dns_server", s)
	{
		instance = handle_update_instance(1, dmctx, &instnbr, update_instance_alias_icwmpd, 3, s, "dns_server_instance", "dns_server_alias");
		if (DM_LINK_INST_OBJ(dmctx, parent_node, (void *)s, instance) == DM_STOP)
			break;

	}
	return 0;
}

int browseRelayForwardingInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	struct uci_section *s = NULL;
	char *instance, *instnbr = NULL;

	dmmap_synchronizeDNSClientRelayServer(dmctx, NULL, NULL, NULL);
	uci_path_foreach_sections(icwmpd, "dmmap_dns", "dns_server", s)
	{
		instance = handle_update_instance(1, dmctx, &instnbr, update_instance_alias_icwmpd, 3, s, "dns_server_instance", "dns_server_alias");
		if (DM_LINK_INST_OBJ(dmctx, parent_node, (void *)s, instance) == DM_STOP)
			break;

	}
	return 0;
}

int browseResultInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	struct NSLookupResult curr_nslookup_results_args = {0};
	FILE* fp = NULL;
	char *Status = NULL, *AnswerType = NULL, *HostNameReturned = NULL, *IPAddresses = NULL, *DNSServerIP = NULL;
	char line[256], *pch, *spch, *tmpPtr, *idx = NULL, *idx_last = NULL, *ResponseTime = NULL;
	int id = 0, is_address_null = 0;

	fp = fopen(NSLOOKUP_LOG_FILE, "r");
	if ( fp != NULL)
	{
		while (fgets(line, 256, fp) != NULL )
		{
			if (line[0] == '\n')
				continue;
			if(strstr(line, "Server:")) {
				strtok_r(line, " \t", &spch);
				pch = strtok_r(NULL, " \t\n\r", &spch);
				if(!strcmp(pch, "0.0.0.0"))
					Status = "Error_DNSServerNotAvailable";
				else
					DNSServerIP = dmstrdup(pch);
			}
			if(strstr(line, "Name:")) {
				strtok_r(line, " \t", &spch);
				pch = strtok_r(NULL, " \t\n\r", &spch);
				HostNameReturned = dmstrdup(pch);
				Status = "Success";
				AnswerType = "Authoritative";
			}
			if(strstr(line, "Address")) {
				if(is_address_null)
					IPAddresses = NULL;
				if(strstr(line, "Address:"))
					continue;
				strtok_r(line, " \t", &spch);
				strtok_r(NULL, " \t", &spch);
				pch = strtok_r(NULL, " \t\n\r", &spch);
				if(IPAddresses == NULL) {
					IPAddresses = dmstrdup(pch);
					is_address_null = 0;
				}
				else {
					tmpPtr = dmstrdup(pch);
					dmasprintf(&IPAddresses, "%s,%s", IPAddresses, tmpPtr);
				}
			}
			if(strstr(line, "ResponseTime:")) {
				strtok_r(line, " \t", &spch);
				pch = strtok_r(NULL, " \t\n\r", &spch);
				ResponseTime = dmstrdup(pch);
			}
			if(strstr(line, "can't resolve")) {
				AnswerType = "None";
				Status = "Error_HostNameNotResolved";
			}
			if(strstr(line, "connection timed out")) {
				AnswerType = "None";
				Status = "Error_Timeout";
			}
			if(strstr(line, "Non-authoritative"))
				AnswerType = "NonAuthoritative";
			if(strstr(line, "******************")) {
				is_address_null = 1;
				init_nslookup_results(&curr_nslookup_results_args, Status, AnswerType, HostNameReturned, IPAddresses, DNSServerIP, ResponseTime);
				idx =  handle_update_instance(1, dmctx, &idx_last, update_instance_without_section, 1, ++id);
				if (DM_LINK_INST_OBJ(dmctx, parent_node, (void *)&curr_nslookup_results_args, idx) == DM_STOP)
					goto end;
			}
		}
		fclose(fp) ;
	}
end:
	return 0;
}

int browseServiceInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	//TODO
	return 0;
}

int browseTextRecordInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	//TODO
	return 0;
}

/*********************************** Add/Delet Object functions *************************/
int add_client_server(char *refparam, struct dmctx *ctx, void *data, char **instance)
{
	struct uci_section *s = NULL;
	char *v, *inst;

	check_create_dmmap_package("dmmap_dns");
	inst = get_last_instance_icwmpd("dmmap_dns", "dns_server", "dns_server_instance");
	dmuci_add_list_value("network", "lan", "dns", "0.0.0.0");
	dmuci_add_section_icwmpd("dmmap_dns", "dns_server", &s, &v);
	dmuci_set_value_by_section(s, "ip", "0.0.0.0");
	dmuci_set_value_by_section(s, "interface", "lan");
	dmuci_set_value_by_section(s, "enable", "1");
	*instance = update_instance_icwmpd(s, inst, "dns_server_instance");
	return 0;
}

int add_relay_forwarding(char *refparam, struct dmctx *ctx, void *data, char **instance)
{
	struct uci_section *s = NULL;
	char *v, *inst;

	check_create_dmmap_package("dmmap_dns");
	inst = get_last_instance_icwmpd("dmmap_dns", "dns_server", "dns_server_instance");
	dmuci_add_list_value("network", "lan", "dns", "0.0.0.0");
	dmuci_add_section_icwmpd("dmmap_dns", "dns_server", &s, &v);
	dmuci_set_value_by_section(s, "ip", "0.0.0.0");
	dmuci_set_value_by_section(s, "interface", "lan");
	dmuci_set_value_by_section(s, "enable", "1");
	*instance = update_instance_icwmpd(s, inst, "dns_server_instance");
	return 0;
}

int delete_client_server(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action)
{
	struct uci_section *s = NULL, *ss = NULL;
	char *interface, *ip, *str, *stmp;
	struct uci_list *v;
	struct uci_element *e, *tmp;

	switch (del_action) {
		case DEL_INST:
			dmuci_get_value_by_section_string((struct uci_section *)data, "peerdns", &str);
			if (str[0] == '1')
				return 0;
			dmuci_get_value_by_section_string((struct uci_section *)data, "interface", &interface);
			dmuci_get_value_by_section_string((struct uci_section *)data, "ip", &ip);
			dmuci_del_list_value("network", interface, "dns", ip);
			dmuci_delete_by_section((struct uci_section *)data, NULL, NULL);
			break;
		case DEL_ALL:
			uci_foreach_sections("network", "interface", s) {
				dmuci_get_value_by_section_string(s, "peerdns", &str);
				if (str[0] == '1')
					continue;
				dmuci_get_value_by_section_list(s, "dns", &v);
				if (v != NULL) {
					uci_foreach_element_safe(v, e, tmp) {
						uci_path_foreach_option_eq_safe(icwmpd, "dmmap_dns", "dns_server", "ip", tmp->name, stmp, ss) {
							dmuci_delete_by_section(ss, NULL, NULL);
						}
						dmuci_del_list_value_by_section(s, "dns", tmp->name);
					}
				}
			}
			break;
	}
	return 0;
}

int delete_relay_forwarding(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action)
{
	struct uci_section *s = NULL, *ss = NULL;
	char *interface, *ip, *str, *stmp;
	struct uci_list *v;
	struct uci_element *e, *tmp;

	switch (del_action) {
		case DEL_INST:
			dmuci_get_value_by_section_string((struct uci_section *)data, "peerdns", str);
			if (str[0] == '1')
				return 0;
			dmuci_get_value_by_section_string((struct uci_section *)data, "interface", interface);
			dmuci_get_value_by_section_string((struct uci_section *)data, "ip", ip);
			dmuci_del_list_value("network", interface, "dns", ip);
			dmuci_delete_by_section((struct uci_section *)data, NULL, NULL);
			break;
		case DEL_ALL:
			uci_foreach_sections("network", "interface", s) {
				dmuci_get_value_by_section_string(s, "peerdns", &str);
				if (str[0] == '1')
					continue;
				dmuci_get_value_by_section_list(s, "dns", &v);
				if (v != NULL) {
					uci_foreach_element_safe(v, e, tmp) {
						uci_path_foreach_option_eq_safe(icwmpd, "dmmap_dns", "dns_server", "ip", tmp->name, stmp, ss) {
							dmuci_delete_by_section(ss, NULL, NULL);
						}
						dmuci_del_list_value_by_section(s, "dns", tmp->name);
					}
				}
			}
			break;
	}
	return 0;
}

/***************************************** Get/Set Parameter functions ***********************/
int get_dns_supported_record_types(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = "A,AAAA,PTR";
	return 0;
}

int get_client_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = "1";
	return 0;
}

int get_client_status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = "Enabled";
	return 0;
}

int get_client_server_number_of_entries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *s = NULL;
	int cnt = 0;

	dmmap_synchronizeDNSClientRelayServer(ctx, NULL, NULL, NULL);
	uci_path_foreach_sections(icwmpd, "dmmap_dns", "dns_server", s)
	{
		cnt++;
	}
	dmasprintf(value, "%d", cnt);
	return 0;
}

int get_server_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
    dmuci_get_value_by_section_string((struct uci_section *)data, "enable", value);
    return 0;
}

int get_server_status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *v;

	dmuci_get_value_by_section_string((struct uci_section *)data, "enable", &v);
	*value = (*v == '1') ? "Enabled" : "Disabled";
	return 0;
}

int get_server_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_value_by_section_string((struct uci_section *)data, "dns_server_alias", value);
	return 0;
}

int get_server_dns_server(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_value_by_section_string((struct uci_section *)data, "ip", value);
	return 0;
}

int get_server_interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *linker;

	dmuci_get_value_by_section_string((struct uci_section *)data, "interface", &linker);
	adm_entry_get_linker_param(ctx, dm_print_path("%s%cIP%cInterface%c", dmroot, dm_delim, dm_delim, dm_delim), linker, value);
	return 0;
}

int get_server_type(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *v;
	*value = "Static";
	dmuci_get_value_by_section_string((struct uci_section *)data, "peerdns", &v);
	if (*v == '1') {
		dmuci_get_value_by_section_string((struct uci_section *)data, "ip", &v);
		if (strchr(v, ':') == NULL) {
			*value = "DHCPv4";
		}
		else {
			*value = "DHCPv6";
		}
	}
	return 0;
}

int get_relay_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *path = "/etc/rc.d/*dnsmasq";
	if (check_file(path))
		*value = "1";
	else
		*value = "0";
	return 0;
}

int get_relay_status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *path = "/etc/rc.d/*dnsmasq";
	if (check_file(path))
		*value = "Enabled";
	else
		*value = "Disabled";
	return 0;
}

int get_relay_forward_number_of_entries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *s = NULL;
	int cnt = 0;

	dmmap_synchronizeDNSClientRelayServer(ctx, NULL, NULL, NULL);
	uci_path_foreach_sections(icwmpd, "dmmap_dns", "dns_server", s)
	{
		cnt++;
	}
	dmasprintf(value, "%d", cnt);
	return 0;
}

int get_forwarding_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
    dmuci_get_value_by_section_string((struct uci_section *)data, "enable", value);
    return 0;
}

int get_forwarding_status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *v;

	dmuci_get_value_by_section_string((struct uci_section *)data, "enable", &v);
	*value = (*v == '1') ? "Enabled" : "Disabled";
	return 0;
}

int get_forwarding_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_value_by_section_string((struct uci_section *)data, "dns_server_alias", value);
	return 0;
}

int get_forwarding_dns_server(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_value_by_section_string((struct uci_section *)data, "ip", value);
	return 0;
}

int get_forwarding_interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *linker;

	dmuci_get_value_by_section_string((struct uci_section *)data, "interface", &linker);
	adm_entry_get_linker_param(ctx, dm_print_path("%s%cIP%cInterface%c", dmroot, dm_delim, dm_delim, dm_delim), linker, value);
	return 0;
}

int get_forwarding_type(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *v;
	*value = "Static";
	dmuci_get_value_by_section_string((struct uci_section *)data, "peerdns", &v);
	if (*v == '1') {
		dmuci_get_value_by_section_string((struct uci_section *)data, "ip", &v);
		if (strchr(v, ':') == NULL) {
			*value = "DHCPv4";
		}
		else {
			*value = "DHCPv6";
		}
	}
	return 0;
}

int get_nslookupdiagnostics_diagnostics_state(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = nslookup_get("DiagnosticState", "None");
	return 0;
}

int get_nslookupdiagnostics_interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_varstate_string("cwmp", "@nslookupdiagnostic[0]", "interface", value);
	return 0;
}

int get_nslookupdiagnostics_host_name(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_varstate_string("cwmp", "@nslookupdiagnostic[0]", "HostName", value);
	return 0;
}

int get_nslookupdiagnostics_d_n_s_server(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_varstate_string("cwmp", "@nslookupdiagnostic[0]", "DNSServer", value);
	return 0;
}

int get_nslookupdiagnostics_timeout(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = nslookup_get("Timeout", "5000");
	return 0;
}

int get_nslookupdiagnostics_number_of_repetitions(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = nslookup_get("NumberOfRepetitions", "1");
	return 0;
}

int get_nslookupdiagnostics_success_count(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	FILE *fp;
	char line[256];
	int cnt = 0, is_true = 0;

	fp = fopen(NSLOOKUP_LOG_FILE, "r");
	if ( fp != NULL) {
		while (fgets(line, 256, fp) != NULL )
		{
			if(!is_true)
				if(strstr(line, "Name:")) {
					cnt++;
					is_true = 1;
				}
			if(strstr(line, "******************"))
				is_true = 0;
		}
		fclose(fp) ;
	}
	dmasprintf(value, "%d", cnt); // MEM WILL BE FREED IN DMMEMCLEAN
	return 0;
}

int get_nslookupdiagnostics_result_number_of_entries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	FILE *fp;
	char line[256];
	int cnt = 0;

	fp = fopen(NSLOOKUP_LOG_FILE, "r");
	if ( fp != NULL) {
		while (fgets(line, 256, fp) != NULL )
		{
			if(strstr(line, "******************"))
				cnt++;
		}
		fclose(fp) ;
	}
	dmasprintf(value, "%d", cnt); // MEM WILL BE FREED IN DMMEMCLEAN
	return 0;
}

int get_result_status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct NSLookupResult *nslookupresultargs = (struct NSLookupResult *)data;
	*value = nslookupresultargs->Status;
	return 0;
}

int get_result_answer_type(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct NSLookupResult *nslookupresultargs = (struct NSLookupResult *)data;
	*value = nslookupresultargs->AnswerType;
	return 0;
}

int get_result_host_name_returned(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct NSLookupResult *nslookupresultargs = (struct NSLookupResult *)data;
	*value = nslookupresultargs->HostNameReturned;
	return 0;
}

int get_result_i_p_addresses(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct NSLookupResult *nslookupresultargs = (struct NSLookupResult *)data;
	*value = nslookupresultargs->IPAddresses;
	return 0;
}

int get_result_d_n_s_server_i_p(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct NSLookupResult *nslookupresultargs = (struct NSLookupResult *)data;
	*value = nslookupresultargs->DNSServerIP;
	return 0;
}

int get_result_response_time(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct NSLookupResult *nslookupresultargs = (struct NSLookupResult *)data;
	*value = nslookupresultargs->ResponseTime;
	return 0;
}

int set_client_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action) {
		case VALUECHECK:
			break;
		case VALUESET:
			break;
	}
	return 0;
}

int set_server_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *str, *ip, *interface;
	bool b, ob;

	switch (action) {
		case VALUECHECK:
			if (string_to_bool(value, &b))
				return FAULT_9007;
			break;
		case VALUESET:
			dmuci_get_value_by_section_string((struct uci_section *)data, "enable", &str);
			string_to_bool(value, &b);
			string_to_bool(str, &ob);
			if (ob == b)
				return 0;
			dmuci_get_value_by_section_string((struct uci_section *)data, "peerdns", &str);
			if (str[0] == '1')
				return 0;
			dmuci_set_value_by_section((struct uci_section *)data, "enable", b ? "1" : "0");
			dmuci_get_value_by_section_string((struct uci_section *)data, "interface", &interface);
			dmuci_get_value_by_section_string((struct uci_section *)data, "ip", &ip);
			if (b == 1) {
				dmuci_add_list_value("network", interface, "dns", ip);
			}
			else {
				dmuci_del_list_value("network", interface, "dns", ip);
			}
			break;
	}
	return 0;
}

int set_server_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action) {
		case VALUECHECK:
			break;
		case VALUESET:
			dmuci_set_value_by_section((struct uci_section *)data, "dns_server_alias", value);
			break;
	}
	return 0;
}

int set_server_dns_server(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *str, *oip, *interface;
	struct uci_list *v;
	struct uci_element *e;
	int count = 0, i = 0;
	char *dns[32] = {0};

	switch (action) {
		case VALUECHECK:
			break;
		case VALUESET:
			dmuci_get_value_by_section_string((struct uci_section *)data, "ip", &oip);
			if (strcmp(oip, value) == 0)
				return 0;
			dmuci_get_value_by_section_string((struct uci_section *)data, "peerdns", &str);
			if (str[0] == '1')
				return 0;
			dmuci_get_value_by_section_string((struct uci_section *)data, "interface", &interface);
			dmuci_get_option_value_list("network", interface, "dns", &v);
			if (v) {
				uci_foreach_element(v, e) {
					if (strcmp(e->name, oip)==0)
						dns[count] = dmstrdup(value);
					else
						dns[count] = dmstrdup(e->name);
					count++;
				}
			}
			dmuci_delete("network", interface, "dns", NULL);
			dmuci_get_value_by_section_string((struct uci_section *)data, "enable", &str);
			if (str[0] == '1') {
				for (i = 0; i < count; i++) {
					dmuci_add_list_value("network", interface, "dns", dns[i] ? dns[i] : "");
					dmfree(dns[i]);
				}
			}
			dmuci_set_value_by_section((struct uci_section *)data, "ip", value);
			break;
	}
	return 0;
}

int set_server_interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *str, *ointerface, *ip, *interface;

	switch (action) {
		case VALUECHECK:
			break;
		case VALUESET:
			dmuci_get_value_by_section_string((struct uci_section *)data, "interface", &ointerface);
			adm_entry_get_linker_value(ctx, value, &interface);
			if (strcmp(ointerface, interface) == 0)
				return 0;
			dmuci_get_value_by_section_string((struct uci_section *)data, "peerdns", &str);
			if (str[0] == '1')
				return 0;
			dmuci_get_value_by_section_string((struct uci_section *)data, "ip", &ip);
			dmuci_del_list_value("network", ointerface, "dns", ip);
			dmuci_get_value_by_section_string((struct uci_section *)data, "enable", &str);
			if (str[0] == '1') {
				dmuci_add_list_value("network", interface, "dns", ip);
			}
			dmuci_set_value_by_section((struct uci_section *)data, "interface", interface);
			break;
	}
	return 0;
}

int set_relay_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	bool b;

	switch (action) {
		case VALUECHECK:
			if (string_to_bool(value, &b))
				return FAULT_9007;
			break;
		case VALUESET:
			string_to_bool(value, &b);
			if(b)
				dmcmd("/etc/init.d/dnsmasq", 1, "enable");
			else
				dmcmd("/etc/init.d/dnsmasq", 1, "disable");
			break;
	}
	return 0;
}

int set_forwarding_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *str, *ip, *interface;
	bool b, ob;

	switch (action) {
		case VALUECHECK:
			if (string_to_bool(value, &b))
				return FAULT_9007;
			break;
		case VALUESET:
			dmuci_get_value_by_section_string((struct uci_section *)data, "enable", &str);
			string_to_bool(value, &b);
			string_to_bool(str, &ob);
			if (ob == b)
				return 0;
			dmuci_get_value_by_section_string((struct uci_section *)data, "peerdns", &str);
			if (str[0] == '1')
				return 0;
			dmuci_set_value_by_section((struct uci_section *)data, "enable", b ? "1" : "0");
			dmuci_get_value_by_section_string((struct uci_section *)data, "interface", &interface);
			dmuci_get_value_by_section_string((struct uci_section *)data, "ip", &ip);
			if (b == 1) {
				dmuci_add_list_value("network", interface, "dns", ip);
			}
			else {
				dmuci_del_list_value("network", interface, "dns", ip);
			}
			break;
	}
	return 0;
}

int set_forwarding_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action) {
		case VALUECHECK:
			break;
		case VALUESET:
			dmuci_set_value_by_section((struct uci_section *)data, "dns_server_alias", value);
			break;
	}
	return 0;
}

int set_forwarding_dns_server(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *str, *oip, *interface;
	struct uci_list *v;
	struct uci_element *e;
	int count = 0, i = 0;
	char *dns[32] = {0};

	switch (action) {
		case VALUECHECK:
			break;
		case VALUESET:
			dmuci_get_value_by_section_string((struct uci_section *)data, "ip", &oip);
			if (strcmp(oip, value) == 0)
				return 0;
			dmuci_get_value_by_section_string((struct uci_section *)data, "peerdns", &str);
			if (str[0] == '1')
				return 0;
			dmuci_get_value_by_section_string((struct uci_section *)data, "interface", &interface);
			dmuci_get_option_value_list("network", interface, "dns", &v);
			if (v) {
				uci_foreach_element(v, e) {
					if (strcmp(e->name, oip)==0)
						dns[count] = dmstrdup(value);
					else
						dns[count] = dmstrdup(e->name);
					count++;
				}
			}
			dmuci_delete("network", interface, "dns", NULL);
			dmuci_get_value_by_section_string((struct uci_section *)data, "enable", &str);
			if (str[0] == '1') {
				for (i = 0; i < count; i++) {
					dmuci_add_list_value("network", interface, "dns", dns[i] ? dns[i] : "");
					dmfree(dns[i]);
				}
			}
			dmuci_set_value_by_section((struct uci_section *)data, "ip", value);
			break;
	}
	return 0;
}

int set_forwarding_interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *str, *ointerface, *ip, *interface;

	switch (action) {
		case VALUECHECK:
			break;
		case VALUESET:
			dmuci_get_value_by_section_string((struct uci_section *)data, "interface", &ointerface);
			adm_entry_get_linker_value(ctx, value, &interface);
			if (strcmp(ointerface, interface) == 0)
				return 0;
			dmuci_get_value_by_section_string((struct uci_section *)data, "peerdns", &str);
			if (str[0] == '1')
				return 0;
			dmuci_get_value_by_section_string((struct uci_section *)data, "ip", &ip);
			dmuci_del_list_value("network", ointerface, "dns", ip);
			dmuci_get_value_by_section_string((struct uci_section *)data, "enable", &str);
			if (str[0] == '1') {
				dmuci_add_list_value("network", interface, "dns", ip);
			}
			dmuci_set_value_by_section((struct uci_section *)data, "interface", interface);
			break;
	}
	return 0;
}

int set_nslookupdiagnostics_diagnostics_state(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *tmp;
	struct uci_section *curr_section = NULL;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			if (strcmp(value, "Requested") == 0) {
				NSLOOKUP_STOP
				curr_section = (struct uci_section *)dmuci_walk_state_section("cwmp", "nslookupdiagnostic", NULL, NULL, CMP_SECTION, NULL, NULL, GET_FIRST_SECTION);
				if(!curr_section)
				{
					dmuci_add_state_section("cwmp", "nslookupdiagnostic", &curr_section, &tmp);
				}
				dmuci_set_varstate_value("cwmp", "@nslookupdiagnostic[0]", "DiagnosticState", value);
				cwmp_set_end_session(END_SESSION_NSLOOKUP_DIAGNOSTIC);
			}
			return 0;
	}
	return 0;
}

int set_nslookupdiagnostics_interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *tmp;
	struct uci_section *curr_section = NULL;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			NSLOOKUP_STOP
			curr_section = dmuci_walk_state_section("cwmp", "nslookupdiagnostic", NULL, NULL, CMP_SECTION, NULL, NULL, GET_FIRST_SECTION);
			if(!curr_section)
			{
				dmuci_add_state_section("cwmp", "nslookupdiagnostic", &curr_section, &tmp);
			}
			dmuci_set_varstate_value("cwmp", "@nslookupdiagnostic[0]", "interface", value);
			return 0;
	}
	return 0;
}

int set_nslookupdiagnostics_host_name(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *tmp;
	struct uci_section *curr_section = NULL;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			NSLOOKUP_STOP
			curr_section = dmuci_walk_state_section("cwmp", "nslookupdiagnostic", NULL, NULL, CMP_SECTION, NULL, NULL, GET_FIRST_SECTION);
			if(!curr_section)
			{
				dmuci_add_state_section("cwmp", "nslookupdiagnostic", &curr_section, &tmp);
			}
			dmuci_set_varstate_value("cwmp", "@nslookupdiagnostic[0]", "HostName", value);
			return 0;
	}
	return 0;
}

int set_nslookupdiagnostics_d_n_s_server(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *tmp;
	struct uci_section *curr_section = NULL;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			NSLOOKUP_STOP
			curr_section = dmuci_walk_state_section("cwmp", "nslookupdiagnostic", NULL, NULL, CMP_SECTION, NULL, NULL, GET_FIRST_SECTION);
			if(!curr_section)
			{
				dmuci_add_state_section("cwmp", "nslookupdiagnostic", &curr_section, &tmp);
			}
			dmuci_set_varstate_value("cwmp", "@nslookupdiagnostic[0]", "DNSServer", value);
			return 0;
	}
	return 0;
}

int set_nslookupdiagnostics_timeout(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *tmp;
	struct uci_section *curr_section = NULL;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			NSLOOKUP_STOP
			curr_section = dmuci_walk_state_section("cwmp", "nslookupdiagnostic", NULL, NULL, CMP_SECTION, NULL, NULL, GET_FIRST_SECTION);
			if(!curr_section)
			{
				dmuci_add_state_section("cwmp", "nslookupdiagnostic", &curr_section, &tmp);
			}
			dmuci_set_varstate_value("cwmp", "@nslookupdiagnostic[0]", "Timeout", value);
			return 0;
	}
	return 0;
}

int set_nslookupdiagnostics_number_of_repetitions(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *tmp;
	struct uci_section *curr_section = NULL;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			NSLOOKUP_STOP
			curr_section = dmuci_walk_state_section("cwmp", "nslookupdiagnostic", NULL, NULL, CMP_SECTION, NULL, NULL, GET_FIRST_SECTION);
			if(!curr_section)
			{
				dmuci_add_state_section("cwmp", "nslookupdiagnostic", &curr_section, &tmp);
			}
			dmuci_set_varstate_value("cwmp", "@nslookupdiagnostic[0]", "NumberOfRepetitions", value);
			return 0;
	}
	return 0;
}


