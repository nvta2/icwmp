/*
*      This program is free software: you can redistribute it and/or modify
*      it under the terms of the GNU General Public License as published by
*      the Free Software Foundation, either version 2 of the License, or
*      (at your option) any later version.
*
*      Copyright (C) 2019 iopsys Software Solutions AB
*		Author: AMIN Ben Ramdhane <amin.benramdhane@pivasoftware.com>
*/

#include <uci.h>
#include <stdio.h>
#include <ctype.h>
#include "dmuci.h"
#include "dmubus.h"
#include "dmcwmp.h"
#include "dmcommon.h"
#include "dmjson.h"
#include "dsl.h"

#define DELIMITOR ","

/* *** Device.DSL. *** */
DMOBJ tDSLObj[] = {
/* OBJ, permission, addobj, delobj, checkobj, browseinstobj, forced_inform, notification, nextobj, leaf, linker*/
{"Line", &DMREAD, NULL, NULL, NULL, browseDSLLineInst, NULL, NULL, tDSLLineObj, tDSLLineParams, get_dsl_line_linker},
{"Channel", &DMREAD, NULL, NULL, NULL, browseDSLChannelInst, NULL, NULL, tDSLChannelObj, tDSLChannelParams, get_dsl_channel_linker},
{0}
};

DMLEAF tDSLParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"LineNumberOfEntries", &DMREAD, DMT_UNINT, get_DSL_LineNumberOfEntries, NULL, NULL, NULL},
{"ChannelNumberOfEntries", &DMREAD, DMT_UNINT, get_DSL_ChannelNumberOfEntries, NULL, NULL, NULL},
{0}
};

/* *** Device.DSL.Line.{i}. *** */
DMOBJ tDSLLineObj[] = {
/* OBJ, permission, addobj, delobj, checkobj, browseinstobj, forced_inform, notification, nextobj, leaf, linker*/
{"Stats", &DMREAD, NULL, NULL, NULL, NULL, NULL, NULL, tDSLLineStatsObj, tDSLLineStatsParams, NULL},
{0}
};

DMLEAF tDSLLineParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"Enable", &DMWRITE, DMT_BOOL, get_DSLLine_Enable, set_DSLLine_Enable, NULL, NULL},
{"Status", &DMREAD, DMT_STRING, get_DSLLine_Status, NULL, NULL, NULL},
{"Alias", &DMWRITE, DMT_STRING, get_DSLLine_Alias, set_DSLLine_Alias, NULL, NULL},
{"Name", &DMREAD, DMT_STRING, get_DSLLine_Name, NULL, NULL, NULL},
{"LowerLayers", &DMWRITE, DMT_STRING, get_DSLLine_LowerLayers, set_DSLLine_LowerLayers, NULL, NULL},
{"Upstream", &DMREAD, DMT_BOOL, get_DSLLine_Upstream, NULL, NULL, NULL},
{"FirmwareVersion", &DMREAD, DMT_STRING, get_DSLLine_FirmwareVersion, NULL, NULL, NULL},
{"LinkStatus", &DMREAD, DMT_STRING, get_DSLLine_LinkStatus, NULL, NULL, NULL},
{"StandardsSupported", &DMREAD, DMT_STRING, get_DSLLine_StandardsSupported, NULL, NULL, NULL},
{"XTSE", &DMREAD, DMT_HEXBIN, get_DSLLine_XTSE, NULL, NULL, NULL},
{"StandardUsed", &DMREAD, DMT_STRING, get_DSLLine_StandardUsed, NULL, NULL, NULL},
{"XTSUsed", &DMREAD, DMT_HEXBIN, get_DSLLine_XTSUsed, NULL, NULL, NULL},
{"LineEncoding", &DMREAD, DMT_STRING, get_DSLLine_LineEncoding, NULL, NULL, NULL},
{"AllowedProfiles", &DMREAD, DMT_STRING, get_DSLLine_AllowedProfiles, NULL, NULL, NULL},
{"CurrentProfile", &DMREAD, DMT_STRING, get_DSLLine_CurrentProfile, NULL, NULL, NULL},
{"PowerManagementState", &DMREAD, DMT_STRING, get_DSLLine_PowerManagementState, NULL, NULL, NULL},
{"SuccessFailureCause", &DMREAD, DMT_UNINT, get_DSLLine_SuccessFailureCause, NULL, NULL, NULL},
{"UPBOKLERPb", &DMREAD, DMT_STRING, get_DSLLine_UPBOKLERPb, NULL, NULL, NULL},
{"RXTHRSHds", &DMREAD, DMT_INT, get_DSLLine_RXTHRSHds, NULL, NULL, NULL},
{"ACTRAMODEds", &DMREAD, DMT_UNINT, get_DSLLine_ACTRAMODEds, NULL, NULL, NULL},
{"ACTRAMODEus", &DMREAD, DMT_UNINT, get_DSLLine_ACTRAMODEus, NULL, NULL, NULL},
{"SNRMROCus", &DMREAD, DMT_UNINT, get_DSLLine_SNRMROCus, NULL, NULL, NULL},
{"LastStateTransmittedDownstream", &DMREAD, DMT_UNINT, get_DSLLine_LastStateTransmittedDownstream, NULL, NULL, NULL},
{"LastStateTransmittedUpstream", &DMREAD, DMT_UNINT, get_DSLLine_LastStateTransmittedUpstream, NULL, NULL, NULL},
{"US0MASK", &DMREAD, DMT_UNINT, get_DSLLine_US0MASK, NULL, NULL, NULL},
{"TRELLISds", &DMREAD, DMT_INT, get_DSLLine_TRELLISds, NULL, NULL, NULL},
{"TRELLISus", &DMREAD, DMT_INT, get_DSLLine_TRELLISus, NULL, NULL, NULL},
{"ACTSNRMODEds", &DMREAD, DMT_UNINT, get_DSLLine_ACTSNRMODEds, NULL, NULL, NULL},
{"ACTSNRMODEus", &DMREAD, DMT_UNINT, get_DSLLine_ACTSNRMODEus, NULL, NULL, NULL},
{"LineNumber", &DMREAD, DMT_INT, get_DSLLine_LineNumber, NULL, NULL, NULL},
{"UpstreamMaxBitRate", &DMREAD, DMT_UNINT, get_DSLLine_UpstreamMaxBitRate, NULL, NULL, NULL},
{"DownstreamMaxBitRate", &DMREAD, DMT_UNINT, get_DSLLine_DownstreamMaxBitRate, NULL, NULL, NULL},
{"UpstreamNoiseMargin", &DMREAD, DMT_INT, get_DSLLine_UpstreamNoiseMargin, NULL, NULL, NULL},
{"DownstreamNoiseMargin", &DMREAD, DMT_INT, get_DSLLine_DownstreamNoiseMargin, NULL, NULL, NULL},
{"SNRMpbus", &DMREAD, DMT_STRING, get_DSLLine_SNRMpbus, NULL, NULL, NULL},
{"SNRMpbds", &DMREAD, DMT_STRING, get_DSLLine_SNRMpbds, NULL, NULL, NULL},
{"UpstreamAttenuation", &DMREAD, DMT_INT, get_DSLLine_UpstreamAttenuation, NULL, NULL, NULL},
{"DownstreamAttenuation", &DMREAD, DMT_INT, get_DSLLine_DownstreamAttenuation, NULL, NULL, NULL},
{"UpstreamPower", &DMREAD, DMT_INT, get_DSLLine_UpstreamPower, NULL, NULL, NULL},
{"DownstreamPower", &DMREAD, DMT_INT, get_DSLLine_DownstreamPower, NULL, NULL, NULL},
{"XTURVendor", &DMREAD, DMT_HEXBIN, get_DSLLine_XTURVendor, NULL, NULL, NULL},
{"XTURCountry", &DMREAD, DMT_HEXBIN, get_DSLLine_XTURCountry, NULL, NULL, NULL},
{"XTURANSIStd", &DMREAD, DMT_UNINT, get_DSLLine_XTURANSIStd, NULL, NULL, NULL},
{"XTURANSIRev", &DMREAD, DMT_UNINT, get_DSLLine_XTURANSIRev, NULL, NULL, NULL},
{"XTUCVendor", &DMREAD, DMT_HEXBIN, get_DSLLine_XTUCVendor, NULL, NULL, NULL},
{"XTUCCountry", &DMREAD, DMT_HEXBIN, get_DSLLine_XTUCCountry, NULL, NULL, NULL},
{"XTUCANSIStd", &DMREAD, DMT_UNINT, get_DSLLine_XTUCANSIStd, NULL, NULL, NULL},
{"XTUCANSIRev", &DMREAD, DMT_UNINT, get_DSLLine_XTUCANSIRev, NULL, NULL, NULL},
{0}
};

