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

/*** DSL. ***/
DMOBJ tDslObj[] = {
/* OBJ, permission, addobj, delobj, browseinstobj, finform, notification, nextobj, leaf, linker*/
{"Line", &DMREAD, NULL, NULL, NULL, browseDslLineInst, NULL, NULL, NULL, tDslLineParams, get_dsl_line_linker},
{"Channel", &DMREAD, NULL, NULL, NULL, browseDslChannelInst, NULL, NULL, NULL, tDslChanelParams, get_dsl_channel_linker},
{0}
};

/*** ATM. ***/
DMOBJ tAtmObj[] = {
/* OBJ, permission, addobj, delobj, browseinstobj, finform, notification, nextobj, leaf, linker*/
{"Link", &DMWRITE, add_atm_link, delete_atm_link, NULL, browseAtmLinkInst, NULL, NULL, tAtmLinkStatsObj, tAtmLineParams, get_atm_linker},
{0}
};

/*** PTM. ***/
DMOBJ tPtmObj[] = {
/* OBJ, permission, addobj, delobj, browseinstobj, finform, notification, nextobj, leaf, linker*/
{"Link", &DMWRITE, add_ptm_link, delete_ptm_link, NULL, browsePtmLinkInst, NULL, NULL, tPtmLinkStatsObj, tPtmLineParams, get_ptm_linker},
{0}
};

/*** DSL.Line ***/
DMLEAF tDslLineParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"Alias", &DMWRITE, DMT_STRING, get_dsl_link_alias, set_dsl_link_alias, NULL, NULL},
{"Name", &DMREAD, DMT_STRING, get_line_name, NULL, NULL, NULL},
{"Status", &DMREAD, DMT_STRING,  get_dsl_status, NULL, NULL, NULL},
{"LinkStatus", &DMREAD, DMT_STRING,get_dsl_link_status, NULL, NULL, NULL},
{"StandardsSupported", &DMREAD, DMT_STRING, get_dsl_link_supported_standard, NULL, NULL, NULL},
{"StandardUsed", &DMREAD, DMT_STRING, get_dsl_link_standard_inuse, NULL, NULL, NULL},
{"AllowedProfiles", &DMREAD, DMT_STRING,  get_vdsl_link_supported_profile, NULL, NULL, NULL},
{"CurrentProfile", &DMREAD, DMT_STRING, get_vdsl_link_profile_inuse, NULL, NULL, NULL},
{"DownstreamMaxBitRate", &DMREAD, DMT_UNINT, get_dsl_link_downstreammaxrate, NULL, NULL, NULL},
{"DownstreamAttenuation", &DMREAD, DMT_INT, get_dsl_link_downstreamattenuation, NULL, NULL, NULL},
{"DownstreamNoiseMargin", &DMREAD, DMT_INT, get_dsl_link_downstreamnoisemargin, NULL, NULL, NULL},
{"UpstreamMaxBitRate", &DMREAD, DMT_UNINT, get_dsl_link_upstreammaxrate, NULL, NULL, NULL},
{"UpstreamAttenuation", &DMREAD, DMT_INT, get_dsl_link_upstreamattenuation, NULL, NULL, NULL},
{"UpstreamNoiseMargin", &DMREAD, DMT_INT, get_dsl_link_upstreamnoisemargin, NULL, NULL, NULL},
{0}
};

/*** DSL.Channel ***/
DMLEAF tDslChanelParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"Alias", &DMWRITE, DMT_STRING, get_channel_alias, set_channel_alias, NULL, NULL},
{"Status", &DMREAD, DMT_STRING, get_dsl_status, NULL, NULL, NULL},
{"Name", &DMREAD, DMT_STRING, get_line_name, NULL, NULL, NULL},
{"LowerLayers", &DMREAD, DMT_STRING, get_channel_lower_layer, NULL, NULL, NULL},
{"DownstreamCurrRate", &DMREAD, DMT_UNINT, get_dsl_channel_downstreamcurrrate, NULL, NULL, NULL},
{"UpstreamCurrRate", &DMREAD, DMT_UNINT, get_dsl_channel_upstreamcurrrate, NULL, NULL, NULL},
{"X_INTENO_SE_AnnexMEnable", &DMWRITE, DMT_BOOL,  get_channel_annexm_status, set_channel_annexm_status, NULL, NULL},
{"LinkEncapsulationSupported", &DMREAD, DMT_STRING, get_channel_supported_encap, NULL, NULL, NULL},
{"LinkEncapsulationUsed", &DMREAD, DMT_STRING, get_empty, NULL, NULL, NULL},
{0}
};

