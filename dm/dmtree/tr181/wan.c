/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2016 PIVA SOFTWARE (www.pivasoftware.com)
 *		Author: Anis Ellouze <anis.ellouze@pivasoftware.com>
 *
 */
#include <ctype.h>
#include <uci.h>
#include "dmcwmp.h"
#include "dmuci.h"
#include "dmubus.h"
#include "dmcommon.h"
#include "wan.h"
#include "dmjson.h"

struct dsl_line_args cur_dsl_line_args = {0};
struct dsl_channel_args cur_dsl_channel_args = {0};
struct atm_args cur_atm_args = {0};
struct ptm_args cur_ptm_args = {0};

inline int init_dsl_link(struct dmctx *ctx, struct uci_section *s, char *type)
{
	struct dsl_line_args *args = &cur_dsl_line_args;
	ctx->args = (void *)args;
	args->line_sec = s;
	args->type = type;
	return 0;
}

inline int init_atm_link(struct dmctx *ctx, struct uci_section *s, char *ifname)
{
	struct atm_args *args = &cur_atm_args;
	ctx->args = (void *)args;
	args->atm_sec = s;
	args->ifname = ifname;
	return 0;
}

inline int init_ptm_link(struct dmctx *ctx, struct uci_section *s, char *ifname)
{
	struct ptm_args *args = &cur_ptm_args;
	ctx->args = (void *)args;
	args->ptm_sec = s;
	args->ifname = ifname;
	return 0;
}

/**************************************************************************
* SET & GET DSL LINK PARAMETERS
***************************************************************************/

int get_dsl_status(char *refparam, struct dmctx *ctx, char **value)
{
	char *status;
	json_object *res;

	*value = "Down";
	if(strcmp(cur_dsl_line_args.type, "adsl") == 0 && check_dsl_link_type() == 1 || strcmp(cur_dsl_line_args.type, "vdsl") == 0 && check_dsl_link_type() == 0) {
		dmubus_call("router.dsl", "stats", UBUS_ARGS{}, 0, &res);
		DM_ASSERT(res, *value = "");
		status = dmjson_get_value(res, 2, "dslstats", "status");
		if (strcmp(status, "Showtime") == 0)
			*value = "Up";
		else
			*value = "Down";
	}
	return 0;
}

int get_dsl_link_status(char *refparam, struct dmctx *ctx, char **value)
{
	char *status;
	json_object *res;

	*value = "NoSignal";
	if(strcmp(cur_dsl_line_args.type, "adsl") == 0 && check_dsl_link_type() == 1 || strcmp(cur_dsl_line_args.type, "vdsl") == 0 && check_dsl_link_type() == 0) {
		dmubus_call("router.dsl", "stats", UBUS_ARGS{}, 0, &res);
		DM_ASSERT(res, *value = "");
		status = dmjson_get_value(res, 2, "dslstats", "status");
		if (strcmp(status, "Showtime") == 0)
			*value = "Up";
		else if (strcmp(status, "Training") == 0)
			*value = "Initializing";
		else if (strcmp(status, "Channel Analysis") == 0)
			*value = "EstablishingLink";
		else if (strcmp(status, "Disabled") == 0)
			*value = "Disabled";
		else
			*value = "NoSignal";
	}
	return 0;
}

int get_dsl_link_supported_standard(char *refparam, struct dmctx *ctx, char **value)
{
	*value = "G.992.1_Annex_A, G.992.2, T1.413, G.992.3_Annex_A, G.992.3_Annex_L, G.992.5_Annex_A, G.992.5_Annex_M";
	return 0;
}

int get_dsl_link_standard_inuse(char *refparam, struct dmctx *ctx, char **value)
{
	char *mode;
	json_object *res = NULL;

	dmubus_call("router.dsl", "stats", UBUS_ARGS{}, 0, &res);
	DM_ASSERT(res, *value = "");
	mode = dmjson_get_value(res, 2, "dslstats", "mode");
	if (strcmp(mode, "G.Dmt") == 0)
		*value = "G.992.1_Annex_A"; // TO CHECK
	else if (strcmp(mode, "G.lite") == 0)
		*value = "G.992.2";
	else if (strcmp(mode, "T1.413") == 0)
		*value = "T1.413";
	else if (strcmp(mode, "ADSL2") == 0)
		*value = "G.992.3_Annex_A";
	else if (strcmp(mode, "AnnexL") == 0)
		*value = "G.992.3_Annex_L";
	else if (strcmp(mode, "ADSL2+") == 0)
		*value = "G.992.5_Annex_A";
	else
		*value = mode;
	return 0;
}

int check_dsl_link_type()
{
	char *mode, *status, *trafic;
	json_object *res = NULL;
	dmubus_call("router.dsl", "stats", UBUS_ARGS{}, 0, &res);
	DM_ASSERT(res, status = "");
	status = dmjson_get_value(res, 2, "dslstats", "status");
	trafic = dmjson_get_value(res, 2, "dslstats", "traffic");
	if (strcmp(status, "Showtime") == 0 && strstr(trafic, "ATM"))
		return 1;
	return 0;
}
int get_vdsl_link_supported_profile(char *refparam, struct dmctx *ctx, char **value)
{
	*value = "8a, 8b, 8c, 8d, 12a, 12b, 17a, 17b, 30a";
	return 0;
}