/* *** Device.DSL.Line.{i}.Stats. *** */
DMOBJ tDSLLineStatsObj[] = {
/* OBJ, permission, addobj, delobj, checkobj, browseinstobj, forced_inform, notification, nextobj, leaf, linker*/
{"Total", &DMREAD, NULL, NULL, NULL, NULL, NULL, NULL, NULL, tDSLLineStatsTotalParams, NULL},
{"Showtime", &DMREAD, NULL, NULL, NULL, NULL, NULL, NULL, NULL, tDSLLineStatsShowtimeParams, NULL},
{"LastShowtime", &DMREAD, NULL, NULL, NULL, NULL, NULL, NULL, NULL, tDSLLineStatsLastShowtimeParams, NULL},
{"CurrentDay", &DMREAD, NULL, NULL, NULL, NULL, NULL, NULL, NULL, tDSLLineStatsCurrentDayParams, NULL},
{"QuarterHour", &DMREAD, NULL, NULL, NULL, NULL, NULL, NULL, NULL, tDSLLineStatsQuarterHourParams, NULL},
{0}
};

DMLEAF tDSLLineStatsParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"TotalStart", &DMREAD, DMT_UNINT, get_DSLLineStats_TotalStart, NULL, NULL, NULL},
{"ShowtimeStart", &DMREAD, DMT_UNINT, get_DSLLineStats_ShowtimeStart, NULL, NULL, NULL},
{"LastShowtimeStart", &DMREAD, DMT_UNINT, get_DSLLineStats_LastShowtimeStart, NULL, NULL, NULL},
{"CurrentDayStart", &DMREAD, DMT_UNINT, get_DSLLineStats_CurrentDayStart, NULL, NULL, NULL},
{"QuarterHourStart", &DMREAD, DMT_UNINT, get_DSLLineStats_QuarterHourStart, NULL, NULL, NULL},
{0}
};

/* *** Device.DSL.Line.{i}.Stats.Total. *** */
DMLEAF tDSLLineStatsTotalParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"ErroredSecs", &DMREAD, DMT_UNINT, get_DSLLineStatsTotal_ErroredSecs, NULL, NULL, NULL},
{"SeverelyErroredSecs", &DMREAD, DMT_UNINT, get_DSLLineStatsTotal_SeverelyErroredSecs, NULL, NULL, NULL},
{0}
};

/* *** Device.DSL.Line.{i}.Stats.Showtime. *** */
DMLEAF tDSLLineStatsShowtimeParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"ErroredSecs", &DMREAD, DMT_UNINT, get_DSLLineStatsShowtime_ErroredSecs, NULL, NULL, NULL},
{"SeverelyErroredSecs", &DMREAD, DMT_UNINT, get_DSLLineStatsShowtime_SeverelyErroredSecs, NULL, NULL, NULL},
{0}
};

/* *** Device.DSL.Line.{i}.Stats.LastShowtime. *** */
DMLEAF tDSLLineStatsLastShowtimeParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"ErroredSecs", &DMREAD, DMT_UNINT, get_DSLLineStatsLastShowtime_ErroredSecs, NULL, NULL, NULL},
{"SeverelyErroredSecs", &DMREAD, DMT_UNINT, get_DSLLineStatsLastShowtime_SeverelyErroredSecs, NULL, NULL, NULL},
{0}
};

/* *** Device.DSL.Line.{i}.Stats.CurrentDay. *** */
DMLEAF tDSLLineStatsCurrentDayParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"ErroredSecs", &DMREAD, DMT_UNINT, get_DSLLineStatsCurrentDay_ErroredSecs, NULL, NULL, NULL},
{"SeverelyErroredSecs", &DMREAD, DMT_UNINT, get_DSLLineStatsCurrentDay_SeverelyErroredSecs, NULL, NULL, NULL},
{0}
};

/* *** Device.DSL.Line.{i}.Stats.QuarterHour. *** */
DMLEAF tDSLLineStatsQuarterHourParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"ErroredSecs", &DMREAD, DMT_UNINT, get_DSLLineStatsQuarterHour_ErroredSecs, NULL, NULL, NULL},
{"SeverelyErroredSecs", &DMREAD, DMT_UNINT, get_DSLLineStatsQuarterHour_SeverelyErroredSecs, NULL, NULL, NULL},
{0}
};

/* *** Device.DSL.Channel.{i}. *** */
DMOBJ tDSLChannelObj[] = {
/* OBJ, permission, addobj, delobj, checkobj, browseinstobj, forced_inform, notification, nextobj, leaf, linker*/
{"Stats", &DMREAD, NULL, NULL, NULL, NULL, NULL, NULL, tDSLChannelStatsObj, tDSLChannelStatsParams, NULL},
{0}
};

DMLEAF tDSLChannelParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"Enable", &DMWRITE, DMT_BOOL, get_DSLChannel_Enable, set_DSLChannel_Enable, NULL, NULL},
{"Status", &DMREAD, DMT_STRING, get_DSLChannel_Status, NULL, NULL, NULL},
{"Alias", &DMWRITE, DMT_STRING, get_DSLChannel_Alias, set_DSLChannel_Alias, NULL, NULL},
{"Name", &DMREAD, DMT_STRING, get_DSLChannel_Name, NULL, NULL, NULL},
{"LowerLayers", &DMREAD, DMT_STRING, get_DSLChannel_LowerLayers, NULL, NULL, NULL},
{"LinkEncapsulationSupported", &DMREAD, DMT_STRING, get_DSLChannel_LinkEncapsulationSupported, NULL, NULL, NULL},
{"LinkEncapsulationUsed", &DMREAD, DMT_STRING, get_DSLChannel_LinkEncapsulationUsed, NULL, NULL, NULL},
{"LPATH", &DMREAD, DMT_UNINT, get_DSLChannel_LPATH, NULL, NULL, NULL},
{"INTLVDEPTH", &DMREAD, DMT_UNINT, get_DSLChannel_INTLVDEPTH, NULL, NULL, NULL},
{"INTLVBLOCK", &DMREAD, DMT_INT, get_DSLChannel_INTLVBLOCK, NULL, NULL, NULL},
{"ActualInterleavingDelay", &DMREAD, DMT_UNINT, get_DSLChannel_ActualInterleavingDelay, NULL, NULL, NULL},
{"ACTINP", &DMREAD, DMT_INT, get_DSLChannel_ACTINP, NULL, NULL, NULL},
{"INPREPORT", &DMREAD, DMT_BOOL, get_DSLChannel_INPREPORT, NULL, NULL, NULL},
{"NFEC", &DMREAD, DMT_INT, get_DSLChannel_NFEC, NULL, NULL, NULL},
{"RFEC", &DMREAD, DMT_INT, get_DSLChannel_RFEC, NULL, NULL, NULL},
{"LSYMB", &DMREAD, DMT_INT, get_DSLChannel_LSYMB, NULL, NULL, NULL},
{"UpstreamCurrRate", &DMREAD, DMT_UNINT, get_DSLChannel_UpstreamCurrRate, NULL, NULL, NULL},
{"DownstreamCurrRate", &DMREAD, DMT_UNINT, get_DSLChannel_DownstreamCurrRate, NULL, NULL, NULL},
{"ACTNDR", &DMREAD, DMT_UNINT, get_DSLChannel_ACTNDR, NULL, NULL, NULL},
{"ACTINPREIN", &DMREAD, DMT_UNINT, get_DSLChannel_ACTINPREIN, NULL, NULL, NULL},
{0}
};

