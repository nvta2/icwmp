/*
*      This program is free software: you can redistribute it and/or modify
*      it under the terms of the GNU General Public License as published by
*      the Free Software Foundation, either version 2 of the License, or
*      (at your option) any later version.
*
*      Copyright (C) 2019 iopsys Software Solutions AB
*		Author: AMIN Ben Ramdhane <amin.benramdhane@pivasoftware.com>
*/

#ifndef __DSL_H
#define __DSL_H

struct dsl_line_args
{
	struct uci_section *line_sec;
	char *id;
};

struct dsl_channel_args
{
	struct uci_section *channel_sec;
	char *id;
};

extern DMOBJ tDSLObj[];
extern DMLEAF tDSLParams[];
extern DMOBJ tDSLLineObj[];
extern DMLEAF tDSLLineParams[];
extern DMOBJ tDSLLineStatsObj[];
extern DMLEAF tDSLLineStatsParams[];
extern DMLEAF tDSLLineStatsTotalParams[];
extern DMLEAF tDSLLineStatsShowtimeParams[];
extern DMLEAF tDSLLineStatsLastShowtimeParams[];
extern DMLEAF tDSLLineStatsCurrentDayParams[];
extern DMLEAF tDSLLineStatsQuarterHourParams[];
extern DMOBJ tDSLChannelObj[];
extern DMLEAF tDSLChannelParams[];
extern DMOBJ tDSLChannelStatsObj[];
extern DMLEAF tDSLChannelStatsParams[];
extern DMLEAF tDSLChannelStatsTotalParams[];
extern DMLEAF tDSLChannelStatsShowtimeParams[];
extern DMLEAF tDSLChannelStatsLastShowtimeParams[];
extern DMLEAF tDSLChannelStatsCurrentDayParams[];
extern DMLEAF tDSLChannelStatsQuarterHourParams[];

int browseDSLLineInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);
int browseDSLChannelInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);

int get_dsl_line_linker(char *refparam, struct dmctx *dmctx, void *data, char *instance, char **linker);
int get_dsl_channel_linker(char *refparam, struct dmctx *dmctx, void *data, char *instance, char **linker);

int get_DSL_LineNumberOfEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSL_ChannelNumberOfEntries(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLLine_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_DSLLine_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_DSLLine_Status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLLine_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_DSLLine_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_DSLLine_Name(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLLine_LowerLayers(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_DSLLine_LowerLayers(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_DSLLine_Upstream(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLLine_FirmwareVersion(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLLine_LinkStatus(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLLine_StandardsSupported(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLLine_XTSE(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLLine_StandardUsed(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLLine_XTSUsed(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLLine_LineEncoding(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLLine_AllowedProfiles(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLLine_CurrentProfile(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLLine_PowerManagementState(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLLine_SuccessFailureCause(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLLine_UPBOKLERPb(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLLine_RXTHRSHds(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLLine_ACTRAMODEds(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLLine_ACTRAMODEus(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLLine_SNRMROCus(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLLine_LastStateTransmittedDownstream(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLLine_LastStateTransmittedUpstream(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLLine_US0MASK(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLLine_TRELLISds(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLLine_TRELLISus(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLLine_ACTSNRMODEds(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLLine_ACTSNRMODEus(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLLine_LineNumber(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLLine_UpstreamMaxBitRate(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLLine_DownstreamMaxBitRate(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLLine_UpstreamNoiseMargin(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLLine_DownstreamNoiseMargin(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLLine_SNRMpbus(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLLine_SNRMpbds(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLLine_UpstreamAttenuation(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLLine_DownstreamAttenuation(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLLine_UpstreamPower(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLLine_DownstreamPower(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLLine_XTURVendor(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLLine_XTURCountry(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLLine_XTURANSIStd(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLLine_XTURANSIRev(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLLine_XTUCVendor(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLLine_XTUCCountry(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLLine_XTUCANSIStd(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLLine_XTUCANSIRev(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLLineStats_TotalStart(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLLineStats_ShowtimeStart(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLLineStats_LastShowtimeStart(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLLineStats_CurrentDayStart(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLLineStats_QuarterHourStart(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLLineStatsTotal_ErroredSecs(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLLineStatsTotal_SeverelyErroredSecs(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLLineStatsShowtime_ErroredSecs(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLLineStatsShowtime_SeverelyErroredSecs(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLLineStatsLastShowtime_ErroredSecs(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLLineStatsLastShowtime_SeverelyErroredSecs(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLLineStatsCurrentDay_ErroredSecs(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLLineStatsCurrentDay_SeverelyErroredSecs(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLLineStatsQuarterHour_ErroredSecs(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLLineStatsQuarterHour_SeverelyErroredSecs(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLChannel_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_DSLChannel_Enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_DSLChannel_Status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLChannel_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_DSLChannel_Alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_DSLChannel_Name(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLChannel_LowerLayers(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLChannel_LinkEncapsulationSupported(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLChannel_LinkEncapsulationUsed(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLChannel_LPATH(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLChannel_INTLVDEPTH(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLChannel_INTLVBLOCK(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLChannel_ActualInterleavingDelay(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLChannel_ACTINP(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLChannel_INPREPORT(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLChannel_NFEC(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLChannel_RFEC(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLChannel_LSYMB(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLChannel_UpstreamCurrRate(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLChannel_DownstreamCurrRate(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLChannel_ACTNDR(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLChannel_ACTINPREIN(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLChannelStats_TotalStart(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLChannelStats_ShowtimeStart(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLChannelStats_LastShowtimeStart(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLChannelStats_CurrentDayStart(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLChannelStats_QuarterHourStart(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLChannelStatsTotal_XTURFECErrors(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLChannelStatsTotal_XTUCFECErrors(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLChannelStatsTotal_XTURHECErrors(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLChannelStatsTotal_XTUCHECErrors(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLChannelStatsTotal_XTURCRCErrors(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLChannelStatsTotal_XTUCCRCErrors(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLChannelStatsShowtime_XTURFECErrors(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLChannelStatsShowtime_XTUCFECErrors(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLChannelStatsShowtime_XTURHECErrors(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLChannelStatsShowtime_XTUCHECErrors(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLChannelStatsShowtime_XTURCRCErrors(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLChannelStatsShowtime_XTUCCRCErrors(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLChannelStatsLastShowtime_XTURFECErrors(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLChannelStatsLastShowtime_XTUCFECErrors(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLChannelStatsLastShowtime_XTURHECErrors(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLChannelStatsLastShowtime_XTUCHECErrors(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLChannelStatsLastShowtime_XTURCRCErrors(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLChannelStatsLastShowtime_XTUCCRCErrors(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLChannelStatsCurrentDay_XTURFECErrors(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLChannelStatsCurrentDay_XTUCFECErrors(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLChannelStatsCurrentDay_XTURHECErrors(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLChannelStatsCurrentDay_XTUCHECErrors(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLChannelStatsCurrentDay_XTURCRCErrors(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLChannelStatsCurrentDay_XTUCCRCErrors(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLChannelStatsQuarterHour_XTURFECErrors(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLChannelStatsQuarterHour_XTUCFECErrors(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLChannelStatsQuarterHour_XTURHECErrors(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLChannelStatsQuarterHour_XTUCHECErrors(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLChannelStatsQuarterHour_XTURCRCErrors(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_DSLChannelStatsQuarterHour_XTUCCRCErrors(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);

#endif //__DSL_H
