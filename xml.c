/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2013-2021 iopsys Software Solutions AB
 *	  Author Mohamed Kallel <mohamed.kallel@pivasoftware.com>
 *	  Author Ahmed Zribi <ahmed.zribi@pivasoftware.com>
 *	  Author Omar Kallel <omar.kallel@pivasoftware.com>
 */
#include "xml.h"
#include "common.h"
#include "log.h"
#include "cwmp_zlib.h"
#include "http.h"
#include "notifications.h"
#include "messages.h"

static const char *soap_env_url = "http://schemas.xmlsoap.org/soap/envelope/";
static const char *soap_enc_url = "http://schemas.xmlsoap.org/soap/encoding/";
static const char *xsd_url = "http://www.w3.org/2001/XMLSchema";
static const char *xsi_url = "http://www.w3.org/2001/XMLSchema-instance";

const char *cwmp_urls[] = { "urn:dslforum-org:cwmp-1-0", "urn:dslforum-org:cwmp-1-1", "urn:dslforum-org:cwmp-1-2", "urn:dslforum-org:cwmp-1-2", "urn:dslforum-org:cwmp-1-2", NULL };
mxml_node_t * /* O - Element node or NULL */
mxmlFindElementOpaque(mxml_node_t *node, /* I - Current node */
		      mxml_node_t *top, /* I - Top node */
		      const char *text, /* I - Element text, if NULL return NULL */
		      int descend) /* I - Descend into tree - MXML_DESCEND, MXML_NO_DESCEND, or MXML_DESCEND_FIRST */
{
	if (!node || !top || !text)
		return (NULL);

	node = mxmlWalkNext(node, top, descend);

	while (node != NULL) {
		if (node->type == MXML_OPAQUE && node->value.opaque && (!strcmp(node->value.opaque, text))) {
			return (node);
		}

		if (descend == MXML_DESCEND)
			node = mxmlWalkNext(node, top, MXML_DESCEND);
		else
			node = node->next;
	}
	return (NULL);
}

int xml_recreate_namespace(mxml_node_t *tree)
{
	const char *cwmp_urn;
	int i;
	mxml_node_t *b = tree;

	do {
		char *c;
		FREE(ns.soap_env);
		FREE(ns.soap_enc);
		FREE(ns.xsd);
		FREE(ns.xsi);
		FREE(ns.cwmp);

		c = (char *)mxmlElementGetAttrName(b, soap_env_url);
		if (c && *(c + 5) == ':') {
			ns.soap_env = strdup((c + 6));
		} else {
			continue;
		}

		c = (char *)mxmlElementGetAttrName(b, soap_enc_url);
		if (c && *(c + 5) == ':') {
			ns.soap_enc = strdup((c + 6));
		} else {
			continue;
		}

		c = (char *)mxmlElementGetAttrName(b, xsd_url);
		if (c && *(c + 5) == ':') {
			ns.xsd = strdup((c + 6));
		} else {
			continue;
		}

		c = (char *)mxmlElementGetAttrName(b, xsi_url);
		if (c && *(c + 5) == ':') {
			ns.xsi = strdup((c + 6));
		} else {
			continue;
		}

		for (i = 0; cwmp_urls[i] != NULL; i++) {
			cwmp_urn = cwmp_urls[i];
			c = (char *)mxmlElementGetAttrName(b, cwmp_urn);
			if (c && *(c + 5) == ':') {
				ns.cwmp = strdup((c + 6));
				break;
			}
		}

		if (!ns.cwmp)
			continue;

		return 0;
	} while ((b = mxmlWalkNext(b, tree, MXML_DESCEND)));

	return -1;
}

void xml_exit(void)
{
	FREE(ns.soap_env);
	FREE(ns.soap_enc);
	FREE(ns.xsd);
	FREE(ns.xsi);
	FREE(ns.cwmp);
}