int get_vdsl_link_profile_inuse(char *refparam, struct dmctx *ctx, char **value)
{
	char *mode;
	json_object *res = NULL;

	dmubus_call("router.dsl", "stats", UBUS_ARGS{}, 0, &res);
	DM_ASSERT(res, *value = "");
	mode = dmjson_get_value(res, 2, "dslstats", "mode");
	*value = mode;
	return 0;
}

int get_dsl_link_downstreammaxrate(char *refparam, struct dmctx *ctx, char **value)
{
	char *max_down;
	json_object *res = NULL;
	json_object *sub_obj = NULL;
	json_object *sub_obj_2 = NULL;
	*value = "";
	if(strcmp(cur_dsl_line_args.type, "adsl") == 0 && check_dsl_link_type() == 1 || strcmp(cur_dsl_line_args.type, "vdsl") == 0 && check_dsl_link_type() == 0) {
		dmubus_call("router.dsl", "stats", UBUS_ARGS{}, 0, &res);
		DM_ASSERT(res, *value = "0");
		sub_obj = dmjson_get_obj(res, 1, "dslstats");
		if (sub_obj)
		{
			sub_obj_2 = dmjson_select_obj_in_array_idx(sub_obj, 0, 1, "bearers");
			max_down = dmjson_get_value(sub_obj_2, 1, "max_rate_down");
		}
		else
			return 0;
		if (max_down && max_down[0] != '\0') {
			*value = max_down;
		}
	}
	return 0;
}

int get_dsl_link_downstreamattenuation(char *refparam, struct dmctx *ctx, char **value)
{
	char *attn_down_x100;
	json_object *res = NULL;
	*value = "";
	if(strcmp(cur_dsl_line_args.type, "adsl") == 0 && check_dsl_link_type() == 1 || strcmp(cur_dsl_line_args.type, "vdsl") == 0 && check_dsl_link_type() == 0) {
		dmubus_call("router.dsl", "stats", UBUS_ARGS{}, 0, &res);
		DM_ASSERT(res, *value = "0");
		attn_down_x100 = dmjson_get_value(res, 2, "dslstats", "attn_down_x100");
		if (attn_down_x100) {
			dmasprintf(&attn_down_x100, "%d", (atoi(attn_down_x100) / 10));// MEM WILL BE FREED IN DMMEMCLEAN
			*value = attn_down_x100;
		}
	}
	return 0;
}

int get_dsl_link_downstreamnoisemargin(char *refparam, struct dmctx *ctx, char **value)
{
	char *snr_down_x100;
	json_object *res;
	*value = "";
	if(strcmp(cur_dsl_line_args.type, "adsl") == 0 && check_dsl_link_type() == 1 || strcmp(cur_dsl_line_args.type, "vdsl") == 0 && check_dsl_link_type() == 0) {
		dmubus_call("router.dsl", "stats", UBUS_ARGS{}, 0, &res);
		DM_ASSERT(res, *value = "0");
		snr_down_x100 = dmjson_get_value(res, 2, "dslstats", "snr_down_x100");
		if (snr_down_x100) {
			dmasprintf(&snr_down_x100, "%d", (atoi(snr_down_x100) / 10));// MEM WILL BE FREED IN DMMEMCLEAN
			*value = snr_down_x100;
		}
	}
	return 0;
}

int get_dsl_link_upstreammaxrate(char *refparam, struct dmctx *ctx, char **value)
{
	char *max_up;
	json_object *res = NULL;
	json_object *sub_obj = NULL;
	json_object *sub_obj_2 = NULL;
	*value = "";
	if(strcmp(cur_dsl_line_args.type, "adsl") == 0 && check_dsl_link_type() == 1 || strcmp(cur_dsl_line_args.type, "vdsl") == 0 && check_dsl_link_type() == 0) {
		dmubus_call("router.dsl", "stats", UBUS_ARGS{}, 0, &res);
		DM_ASSERT(res, *value = "0");
		sub_obj = dmjson_get_obj(res, 1, "dslstats");
		if (sub_obj)
		{
			sub_obj_2 = dmjson_select_obj_in_array_idx(sub_obj, 0, 1, "bearers");
			max_up = dmjson_get_value(sub_obj_2, 1, "max_rate_up");
		}
		else
			return 0;
		*value = max_up;
	}
	return 0;
}

int get_dsl_link_upstreamattenuation(char *refparam, struct dmctx *ctx, char **value)
{
	char *attn_up_x100;
	json_object *res = NULL;
	*value = "";
	if(strcmp(cur_dsl_line_args.type, "adsl") == 0 && check_dsl_link_type() == 1 || strcmp(cur_dsl_line_args.type, "vdsl") == 0 && check_dsl_link_type() == 0) {
		dmubus_call("router.dsl", "stats", UBUS_ARGS{}, 0, &res);
		DM_ASSERT(res, *value = "0");
		attn_up_x100 = dmjson_get_value(res, 2, "dslstats", "attn_up_x100");
		if (attn_up_x100) {
			dmasprintf(&attn_up_x100, "%d", (atoi(attn_up_x100) / 10)); // MEM WILL BE FREED IN DMMEMCLEAN
			*value = attn_up_x100;
		}
	}
	return 0;
}

