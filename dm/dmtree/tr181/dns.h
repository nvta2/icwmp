/*
*      This program is free software: you can redistribute it and/or modify
*      it under the terms of the GNU General Public License as published by
*      the Free Software Foundation, either version 2 of the License, or
*      (at your option) any later version.
*
*	Copyright (C) 2019 iopsys Software Solutions AB
*		Author: Amin Ben Ramdhane <amin.benramdhane@pivasoftware.com>
*/

#ifndef _DNS_H
#define _DNS_H

#include "dmcwmp.h"


extern DMOBJ tDNSObj[];
extern DMLEAF tDNSParams[];
extern DMLEAF tClientParams[];
extern DMOBJ tClientObj[];
extern DMLEAF tServerParams[];
extern DMLEAF tRelayParams[];
extern DMOBJ tRelayObj[];
extern DMLEAF tForwardingParams[];
extern DMOBJ tDiagnosticsObj[];
extern DMLEAF tNSLookupDiagnosticsParams[];
extern DMOBJ tNSLookupDiagnosticsObj[];
extern DMLEAF tResultParams[];
extern DMLEAF tSDParams[];
extern DMOBJ tSDObj[];
extern DMLEAF tServiceParams[];
extern DMOBJ tSDServiceObj[];
extern DMLEAF tTextRecordParams[];

struct NSLookupResult
{
	char *Status;
	char *AnswerType;
	char *HostNameReturned;
	char *IPAddresses;
	char *DNSServerIP;
	char *ResponseTime;
};

int browseServerInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);
int browseRelayForwardingInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);
int browseResultInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);
int browseServiceInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);
int browseTextRecordInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);

int add_client_server(char *refparam, struct dmctx *ctx, void *data, char **instance);
int add_relay_forwarding(char *refparam, struct dmctx *ctx, void *data, char **instance);

int delete_client_server(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action);
int delete_relay_forwarding(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action);

int get_dns_supported_record_types(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_client_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_client_status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_client_server_number_of_entries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_server_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_server_status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_server_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_server_dns_server(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_server_interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_server_type(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_relay_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_relay_status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_relay_forward_number_of_entries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_forwarding_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_forwarding_status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_forwarding_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_forwarding_dns_server(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_forwarding_interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_forwarding_type(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_nslookupdiagnostics_diagnostics_state(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_nslookupdiagnostics_interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_nslookupdiagnostics_host_name(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_nslookupdiagnostics_d_n_s_server(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_nslookupdiagnostics_timeout(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_nslookupdiagnostics_number_of_repetitions(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_nslookupdiagnostics_success_count(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_nslookupdiagnostics_result_number_of_entries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_result_status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_result_answer_type(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_result_host_name_returned(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_result_i_p_addresses(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_result_d_n_s_server_i_p(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_result_response_time(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_sd_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_sd_service_number_of_entries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_sd_advertised_interfaces(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_service_instance_name(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_service_application_protocol(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_service_transport_protocol(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_service_domain(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_service_port(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_service_target(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_service_status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_service_last_update(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_service_host(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_service_time_to_live(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_service_priority(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_service_weight(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_service_text_record_number_of_entries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_textrecord_key(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_textrecord_value(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);

int set_client_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_server_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_server_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_server_dns_server(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_server_interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_relay_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_forwarding_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_forwarding_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_forwarding_dns_server(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_forwarding_interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_nslookupdiagnostics_diagnostics_state(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_nslookupdiagnostics_interface(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_nslookupdiagnostics_host_name(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_nslookupdiagnostics_d_n_s_server(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_nslookupdiagnostics_timeout(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_nslookupdiagnostics_number_of_repetitions(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_sd_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_sd_advertised_interfaces(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);

#endif
