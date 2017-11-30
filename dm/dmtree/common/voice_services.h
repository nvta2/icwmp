/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2012-2014 PIVA SOFTWARE (www.pivasoftware.com)
*		Author: Imen Bhiri <imen.bhiri@pivasoftware.com>
 *		Author: Feten Besbes <feten.besbes@pivasoftware.com>
 *		Author: Mohamed Kallel <mohamed.kallel@pivasoftware.com>
 *		Author: Anis Ellouze <anis.ellouze@pivasoftware.com>
 */
#ifndef __VOICE_H
#define __VOICE_H

extern DMOBJ tServiceObj[];
extern DMOBJ tVoiceServiceObj[];
extern DMLEAF tVoiceServiceParam[];
extern DMLEAF tCapabilitiesParams[];
extern DMOBJ tCapabilitiesObj[];
extern DMLEAF tSIPParams[];
extern DMLEAF tCodecsParams[] ;
extern DMOBJ tProfileObj[] ;
extern DMLEAF tProfileSIPParams[];
extern DMLEAF tServiceProviderInfoParams[];
extern DMLEAF tProfileParam[];
extern DMOBJ tLineObj[];
extern DMOBJ tLineCodecObj[];
extern DMLEAF tLineCodecListParams[];
extern DMLEAF tLineSIPParams[];
extern DMLEAF tVoiceProcessingParams[];
extern DMLEAF tCallingFeaturesParams[];
extern DMLEAF tLineParams[];
extern DMLEAF tRTPParams[];
extern DMOBJ tRTPObj[];
extern DMLEAF tSRTPParam[];
extern DMLEAF tRTCPParams[];
extern DMLEAF tFaxT38Params[];

/*Args of get_voice_service_capabilities_codecs_generic*/
struct codec_args
{
	char *cdc;
	char *id;
	int enumid;
	struct uci_section *codec_section;
};

struct rtp_tos
{
	char *key;
	char *val;
};

struct cap_sip_codec
{
	int enumid;
	char *c1;
	char *c2;
	char *c3;
	char *c4;
	char *c5;
};

struct sip_args
{
	struct uci_section *sip_section;
	char *profile_num;
};

struct brcm_args
{
	struct uci_section *brcm_section;
	struct uci_section *sip_section;
	char *profile_num;
};

struct allow_sip_codec
{
	int enumid;
	char *id;
	char *allowed_cdc;
	char *priority_cdc;
	char *ptime_cdc;
};

struct line_codec_args
{
	int enumid;
	char *sip_id;
	char *cdc;
	char *id;
	char *priority_cdc;
	char *ptime_cdc;
	struct uci_section *sip_section;
	struct uci_section *codec_sec;
};

struct region
{
	char *country;
	char *id;
};

struct codec
{
	char *cdc;
	char *id;
	char *priority;
};

enum enum_cap_sip_codecs {
	SIP_CODEC_G723,
	SIP_CODEC_GSM,
	SIP_CODEC_ULAW,
	SIP_CODEC_ALAW,
	SIP_CODEC_G726AAL2,
	SIP_CODEC_ADPCM,
	SIP_CODEC_SLIN,
	SIP_CODEC_LPC10,
	SIP_CODEC_G729,
	SIP_CODEC_SPEEX,
	SIP_CODEC_ILBC,
	SIP_CODEC_G726,
	SIP_CODEC_G722,
	SIP_CODEC_SIREN7,
	SIP_CODEC_SIREN14,
	SIP_CODEC_SLIN16,
	SIP_CODEC_G719,
	SIP_CODEC_SPEEX16,
	SIP_CODEC_TESTLAW
};

bool dm_service_enable_set(void);
int browseVoiceServiceInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);
int browseCodecsInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);
int browseProfileInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);
int browseLineInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);
int browseLineCodecListInst(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);

int add_profile_object(char *refparam, struct dmctx *ctx, void *data, char **instancepara);
int delete_profile_object(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action);
int add_line_object(char *refparam, struct dmctx *ctx, void *data, char **instancepara);
int delete_line_object(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action);