int get_dsl_link_upstreamnoisemargin(char *refparam, struct dmctx *ctx, char **value)
{
	char *snr_up_x100;
	json_object *res;
	*value = "";
	if(strcmp(cur_dsl_line_args.type, "adsl") == 0 && check_dsl_link_type() == 1 || strcmp(cur_dsl_line_args.type, "vdsl") == 0 && check_dsl_link_type() == 0) {
		dmubus_call("router.dsl", "stats", UBUS_ARGS{}, 0, &res);
		DM_ASSERT(res, *value = "0");
		snr_up_x100 = dmjson_get_value(res, 2, "dslstats", "snr_up_x100");
		if (snr_up_x100) {
			dmasprintf(&snr_up_x100, "%d", (atoi(snr_up_x100) / 10));// MEM WILL BE FREED IN DMMEMCLEAN
			*value = snr_up_x100;
		}
		else {
			*value = "0";
		}
	}
	return 0;
}

int get_channel_lower_layer(char *refparam, struct dmctx *ctx, char **value)
{
	adm_entry_get_linker_param(DMROOT"DSL.Line.", cur_dsl_line_args.type, value); // MEM WILL BE FREED IN DMMEMCLEAN
	if (*value == NULL)
		*value = "";
	return 0;
}

int get_dsl_channel_downstreamcurrrate(char *refparam, struct dmctx *ctx, char **value)
{
	char *rate_down;
	json_object *res = NULL;
	json_object *sub_obj= NULL;
	json_object *sub_obj_2= NULL;

	dmubus_call("router.dsl", "stats", UBUS_ARGS{}, 0, &res);
	DM_ASSERT(res, *value = "0");
	sub_obj = dmjson_get_obj(res, 1, "dslstats");
	if (sub_obj)
	{
		sub_obj_2 = dmjson_select_obj_in_array_idx(sub_obj, 0, 1, "bearers");
		rate_down = dmjson_get_value(sub_obj_2, 1, "rate_down");
	}
	else
		return 0;
	if (rate_down && rate_down[0] != '\0') {
		*value = rate_down;
	}
	return 0;
}

int get_dsl_channel_upstreamcurrrate(char *refparam, struct dmctx *ctx, char **value)
{
	char *rate_up;
	json_object *res = NULL;
	json_object *sub_obj = NULL;
	json_object *sub_obj_2 = NULL;

	dmubus_call("router.dsl", "stats", UBUS_ARGS{}, 0, &res);
	DM_ASSERT(res, *value = "0");
	sub_obj = dmjson_get_obj(res, 1, "dslstats");
	if (sub_obj)
	{
		sub_obj_2 = dmjson_select_obj_in_array_idx(sub_obj, 0, 1, "bearers");
		rate_up = dmjson_get_value(sub_obj_2, 1, "rate_up");
	}
	else
		return 0;
	*value = rate_up;
	return 0;
}

int get_channel_annexm_status(char *refparam, struct dmctx *ctx, char **value)
{
	char *val = "0";
	*value = "";
	if (strcmp(cur_dsl_line_args.type, "adsl") == 0) {
		dmuci_get_option_value_string("layer2_interface", "capabilities", "AnnexM", &val);
		if (val[0] != '\0') {
			if (strcasecmp(val, "enabled") == 0) {
				*value = "1";
				return 0;
			}
		}
	}
	return 0;
}

int set_channel_annexm_status(char *refparam, struct dmctx *ctx, int action, char *value)
{
	bool b;

	struct wanargs *wandargs = (struct wanargs *)ctx->args;

	switch (action) {
		case VALUECHECK:
			if (string_to_bool(value, &b))
				return FAULT_9007;
			return 0;
		case VALUESET:
			string_to_bool(value, &b);
			if(b) {
				dmuci_set_value("layer2_interface", "capabilities", "AnnexM", "Enabled");
			}
			else
				dmuci_set_value("layer2_interface", "capabilities", "AnnexM", "");
			return 0;
	}
	return 0;
}

int get_channel_supported_encap(char *refparam, struct dmctx *ctx, char **value)
{
	*value = "G.992.3_Annex_K_ATM, G.992.3_Annex_K_PTM, G.993.2_Annex_K_ATM, G.993.2_Annex_K_PTM, G.994.1";
	return 0;
}


int get_atm_destination_address(char *refparam, struct dmctx *ctx, char **value)
{
	char *vpi, *vci;

	dmuci_get_value_by_section_string(cur_atm_args.atm_sec, "vpi", &vpi);
	dmuci_get_value_by_section_string(cur_atm_args.atm_sec, "vci", &vci);
	dmasprintf(value, "PVC: %s/%s", vpi, vci); // MEM WILL BE FREED IN DMMEMCLEAN
	return 0;
}

int set_atm_destination_address(char *refparam, struct dmctx *ctx, int action, char *value)
{
	char *vpi = NULL, *vci = NULL, *spch, *val;
	struct uci_section *s;
	struct wancdevargs *wandcdevargs = (struct wancdevargs *)ctx->args;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
				if (strstr(value, "PVC: "))
					value += 5;
				else
					return 0;
				val = dmstrdup(value);
				vpi = strtok_r(val, "/", &spch);
				if (vpi) {
					vci = strtok_r(NULL, "/", &spch);
				}
				if (vpi && vci) {
					dmuci_set_value_by_section(cur_atm_args.atm_sec, "vpi", vpi);
					dmuci_set_value_by_section(cur_atm_args.atm_sec, "vci", vci);
				}
				dmfree(val);
				break;
			return 0;
	}
	return 0;
}

int get_atm_link_name(char *refparam, struct dmctx *ctx, char **value)
{
	dmuci_get_value_by_section_string(cur_atm_args.atm_sec, "name", value);
	return 0;
}