/* *** Device.DSL.Channel.{i}.Stats. *** */
DMOBJ tDSLChannelStatsObj[] = {
/* OBJ, permission, addobj, delobj, checkobj, browseinstobj, forced_inform, notification, nextobj, leaf, linker*/
{"Total", &DMREAD, NULL, NULL, NULL, NULL, NULL, NULL, NULL, tDSLChannelStatsTotalParams, NULL},
{"Showtime", &DMREAD, NULL, NULL, NULL, NULL, NULL, NULL, NULL, tDSLChannelStatsShowtimeParams, NULL},
{"LastShowtime", &DMREAD, NULL, NULL, NULL, NULL, NULL, NULL, NULL, tDSLChannelStatsLastShowtimeParams, NULL},
{"CurrentDay", &DMREAD, NULL, NULL, NULL, NULL, NULL, NULL, NULL, tDSLChannelStatsCurrentDayParams, NULL},
{"QuarterHour", &DMREAD, NULL, NULL, NULL, NULL, NULL, NULL, NULL, tDSLChannelStatsQuarterHourParams, NULL},
{0}
};

DMLEAF tDSLChannelStatsParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"TotalStart", &DMREAD, DMT_UNINT, get_DSLChannelStats_TotalStart, NULL, NULL, NULL},
{"ShowtimeStart", &DMREAD, DMT_UNINT, get_DSLChannelStats_ShowtimeStart, NULL, NULL, NULL},
{"LastShowtimeStart", &DMREAD, DMT_UNINT, get_DSLChannelStats_LastShowtimeStart, NULL, NULL, NULL},
{"CurrentDayStart", &DMREAD, DMT_UNINT, get_DSLChannelStats_CurrentDayStart, NULL, NULL, NULL},
{"QuarterHourStart", &DMREAD, DMT_UNINT, get_DSLChannelStats_QuarterHourStart, NULL, NULL, NULL},
{0}
};

/* *** Device.DSL.Channel.{i}.Stats.Total. *** */
DMLEAF tDSLChannelStatsTotalParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"XTURFECErrors", &DMREAD, DMT_UNINT, get_DSLChannelStatsTotal_XTURFECErrors, NULL, NULL, NULL},
{"XTUCFECErrors", &DMREAD, DMT_UNINT, get_DSLChannelStatsTotal_XTUCFECErrors, NULL, NULL, NULL},
{"XTURHECErrors", &DMREAD, DMT_UNINT, get_DSLChannelStatsTotal_XTURHECErrors, NULL, NULL, NULL},
{"XTUCHECErrors", &DMREAD, DMT_UNINT, get_DSLChannelStatsTotal_XTUCHECErrors, NULL, NULL, NULL},
{"XTURCRCErrors", &DMREAD, DMT_UNINT, get_DSLChannelStatsTotal_XTURCRCErrors, NULL, NULL, NULL},
{"XTUCCRCErrors", &DMREAD, DMT_UNINT, get_DSLChannelStatsTotal_XTUCCRCErrors, NULL, NULL, NULL},
{0}
};

/* *** Device.DSL.Channel.{i}.Stats.Showtime. *** */
DMLEAF tDSLChannelStatsShowtimeParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"XTURFECErrors", &DMREAD, DMT_UNINT, get_DSLChannelStatsShowtime_XTURFECErrors, NULL, NULL, NULL},
{"XTUCFECErrors", &DMREAD, DMT_UNINT, get_DSLChannelStatsShowtime_XTUCFECErrors, NULL, NULL, NULL},
{"XTURHECErrors", &DMREAD, DMT_UNINT, get_DSLChannelStatsShowtime_XTURHECErrors, NULL, NULL, NULL},
{"XTUCHECErrors", &DMREAD, DMT_UNINT, get_DSLChannelStatsShowtime_XTUCHECErrors, NULL, NULL, NULL},
{"XTURCRCErrors", &DMREAD, DMT_UNINT, get_DSLChannelStatsShowtime_XTURCRCErrors, NULL, NULL, NULL},
{"XTUCCRCErrors", &DMREAD, DMT_UNINT, get_DSLChannelStatsShowtime_XTUCCRCErrors, NULL, NULL, NULL},
{0}
};

/* *** Device.DSL.Channel.{i}.Stats.LastShowtime. *** */
DMLEAF tDSLChannelStatsLastShowtimeParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"XTURFECErrors", &DMREAD, DMT_UNINT, get_DSLChannelStatsLastShowtime_XTURFECErrors, NULL, NULL, NULL},
{"XTUCFECErrors", &DMREAD, DMT_UNINT, get_DSLChannelStatsLastShowtime_XTUCFECErrors, NULL, NULL, NULL},
{"XTURHECErrors", &DMREAD, DMT_UNINT, get_DSLChannelStatsLastShowtime_XTURHECErrors, NULL, NULL, NULL},
{"XTUCHECErrors", &DMREAD, DMT_UNINT, get_DSLChannelStatsLastShowtime_XTUCHECErrors, NULL, NULL, NULL},
{"XTURCRCErrors", &DMREAD, DMT_UNINT, get_DSLChannelStatsLastShowtime_XTURCRCErrors, NULL, NULL, NULL},
{"XTUCCRCErrors", &DMREAD, DMT_UNINT, get_DSLChannelStatsLastShowtime_XTUCCRCErrors, NULL, NULL, NULL},
{0}
};

/* *** Device.DSL.Channel.{i}.Stats.CurrentDay. *** */
DMLEAF tDSLChannelStatsCurrentDayParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"XTURFECErrors", &DMREAD, DMT_UNINT, get_DSLChannelStatsCurrentDay_XTURFECErrors, NULL, NULL, NULL},
{"XTUCFECErrors", &DMREAD, DMT_UNINT, get_DSLChannelStatsCurrentDay_XTUCFECErrors, NULL, NULL, NULL},
{"XTURHECErrors", &DMREAD, DMT_UNINT, get_DSLChannelStatsCurrentDay_XTURHECErrors, NULL, NULL, NULL},
{"XTUCHECErrors", &DMREAD, DMT_UNINT, get_DSLChannelStatsCurrentDay_XTUCHECErrors, NULL, NULL, NULL},
{"XTURCRCErrors", &DMREAD, DMT_UNINT, get_DSLChannelStatsCurrentDay_XTURCRCErrors, NULL, NULL, NULL},
{"XTUCCRCErrors", &DMREAD, DMT_UNINT, get_DSLChannelStatsCurrentDay_XTUCCRCErrors, NULL, NULL, NULL},
{0}
};

/* *** Device.DSL.Channel.{i}.Stats.QuarterHour. *** */
DMLEAF tDSLChannelStatsQuarterHourParams[] = {
/* PARAM, permission, type, getvlue, setvalue, forced_inform, notification*/
{"XTURFECErrors", &DMREAD, DMT_UNINT, get_DSLChannelStatsQuarterHour_XTURFECErrors, NULL, NULL, NULL},
{"XTUCFECErrors", &DMREAD, DMT_UNINT, get_DSLChannelStatsQuarterHour_XTUCFECErrors, NULL, NULL, NULL},
{"XTURHECErrors", &DMREAD, DMT_UNINT, get_DSLChannelStatsQuarterHour_XTURHECErrors, NULL, NULL, NULL},
{"XTUCHECErrors", &DMREAD, DMT_UNINT, get_DSLChannelStatsQuarterHour_XTUCHECErrors, NULL, NULL, NULL},
{"XTURCRCErrors", &DMREAD, DMT_UNINT, get_DSLChannelStatsQuarterHour_XTURCRCErrors, NULL, NULL, NULL},
{"XTUCCRCErrors", &DMREAD, DMT_UNINT, get_DSLChannelStatsQuarterHour_XTUCCRCErrors, NULL, NULL, NULL},
{0}
};