int xml_send_message(struct rpc *rpc)
{
	char *s, *msg_out = NULL, *msg_in = NULL;
	char c[512];
	int msg_out_len = 0, f, r = 0;
	mxml_node_t *b;

	if (cwmp_main->session->tree_out) {
		unsigned char *zmsg_out;
		msg_out = mxmlSaveAllocString(cwmp_main->session->tree_out, whitespace_cb);
		CWMP_LOG_XML_MSG(DEBUG, msg_out, XML_MSG_OUT);
		if (cwmp_main->conf.compression != COMP_NONE) {
			if (zlib_compress(msg_out, &zmsg_out, &msg_out_len, cwmp_main->conf.compression)) {
				return -1;
			}
			FREE(msg_out);
			msg_out = (char *)zmsg_out;
		} else {
			msg_out_len = strlen(msg_out);
		}
	}
	while (1) {
		f = 0;
		if (http_send_message(msg_out, msg_out_len, &msg_in)) {
			goto error;
		}
		if (msg_in) {
			CWMP_LOG_XML_MSG(DEBUG, msg_in, XML_MSG_IN);
			if ((s = strstr(msg_in, "<FaultCode>")))
				sscanf(s, "<FaultCode>%d</FaultCode>", &f);
			if (f) {
				if (f == 8005) {
					r++;
					if (r < 5) {
						FREE(msg_in);
						continue;
					}
					goto error;
				} else if (rpc && rpc->type != RPC_ACS_INFORM) {
					break;
				} else {
					goto error;
				}
			} else {
				break;
			}
		} else {
			goto end;
		}
	}

	cwmp_main->session->tree_in = mxmlLoadString(NULL, msg_in, MXML_OPAQUE_CALLBACK);
	if (!cwmp_main->session->tree_in)
		goto error;

	xml_recreate_namespace(cwmp_main->session->tree_in);

	/* get NoMoreRequests or HolRequest*/
	cwmp_main->session->hold_request = false;

	if (snprintf(c, sizeof(c), "%s:%s", ns.cwmp, "NoMoreRequests") == -1)
		goto error;
	b = mxmlFindElement(cwmp_main->session->tree_in, cwmp_main->session->tree_in, c, NULL, NULL, MXML_DESCEND);
	if (b) {
		b = mxmlWalkNext(b, cwmp_main->session->tree_in, MXML_DESCEND_FIRST);
		if (b && b->type == MXML_OPAQUE && b->value.opaque)
			cwmp_main->session->hold_request = (atoi(b->value.opaque)) ? true : false;
	} else {
		if (snprintf(c, sizeof(c), "%s:%s", ns.cwmp, "HoldRequests") == -1)
			goto error;

		b = mxmlFindElement(cwmp_main->session->tree_in, cwmp_main->session->tree_in, c, NULL, NULL, MXML_DESCEND);
		if (b) {
			b = mxmlWalkNext(b, cwmp_main->session->tree_in, MXML_DESCEND_FIRST);
			if (b && b->type == MXML_OPAQUE && b->value.opaque)
				cwmp_main->session->hold_request = (atoi(b->value.opaque)) ? true : false;
		}
	}

end:
	FREE(msg_out);
	FREE(msg_in);
	return 0;

error:
	FREE(msg_out);
	FREE(msg_in);
	return -1;
}

int xml_prepare_msg_out()
{
	struct config *conf = &(cwmp_main->conf);
	mxml_node_t *n;

	cwmp_main->session->tree_out = mxmlLoadString(NULL, CWMP_RESPONSE_MESSAGE, MXML_OPAQUE_CALLBACK);
	n = mxmlFindElement(cwmp_main->session->tree_out, cwmp_main->session->tree_out, "soap_env:Envelope", NULL, NULL, MXML_DESCEND);
	if (!n) {
		return -1;
	}

	mxmlElementSetAttr(n, "xmlns:cwmp", cwmp_urls[(conf->amd_version) - 1]);
	if (!cwmp_main->session->tree_out)
		return -1;

	return 0;
}

int xml_set_cwmp_id()
{
	char c[32];
	mxml_node_t *b;

	/* define cwmp id */
	if (snprintf(c, sizeof(c), "%u", ++(cwmp_main->cwmp_id)) == -1)
		return -1;

	b = mxmlFindElement(cwmp_main->session->tree_out, cwmp_main->session->tree_out, "cwmp:ID", NULL, NULL, MXML_DESCEND);
	if (!b)
		return -1;

	b = mxmlNewOpaque(b, c);
	if (!b)
		return -1;

	return 0;
}

int xml_set_cwmp_id_rpc_cpe()
{
	char c[512];
	mxml_node_t *b;

	/* handle cwmp:ID */
	if (snprintf(c, sizeof(c), "%s:%s", ns.cwmp, "ID") == -1)
		return -1;

	b = mxmlFindElement(cwmp_main->session->tree_in, cwmp_main->session->tree_in, c, NULL, NULL, MXML_DESCEND);

	if (b) {
		/* ACS send ID parameter */
		b = mxmlWalkNext(b, cwmp_main->session->tree_in, MXML_DESCEND_FIRST);
		if (!b || b->type != MXML_OPAQUE || !b->value.opaque)
			return 0;
		snprintf(c, sizeof(c), "%s", b->value.opaque);

		b = mxmlFindElement(cwmp_main->session->tree_out, cwmp_main->session->tree_out, "cwmp:ID", NULL, NULL, MXML_DESCEND);
		if (!b)
			return -1;

		b = mxmlNewOpaque(b, c);
		if (!b)
			return -1;
	} else {
		/* ACS does not send ID parameter */
		int r = xml_set_cwmp_id(cwmp_main->session);
		return r;
	}
	return 0;
}

