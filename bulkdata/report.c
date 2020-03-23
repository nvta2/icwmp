/*
*	This program is free software: you can redistribute it and/or modify
*	it under the terms of the GNU General Public License as published by
*	the Free Software Foundation, either version 2 of the License, or
*	(at your option) any later version.
*
*	Copyright (C) 2019 iopsys Software Solutions AB
*		Author: Amin Ben Ramdhane <amin.benramdhane@pivasoftware.com>
*		Author: Omar Kallel <omar.kallel@pivasoftware.com>
*
*/

#include "report.h"

static void add_new_json_obj(json_object *json_obj, char *name, char *data, char *type)
{
	json_object *jobj;
	if(strstr(type, "unsignedInt") || strstr(type, "int") || strstr(type, "long"))
		jobj = json_object_new_int64(atoi(data));
	else if(strstr(type, "bool"))
		jobj = json_object_new_boolean(atoi(data));
	else
		jobj = json_object_new_string(data);

	json_object_object_add(json_obj, name, jobj);
}

static void create_json_bulkdata_report_object_hierarchy(struct profile *profile, char **report)
{
	struct json_object *json_obj, *json_obj1, *json_obj2;
	struct resultsnode *p;
	int i, j, profile_param_number = profile->profile_parameter_number;
	char *param_name, *result, *pch, *pchr, *collection_time = NULL;
	char buf[1024] = {0};

	json_obj = json_object_new_object();
	get_time_stamp(profile->json_encoding_report_time_stamp, &collection_time);
	if(collection_time) {
		if(strcmp(profile->json_encoding_report_time_stamp, "iso8601") == 0)
			json_object_object_add(json_obj, "CollectionTime", json_object_new_string(collection_time));
		else
			json_object_object_add(json_obj, "CollectionTime", json_object_new_int64(atoi(collection_time)));
		free(collection_time);
	}
	json_obj2 = json_obj;
	for (i = 0; i < profile_param_number; i++) {
		LIST_HEAD(results_list);
		bulkdata_get_value_results(profile->profile_parameter[i].reference, &results_list);
		list_for_each_entry(p, &results_list, list) {
			char *argv[128] = {0};
			j = 0;
			param_name = get_bulkdata_profile_parameter_name(profile->profile_parameter[i].reference, profile->profile_parameter[i].name, p->name);
			strcpy(buf, param_name);
			for (pch = strtok_r(buf, ".", &pchr); pch != NULL; pch = strtok_r(NULL, ".", &pchr)) {
				argv[j] = pch;
				json_obj1 = (json_object *)bbfdmjson_select_obj(json_obj, argv);
				if (json_obj1)
					json_obj2 = json_obj1;
				else {
					if (pchr != NULL && *pchr != '\0') {
						json_object *new_obj = json_object_new_object();
						json_object_object_add(json_obj2, pch, new_obj);
						json_obj2 = new_obj;
					}
					else
						add_new_json_obj(json_obj2, pch, p->data, p->type);
				}
				j++;
			}
		}
		bulkdata_free_data_from_list(&results_list);
		FREE(param_name);
	}
	result = (char *)json_object_to_json_string_ext(json_obj, JSON_C_TO_STRING_PRETTY);
	*report = strdup(result);
	json_object_put(json_obj);
}

static void create_json_bulkdata_report_name_value_pair(struct profile *profile, char **report)
{
	struct json_object *json_obj;
	struct resultsnode *p;
	char *param_name, *result, *collection_time = NULL;
	int i = 0, profile_param_number = profile->profile_parameter_number;

	json_obj = json_object_new_object();
	get_time_stamp(profile->json_encoding_report_time_stamp, &collection_time);
	if(collection_time) {
		if(strcmp(profile->json_encoding_report_time_stamp, "iso8601") == 0)
			json_object_object_add(json_obj, "CollectionTime", json_object_new_string(collection_time));
		else
			json_object_object_add(json_obj, "CollectionTime", json_object_new_int64(atoi(collection_time)));
		free(collection_time);
	}
	for (i = 0; i < profile_param_number; i++) {
		LIST_HEAD(results_list);
		bulkdata_get_value_results(profile->profile_parameter[i].reference, &results_list);
		list_for_each_entry(p, &results_list, list) {
			param_name = get_bulkdata_profile_parameter_name(profile->profile_parameter[i].reference, profile->profile_parameter[i].name, p->name);
			add_new_json_obj(json_obj, param_name, p->data, p->type);
			FREE(param_name);
		}
		bulkdata_free_data_from_list(&results_list);
	}
	result = (char *)json_object_to_json_string_ext(json_obj, JSON_C_TO_STRING_PRETTY);
	*report = strdup(result);
	json_object_put(json_obj);
}