/**************************************************************************
* LINKER
***************************************************************************/
int get_dsl_line_linker(char *refparam, struct dmctx *dmctx, void *data, char *instance, char **linker)
{
	if (data && ((struct dsl_line_args *)data)->id) {
		dmasprintf(linker, "line_%s", ((struct dsl_line_args *)data)->id);
		return 0;
	}
	*linker = "" ;
	return 0;
}

int get_dsl_channel_linker(char *refparam, struct dmctx *dmctx, void *data, char *instance, char **linker)
{
	if (data && ((struct dsl_line_args *)data)->id){
		dmasprintf(linker, "channel_%s", ((struct dsl_line_args *)data)->id);
		return 0;
	}
	*linker = "" ;
	return 0;
}

/**************************************************************************
* INIT
***************************************************************************/
inline int init_dsl_line(struct dsl_line_args *args, struct uci_section *s)
{
	args->line_sec = s;
	return 0;
}

inline int init_dsl_channel(struct dsl_channel_args *args, struct uci_section *s)
{
	args->channel_sec = s;
	return 0;
}

/*************************************************************/
static struct uci_section *update_create_dmmap_dsl_line(char *curr_id)
{
	struct uci_section *s = NULL;
	char *name, *instance;

	uci_path_foreach_option_eq(icwmpd, "dmmap", "dsl_line", "id", curr_id, s) {
		return s;
	}
	if (!s) {
		dmasprintf(&instance, "%d", atoi(curr_id)+1);
		DMUCI_ADD_SECTION(icwmpd, "dmmap", "dsl_line", &s, &name);
		DMUCI_SET_VALUE_BY_SECTION(icwmpd, s, "id", curr_id);
		DMUCI_SET_VALUE_BY_SECTION(icwmpd, s, "dsl_line_instance", instance);
		dmfree(instance);
	}
	return s;
}

static struct uci_section *update_create_dmmap_dsl_channel(char *curr_id)
{
	struct uci_section *s = NULL;
	char *name, *instance;

	uci_path_foreach_option_eq(icwmpd, "dmmap", "dsl_channel", "id", curr_id, s) {
		return s;
	}
	if (!s) {
		dmasprintf(&instance, "%d", atoi(curr_id)+1);
		DMUCI_ADD_SECTION(icwmpd, "dmmap", "dsl_channel", &s, &name);
		DMUCI_SET_VALUE_BY_SECTION(icwmpd, s, "id", curr_id);
		DMUCI_SET_VALUE_BY_SECTION(icwmpd, s, "dsl_channel_instance", instance);
		dmfree(instance);
	}
	return s;
}
/*************************************************************
 * ENTRY METHOD
/*************************************************************/
int browseDSLLineInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	json_object *res = NULL, *line_obj = NULL;
	struct dsl_line_args cur_dsl_line_args = {0};
	struct uci_section *s = NULL;
	char *dsl_int = NULL, *dsl_int_last = NULL;
	int entries = 0;

	dmubus_call("dsl", "status", UBUS_ARGS{}, 0, &res);
	while (res) {
		line_obj = dmjson_select_obj_in_array_idx(res, entries, 1, "line");
		if(line_obj) {
			cur_dsl_line_args.id = dmjson_get_value(line_obj, 1, "id");
			entries++;
			s = update_create_dmmap_dsl_line(cur_dsl_line_args.id);
			init_dsl_line(&cur_dsl_line_args, s);
			dsl_int = handle_update_instance(1, dmctx, &dsl_int_last, update_instance_alias, 3, s, "dsl_line_instance", "dsl_line_alias");
			if (DM_LINK_INST_OBJ(dmctx, parent_node, (void *)&cur_dsl_line_args, dsl_int) == DM_STOP)
				break;
		}
		else
			break;
	}
	return 0;
}

int browseDSLChannelInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)
{
	json_object *res = NULL, *line_obj = NULL, *channel_obj = NULL;
	struct dsl_channel_args cur_dsl_channel_args = {0};
	struct uci_section *s = NULL;
	char *dsl_int = NULL, *dsl_int_last = NULL;
	int entries_line = 0, entries_channel = 0;

	dmubus_call("dsl", "status", UBUS_ARGS{}, 0, &res);
	while (res) {
		line_obj = dmjson_select_obj_in_array_idx(res, entries_line, 1, "line");
		while (line_obj) {
			channel_obj = dmjson_select_obj_in_array_idx(line_obj, entries_channel, 1, "channel");
			if(channel_obj) {
				cur_dsl_channel_args.id = dmjson_get_value(channel_obj, 1, "id");
				entries_channel++;
				s = update_create_dmmap_dsl_channel(cur_dsl_channel_args.id);
				init_dsl_channel(&cur_dsl_channel_args, s);
				dsl_int = handle_update_instance(1, dmctx, &dsl_int_last, update_instance_alias, 3, s, "dsl_channel_instance", "dsl_channel_alias");
				if (DM_LINK_INST_OBJ(dmctx, parent_node, (void *)&cur_dsl_channel_args, dsl_int) == DM_STOP)
					break;
			}
			else
				break;
		}
		entries_line++;
		if(!line_obj)
			break;
	}
	return 0;
}

/**************************************************************************
* COMMON FUNCTIONS
***************************************************************************/
static char *get_dsl_value_without_argument(char *command1, char *id, char *command2, char *key)
{
	json_object *res;
	char command[16], *value;

	sprintf(command, "%s.%s", command1, id);
	dmubus_call(command, command2, UBUS_ARGS{}, 0, &res);
	DM_ASSERT(res, value = "0");
	value = dmjson_get_value(res, 1, key);
	return value;
}

static char *get_dsl_value_without_argument_and_with_two_key(char *command1, char *id, char *command2, char *key1, char *key2)
{
	json_object *res;
	char command[16], *value;

	sprintf(command, "%s.%s", command1, id);
	dmubus_call(command, command2, UBUS_ARGS{}, 0, &res);
	DM_ASSERT(res, value = "0");
	value = dmjson_get_value(res, 2, key1, key2);
	return value;
}

static char *get_dsl_value_with_argument(char *command1, char *id, char *command2, char *argument, char *key)
{
	json_object *res;
	char command[16], *value;

	sprintf(command, "%s.%s", command1, id);
	dmubus_call(command, command2, UBUS_ARGS{{"interval", argument, String}}, 1, &res);
	DM_ASSERT(res, value = "0");
	value = dmjson_get_value(res, 1, key);
	return value;
}

static char *get_dsl_value_array_without_argument(char *command1, char *id, char *command2, char *key)
{
	json_object *res;
	char command[16], *value;

	sprintf(command, "%s.%s", command1, id);
	dmubus_call(command, command2, UBUS_ARGS{}, 0, &res);
	DM_ASSERT(res, value = "0");
	value = dmjson_get_value_array_all(res, DELIMITOR, 1, key);
	return value;
}

/**************************************************************************
* GET & SET DSL PARAMETERS
***************************************************************************/
int get_DSL_LineNumberOfEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *s = NULL;
	int cnt = 0;

	uci_path_foreach_sections(icwmpd, "dmmap", "dsl_line", s) {
		cnt++;
	}
	dmasprintf(value, "%d", cnt);
	return 0;
}

int get_DSL_ChannelNumberOfEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	struct uci_section *s = NULL;
	int cnt = 0;

	uci_path_foreach_sections(icwmpd, "dmmap", "dsl_channel", s) {
		cnt++;
	}
	dmasprintf(value, "%d", cnt);
	return 0;;
}

int get_DSLLine_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *status = get_dsl_value_without_argument("dsl.line", ((struct dsl_line_args*)data)->id, "status", "status");
		*value = (strcmp(status, "up") == 0) ? "1" : "0";
		return 0;
}

