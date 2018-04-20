#include "upnp_common.h"
#include "dmcwmp.h"
#include "dmuci.h"
#include "dmubus.h"
#include "dmjson.h"
#include "dmcommon.h"
#include <json-c/json.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char **strsplit(const char* str, const char* delim, size_t* numtokens) {
    // copy the original string so that we don't overwrite parts of it
    // (don't do this if you don't need to keep the old line,
    // as this is less efficient)
    char *s = strdup(str);
    // these three variables are part of a very common idiom to
    // implement a dynamically-growing array
    size_t tokens_alloc = 1;
    size_t tokens_used = 0;
    char **tokens = calloc(tokens_alloc, sizeof(char*));
    char *token, *strtok_ctx;
    for (token = strtok_r(s, delim, &strtok_ctx);
            token != NULL;
            token = strtok_r(NULL, delim, &strtok_ctx)) {
        // check if we need to allocate more space for tokens
        if (tokens_used == tokens_alloc) {
            tokens_alloc *= 2;
            tokens = realloc(tokens, tokens_alloc * sizeof(char*));
        }
        tokens[tokens_used++] = strdup(token);
    }
    // cleanup
    if (tokens_used == 0) {
        free(tokens);
        tokens = NULL;
    } else {
        tokens = realloc(tokens, tokens_used * sizeof(char*));
    }
    *numtokens = tokens_used;
    free(s);
    return tokens;
}


int upnp_get_NetworkInterfaceNumberOfEntries(){
	int n = 0;
	struct uci_section *s = NULL;
	uci_foreach_sections("network", "interface", s) {
		n++;
	}
	return n;
}

int upnp_get_IPInterfaceNumberOfEntries(){
	char *value1 = NULL, *value2 = NULL;
	int n = 0;
	struct uci_section *s = NULL;

	uci_foreach_sections("network", "interface", s) {
		dmuci_get_value_by_section_string(s, "ipaddr", &value1);
		if(value1 !=NULL && strlen(value1)>0) {
			n++;
			continue;
		}
		dmuci_get_value_by_section_string(s, "proto", &value2);
		if(value2 !=NULL && strlen(value2)>0 && strstr(value2, "dhcp") != NULL) {
			n++;
			continue;
		}
	}
	return n;
}

char *upnp_get_softwareversion()
{
	char *v=NULL, *tmp=NULL, *val=NULL;

	db_get_value_string("hw", "board", "iopVersion", &v);
	tmp = dmstrdup(v);// MEM WILL BE FREED IN DMMEMCLEAN
	if(!tmp) return NULL;
	val = tmp;
	return val;
}

void upnp_getMacAddress(char *interfaceName, char **macAddress){
	json_object *res = NULL, *res1 =NULL;
	int length;

	char *device = NULL;
	char **devices =NULL;

	dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", interfaceName}}, 1, &res);
	if(!(res)) goto not_found;
	device = dmjson_get_value(res, 1, "device");
	devices = strsplit(device, " ", &length);
	dmubus_call("network.device", "status", UBUS_ARGS{{}}, 0, &res1);
	if(!(res1)) goto not_found;
	*macAddress = dmjson_get_value(res1, 2, devices[0], "macaddr");
	not_found:
	return;
}

void upnp_getInterfaceStatus(char *interfaceName, char **status){
	json_object *res= NULL;
	char *up= NULL;

	dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", interfaceName}}, 1, &res);
	if (res == NULL) {
		*status = NULL;
		goto end_bloc;
	}
	up = dm_ubus_get_value(res, 1, "up");
	if(up == NULL) {
		*status = NULL;
		goto end_bloc;
	}
	if(!strcmp(up,"true")) dmasprintf(status, "UP"); else dmasprintf(status, "DOWN");

	end_bloc:
		return;
}

int  upnp_getInterfaceTotalPacketSent(char *interfaceName, char **totalPktSent){
	json_object *res= NULL, *res1= NULL, *res2= NULL;
	char *device = NULL, *tx_packets = NULL;
	int length;
	char **devices = NULL;

	dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", interfaceName}}, 1, &res);
	if(res == NULL) {
		*totalPktSent = NULL;
		goto end_bloc;
	}
	device = dmjson_get_value(res, 1, "device");
	if(device == NULL || strlen(device)<=0){
		*totalPktSent = NULL;
		goto end_bloc;
	}
	devices = strsplit(device, " ", &length);

	dmubus_call("network.device", "status", UBUS_ARGS{{"name", devices[0]}}, 1, &res1);
	if(res1 == NULL){
		*totalPktSent = NULL;
		goto end_bloc;
	}
	DM_ASSERT(res1, *totalPktSent = "");
	*totalPktSent = dmjson_get_value(res1, 2, "statistics", "tx_packets");

	end_bloc:
		return 0;
}

int upnp_getInterfaceTotalPacketReceived(char *interfaceName, char **totalPktReceived){
	json_object *res= NULL, *res1= NULL, *res2= NULL;
	char *device = NULL, *rx_packets = NULL;
	int length;
	char **devices = NULL;

	dmubus_call("network.interface", "status", UBUS_ARGS{{"interface", interfaceName}}, 1, &res);
	if(res == NULL){
		*totalPktReceived = NULL;
		goto end_bloc;
	}
	device = dmjson_get_value(res, 1, "device");
	if(device == NULL || strlen(device)<=0){
		*totalPktReceived = NULL;
		goto end_bloc;
	}
	devices = strsplit(device, " ", &length);
	dmubus_call("network.device", "status", UBUS_ARGS{{"name", devices[0]}}, 1, &res1);
	if(res1 == NULL){
		*totalPktReceived = NULL;
		goto end_bloc;
	}
	DM_ASSERT(res1, *totalPktReceived = "");
	*totalPktReceived = dmjson_get_value(res1, 2, "statistics", "rx_packets");

	end_bloc:
		return 0;
}

char *upnp_get_deviceid_manufactureroui()
{
	char *v = NULL;
	char str[16];
	char *mac = NULL;
	json_object *res= NULL;
	size_t ln;

	dmubus_call("router.system", "info", UBUS_ARGS{{}}, 0, &res);
	if(!(res)) goto not_found;
	mac = dmjson_get_value(res, 2, "system", "basemac");
	if(mac)
	{
		ln = strlen(mac);
		if (ln<17) goto not_found;
		sscanf (mac,"%2c:%2c:%2c",str,str+2,str+4);
		str[6] = '\0';
		v = dmstrdup(str); // MEM WILL BE FREED IN DMMEMCLEAN
		return v;
	}
not_found:
	v = "";
	return v;
}