const char *whitespace_cb(mxml_node_t *node, int where)
{
	static char tab_space[10 * sizeof(CWMP_MXML_TAB_SPACE) + 1];

	if (node->type != MXML_ELEMENT)
		return NULL;

	switch (where) {
	case MXML_WS_BEFORE_CLOSE:
		if (node->child && node->child->type != MXML_ELEMENT)
			return NULL;
		break;
	case MXML_WS_BEFORE_OPEN:
		tab_space[0] = '\0';
		while ((node = node->parent))
			strcat(tab_space, CWMP_MXML_TAB_SPACE);
		return tab_space;
	case MXML_WS_AFTER_OPEN:
		return ((!node->child || node->child->type == MXML_ELEMENT) ? "\n" : NULL);
	case MXML_WS_AFTER_CLOSE:
		return "\n";
	default:
		return NULL;
	}
	return NULL;
}

char *xml_get_cwmp_version(int version)
{
	static char versions[60];
	unsigned pos = 0;
	int k;

	versions[0] = '\0';
	for (k = 0; k < version; k++) {
		pos += snprintf(&versions[pos], sizeof(versions) - pos, "1.%d, ", k);
	}

	if (pos)
		versions[pos - 2] = 0;

	return versions;
}

static int xml_prepare_lwnotifications(mxml_node_t *parameter_list)
{
	mxml_node_t *b, *n;

	struct list_head *p;
	struct cwmp_dm_parameter *lw_notification;
	list_for_each (p, &list_lw_value_change) {
		lw_notification = list_entry(p, struct cwmp_dm_parameter, list);

		n = mxmlNewElement(parameter_list, "Param");
		if (!n)
			goto error;

		b = mxmlNewElement(n, "Name");
		if (!b)
			goto error;

		b = mxmlNewOpaque(b, lw_notification->name);
		if (!b)
			goto error;

		b = mxmlNewElement(n, "Value");
		if (!b)
			goto error;
#ifdef ACS_MULTI
		mxmlElementSetAttr(b, "xsi:type", lw_notification->type);
#endif
		b = mxmlNewOpaque(b, lw_notification->value);
		if (!b)
			goto error;
	}
	return 0;

error:
	return -1;
}

int xml_prepare_lwnotification_message(char **msg_out)
{
	mxml_node_t *lw_tree, *b, *parameter_list;
	struct config *conf = &(cwmp_main->conf);
	char *c = NULL;

	lw_tree = mxmlLoadString(NULL, CWMP_LWNOTIFICATION_MESSAGE, MXML_OPAQUE_CALLBACK);
	if (!lw_tree)
		goto error;

	b = mxmlFindElement(lw_tree, lw_tree, "TS", NULL, NULL, MXML_DESCEND);
	if (!b)
		goto error;

	if (cwmp_asprintf(&c, "%ld", time(NULL)) == -1)
		goto error;
	b = mxmlNewOpaque(b, c);
	FREE(c);
	if (!b)
		goto error;

	b = mxmlFindElement(lw_tree, lw_tree, "UN", NULL, NULL, MXML_DESCEND);
	if (!b)
		goto error;

	b = mxmlNewOpaque(b, conf->acs_userid);
	if (!b)
		goto error;

	b = mxmlFindElement(lw_tree, lw_tree, "CN", NULL, NULL, MXML_DESCEND);
	if (!b)
		goto error;

	c = (char *)calculate_lwnotification_cnonce();
	b = mxmlNewOpaque(b, c);
	free(c);
	if (!b)
		goto error;

	b = mxmlFindElement(lw_tree, lw_tree, "OUI", NULL, NULL, MXML_DESCEND);
	if (!b)
		goto error;

	b = mxmlNewOpaque(b, cwmp_main->deviceid.oui);
	if (!b)
		goto error;

	b = mxmlFindElement(lw_tree, lw_tree, "ProductClass", NULL, NULL, MXML_DESCEND);
	if (!b)
		goto error;

	b = mxmlNewOpaque(b, cwmp_main->deviceid.productclass ? cwmp_main->deviceid.productclass : "");
	if (!b)
		goto error;

	b = mxmlFindElement(lw_tree, lw_tree, "SerialNumber", NULL, NULL, MXML_DESCEND);
	if (!b)
		goto error;

	b = mxmlNewOpaque(b, cwmp_main->deviceid.serialnumber ? cwmp_main->deviceid.serialnumber : "");
	if (!b)
		goto error;

	parameter_list = mxmlFindElement(lw_tree, lw_tree, "Notification", NULL, NULL, MXML_DESCEND);
	if (!parameter_list)
		goto error;
	if (xml_prepare_lwnotifications(parameter_list))
		goto error;

	*msg_out = mxmlSaveAllocString(lw_tree, whitespace_cb);

	mxmlDelete(lw_tree);
	return 0;

error:
	return -1;
}