int set_DSLLine_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			break;
	}
	return 0;
}

int get_DSLLine_Status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *status = get_dsl_value_without_argument("dsl.line", ((struct dsl_line_args*)data)->id, "status", "status");
	*value = (strcmp(status, "up") == 0) ? "Up" : "Down";
	return 0;
}

int get_DSLLine_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_value_by_section_string(((struct dsl_line_args *)data)->line_sec, "dsl_line_alias", value);
	return 0;
}

int set_DSLLine_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			dmuci_set_value_by_section(((struct dsl_line_args *)data)->line_sec, "dsl_line_alias", value);
			break;
	}
	return 0;
}

int get_DSLLine_Name(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = ((struct dsl_line_args*)data)->id;
	return 0;
}

int get_DSLLine_LowerLayers(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = "";
	return 0;
}

int set_DSLLine_LowerLayers(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			break;
	}
	return 0;
}

int get_DSLLine_Upstream(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_without_argument("dsl.line", ((struct dsl_line_args*)data)->id, "status", "upstream");
	return 0;
}

int get_DSLLine_FirmwareVersion(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_without_argument("dsl.line", ((struct dsl_line_args*)data)->id, "status", "firmware_version");
	return 0;
}

int get_DSLLine_LinkStatus(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *link_status = get_dsl_value_without_argument("dsl.line", ((struct dsl_line_args*)data)->id, "status", "link_status");
	if(strcmp(link_status, "up") == 0)
		*value = "Up";
	else if(strcmp(link_status, "initializing") == 0)
		*value = "Initializing";
	else if(strcmp(link_status, "no_signal") == 0)
		*value = "NoSignal";
	else if(strcmp(link_status, "disabled") == 0)
		*value = "Disabled";
	else if(strcmp(link_status, "establishing") == 0)
		*value = "EstablishingLink";
	else if(strcmp(link_status, "error") == 0)
		*value = "Error";
	else
		*value = link_status;
	return 0;
}

static char *get_dsl_standard(char *str)
{
	char *dsl_standard;

	if(strcmp(str, "gdmt_annexa") == 0)
		dsl_standard = "G.992.1_Annex_A";
	else if(strcmp(str, "gdmt_annexb") == 0)
		dsl_standard = "G.992.1_Annex_B";
	else if(strcmp(str, "gdmt_annexc") == 0)
		dsl_standard = "G.992.1_Annex_C";
	else if(strcmp(str, "t1413") == 0)
		dsl_standard = "T1.413";
	else if(strcmp(str, "t1413_i2") == 0)
		dsl_standard = "T1.413i2";
	else if(strcmp(str, "glite") == 0)
		dsl_standard = "G.992.2";
	else if(strcmp(str, "etsi_101_388") == 0)
		dsl_standard = "ETSI_101_388";
	else if(strcmp(str, "adsl2_annexa") == 0)
		dsl_standard = "G.992.3_Annex_A";
	else if(strcmp(str, "adsl2_annexb") == 0)
		dsl_standard = "G.992.3_Annex_B";
	else if(strcmp(str, "adsl2_annexc") == 0)
		dsl_standard = "G.992.3_Annex_C";
	else if(strcmp(str, "adsl2_annexi") == 0)
		dsl_standard = "G.992.3_Annex_I";
	else if(strcmp(str, "adsl2_annexj") == 0)
		dsl_standard = "G.992.3_Annex_J";
	else if(strcmp(str, "adsl2_annexl") == 0)
		dsl_standard = "G.992.3_Annex_L";
	else if(strcmp(str, "adsl2_annexm") == 0)
		dsl_standard = "G.992.3_Annex_M";
	else if(strcmp(str, "splitterless_adsl2") == 0)
		dsl_standard = "G.992.4";
	else if(strcmp(str, "adsl2p_annexa") == 0)
		dsl_standard = "G.992.5_Annex_A";
	else if(strcmp(str, "adsl2p_annexb") == 0)
		dsl_standard = "G.992.5_Annex_B";
	else if(strcmp(str, "adsl2p_annexc") == 0)
		dsl_standard = "G.992.5_Annex_C";
	else if(strcmp(str, "adsl2p_annexi") == 0)
		dsl_standard = "G.992.5_Annex_I";
	else if(strcmp(str, "adsl2p_annexj") == 0)
		dsl_standard = "G.992.5_Annex_J";
	else if(strcmp(str, "adsl2p_annexm") == 0)
		dsl_standard = "G.992.5_Annex_M";
	else if(strcmp(str, "vdsl") == 0)
		dsl_standard = "G.993.1";
	else if(strcmp(str, "vdsl2_annexa") == 0)
		dsl_standard = "G.993.2_Annex_A";
	else if(strcmp(str, "vdsl2_annexb") == 0)
		dsl_standard = "G.993.2_Annex_B";
	else if(strcmp(str, "vdsl2_annexc") == 0)
		dsl_standard = "G.993.2_Annex_C";
	else
		dsl_standard = str;

	return dsl_standard;
}

int get_DSLLine_StandardsSupported(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *standards_supported,*pch, *spch, *tmp, *tmpPtr, *str = "";

	*value = "";
	standards_supported = get_dsl_value_array_without_argument("dsl.line", ((struct dsl_line_args*)data)->id, "status", "standards_supported");
	if(standards_supported[0] == '\0')
		return 0;
	for (pch = strtok_r(standards_supported, ",", &spch); pch != NULL; pch = strtok_r(NULL, ",", &spch))
	{
		tmp = get_dsl_standard(pch);
		if(*str == '\0')
			dmasprintf(&str, "%s", tmp);
		else {
			tmpPtr = str;
			dmasprintf(&str, "%s,%s", tmpPtr, tmp);
		}
	}
	*value = str;
	return 0;
}

int get_DSLLine_XTSE(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *xtse,*pch, *spch, *tmpPtr, *str = "";

	*value = "";
	xtse = get_dsl_value_array_without_argument("dsl.line", ((struct dsl_line_args*)data)->id, "status", "xtse");
	if(xtse[0] == '\0')
		return 0;
	for (pch = strtok_r(xtse, ",", &spch); pch != NULL; pch = strtok_r(NULL, ",", &spch))
	{
		if(*str == '\0')
			dmasprintf(&str, "%s", pch);
		else {
			tmpPtr = str;
			dmasprintf(&str, "%s%s", tmpPtr, pch);
		}
	}
	*value = str;
	return 0;
}

int get_DSLLine_StandardUsed(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *standard_used = get_dsl_value_without_argument("dsl.line", ((struct dsl_line_args*)data)->id, "status", "standard_used");
	*value = get_dsl_standard(standard_used);
	return 0;
}

int get_DSLLine_XTSUsed(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *xtse_used,*pch, *spch, *tmpPtr, *str = "";

	*value = "";
	xtse_used = get_dsl_value_array_without_argument("dsl.line", ((struct dsl_line_args*)data)->id, "status", "xtse_used");
	if(xtse_used[0] == '\0')
		return 0;
	for (pch = strtok_r(xtse_used, ",", &spch); pch != NULL; pch = strtok_r(NULL, ",", &spch))
	{
		if(*str == '\0')
			dmasprintf(&str, "%s", pch);
		else {
			tmpPtr = str;
			dmasprintf(&str, "%s%s", tmpPtr, pch);
		}
	}
	*value = str;
	return 0;
}

int get_DSLLine_LineEncoding(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *line_encoding = get_dsl_value_without_argument("dsl.line", ((struct dsl_line_args*)data)->id, "status", "line_encoding");
	if(strcmp(line_encoding, "dmt") == 0)
		*value = "DMT";
	else if(strcmp(line_encoding, "cap") == 0)
		*value = "CAP";
	else if(strcmp(line_encoding, "2b1q") == 0)
		*value = "2B1Q";
	else if(strcmp(line_encoding, "43bt") == 0)
		*value = "43BT";
	else if(strcmp(line_encoding, "pam") == 0)
		*value = "PAM";
	else if(strcmp(line_encoding, "qam") == 0)
		*value = "QAM";
	else
		*value = line_encoding;
	return 0;
}