static void add_failed_reports_to_report_json(struct profile *profile, char *new_report, char **report, int isnext)
{
	json_object *json_obj, *json_array, *json_string;
	struct failed_reports *retreport = NULL;
	char *msgout = NULL;
	int j = 0;

	json_obj = json_object_new_object();
	json_array = json_object_new_array();
	json_object_object_add(json_obj,"Report", json_array);

	if(list_empty(profile->failed_reports))
		goto new_report;

	list_for_each_entry(retreport, profile->failed_reports, list) {
		if(!j && isnext) {
			j = 1;
			continue;
		}
		json_string = json_tokener_parse(retreport->freport);
		json_object_array_add(json_array, json_string);
	}

new_report :
	if(new_report) {
		json_string = json_tokener_parse(new_report);
		json_object_array_add(json_array, json_string);
	}

	msgout = (char *)json_object_to_json_string_ext(json_obj, JSON_C_TO_STRING_PRETTY);
	*report = strdup(msgout);
	json_object_put(json_obj);
}

static void create_report_json(char *new_report, char **report)
{
	json_object *json_obj, *json_array, *json_string;
	char *msgout = NULL;

	json_obj = json_object_new_object();
	json_array = json_object_new_array();
	json_object_object_add(json_obj,"Report", json_array);

	if(new_report) {
		json_string = json_tokener_parse(new_report);
		json_object_array_add(json_array, json_string);
	}

	msgout = (char *)json_object_to_json_string_ext(json_obj, JSON_C_TO_STRING_PRETTY);
	*report = strdup(msgout);
	json_object_put(json_obj);
}

int create_json_bulkdata_report(struct profile *profile, char **report)
{
	/*
	 * create json msg of current report
	 * parse failed reports list and add it to the report
	 * then add new report to the report
	 */
	char *msgout;

	profile->new_report = NULL;
	if(strcmp(profile->json_encoding_report_format, "objecthierarchy") == 0) {
		create_json_bulkdata_report_object_hierarchy(profile, &msgout);
	} else if(strcmp(profile->json_encoding_report_format, "namevaluepair") == 0) {
		create_json_bulkdata_report_name_value_pair(profile, &msgout);
	}

	if(profile->nbre_of_retained_failed_reports != 0) {
		if(profile->nbre_failed_reports >= profile->nbre_of_retained_failed_reports && profile->nbre_of_retained_failed_reports > 0)
			add_failed_reports_to_report_json(profile, msgout, report, 1);
		else
			add_failed_reports_to_report_json(profile, msgout, report, 0);
	} else {
		create_report_json(msgout, report);
	}

	append_string_to_string(msgout, &profile->new_report);
	FREE(msgout);
	return 0;
}