int get_ptm_link_name(char *refparam, struct dmctx *ctx, char **value)
{
	dmuci_get_value_by_section_string(cur_ptm_args.ptm_sec, "name", value);
	return 0;
}


int get_atm_encapsulation(char *refparam, struct dmctx *ctx, char **value)
{
	char *type, *encapsulation;
	dmuci_get_value_by_section_string(cur_atm_args.atm_sec,"link_type", &type);
	if (strcmp(type, "EoA") == 0 ) {
		type = "encapseoa";
	} else if (strcmp(type, "PPPoA") == 0) {
		type = "encapspppoa";
	} else if (strcmp(type, "IPoA") == 0) {
		type = "encapsipoa";
	}
	dmuci_get_value_by_section_string(cur_atm_args.atm_sec, type, &encapsulation);
	if (strstr(encapsulation, "vcmux")) {
		*value = "VCMUX";
	}
	else if (strstr(encapsulation, "llc")) {
		*value = "LLC";
	} else {
		*value = "";
	}
	return 0;
}

int set_atm_encapsulation(char *refparam, struct dmctx *ctx, int action, char *value)
{
	int i;
	struct uci_section *s;
	char *type, *encapsulation, *encaptype, *pch;
	struct wancdevargs *wandcdevargs = (struct wancdevargs *)ctx->args;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_get_value_by_section_string(cur_atm_args.atm_sec, "link_type", &type);
			int enc;
			if (strstr(value, "VCMUX")) {
				enc = 1;
			}
			else if (strstr(value, "LLC")) {
				enc = 0;
			}
			else
				return 0;
			if (strstr(type, "EoA")) {
				encaptype = "encapseoa";
				encapsulation = enc ? "vcmux_eth" : "llcsnap_eth";
			}
			else if (strstr(type, "PPPoA")) {
				encaptype = "encapspppoa";
				encapsulation = enc ? "vcmux_pppoa" : "llcencaps_ppp";
			}
			else if (strstr(type, "IPoA")) {
				encaptype = "encapsipoa";
				encapsulation = enc ? "vcmux_ipoa" : "llcsnap_rtip";
			}
			else
				return 0;

			dmuci_set_value_by_section(cur_atm_args.atm_sec, encaptype, encapsulation);
			return 0;
	}
	return 0;
}


int get_atm_link_type(char *refparam, struct dmctx *ctx, char **value)
{
	*value = "";
	dmuci_get_value_by_section_string(cur_atm_args.atm_sec, "link_type", value);
	return 0;
}

int set_atm_link_type(char *refparam, struct dmctx *ctx, int action, char *value)
{
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(cur_atm_args.atm_sec, "link_type", value);
			return 0;
	}
	return 0;
}

int get_atm_lower_layer(char *refparam, struct dmctx *ctx, char **value)
{
	char *linker = "adsl";
	adm_entry_get_linker_param(DMROOT"DSL.Channel.", linker, value); // MEM WILL BE FREED IN DMMEMCLEAN
	if (*value == NULL)
		*value = "";
	return 0;
}

int get_ptm_lower_layer(char *refparam, struct dmctx *ctx, char **value)
{
	char *linker = "vdsl";
	adm_entry_get_linker_param(DMROOT"DSL.Channel.", linker, value); // MEM WILL BE FREED IN DMMEMCLEAN
	if (*value == NULL)
		*value = "";
	return 0;
}


inline int ubus_atm_stats(json_object *res, char **value, char *stat_mod)
{

	dmubus_call("network.device", "status", UBUS_ARGS{{"name", cur_atm_args.ifname, String}}, 1, &res);
	DM_ASSERT(res, *value = "");
	*value = dmjson_get_value(res, 2, "statistics", stat_mod);
	return 0;
}

int get_atm_stats_bytes_received(char *refparam, struct dmctx *ctx, char **value)
{
	json_object *res;
	ubus_atm_stats(res, value, "rx_bytes");
	return 0;
}

int get_atm_stats_bytes_sent(char *refparam, struct dmctx *ctx, char **value)
{
	json_object *res;
	ubus_atm_stats(res, value, "tx_bytes");
	return 0;
}

int get_atm_stats_pack_received(char *refparam, struct dmctx *ctx, char **value)
{
	json_object *res;
	ubus_atm_stats(res, value, "rx_packets");
	return 0;
}

int get_atm_stats_pack_sent(char *refparam, struct dmctx *ctx, char **value)
{
	json_object *res;
	ubus_atm_stats(res, value, "tx_packets");
	return 0;
}

inline int ubus_ptm_stats(json_object *res, char **value, char *stat_mod)
{

	dmubus_call("network.device", "status", UBUS_ARGS{{"name", cur_ptm_args.ifname, String}}, 1, &res);
	DM_ASSERT(res, *value = "");
	*value = dmjson_get_value(res, 2, "statistics", stat_mod);
	return 0;
}

int get_ptm_stats_bytes_received(char *refparam, struct dmctx *ctx, char **value)
{
	json_object *res;
	ubus_ptm_stats(res, value, "rx_bytes");
	return 0;
}

int get_ptm_stats_bytes_sent(char *refparam, struct dmctx *ctx, char **value)
{
	json_object *res;
	ubus_ptm_stats(res, value, "tx_bytes");
	return 0;
}

int get_ptm_stats_pack_received(char *refparam, struct dmctx *ctx, char **value)
{
	json_object *res;
	ubus_ptm_stats(res, value, "rx_packets");
	return 0;
}