int get_DSLLine_AllowedProfiles(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_array_without_argument("dsl.line", ((struct dsl_line_args*)data)->id, "status", "allowed_profiles");
	return 0;
}

int get_DSLLine_CurrentProfile(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_without_argument("dsl.line", ((struct dsl_line_args*)data)->id, "status", "current_profile");
	return 0;
}

int get_DSLLine_PowerManagementState(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *power_management_state = get_dsl_value_without_argument("dsl.line", ((struct dsl_line_args*)data)->id, "status", "power_management_state");
	if(strcmp(power_management_state, "l0") == 0)
		*value = "L0";
	else if(strcmp(power_management_state, "l1") == 0)
		*value = "L1";
	else if(strcmp(power_management_state, "l2") == 0)
		*value = "L2";
	else if(strcmp(power_management_state, "l3") == 0)
		*value = "L3";
	else if(strcmp(power_management_state, "l4") == 0)
		*value = "L4";
	else
		*value = power_management_state;
	return 0;
}

int get_DSLLine_SuccessFailureCause(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_without_argument("dsl.line", ((struct dsl_line_args*)data)->id, "status", "success_failure_cause");
	return 0;
}

int get_DSLLine_UPBOKLERPb(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_array_without_argument("dsl.line", ((struct dsl_line_args*)data)->id, "status", "upbokler_pb");
	return 0;
}

int get_DSLLine_RXTHRSHds(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_array_without_argument("dsl.line", ((struct dsl_line_args*)data)->id, "status", "rxthrsh_ds");
	return 0;
}

int get_DSLLine_ACTRAMODEds(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_without_argument_and_with_two_key("dsl.line", ((struct dsl_line_args*)data)->id, "status", "act_ra_mode", "ds");
	return 0;
}

int get_DSLLine_ACTRAMODEus(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_without_argument_and_with_two_key("dsl.line", ((struct dsl_line_args*)data)->id, "status", "act_ra_mode", "us");
	return 0;
}

int get_DSLLine_SNRMROCus(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_without_argument("dsl.line", ((struct dsl_line_args*)data)->id, "status", "snr_mroc_us");
	return 0;
}

int get_DSLLine_LastStateTransmittedDownstream(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_without_argument_and_with_two_key("dsl.line", ((struct dsl_line_args*)data)->id, "status", "last_state_transmitted", "ds");
	return 0;
}

int get_DSLLine_LastStateTransmittedUpstream(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_without_argument_and_with_two_key("dsl.line", ((struct dsl_line_args*)data)->id, "status", "last_state_transmitted", "us");
	return 0;
}

int get_DSLLine_US0MASK(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_without_argument("dsl.line", ((struct dsl_line_args*)data)->id, "status", "us0_mask");
	return 0;
}

int get_DSLLine_TRELLISds(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_without_argument_and_with_two_key("dsl.line", ((struct dsl_line_args*)data)->id, "status", "trellis", "ds");
	return 0;
}

int get_DSLLine_TRELLISus(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_without_argument_and_with_two_key("dsl.line", ((struct dsl_line_args*)data)->id, "status", "trellis", "us");
	return 0;
}

int get_DSLLine_ACTSNRMODEds(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_without_argument_and_with_two_key("dsl.line", ((struct dsl_line_args*)data)->id, "status", "act_snr_mode", "ds");
	return 0;
}

int get_DSLLine_ACTSNRMODEus(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_without_argument_and_with_two_key("dsl.line", ((struct dsl_line_args*)data)->id, "status", "act_snr_mode", "us");
	return 0;
}

int get_DSLLine_LineNumber(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_without_argument("dsl.line", ((struct dsl_line_args*)data)->id, "status", "line_number");
	return 0;
}

int get_DSLLine_UpstreamMaxBitRate(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_without_argument_and_with_two_key("dsl.line", ((struct dsl_line_args*)data)->id, "status", "max_bit_rate", "us");
	return 0;
}

int get_DSLLine_DownstreamMaxBitRate(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_without_argument_and_with_two_key("dsl.line", ((struct dsl_line_args*)data)->id, "status", "max_bit_rate", "ds");
	return 0;
}

int get_DSLLine_UpstreamNoiseMargin(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_without_argument_and_with_two_key("dsl.line", ((struct dsl_line_args*)data)->id, "status", "noise_margin", "us");
	return 0;
}

int get_DSLLine_DownstreamNoiseMargin(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_without_argument_and_with_two_key("dsl.line", ((struct dsl_line_args*)data)->id, "status", "noise_margin", "ds");
	return 0;
}

int get_DSLLine_SNRMpbus(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_array_without_argument("dsl.line", ((struct dsl_line_args*)data)->id, "status", "snr_mpb_us");
	return 0;
}

int get_DSLLine_SNRMpbds(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_array_without_argument("dsl.line", ((struct dsl_line_args*)data)->id, "status", "snr_mpb_ds");
	return 0;
}

int get_DSLLine_UpstreamAttenuation(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_without_argument_and_with_two_key("dsl.line", ((struct dsl_line_args*)data)->id, "status", "attenuation", "us");
	return 0;
}

int get_DSLLine_DownstreamAttenuation(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_without_argument_and_with_two_key("dsl.line", ((struct dsl_line_args*)data)->id, "status", "attenuation", "ds");
	return 0;
}

int get_DSLLine_UpstreamPower(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_without_argument_and_with_two_key("dsl.line", ((struct dsl_line_args*)data)->id, "status", "power", "us");
	return 0;
}

int get_DSLLine_DownstreamPower(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_without_argument_and_with_two_key("dsl.line", ((struct dsl_line_args*)data)->id, "status", "power", "ds");
	return 0;
}

int get_DSLLine_XTURVendor(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_without_argument("dsl.line", ((struct dsl_line_args*)data)->id, "status", "xtur_vendor");
	return 0;
}

int get_DSLLine_XTURCountry(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_without_argument("dsl.line", ((struct dsl_line_args*)data)->id, "status", "xtur_country");
	return 0;
}

int get_DSLLine_XTURANSIStd(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_without_argument("dsl.line", ((struct dsl_line_args*)data)->id, "status", "xtur_ansi_std");
	return 0;
}

int get_DSLLine_XTURANSIRev(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_without_argument("dsl.line", ((struct dsl_line_args*)data)->id, "status", "xtur_ansi_rev");
	return 0;
}

int get_DSLLine_XTUCVendor(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_without_argument("dsl.line", ((struct dsl_line_args*)data)->id, "status", "xtuc_vendor");
	return 0;
}

int get_DSLLine_XTUCCountry(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_without_argument("dsl.line", ((struct dsl_line_args*)data)->id, "status", "xtuc_country");
	return 0;
}

int get_DSLLine_XTUCANSIStd(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_without_argument("dsl.line", ((struct dsl_line_args*)data)->id, "status", "xtuc_ansi_std");
	return 0;
}

int get_DSLLine_XTUCANSIRev(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_without_argument("dsl.line",((struct dsl_line_args*)data)->id, "status", "xtuc_ansi_rev");
	return 0;
}

int get_DSLLineStats_TotalStart(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_without_argument("dsl.line", ((struct dsl_line_args*)data)->id, "stats", "total_start");
	return 0;
}

int get_DSLLineStats_ShowtimeStart(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_without_argument("dsl.line", ((struct dsl_line_args*)data)->id, "stats", "showtime_start");
	return 0;
}

int get_DSLLineStats_LastShowtimeStart(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_without_argument("dsl.line", ((struct dsl_line_args*)data)->id, "stats", "last_showtime_start");
	return 0;
}

int get_DSLLineStats_CurrentDayStart(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_without_argument("dsl.line", ((struct dsl_line_args*)data)->id, "stats", "current_day_start");
	return 0;
}

int get_DSLLineStats_QuarterHourStart(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_without_argument("dsl.line", ((struct dsl_line_args*)data)->id, "stats", "quarter_hour_start");
	return 0;
}