/*** ATM.Link. ***/

DMOBJ tAtmLinkStatsObj[] = {
/* OBJ, permission, addobj, delobj, browseinstobj, finform, notification, nextobj, leaf, linker*/
{"Stats", &DMREAD, NULL, NULL, NULL, NULL, NULL, NULL, NULL, tAtmLinkStatsParams, NULL},
{0}
};

DMLEAF tAtmLineParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"Alias", &DMWRITE, DMT_STRING,  get_atm_alias, set_atm_alias, NULL, NULL},
{"Enable", &DMREAD, DMT_BOOL, get_atm_enable, NULL, NULL, NULL},
{"Name", &DMREAD, DMT_STRING, get_atm_link_name, NULL, NULL, NULL},
{"Status", &DMREAD, DMT_STRING, get_atm_enable, NULL, NULL, NULL},
{"LowerLayers", &DMREAD, DMT_STRING, get_atm_lower_layer, NULL, NULL, NULL},
{"LinkType", &DMWRITE, DMT_STRING, get_atm_link_type, set_atm_link_type, NULL, NULL},
{"DestinationAddress", &DMWRITE, DMT_STRING, get_atm_destination_address, set_atm_destination_address, NULL, NULL},
{"Encapsulation", &DMWRITE, DMT_STRING, get_atm_encapsulation, set_atm_encapsulation, NULL, NULL},
{0}
};

/*** ATM.Link.Stats. ***/
DMLEAF tAtmLinkStatsParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, NOTIFICATION, linker*/
{"BytesSent", &DMREAD, DMT_UNINT, get_atm_stats_bytes_sent, NULL, NULL, NULL},
{"BytesReceived", &DMREAD, DMT_UNINT, get_atm_stats_bytes_received, NULL, NULL, NULL},
{"PacketsSent", &DMREAD, DMT_UNINT, get_atm_stats_pack_sent, NULL, NULL, NULL},
{"PacketsReceived", &DMREAD, DMT_UNINT, get_atm_stats_pack_received, NULL, NULL, NULL},
{0}
};

/*** PTM.Link. ***/
DMOBJ tPtmLinkStatsObj[] = {
/* OBJ, permission, addobj, delobj, browseinstobj, finform, notification, nextobj, leaf, linker*/
{"Stats", &DMREAD, NULL, NULL, NULL, NULL, NULL, NULL, NULL, tPtmLinkStatsParams, NULL},
{0}
};

DMLEAF tPtmLineParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"Alias", &DMWRITE, DMT_STRING, get_ptm_alias, set_ptm_alias, NULL, NULL},
{"Enable", &DMREAD, DMT_BOOL, get_ptm_enable, NULL, NULL, NULL},
{"Name", &DMREAD, DMT_STRING, get_ptm_link_name, NULL, NULL, NULL},
{"Status", &DMREAD, DMT_STRING, get_ptm_enable, NULL, NULL, NULL},
{"LowerLayers", &DMREAD, DMT_STRING, get_ptm_lower_layer, NULL, NULL, NULL},
{0}
};

/*** PTM.Link.Stats. ***/
DMLEAF tPtmLinkStatsParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, NOTIFICATION, linker*/
{"BytesSent", &DMREAD, DMT_UNINT, get_ptm_stats_bytes_sent, NULL, NULL, NULL},
{"BytesReceived", &DMREAD, DMT_UNINT, get_ptm_stats_bytes_received, NULL, NULL, NULL},
{"PacketsSent", &DMREAD, DMT_UNINT, get_ptm_stats_pack_sent, NULL, NULL, NULL},
{"PacketsReceived", &DMREAD, DMT_UNINT, get_ptm_stats_pack_received, NULL, NULL, NULL},
{0}
};

/**************************************************************************
* LINKER
***************************************************************************/
int get_atm_linker(char *refparam, struct dmctx *dmctx, void *data, char *instance, char **linker) {
	if (((struct atm_args *)data)->ifname) {
		*linker =  ((struct atm_args *)data)->ifname;
		return 0;
	}
	*linker = "" ;
	return 0;
}