int get_ptm_stats_pack_sent(char *refparam, struct dmctx *ctx, char **value)
{
	json_object *res;
	ubus_ptm_stats(res, value, "tx_packets");
	return 0;
}

int get_line_name(char *refparam, struct dmctx *ctx, char **value)
{
	*value = dmstrdup(cur_dsl_line_args.type);
	return 0;
}

int get_atm_enable(char *refparam, struct dmctx *ctx, char **value)
{
	*value = "true";
	return 0;
}

int get_ptm_enable(char *refparam, struct dmctx *ctx, char **value)
{
	*value = "true";
	return 0;
}
/*************************************************************
 * ADD OBJ
/*************************************************************/
int add_atm_link(struct dmctx *ctx, char **instancepara)
{
	int idx;
	char *value;
	char *instance;
	char ifname[16] = {0};
	char buf[16] = {0};
	struct uci_section *s = NULL;

	idx = get_cfg_layer2idx("layer2_interface_adsl", "atm_bridge", "baseifname", sizeof("atm")-1);
	sprintf(buf, "atm%d",idx);
	sprintf(ifname,"%s.1",buf);
	instance = get_last_instance("layer2_interface_adsl", "atm_bridge", "atmlinkinstance");
	dmuci_add_section("layer2_interface_adsl", "atm_bridge", &s, &value);
	dmuci_set_value_by_section(s, "baseifname", buf);
	dmuci_set_value_by_section(s, "bridge", "0");
	dmuci_set_value_by_section(s, "encapseoa", "llcsnap_eth");
	dmuci_set_value_by_section(s, "ifname", ifname);
	dmuci_set_value_by_section(s, "link_type", "EoA");
	dmuci_set_value_by_section(s, "unit", buf + 3);
	dmuci_set_value_by_section(s, "vci", "35");
	dmuci_set_value_by_section(s, "vpi", "8");
	*instancepara = update_instance(s, instance, "atmlinkinstance");
	return 0;
}

int add_ptm_link(struct dmctx *ctx, char **instancepara)
{
	int idx;
	char *value;
	char *instance;
	char ifname[16] = {0};
	char buf[16] = {0};
	struct uci_section *s = NULL;

	idx = get_cfg_layer2idx("layer2_interface_vdsl", "vdsl_interface", "baseifname", sizeof("ptm")-1);
	sprintf(buf,"ptm%d", idx);
	sprintf(ifname,"%s.1",buf);
	instance = get_last_instance("layer2_interface_vdsl", "vdsl_interface", "ptmlinkinstance");
	dmuci_add_section("layer2_interface_vdsl", "vdsl_interface", &s, &value);
	dmuci_set_value_by_section(s, "baseifname", buf);
	dmuci_set_value_by_section(s, "bridge", "0");
	dmuci_set_value_by_section(s, "ifname", ifname);
	dmuci_set_value_by_section(s, "unit", buf + 3);
	*instancepara = update_instance(s, instance, "ptmlinkinstance");
	return 0;
}

int delete_atm_link_all(struct dmctx *ctx)
{
	struct uci_section *s = NULL;
	struct uci_section *ss = NULL;

	struct uci_section *ns = NULL;
	struct uci_section *nss = NULL;
	char *ifname;
	uci_foreach_sections("layer2_interface_adsl", "atm_bridge", s) {
		if (ss){
			dmuci_get_value_by_section_string(ss, "ifname", &ifname);
			dmuci_delete_by_section(ss, NULL, NULL);
			uci_foreach_option_cont("network", "interface", "ifname", ifname, ns) {
				if (nss)
					wan_remove_dev_interface(nss, ifname);
				nss = ns;
			}
			if (nss != NULL)
				wan_remove_dev_interface(nss, ifname);
		}
		ss = s;
	}
	if (ss != NULL) {
		dmuci_get_value_by_section_string(ss, "ifname", &ifname);
		dmuci_delete_by_section(ss, NULL, NULL);

		uci_foreach_option_cont("network", "interface", "ifname", ifname, ns) {
			if (nss)
				wan_remove_dev_interface(nss, ifname);
			nss = ns;
	}
		if (nss != NULL)
			wan_remove_dev_interface(nss, ifname);
	}	return 0;
}

int delete_atm_link(struct dmctx *ctx)
{
	struct uci_section *s = NULL;
	struct uci_section *ss = NULL;
	struct uci_section *ns = NULL;
	struct uci_section *nss = NULL;
	char *ifname;

	dmuci_delete_by_section(cur_atm_args.atm_sec, NULL, NULL);
	uci_foreach_option_cont("network", "interface", "ifname", cur_atm_args.ifname, s) {
		if (ss)
			wan_remove_dev_interface(ss, cur_atm_args.ifname);
		ss = s;
	}
	if (ss != NULL)
		wan_remove_dev_interface(ss, cur_atm_args.ifname);
	return 0;
}