int get_DSLLineStatsTotal_ErroredSecs(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_with_argument("dsl.line", ((struct dsl_line_args*)data)->id, "stats", "total", "errored_secs");
	return 0;
}

int get_DSLLineStatsTotal_SeverelyErroredSecs(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_with_argument("dsl.line", ((struct dsl_line_args*)data)->id, "stats", "total", "severely_errored_secs");
	return 0;
}

int get_DSLLineStatsShowtime_ErroredSecs(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_with_argument("dsl.line", ((struct dsl_line_args*)data)->id, "stats", "showtime", "errored_secs");
	return 0;
}

int get_DSLLineStatsShowtime_SeverelyErroredSecs(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_with_argument("dsl.line", ((struct dsl_line_args*)data)->id, "stats", "showtime", "severely_errored_secs");
	return 0;
}

int get_DSLLineStatsLastShowtime_ErroredSecs(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_with_argument("dsl.line", ((struct dsl_line_args*)data)->id, "stats", "lastshowtime", "errored_secs");
	return 0;
}

int get_DSLLineStatsLastShowtime_SeverelyErroredSecs(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_with_argument("dsl.line", ((struct dsl_line_args*)data)->id, "stats", "lastshowtime", "severely_errored_secs");
	return 0;
}

int get_DSLLineStatsCurrentDay_ErroredSecs(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_with_argument("dsl.line", ((struct dsl_line_args*)data)->id, "stats", "currentday", "errored_secs");
	return 0;
}

int get_DSLLineStatsCurrentDay_SeverelyErroredSecs(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_with_argument("dsl.line", ((struct dsl_line_args*)data)->id, "stats", "currentday", "severely_errored_secs");
	return 0;
}

int get_DSLLineStatsQuarterHour_ErroredSecs(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_with_argument("dsl.line", ((struct dsl_line_args*)data)->id, "stats", "quarterhour", "errored_secs");
	return 0;
}

int get_DSLLineStatsQuarterHour_SeverelyErroredSecs(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_with_argument("dsl.line", ((struct dsl_line_args*)data)->id, "stats", "quarterhour", "severely_errored_secs");
	return 0;
}

int get_DSLChannel_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *status = get_dsl_value_without_argument("dsl.channel", ((struct dsl_channel_args*)data)->id, "status", "status");
	*value = (strcmp(status, "up") == 0) ? "1" : "0";
	return 0;
}

int set_DSLChannel_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			break;
	}
	return 0;
}

int get_DSLChannel_Status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *status = get_dsl_value_without_argument("dsl.channel", ((struct dsl_channel_args*)data)->id, "status", "status");
	*value = (strcmp(status, "up") == 0) ? "Up" : "Down";
	return 0;
}

int get_DSLChannel_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	dmuci_get_value_by_section_string(((struct dsl_channel_args *)data)->channel_sec, "dsl_channel_alias", value);
	return 0;
}

int set_DSLChannel_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)
{
	switch (action)	{
		case VALUECHECK:
			break;
		case VALUESET:
			dmuci_set_value_by_section(((struct dsl_channel_args *)data)->channel_sec, "dsl_channel_alias", value);
			break;
	}
	return 0;
}

int get_DSLChannel_Name(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = ((struct dsl_channel_args*)data)->id;
	return 0;
}

int get_DSLChannel_LowerLayers(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char linker[8];
	sprintf(linker, "line_%s", ((struct dsl_line_args *)data)->id);
	adm_entry_get_linker_param(ctx, dm_print_path("%s%cDSL%cLine%c", dmroot, dm_delim, dm_delim, dm_delim), linker, value); // MEM WILL BE FREED IN DMMEMCLEAN
	if (*value == NULL)
		*value = "";
	return 0;
}

static char *get_dsl_link_encapsulation_standard(char *str)
{
	char *dsl_link_encapsulation_standard;

	if(strcmp(str, "adsl2_atm") == 0)
		dsl_link_encapsulation_standard = "G.992.3_Annex_K_ATM";
	else if(strcmp(str, "adsl2_ptm") == 0)
		dsl_link_encapsulation_standard = "G.992.3_Annex_K_PTM";
	else if(strcmp(str, "vdsl2_atm") == 0)
		dsl_link_encapsulation_standard = "G.993.2_Annex_K_ATM";
	else if(strcmp(str, "vdsl2_ptm") == 0)
		dsl_link_encapsulation_standard = "G.993.2_Annex_K_PTM";
	else if(strcmp(str, "auto") == 0)
		dsl_link_encapsulation_standard = "G.994.1";

	return dsl_link_encapsulation_standard;
}

int get_DSLChannel_LinkEncapsulationSupported(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *link_encapsulation_supported,*pch, *spch, *tmp, *tmpPtr, *str = "";

	*value = "";
	link_encapsulation_supported = get_dsl_value_array_without_argument("dsl.channel", ((struct dsl_channel_args*)data)->id, "status", "link_encapsulation_supported");
	if(link_encapsulation_supported[0] == '\0')
		return 0;
	for (pch = strtok_r(link_encapsulation_supported, ",", &spch); pch != NULL; pch = strtok_r(NULL, ",", &spch))
	{
		tmp = get_dsl_link_encapsulation_standard(pch);
		if(*str == '\0')
			dmasprintf(&str, "%s", tmp);
		else {
			tmpPtr = str;
			dmasprintf(&str, "%s,%s", tmpPtr, tmp);
		}
	}
	*value = str;
	return 0;
}

int get_DSLChannel_LinkEncapsulationUsed(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	char *link_encapsulation_used = get_dsl_value_without_argument("dsl.channel", ((struct dsl_channel_args*)data)->id, "status", "link_encapsulation_used");
	*value = get_dsl_link_encapsulation_standard(link_encapsulation_used);
	return 0;
}

int get_DSLChannel_LPATH(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_without_argument("dsl.channel", ((struct dsl_channel_args*)data)->id, "status", "lpath");
	return 0;
}

int get_DSLChannel_INTLVDEPTH(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_without_argument("dsl.channel", ((struct dsl_channel_args*)data)->id, "status", "intlvdepth");
	return 0;
}

int get_DSLChannel_INTLVBLOCK(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_without_argument("dsl.channel", ((struct dsl_channel_args*)data)->id, "status", "intlvblock");
	return 0;
}

int get_DSLChannel_ActualInterleavingDelay(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_without_argument("dsl.channel", ((struct dsl_channel_args*)data)->id, "status", "actual_interleaving_delay");
	return 0;
}

int get_DSLChannel_ACTINP(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_without_argument("dsl.channel", ((struct dsl_channel_args*)data)->id, "status", "actinp");
	return 0;
}

int get_DSLChannel_INPREPORT(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_without_argument("dsl.channel", ((struct dsl_channel_args*)data)->id, "status", "inpreport");
	return 0;
}

int get_DSLChannel_NFEC(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_without_argument("dsl.channel", ((struct dsl_channel_args*)data)->id, "status", "nfec");
	return 0;
}

int get_DSLChannel_RFEC(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_without_argument("dsl.channel", ((struct dsl_channel_args*)data)->id, "status", "rfec");
	return 0;
}

int get_DSLChannel_LSYMB(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_without_argument("dsl.channel", ((struct dsl_channel_args*)data)->id, "status", "lsymb");
	return 0;
}

int get_DSLChannel_UpstreamCurrRate(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_without_argument_and_with_two_key("dsl.channel", ((struct dsl_channel_args*)data)->id, "status", "curr_rate", "us");
	return 0;
}

int get_DSLChannel_DownstreamCurrRate(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_without_argument_and_with_two_key("dsl.channel", ((struct dsl_channel_args*)data)->id, "status", "curr_rate", "ds");
	return 0;
}

int get_DSLChannel_ACTNDR(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_without_argument_and_with_two_key("dsl.channel", ((struct dsl_channel_args*)data)->id, "status", "actndr", "ds");
	return 0;
}

