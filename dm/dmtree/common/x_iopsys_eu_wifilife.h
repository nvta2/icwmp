/*
*      This program is free software: you can redistribute it and/or modify
*      it under the terms of the GNU General Public License as published by
*      the Free Software Foundation, either version 2 of the License, or
*      (at your option) any later version.
*
*      Copyright (C) 2019 iopsys Software Solutions AB
*		Author: Amin Ben Ramdhane <amin.benramdhane@pivasoftware.com>
*/

#ifndef __WIFILIFE_H
#define __WIFILIFE_H

extern DMOBJ X_IOPSYS_EU_WiFiLifeObj[];
extern DMLEAF X_IOPSYS_EU_WiFiLifeParams[];
extern DMLEAF WiFiLifeSteeringParams[];

int browseWifiLifeSteeringObj(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);

int get_x_iopsys_eu_wifilife_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_x_iopsys_eu_wifilife_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_wifilife_steering_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_wifilife_steering_enable(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);
int get_wifilife_steering_legacy_fallback(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);
int set_wifilife_steering_legacy_fallback(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);

#endif // __WIFILIFE_H