int delete_ptm_link_all(struct dmctx *ctx)
{
	struct uci_section *s = NULL;
	struct uci_section *ss = NULL;
	struct uci_section *ns = NULL;
	struct uci_section *nss = NULL;
	char *ifname;

	uci_foreach_sections("layer2_interface_vdsl", "vdsl_interface", s) {
		if (ss){
			dmuci_get_value_by_section_string(ss, "ifname", &ifname);
			dmuci_delete_by_section(ss, NULL, NULL);
			uci_foreach_option_cont("network", "interface", "ifname", ifname, ns) {
				if (nss)
					wan_remove_dev_interface(nss, ifname);
				nss = ns;
			}
			if (nss != NULL)
				wan_remove_dev_interface(nss, ifname);
		}
		ss = s;
	}
	if (ss != NULL) {
		dmuci_get_value_by_section_string(ss, "ifname", &ifname);
		dmuci_delete_by_section(ss, NULL, NULL);

		uci_foreach_option_cont("network", "interface", "ifname", ifname, ns) {
			if (nss)
				wan_remove_dev_interface(nss, ifname);
			nss = ns;
	}
		if (nss != NULL)
			wan_remove_dev_interface(nss, ifname);
	}
	return 0;
}

int delete_ptm_link(struct dmctx *ctx)
{
	struct uci_section *s = NULL;
	struct uci_section *ss = NULL;

	dmuci_delete_by_section(cur_ptm_args.ptm_sec, NULL, NULL);
	uci_foreach_option_cont("network", "interface", "ifname", cur_ptm_args.ifname, s) {
		if (ss)
			wan_remove_dev_interface(ss, cur_ptm_args.ifname);
		ss = s;
	}
	if (ss != NULL)
		wan_remove_dev_interface(ss, cur_ptm_args.ifname);
	return 0;
}
/*************************************************************
 * SET AND GET ALIAS
/*************************************************************/
int get_dsl_link_alias(char *refparam, struct dmctx *ctx, char **value)
{
	dmuci_get_value_by_section_string(cur_dsl_line_args.line_sec, "dsllinkalias", value);
	return 0;
}

int set_dsl_link_alias(char *refparam, struct dmctx *ctx, int action, char *value)
{
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(cur_dsl_line_args.line_sec, "dsllinkalias", value);
			return 0;
	}
	return 0;
}

int get_channel_alias(char *refparam, struct dmctx *ctx, char **value)
{
	dmuci_get_value_by_section_string(cur_dsl_line_args.line_sec, "channelalias", value);
	return 0;
}

int set_channel_alias(char *refparam, struct dmctx *ctx, int action, char *value)
{
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(cur_dsl_line_args.line_sec, "channelalias", value);
			return 0;
	}
	return 0;
}

int get_atm_alias(char *refparam, struct dmctx *ctx, char **value)
{
	dmuci_get_value_by_section_string(cur_atm_args.atm_sec, "atmlinkalias", value);
	return 0;
}

int set_atm_alias(char *refparam, struct dmctx *ctx, int action, char *value)
{
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(cur_atm_args.atm_sec, "atmlinkalias", value);
			return 0;
	}
	return 0;
}

int get_ptm_alias(char *refparam, struct dmctx *ctx, char **value)
{
	dmuci_get_value_by_section_string(cur_ptm_args.ptm_sec, "ptmlinkalias", value);
	return 0;
}

int set_ptm_alias(char *refparam, struct dmctx *ctx, int action, char *value)
{
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(cur_ptm_args.ptm_sec, "ptmlinkalias", value);
			return 0;
	}
	return 0;
}
/*************************************************************
 * SUB ENTRIES
/*************************************************************/
int entry_method_root_wan_dsl(struct dmctx *ctx)
{
	IF_MATCH(ctx, DMROOT"DSL.") {
		DMOBJECT(DMROOT"DSL.", ctx, "0", 0, NULL, NULL, NULL);
		DMOBJECT(DMROOT"DSL.Line.", ctx, "0", 0, NULL, NULL, NULL);
		DMOBJECT(DMROOT"DSL.Channel.", ctx, "0", 0, NULL, NULL, NULL);
		SUBENTRY(entry_dsl_link, ctx);
		return 0;
	}
	return FAULT_9005;
}

int entry_method_root_wan_atm(struct dmctx *ctx)
{
	IF_MATCH(ctx, DMROOT"ATM.") {
		DMOBJECT(DMROOT"ATM.", ctx, "0", 0, NULL, NULL, NULL);
		DMOBJECT(DMROOT"ATM.Link.", ctx, "0", 0, add_atm_link, delete_atm_link_all, NULL);
		SUBENTRY(entry_atm_link, ctx);
		return 0;
	}
	return FAULT_9005;
}

int entry_method_root_wan_ptm(struct dmctx *ctx)
{
	IF_MATCH(ctx, DMROOT"PTM.") {
		DMOBJECT(DMROOT"PTM.", ctx, "0", 0, NULL, NULL, NULL);
		DMOBJECT(DMROOT"PTM.Link.", ctx, "0", 0, add_ptm_link, delete_ptm_link_all, NULL);
		SUBENTRY(entry_ptm_link, ctx);
		return 0;
	}
	return FAULT_9005;
}

inline int entry_dsl_link(struct dmctx *ctx)
{
	char *wnum = NULL, *wnum_last = NULL, *channel_last = NULL, *type;
	struct uci_section *s = NULL;
	uci_foreach_sections("layer2_interface", "dsltype", s) {
		init_dsl_link(ctx, s, section_name(s));
		wnum = handle_update_instance(1, ctx, &wnum_last, update_instance_alias, 3, s, "dsllinkinstance", "dsllinkalias");
		SUBENTRY(entry_dsl_line_instance, ctx, wnum);
		wnum = handle_update_instance(1, ctx, &channel_last, update_instance_alias, 3, s, "channelinstance", "channelalias");
		SUBENTRY(entry_dsl_channel_instance, ctx, wnum);
	}
	return 0;
}