int get_DSLChannel_ACTINPREIN(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_without_argument_and_with_two_key("dsl.channel", ((struct dsl_channel_args*)data)->id, "status", "actinprein", "ds");
	return 0;
}

int get_DSLChannelStats_TotalStart(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_without_argument("dsl.channel", ((struct dsl_channel_args*)data)->id, "stats", "total_start");
	return 0;
}

int get_DSLChannelStats_ShowtimeStart(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_without_argument("dsl.channel", ((struct dsl_channel_args*)data)->id, "stats", "showtime_start");
	return 0;
}

int get_DSLChannelStats_LastShowtimeStart(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_without_argument("dsl.channel", ((struct dsl_channel_args*)data)->id, "stats", "last_showtime_start");
	return 0;
}

int get_DSLChannelStats_CurrentDayStart(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_without_argument("dsl.channel", ((struct dsl_channel_args*)data)->id, "stats", "current_day_start");
	return 0;
}

int get_DSLChannelStats_QuarterHourStart(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_without_argument("dsl.channel", ((struct dsl_channel_args*)data)->id, "stats", "quarter_hour_start");
	return 0;
}

int get_DSLChannelStatsTotal_XTURFECErrors(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_with_argument("dsl.channel", ((struct dsl_channel_args*)data)->id, "stats", "total", "xtur_fec_errors");
	return 0;
}

int get_DSLChannelStatsTotal_XTUCFECErrors(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_with_argument("dsl.channel", ((struct dsl_channel_args*)data)->id, "stats", "total", "xtuc_fec_errors");
	return 0;
}

int get_DSLChannelStatsTotal_XTURHECErrors(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_with_argument("dsl.channel", ((struct dsl_channel_args*)data)->id, "stats", "total", "xtur_hec_errors");
	return 0;
}

int get_DSLChannelStatsTotal_XTUCHECErrors(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_with_argument("dsl.channel", ((struct dsl_channel_args*)data)->id, "stats", "total", "xtuc_hec_errors");
	return 0;
}

int get_DSLChannelStatsTotal_XTURCRCErrors(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_with_argument("dsl.channel", ((struct dsl_channel_args*)data)->id, "stats", "total", "xtur_crc_errors");
	return 0;
}

int get_DSLChannelStatsTotal_XTUCCRCErrors(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_with_argument("dsl.channel", ((struct dsl_channel_args*)data)->id, "stats", "total", "xtuc_crc_errors");
	return 0;
}

int get_DSLChannelStatsShowtime_XTURFECErrors(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_with_argument("dsl.channel", ((struct dsl_channel_args*)data)->id, "stats", "showtime", "xtur_fec_errors");
	return 0;
}

int get_DSLChannelStatsShowtime_XTUCFECErrors(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_with_argument("dsl.channel", ((struct dsl_channel_args*)data)->id, "stats", "showtime", "xtuc_fec_errors");
	return 0;}

int get_DSLChannelStatsShowtime_XTURHECErrors(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_with_argument("dsl.channel", ((struct dsl_channel_args*)data)->id, "stats", "showtime", "xtur_hec_errors");
	return 0;
}

int get_DSLChannelStatsShowtime_XTUCHECErrors(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_with_argument("dsl.channel", ((struct dsl_channel_args*)data)->id, "stats", "showtime", "xtuc_hec_errors");
	return 0;
}

int get_DSLChannelStatsShowtime_XTURCRCErrors(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_with_argument("dsl.channel", ((struct dsl_channel_args*)data)->id, "stats", "showtime", "xtur_crc_errors");
	return 0;
}

int get_DSLChannelStatsShowtime_XTUCCRCErrors(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_with_argument("dsl.channel", ((struct dsl_channel_args*)data)->id, "stats", "showtime", "xtuc_crc_errors");
	return 0;
}

int get_DSLChannelStatsLastShowtime_XTURFECErrors(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_with_argument("dsl.channel", ((struct dsl_channel_args*)data)->id, "stats", "lastshowtime", "xtur_fec_errors");
	return 0;
}

int get_DSLChannelStatsLastShowtime_XTUCFECErrors(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_with_argument("dsl.channel", ((struct dsl_channel_args*)data)->id, "stats", "lastshowtime", "xtuc_fec_errors");
	return 0;
}

int get_DSLChannelStatsLastShowtime_XTURHECErrors(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_with_argument("dsl.channel", ((struct dsl_channel_args*)data)->id, "stats", "lastshowtime", "xtur_hec_errors");
	return 0;
}

int get_DSLChannelStatsLastShowtime_XTUCHECErrors(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_with_argument("dsl.channel", ((struct dsl_channel_args*)data)->id, "stats", "lastshowtime", "xtuc_hec_errors");
	return 0;
}

int get_DSLChannelStatsLastShowtime_XTURCRCErrors(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_with_argument("dsl.channel", ((struct dsl_channel_args*)data)->id, "stats", "lastshowtime", "xtur_crc_errors");
	return 0;
}

int get_DSLChannelStatsLastShowtime_XTUCCRCErrors(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_with_argument("dsl.channel", ((struct dsl_channel_args*)data)->id, "stats", "lastshowtime", "xtuc_crc_errors");
	return 0;
}

int get_DSLChannelStatsCurrentDay_XTURFECErrors(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_with_argument("dsl.channel", ((struct dsl_channel_args*)data)->id, "stats", "currentday", "xtur_fec_errors");
	return 0;
}

int get_DSLChannelStatsCurrentDay_XTUCFECErrors(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_with_argument("dsl.channel", ((struct dsl_channel_args*)data)->id, "stats", "currentday", "xtuc_fec_errors");
	return 0;
}

int get_DSLChannelStatsCurrentDay_XTURHECErrors(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_with_argument("dsl.channel", ((struct dsl_channel_args*)data)->id, "stats", "currentday", "xtur_hec_errors");
	return 0;
}

int get_DSLChannelStatsCurrentDay_XTUCHECErrors(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_with_argument("dsl.channel", ((struct dsl_channel_args*)data)->id, "stats", "currentday", "xtuc_hec_errors");
	return 0;
}

int get_DSLChannelStatsCurrentDay_XTURCRCErrors(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_with_argument("dsl.channel", ((struct dsl_channel_args*)data)->id, "stats", "currentday", "xtur_crc_errors");
	return 0;
}

int get_DSLChannelStatsCurrentDay_XTUCCRCErrors(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_with_argument("dsl.channel", ((struct dsl_channel_args*)data)->id, "stats", "currentday", "xtuc_crc_errors");
	return 0;
}

int get_DSLChannelStatsQuarterHour_XTURFECErrors(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_with_argument("dsl.channel", ((struct dsl_channel_args*)data)->id, "stats", "quarterhour", "xtur_fec_errors");
	return 0;
}

int get_DSLChannelStatsQuarterHour_XTUCFECErrors(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_with_argument("dsl.channel", ((struct dsl_channel_args*)data)->id, "stats", "quarterhour", "xtuc_fec_errors");
	return 0;
}

int get_DSLChannelStatsQuarterHour_XTURHECErrors(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_with_argument("dsl.channel", ((struct dsl_channel_args*)data)->id, "stats", "quarterhour", "xtur_hec_errors");
	return 0;
}

int get_DSLChannelStatsQuarterHour_XTUCHECErrors(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_with_argument("dsl.channel", ((struct dsl_channel_args*)data)->id, "stats", "quarterhour", "xtuc_hec_errors");
	return 0;
}

int get_DSLChannelStatsQuarterHour_XTURCRCErrors(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_with_argument("dsl.channel", ((struct dsl_channel_args*)data)->id, "stats", "quarterhour", "xtur_crc_errors");
	return 0;
}

int get_DSLChannelStatsQuarterHour_XTUCCRCErrors(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)
{
	*value = get_dsl_value_with_argument("dsl.channel", ((struct dsl_channel_args*)data)->id, "stats", "quarterhour", "xtuc_crc_errors");
	return 0;
}