int get_service_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_max_profile_count(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_max_line_count(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_true_value(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_max_session_count(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_signal_protocols(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_regions(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_false_value(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_sip_role(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_sip_extension(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_sip_transport(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_sip_tls_auth_protocols(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_sip_tls_enc_protocols(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_sip_tls_key_protocols(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_cap_codec_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_entry_id(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_capabilities_sip_codec(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_capabilities_sip_bitrate(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_capabilities_sip_pperiod(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_voice_profile_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_voice_profile_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_voice_profile_name(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_voice_profile_signalprotocol(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_voice_profile_max_sessions(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_voice_profile_number_of_lines(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_voice_profile_sip_dtmfmethod(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_sip_profile_region(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_voice_profile_sip_proxyserver(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_sip_proxy_server_transport(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_voice_profile_sip_registerserver(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_voice_profile_sip_registerserverport(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_sip_registrar_server_transport(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_sip_user_agent_domain(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_sip_user_agent_port(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_sip_user_agent_transport(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_sip_outbound_proxy(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_sip_outbound_proxy_port(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_sip_registration_period(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_sip_re_invite_expires(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_sip_x_002207_call_lines(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_voice_service_serviceproviderinfo_name(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_sip_fax_t38_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_voice_service_vp_rtp_portmin(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_voice_service_vp_rtp_portmax(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_voice_service_vp_rtp_dscp(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_voice_service_vp_rtp_rtcp_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_voice_service_vp_rtp_rtcp_txrepeatinterval(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_voice_service_vp_rtp_srtp_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_line_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_voice_profile_line_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_voice_profile_line_status(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_voice_profile_line_callstate(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_line_x_002207_line_profile(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_line_x_002207_brcm_line(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_line_confort_noise_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_line_voice_processing_cancellation_enable(char *refparam, struct dmctx *ctx,  void *data, char *instance, char **value);
int get_line_calling_features_caller_id_name(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_line_calling_features_callwaiting(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_line_sip_auth_username(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_line_sip_uri(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_line_codec_list_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_codec_entry_id(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int capabilities_sip_codecs_get_codec(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int capabilities_sip_codecs_get_bitrate(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_capabilities_sip_codecs_pperiod(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_line_codec_list_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_line_codec_list_priority(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int get_line_directory_number(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);

int set_service_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_cap_codec_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_voice_profile_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_voice_profile_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_voice_profile_reset(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_voice_profile_signaling_protocol(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_voice_profile_sip_dtmfmethod(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_voice_profile_sip_proxyserver(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_sip_proxy_server_port(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_sip_proxy_server_transport(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_voice_profile_sip_registerserver(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_voice_profile_sip_registerserverport(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_sip_registrar_server_transport(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_sip_user_agent_domain(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_sip_user_agent_port(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_sip_user_agent_transport(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_sip_outbound_proxy(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_sip_outbound_proxy_port(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_sip_registration_period(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_sip_re_invite_expires(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_sip_x_002207_call_lines(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_voice_service_serviceproviderinfo_name(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_sip_fax_t38_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_voice_service_vp_rtp_portmin(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_voice_profile_rtp_localportmax(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_voice_service_vp_rtp_dscp(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_voice_service_vp_rtp_rtcp_txrepeatinterval(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_voice_service_vp_rtp_srtp_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_line_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_voice_profile_line_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_line_directory_number(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_line_x_002207_line_profile(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_line_x_002207_brcm_line(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_line_confort_noise_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_line_calling_features_caller_id_name(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_line_voice_processing_cancellation_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_line_calling_features_callwaiting(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_line_sip_auth_username(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_line_sip_auth_password(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_line_sip_uri(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_line_codec_list_alias(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_line_codec_list_packetization(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_line_codec_list_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_line_codec_list_priority(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int set_sip_profile_region(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_voice_service_max_line();

#endif