inline int entry_atm_link(struct dmctx *ctx)
{
	char *wnum = NULL, *wnum_last = NULL, *channel_last = NULL, *ifname;
	struct uci_section *s = NULL;
	uci_foreach_sections("layer2_interface_adsl", "atm_bridge", s) {
		dmuci_get_value_by_section_string(s, "ifname", &ifname);
		init_atm_link(ctx, s, ifname);
		wnum = handle_update_instance(1, ctx, &channel_last, update_instance_alias, 3, s, "atmlinkinstance", "atmlinkalias");
		SUBENTRY(entry_atm_link_instance, ctx, wnum);
	}
	return 0;
}

inline int entry_ptm_link(struct dmctx *ctx)
{
	char *wnum = NULL, *wnum_last = NULL, *channel_last = NULL, *ifname;
	struct uci_section *s = NULL;
	uci_foreach_sections("layer2_interface_vdsl", "vdsl_interface", s) {
		dmuci_get_value_by_section_string(s, "ifname", &ifname);
		init_ptm_link(ctx, s, ifname);
		wnum = handle_update_instance(1, ctx, &channel_last, update_instance_alias, 3, s, "ptmlinkinstance", "ptmlinkalias"); //finish here
		SUBENTRY(entry_ptm_link_instance, ctx, wnum);
	}
	return 0;
}
inline int entry_dsl_line_instance(struct dmctx *ctx, char *dev)
{
	IF_MATCH(ctx, DMROOT"DSL.Line.%s.", dev) {
		char linker[8];
		strcpy(linker, cur_dsl_line_args.type);
		DMOBJECT(DMROOT"DSL.Line.%s.", ctx, "0", NULL, NULL, NULL, linker, dev);
		DMPARAM("Alias", ctx, "1", get_dsl_link_alias, set_dsl_link_alias, NULL, 0, 1, UNDEF, NULL);
		DMPARAM("Name", ctx, "0", get_line_name, NULL, NULL, 0, 1, UNDEF, NULL);
		if (strcmp(cur_dsl_line_args.type, "adsl") == 0) {
			DMPARAM("Status", ctx, "0", get_dsl_status, NULL, NULL, 0, 1, UNDEF, NULL);
			DMPARAM("LinkStatus", ctx, "0", get_dsl_link_status, NULL, NULL, 0, 1, UNDEF, NULL);
			DMPARAM("StandardsSupported", ctx, "0", get_dsl_link_supported_standard, NULL, NULL, 0, 1, UNDEF, NULL);
			DMPARAM("StandardUsed", ctx, "0", get_dsl_link_standard_inuse, NULL, NULL, 0, 1, UNDEF, NULL);
		} else if (strcmp(cur_dsl_line_args.type, "vdsl") == 0) {
			DMPARAM("Status", ctx, "0", get_dsl_status, NULL, NULL, 0, 1, UNDEF, NULL);
			DMPARAM("LinkStatus", ctx, "0", get_dsl_link_status, NULL, NULL, 0, 1, UNDEF, NULL);
			DMPARAM("AllowedProfiles", ctx, "0", get_vdsl_link_supported_profile, NULL, NULL, 0, 1, UNDEF, NULL);
			DMPARAM("CurrentProfile", ctx, "0", get_vdsl_link_profile_inuse, NULL, NULL, 0, 1, UNDEF, NULL);
		}
		DMPARAM("DownstreamMaxBitRate", ctx, "0", get_dsl_link_downstreammaxrate, NULL, "xsd:unsignedInt", 0, 1, UNDEF, NULL);
		DMPARAM("DownstreamAttenuation", ctx, "0", get_dsl_link_downstreamattenuation, NULL, "xsd:int", 0, 1, UNDEF, NULL);
		DMPARAM("DownstreamNoiseMargin", ctx, "0", get_dsl_link_downstreamnoisemargin, NULL, "xsd:int", 0, 1, UNDEF, NULL);
		DMPARAM("UpstreamMaxBitRate", ctx, "0", get_dsl_link_upstreammaxrate, NULL, "xsd:unsignedInt", 0, 1, UNDEF, NULL);
		DMPARAM("UpstreamAttenuation", ctx, "0", get_dsl_link_upstreamattenuation, NULL, "xsd:int", 0, 1, UNDEF, NULL);
		DMPARAM("UpstreamNoiseMargin", ctx, "0", get_dsl_link_upstreamnoisemargin, NULL, "xsd:int", 0, 1, UNDEF, NULL);
		return 0;
	}
	return FAULT_9005;
}