int create_csv_bulkdata_report(struct profile *profile, char **report)
{
	/*
	 * create csv msg of current report
	 * parse failed reports list and add it to the report
	 */
	int i;
	struct resultsnode *p;
	char *str1 = NULL, *str2 = NULL, *str = NULL, *paramprofilename, *timestamp = NULL, *type = NULL, rowseparator = '\0', separator = '\0';

	if(strcmp(profile->csv_encoding_row_separator, "&#10;") == 0)
		rowseparator = '\n';
	else if(strcmp(profile->csv_encoding_row_separator, "&#13;") == 0)
		rowseparator = '\r';

	if(profile->csv_encoding_field_separator)
		separator = profile->csv_encoding_field_separator[0];

	get_time_stamp(profile->csv_encoding_row_time_stamp, &timestamp);
	/*
	 * Create header ReportTimestamp,ParameterName,ParameterValue,ParameterType in case of ParameterPerRow
	 */
	if(strcmp(profile->csv_encoding_report_format, "row") == 0) {
		if(timestamp == NULL)
			asprintf(&str, "ParameterName%cParameterValue%cParameterType%c", separator, separator, rowseparator);
		else
			asprintf(&str, "ReportTimestamp%cParameterName%cParameterValue%cParameterType%c", separator, separator, separator, rowseparator);
		append_string_to_string(str, report);
		FREE(str);
		if(profile->nbre_of_retained_failed_reports != 0) {
			if(profile->nbre_failed_reports >= profile->nbre_of_retained_failed_reports && profile->nbre_of_retained_failed_reports > 0)
				add_failed_reports_to_report_csv(profile, report, 1);
			else
				add_failed_reports_to_report_csv(profile, report, 0);
		}
	}
	if(strcmp(profile->csv_encoding_report_format, "column") == 0 && timestamp != NULL) {
		if(profile->nbre_of_retained_failed_reports != 0) {
			if(profile->nbre_failed_reports >= profile->nbre_of_retained_failed_reports && profile->nbre_of_retained_failed_reports > 0)
				add_failed_reports_to_report_csv(profile, report, 1);
			else
				add_failed_reports_to_report_csv(profile, report, 0);
		}
		append_string_to_string("ReportTimestamp", &str1);
		append_string_to_string(timestamp, &str2);
	}

	/*
	 * Add New reports
	 */
	profile->new_report = NULL;
	for(i = 0; i < profile->profile_parameter_number; i++) {
		LIST_HEAD(results_list);
		bulkdata_get_value_results(profile->profile_parameter[i].reference, &results_list);
		list_for_each_entry(p, &results_list, list) {
			paramprofilename = get_bulkdata_profile_parameter_name(profile->profile_parameter[i].reference, profile->profile_parameter[i].name, p->name);
			if(strcmp(profile->csv_encoding_report_format, "row") == 0) {
				type = strstr(p->type, ":");
				if(timestamp == NULL)
					asprintf(&str, "%s%c%s%c%s%c", paramprofilename, separator, p->data, separator, type+1, rowseparator);
				else
					asprintf(&str, "%s%c%s%c%s%c%s%c", timestamp, separator, paramprofilename, separator, p->data, separator, type+1, rowseparator);
				append_string_to_string(str, report);
				append_string_to_string(str, &profile->new_report);
				FREE(str);
			} else if(strcmp(profile->csv_encoding_report_format, "column") == 0) {
				if(str1 == NULL || strlen(str1) <= 0)
					asprintf(&str, "%s", paramprofilename);
				else
					asprintf(&str, "%c%s", separator, paramprofilename);
				append_string_to_string(str, &str1);
				FREE(str);
				if(str2 == NULL || strlen(str2) <= 0)
					asprintf(&str, "%s", p->data);
				else
					asprintf(&str, "%c%s", separator, p->data);
				append_string_to_string(str, &str2);
				FREE(str);
			}
			FREE(paramprofilename);
		}
		bulkdata_free_data_from_list(&results_list);
	}
	if(strcmp(profile->csv_encoding_report_format, "column") == 0) {
		asprintf(&str, "%c", rowseparator);
		append_string_to_string(str, &str1);
		append_string_to_string(str, &str2);
		append_string_to_string(str1, report);
		append_string_to_string(str2, report);
		append_string_to_string(str1, &profile->new_report);
		append_string_to_string(str2, &profile->new_report);
	}
	FREE(str);
	FREE(str1);
	FREE(str2);
	FREE(timestamp);
	return 0;
}

static void create_json_failed_report(struct profile *profile, char **report)
{
	add_failed_reports_to_report_json(profile, NULL, report, 0);
}

static void create_csv_failed_report(struct profile *profile, char **report)
{
	char rowseparator = '\0', separator = '\0', *timestamp = NULL;

	if(strcmp(profile->csv_encoding_row_separator, "&#10;") == 0) {
		rowseparator = '\n';
	} else if(strcmp(profile->csv_encoding_row_separator, "&#13;") == 0) {
		rowseparator = '\r';
	}

	if(profile->csv_encoding_field_separator)
		separator = profile->csv_encoding_field_separator[0];

	get_time_stamp(profile->csv_encoding_row_time_stamp, &timestamp);
	if(strcmp(profile->csv_encoding_report_format, "row") == 0) {
		if(timestamp == NULL)
			asprintf(report, "ParameterName%cParameterValue%cParameterType%c", separator, separator, rowseparator);
		else
			asprintf(report, "ReportTimestamp%cParameterName%cParameterValue%cParameterType%c", separator, separator, separator, rowseparator);
	}
	add_failed_reports_to_report_csv(profile, report, 0);
}

void create_encoding_bulkdata_report(struct profile *profile, char **report)
{
	if(strcasecmp(profile->encoding_type, "json") == 0) {
		create_json_bulkdata_report(profile, report);
	} else if(strcasecmp(profile->encoding_type, "csv") == 0) {
		create_csv_bulkdata_report(profile, report);
	}
}

void create_failed_report(struct profile *profile, char **report)
{
	if(strcasecmp(profile->encoding_type, "json") == 0) {
		create_json_failed_report(profile, report);
	} else if(strcasecmp(profile->encoding_type, "csv") == 0) {
		create_csv_failed_report(profile, report);
	}
}