int get_ptm_linker(char *refparam, struct dmctx *dmctx, void *data, char *instance, char **linker) {
	if (((struct ptm_args *)data)->ifname){
		*linker =  ((struct ptm_args *)data)->ifname;
		return 0;
	}
	*linker = "" ;
	return 0;
}
int get_dsl_line_linker(char *refparam, struct dmctx *dmctx, void *data, char *instance, char **linker) {
	if (((struct dsl_line_args *)data)->type) {
		*linker = ((struct dsl_line_args *)data)->type;
		return 0;
	}
	*linker = "" ;
	return 0;
}

int get_dsl_channel_linker(char *refparam, struct dmctx *dmctx, void *data, char *instance, char **linker) {
	if (((struct dsl_line_args *)data)->type){
		*linker = ((struct dsl_line_args *)data)->type;
		return 0;
	}
	*linker = "" ;
	return 0;
}
/**************************************************************************
* INIT
***************************************************************************/
inline int init_dsl_link(struct dsl_line_args *args, struct uci_section *s, char *type)
{
	args->line_sec = s;
	args->type = type;
	return 0;
}

inline int init_atm_link(struct atm_args *args, struct uci_section *s, char *ifname)
{
	args->atm_sec = s;
	args->ifname = ifname;
	return 0;
}

inline int init_ptm_link(struct ptm_args *args, struct uci_section *s, char *ifname)
{
	args->ptm_sec = s;
	args->ifname = ifname;
	return 0;
}

/**************************************************************************
* SET & GET DSL LINK PARAMETERS
***************************************************************************/