inline int entry_dsl_channel_instance(struct dmctx *ctx, char *dev)
{
	IF_MATCH(ctx, DMROOT"DSL.Channel.%s.", dev) {
		char linker[8];
		strcpy(linker, cur_dsl_line_args.type);
		DMOBJECT(DMROOT"DSL.Channel.%s.",  ctx, "0", NULL, NULL, NULL, linker, dev);
		DMPARAM("Alias", ctx, "1", get_channel_alias, set_channel_alias, NULL, 0, 1, UNDEF, NULL);
		DMPARAM("Status", ctx, "0", get_dsl_status, NULL, NULL, 0, 1, UNDEF, NULL);
		DMPARAM("Name", ctx, "0", get_line_name, NULL, NULL, 0, 1, UNDEF, NULL);
		DMPARAM("LowerLayers", ctx, "0", get_channel_lower_layer, NULL, NULL, 0, 1, UNDEF, NULL);
		DMPARAM("DownstreamCurrRate", ctx, "0", get_dsl_channel_downstreamcurrrate, NULL, "xsd:unsignedInt", 0, 1, UNDEF, NULL);
		DMPARAM("UpstreamCurrRate", ctx, "0", get_dsl_channel_upstreamcurrrate, NULL, "xsd:unsignedInt", 0, 1, UNDEF, NULL);
		DMPARAM("X_INTENO_SE_AnnexMEnable", ctx, "1", get_channel_annexm_status, set_channel_annexm_status, "xsd:boolean", 0, 1, UNDEF, NULL);
		DMPARAM("LinkEncapsulationSupported", ctx, "0", get_channel_supported_encap, NULL, NULL, 0, 1, UNDEF, NULL);
		DMPARAM("LinkEncapsulationUsed", ctx, "0", get_empty, NULL, NULL, 0, 1, UNDEF, NULL);
		return 0;
	}
	return FAULT_9005;
}

inline int entry_atm_link_instance(struct dmctx *ctx, char *idev)
{
		IF_MATCH(ctx, DMROOT"ATM.Link.%s.", idev) {
			DMOBJECT(DMROOT"ATM.Link.%s.", ctx, "1", NULL, NULL, delete_atm_link, cur_atm_args.ifname, idev);
			DMPARAM("Alias", ctx, "1", get_atm_alias, set_atm_alias, NULL, 0, 1, UNDEF, NULL);
			DMPARAM("Enable", ctx, "0", get_atm_enable, NULL, "xsd:boolean", 0, 1, UNDEF, NULL);
			DMPARAM("Name", ctx, "0", get_atm_link_name, NULL, NULL, 0, 1, UNDEF, NULL);
			DMPARAM("Status", ctx, "0", get_atm_enable, NULL, NULL, 0, 1, UNDEF, NULL);
			DMPARAM("LowerLayers", ctx, "0", get_atm_lower_layer, NULL, NULL, 0, 1, UNDEF, NULL);
			DMPARAM("LinkType", ctx, "1", get_atm_link_type, set_atm_link_type, NULL, 0, 1, UNDEF, NULL);
			DMPARAM("DestinationAddress", ctx, "1", get_atm_destination_address, set_atm_destination_address, NULL, 0, 1, UNDEF, NULL);
			DMPARAM("Encapsulation", ctx, "1", get_atm_encapsulation, set_atm_encapsulation, NULL, 0, 1, UNDEF, NULL);
			DMOBJECT(DMROOT"ATM.Link.%s.Stats.", ctx, "1", 1, NULL, NULL, NULL, idev);
			DMPARAM("BytesSent", ctx, "0", get_atm_stats_bytes_sent, NULL, "xsd:unsignedInt", 0, 1, UNDEF, NULL);
			DMPARAM("BytesReceived", ctx, "0", get_atm_stats_bytes_received, NULL, "xsd:unsignedInt", 0, 1, UNDEF, NULL);
			DMPARAM("PacketsSent", ctx, "0", get_atm_stats_pack_sent, NULL, "xsd:unsignedInt", 0, 1, UNDEF, NULL);
			DMPARAM("PacketsReceived", ctx, "0", get_atm_stats_pack_received, NULL, "xsd:unsignedInt", 0, 1, UNDEF, NULL);
			return 0;
		}
	return FAULT_9005;
}

inline int entry_ptm_link_instance(struct dmctx *ctx, char *idev)
{
		IF_MATCH(ctx, DMROOT"PTM.Link.%s.", idev) {
			DMOBJECT(DMROOT"PTM.Link.%s.", ctx, "1", NULL, NULL, delete_ptm_link, cur_ptm_args.ifname, idev);
			DMPARAM("Alias", ctx, "1", get_ptm_alias, set_ptm_alias, NULL, 0, 1, UNDEF, NULL);
			DMPARAM("Enable", ctx, "0", get_ptm_enable, NULL, "xsd:boolean", 0, 1, UNDEF, NULL);
			DMPARAM("Name", ctx, "0", get_ptm_link_name, NULL, NULL, 0, 1, UNDEF, NULL);
			DMPARAM("Status", ctx, "0", get_ptm_enable, NULL, NULL, 0, 1, UNDEF, NULL);
			DMPARAM("LowerLayers", ctx, "0", get_ptm_lower_layer, NULL, NULL, 0, 1, UNDEF, NULL);
			DMOBJECT(DMROOT"PTM.Link.%s.Stats.", ctx, "1", 1, NULL, NULL, NULL, idev);
			DMPARAM("BytesSent", ctx, "0", get_ptm_stats_bytes_sent, NULL, "xsd:unsignedInt", 0, 1, UNDEF, NULL);
			DMPARAM("BytesReceived", ctx, "0", get_ptm_stats_bytes_received, NULL, "xsd:unsignedInt", 0, 1, UNDEF, NULL);
			DMPARAM("PacketsSent", ctx, "0", get_ptm_stats_pack_sent, NULL, "xsd:unsignedInt", 0, 1, UNDEF, NULL);
			DMPARAM("PacketsReceived", ctx, "0", get_ptm_stats_pack_received, NULL, "xsd:unsignedInt", 0, 1, UNDEF, NULL);
			return 0;
		}
	return FAULT_9005;
}
