#ifndef __XML__H_
#define __XML__H_

#include <microxml.h>
#include <stdio.h>
#include <stdlib.h>

#include "session.h"

#define CWMP_MXML_TAB_SPACE "  "
#define MAX_SCHEDULE_INFORM_QUEUE 10

#define MXML_DELETE(X)                                                                                                                                                                                                                                                                                     \
	do {                                                                                                                                                                                                                                                                                               \
		if (X) {                                                                                                                                                                                                                                                                                   \
			mxmlDelete(X);                                                                                                                                                                                                                                                                     \
			X = NULL;                                                                                                                                                                                                                                                                          \
		}                                                                                                                                                                                                                                                                                          \
	} while (0)

extern const char *cwmp_urls[];
int xml_prepare_msg_out(struct session *session);
int xml_prepare_lwnotification_message(char **msg_out);
int xml_set_cwmp_id_rpc_cpe(struct session *session);
int xml_recreate_namespace(mxml_node_t *tree);
const char *whitespace_cb(mxml_node_t *node, int where);
int xml_set_cwmp_id(struct session *session);
int xml_send_message(struct cwmp *cwmp, struct session *session, struct rpc *rpc);
mxml_node_t *mxmlFindElementOpaque(mxml_node_t *node, mxml_node_t *top, const char *text, int descend);
char *xml_get_cwmp_version(int version);
void xml_exit(void);

#endif