int get_dsl_status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *status;
	json_object *res;

	*value = "Down";
	if(strcmp(((struct dsl_line_args *)data)->type, "adsl") == 0 && check_dsl_link_type() == 1 || strcmp(((struct dsl_line_args *)data)->type, "vdsl") == 0 && check_dsl_link_type() == 0) {
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

int get_dsl_link_status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *status;
	json_object *res;

	*value = "NoSignal";
	if(strcmp(((struct dsl_line_args *)data)->type, "adsl") == 0 && check_dsl_link_type() == 1 || strcmp(((struct dsl_line_args *)data)->type, "vdsl") == 0 && check_dsl_link_type() == 0) {
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

int get_dsl_link_supported_standard(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = "G.992.1_Annex_A, G.992.2, T1.413, G.992.3_Annex_A, G.992.3_Annex_L, G.992.5_Annex_A, G.992.5_Annex_M";
	return 0;
}

int get_dsl_link_standard_inuse(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *mode;
	json_object *res = NULL;
	*value = "";
	if(strcmp(((struct dsl_line_args *)data)->type, "adsl") == 0) {
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
	}
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
int get_vdsl_link_supported_profile(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = "8a, 8b, 8c, 8d, 12a, 12b, 17a, 17b, 30a";
	return 0;
}

int get_vdsl_link_profile_inuse(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *mode;
	json_object *res = NULL;
	*value = "";
	if (strcmp(((struct dsl_line_args *)data)->type, "vdsl") == 0) {
		dmubus_call("router.dsl", "stats", UBUS_ARGS{}, 0, &res);
		DM_ASSERT(res, *value = "");
		mode = dmjson_get_value(res, 2, "dslstats", "mode");
		*value = mode;
	}
	return 0;
}

int get_dsl_link_downstreammaxrate(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *max_down;
	json_object *res = NULL;
	json_object *sub_obj = NULL;
	json_object *sub_obj_2 = NULL;
	*value = "";
	if((strcmp(((struct dsl_line_args *)data)->type, "adsl") == 0 && check_dsl_link_type() == 1) || (strcmp(((struct dsl_line_args *)data)->type, "vdsl") == 0 && check_dsl_link_type() == 0)) {
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

int get_dsl_link_downstreamattenuation(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *attn_down_x100;
	json_object *res = NULL;
	*value = "";
	if((strcmp(((struct dsl_line_args *)data)->type, "adsl") == 0 && check_dsl_link_type() == 1) || (strcmp(((struct dsl_line_args *)data)->type, "vdsl") == 0 && check_dsl_link_type() == 0)) {
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

int get_dsl_link_downstreamnoisemargin(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *snr_down_x100;
	json_object *res;
	*value = "";
	if((strcmp(((struct dsl_line_args *)data)->type, "adsl") == 0 && check_dsl_link_type() == 1) || (strcmp(((struct dsl_line_args *)data)->type, "vdsl") == 0 && check_dsl_link_type() == 0)) {
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

int get_dsl_link_upstreammaxrate(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *max_up;
	json_object *res = NULL;
	json_object *sub_obj = NULL;
	json_object *sub_obj_2 = NULL;
	*value = "";
	if((strcmp(((struct dsl_line_args *)data)->type, "adsl") == 0 && check_dsl_link_type() == 1) || (strcmp(((struct dsl_line_args *)data)->type, "vdsl") == 0 && check_dsl_link_type() == 0)) {
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

int get_dsl_link_upstreamattenuation(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *attn_up_x100;
	json_object *res = NULL;
	*value = "";
	if((strcmp(((struct dsl_line_args *)data)->type, "adsl") == 0 && check_dsl_link_type() == 1) || (strcmp(((struct dsl_line_args *)data)->type, "vdsl") == 0 && check_dsl_link_type() == 0)) {
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

int get_dsl_link_upstreamnoisemargin(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *snr_up_x100;
	json_object *res;
	*value = "";
	if((strcmp(((struct dsl_line_args *)data)->type, "adsl") == 0 && check_dsl_link_type() == 1) || (strcmp(((struct dsl_line_args *)data)->type, "vdsl") == 0 && check_dsl_link_type() == 0)) {
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

int get_channel_lower_layer(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	adm_entry_get_linker_param(ctx, dm_print_path("%s%cDSL%cLine%c", DMROOT, dm_delim, dm_delim, dm_delim), ((struct dsl_line_args *)data)->type, value); // MEM WILL BE FREED IN DMMEMCLEAN
	if (*value == NULL)
		*value = "";
	return 0;
}

int get_dsl_channel_downstreamcurrrate(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
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

int get_dsl_channel_upstreamcurrrate(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
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

int get_channel_annexm_status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *val = "0";
	*value = "";
	if (strcmp(((struct dsl_line_args *)data)->type, "adsl") == 0) {
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

int set_channel_annexm_status(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	bool b;

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

int get_channel_supported_encap(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = "G.992.3_Annex_K_ATM, G.992.3_Annex_K_PTM, G.993.2_Annex_K_ATM, G.993.2_Annex_K_PTM, G.994.1";
	return 0;
}


int get_atm_destination_address(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *vpi, *vci;

	dmuci_get_value_by_section_string(((struct atm_args *)data)->atm_sec, "vpi", &vpi);
	dmuci_get_value_by_section_string(((struct atm_args *)data)->atm_sec, "vci", &vci);
	dmasprintf(value, "PVC: %s/%s", vpi, vci); // MEM WILL BE FREED IN DMMEMCLEAN
	return 0;
}

int set_atm_destination_address(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	char *vpi = NULL, *vci = NULL, *spch, *val;
	struct uci_section *s;

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
					dmuci_set_value_by_section(((struct atm_args *)data)->atm_sec, "vpi", vpi);
					dmuci_set_value_by_section(((struct atm_args *)data)->atm_sec, "vci", vci);
				}
				dmfree(val);
				break;
			return 0;
	}
	return 0;
}

int get_atm_link_name(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_value_by_section_string(((struct atm_args *)data)->atm_sec, "name", value);
	return 0;
}

int get_ptm_link_name(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_value_by_section_string(((struct ptm_args *)data)->ptm_sec, "name", value);
	return 0;
}


int get_atm_encapsulation(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *type, *encapsulation;
	dmuci_get_value_by_section_string(((struct atm_args *)data)->atm_sec,"link_type", &type);
	if (strcmp(type, "EoA") == 0 ) {
		type = "encapseoa";
	} else if (strcmp(type, "PPPoA") == 0) {
		type = "encapspppoa";
	} else if (strcmp(type, "IPoA") == 0) {
		type = "encapsipoa";
	}
	dmuci_get_value_by_section_string(((struct atm_args *)data)->atm_sec, type, &encapsulation);
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

int set_atm_encapsulation(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	int i;
	struct uci_section *s;
	char *type, *encapsulation, *encaptype, *pch;

	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_get_value_by_section_string(((struct atm_args *)data)->atm_sec, "link_type", &type);
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

			dmuci_set_value_by_section(((struct atm_args *)data)->atm_sec, encaptype, encapsulation);
			return 0;
	}
	return 0;
}


int get_atm_link_type(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = "";
	dmuci_get_value_by_section_string(((struct atm_args *)data)->atm_sec, "link_type", value);
	return 0;
}

int set_atm_link_type(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(((struct atm_args *)data)->atm_sec, "link_type", value);
			return 0;
	}
	return 0;
}

int get_atm_lower_layer(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *linker = "adsl";
	adm_entry_get_linker_param(ctx, dm_print_path("%s%cDSL%cChannel%c", DMROOT, dm_delim, dm_delim, dm_delim), linker, value); // MEM WILL BE FREED IN DMMEMCLEAN
	if (*value == NULL)
		*value = "";
	return 0;
}

int get_ptm_lower_layer(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *linker = "vdsl";
	adm_entry_get_linker_param(ctx, dm_print_path("%s%cDSL%cChannel%c", DMROOT, dm_delim, dm_delim, dm_delim), linker, value); // MEM WILL BE FREED IN DMMEMCLEAN
	if (*value == NULL)
		*value = "";
	return 0;
}


inline int ubus_atm_stats(json_object *res, char **value, char *stat_mod, void *data)
{

	dmubus_call("network.device", "status", UBUS_ARGS{{"name", ((struct atm_args *)data)->ifname, String}}, 1, &res);
	DM_ASSERT(res, *value = "");
	*value = dmjson_get_value(res, 2, "statistics", stat_mod);
	return 0;
}

int get_atm_stats_bytes_received(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;
	ubus_atm_stats(res, value, "rx_bytes", data);
	return 0;
}

int get_atm_stats_bytes_sent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;
	ubus_atm_stats(res, value, "tx_bytes", data);
	return 0;
}

int get_atm_stats_pack_received(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;
	ubus_atm_stats(res, value, "rx_packets", data);
	return 0;
}

int get_atm_stats_pack_sent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;
	ubus_atm_stats(res, value, "tx_packets", data);
	return 0;
}

inline int ubus_ptm_stats(json_object *res, char **value, char *stat_mod, void *data)
{

	dmubus_call("network.device", "status", UBUS_ARGS{{"name", ((struct ptm_args *)data)->ifname, String}}, 1, &res);
	DM_ASSERT(res, *value = "");
	*value = dmjson_get_value(res, 2, "statistics", stat_mod);
	return 0;
}

int get_ptm_stats_bytes_received(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;
	ubus_ptm_stats(res, value, "rx_bytes", data);
	return 0;
}

int get_ptm_stats_bytes_sent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;
	ubus_ptm_stats(res, value, "tx_bytes", data);
	return 0;
}

int get_ptm_stats_pack_received(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;
	ubus_ptm_stats(res, value, "rx_packets", data);
	return 0;
}

int get_ptm_stats_pack_sent(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	json_object *res;
	ubus_ptm_stats(res, value, "tx_packets", data);
	return 0;
}

int get_line_name(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = dmstrdup(((struct dsl_line_args *)data)->type);
	return 0;
}

int get_atm_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = "true";
	return 0;
}

int get_ptm_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = "true";
	return 0;
}
/*************************************************************
 * ADD OBJ
/*************************************************************/
int add_atm_link(char *refparam, struct dmctx *ctx, void *data, char **instancepara)
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

int add_ptm_link(char *refparam, struct dmctx *ctx, void *data, char **instancepara)
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

int delete_atm_link(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action)
{
	struct uci_section *s = NULL;
	struct uci_section *ss = NULL;

	struct uci_section *ns = NULL;
	struct uci_section *nss = NULL;
	char *ifname;
	switch (del_action) {
		case DEL_INST:
			dmuci_delete_by_section(((struct atm_args *)data)->atm_sec, NULL, NULL);
			uci_foreach_option_cont("network", "interface", "ifname", ((struct atm_args *)data)->ifname, s) {
				if (ss)
					wan_remove_dev_interface(ss, ((struct atm_args *)data)->ifname);
				ss = s;
			}
			if (ss != NULL)
				wan_remove_dev_interface(ss, ((struct atm_args *)data)->ifname);
			break;
		case DEL_ALL:
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
			}
			break;
	}
	return 0;
}

int delete_ptm_link(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action)
{
	char *ifname;
	struct uci_section *s = NULL;
	struct uci_section *ss = NULL;

	struct uci_section *ns = NULL;
	struct uci_section *nss = NULL;

	switch (del_action) {
	case DEL_INST:
		dmuci_delete_by_section(((struct ptm_args *)data)->ptm_sec, NULL, NULL);
		uci_foreach_option_cont("network", "interface", "ifname", ((struct ptm_args *)data)->ifname, s) {
			if (ss)
				wan_remove_dev_interface(ss, ((struct ptm_args *)data)->ifname);
			ss = s;
		}
		if (ss != NULL)
			wan_remove_dev_interface(ss, ((struct ptm_args *)data)->ifname);
		break;
	case DEL_ALL:
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
		break;
	}
	return 0;
}
/*************************************************************
 * SET AND GET ALIAS
/*************************************************************/
int get_dsl_link_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_value_by_section_string(((struct dsl_line_args *)data)->line_sec, "dsllinkalias", value);
	return 0;
}

int set_dsl_link_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(((struct dsl_line_args *)data)->line_sec, "dsllinkalias", value);
			return 0;
	}
	return 0;
}

int get_channel_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_value_by_section_string(((struct dsl_line_args *)data)->line_sec, "channelalias", value);
	return 0;
}

int set_channel_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(((struct dsl_line_args *)data)->line_sec, "channelalias", value);
			return 0;
	}
	return 0;
}

int get_atm_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_value_by_section_string(((struct atm_args *)data)->atm_sec, "atmlinkalias", value);
	return 0;
}

int set_atm_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(((struct atm_args *)data)->atm_sec, "atmlinkalias", value);
			return 0;
	}
	return 0;
}

int get_ptm_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_value_by_section_string(((struct ptm_args *)data)->ptm_sec, "ptmlinkalias", value);
	return 0;
}

int set_ptm_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action) {
		case VALUECHECK:
			return 0;
		case VALUESET:
			dmuci_set_value_by_section(((struct ptm_args *)data)->ptm_sec, "ptmlinkalias", value);
			return 0;
	}
	return 0;
}

/*************************************************************
 * ENTRY METHOD
/*************************************************************/

int browseDslLineInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	char *wnum = NULL, *wnum_last = NULL, *channel_last = NULL, *type;
	struct uci_section *s = NULL;
	struct dsl_line_args curr_dsl_line_args = {0};

	uci_foreach_sections("layer2_interface", "dsltype", s) {
		init_dsl_link(&curr_dsl_line_args, s, section_name(s));
		wnum = handle_update_instance(1, dmctx, &wnum_last, update_instance_alias, 3, s, "dsllinkinstance", "dsllinkalias");
		if (DM_LINK_INST_OBJ(dmctx, parent_node, (void *)&curr_dsl_line_args, wnum) == DM_STOP)
			break;
	}
	return 0;
}

int browseDslChannelInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	char *wnum = NULL, *wnum_last = NULL, *channel_last = NULL, *type;
	struct uci_section *s = NULL;
	struct dsl_line_args curr_dsl_line_args = {0};

	uci_foreach_sections("layer2_interface", "dsltype", s) {
		init_dsl_link(&curr_dsl_line_args, s, section_name(s));
		wnum = handle_update_instance(1, dmctx, &channel_last, update_instance_alias, 3, s, "channelinstance", "channelalias");
		if (DM_LINK_INST_OBJ(dmctx, parent_node, (void *)&curr_dsl_line_args, wnum) == DM_STOP)
			break;
	}
	return 0;
}

int browseAtmLinkInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	char *wnum = NULL, *wnum_last = NULL, *channel_last = NULL, *ifname;
	struct uci_section *s = NULL;
	struct atm_args curr_atm_args = {0};

	uci_foreach_sections("layer2_interface_adsl", "atm_bridge", s) {
		dmuci_get_value_by_section_string(s, "ifname", &ifname);
		init_atm_link(&curr_atm_args, s, ifname);
		wnum = handle_update_instance(1, dmctx, &channel_last, update_instance_alias, 3, s, "atmlinkinstance", "atmlinkalias");
		if (DM_LINK_INST_OBJ(dmctx, parent_node, (void *)&curr_atm_args, wnum) == DM_STOP)
			break;
	}
	return 0;
}

int browsePtmLinkInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	char *wnum = NULL, *wnum_last = NULL, *channel_last = NULL, *ifname;
	struct uci_section *s = NULL;
	struct ptm_args curr_ptm_args = {0};

	uci_foreach_sections("layer2_interface_vdsl", "vdsl_interface", s) {
		dmuci_get_value_by_section_string(s, "ifname", &ifname);
		init_ptm_link(&curr_ptm_args, s, ifname);
		wnum = handle_update_instance(1, dmctx, &channel_last, update_instance_alias, 3, s, "ptmlinkinstance", "ptmlinkalias"); //finish here
		if (DM_LINK_INST_OBJ(dmctx, parent_node, (void *)&curr_ptm_args, wnum) == DM_STOP)
			break;
	}
	return 0;
}
