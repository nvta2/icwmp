/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2013-2021 iopsys Software Solutions AB
 *	  Author Omar Kallel <omar.kallel@pivasoftware.com>
 *
 */
#include <stdio.h>
#include <getopt.h>
#include <sys/stat.h>
#include <curl/curl.h>
#include <unistd.h>
#include <sys/reboot.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/file.h>
#include <regex.h>
#include <openssl/rand.h>

#include "common.h"
#include "cwmp_uci.h"
#include "ubus.h"
#include "log.h"
#include "cwmp_cli.h"
#include "cwmp_du_state.h"

#ifndef CWMP_REVISION
#define CWMP_REVISION "8.2.10"
#endif

char *commandKey = NULL;
bool thread_end = false;
bool signal_exit = false;
bool ubus_exit = false;
long int flashsize = 256000000;
struct cwmp cwmp_main = { 0 };
static int nbre_services = 0;
static char *list_services[MAX_NBRE_SERVICES] = { 0 };
LIST_HEAD(cwmp_memory_list);

struct cwmp_mem {
	struct list_head list;
	char mem[0];
};

struct option cwmp_long_options[] = {
	{ "boot-event", no_argument, NULL, 'b' },    { "get-rpc-methods", no_argument, NULL, 'g' },
	{ "command-input", no_argument, NULL, 'c' }, { "help", no_argument, NULL, 'h' },
	{ "version", no_argument, NULL, 'v' },	     { NULL, 0, NULL, 0 }
};

struct FAULT_CPE FAULT_CPE_ARRAY[] = {
	[FAULT_CPE_METHOD_NOT_SUPPORTED] = { "9000", FAULT_9000, FAULT_CPE_TYPE_SERVER, "Method not supported" },
	[FAULT_CPE_REQUEST_DENIED] = { "9001", FAULT_9001, FAULT_CPE_TYPE_SERVER,
				       "Request denied (no reason specified)" },
	[FAULT_CPE_INTERNAL_ERROR] = { "9002", FAULT_9002, FAULT_CPE_TYPE_SERVER, "Internal error" },
	[FAULT_CPE_INVALID_ARGUMENTS] = { "9003", FAULT_9003, FAULT_CPE_TYPE_CLIENT, "Invalid arguments" },
	[FAULT_CPE_RESOURCES_EXCEEDED] = { "9004", FAULT_9004, FAULT_CPE_TYPE_SERVER, "Resources exceeded" },
	[FAULT_CPE_INVALID_PARAMETER_NAME] = { "9005", FAULT_9005, FAULT_CPE_TYPE_CLIENT, "Invalid parameter name" },
	[FAULT_CPE_INVALID_PARAMETER_TYPE] = { "9006", FAULT_9006, FAULT_CPE_TYPE_CLIENT, "Invalid parameter type" },
	[FAULT_CPE_INVALID_PARAMETER_VALUE] = { "9007", FAULT_9007, FAULT_CPE_TYPE_CLIENT, "Invalid parameter value" },
	[FAULT_CPE_NON_WRITABLE_PARAMETER] = { "9008", FAULT_9008, FAULT_CPE_TYPE_CLIENT,
					       "Attempt to set a non-writable parameter" },
	[FAULT_CPE_NOTIFICATION_REJECTED] = { "9009", FAULT_9009, FAULT_CPE_TYPE_SERVER,
					      "Notification request rejected" },
	[FAULT_CPE_DOWNLOAD_FAILURE] = { "9010", FAULT_9010, FAULT_CPE_TYPE_SERVER, "Download failure" },
	[FAULT_CPE_UPLOAD_FAILURE] = { "9011", FAULT_9011, FAULT_CPE_TYPE_SERVER, "Upload failure" },
	[FAULT_CPE_FILE_TRANSFER_AUTHENTICATION_FAILURE] = { "9012", FAULT_9012, FAULT_CPE_TYPE_SERVER,
							     "File transfer server authentication failure" },
	[FAULT_CPE_FILE_TRANSFER_UNSUPPORTED_PROTOCOL] = { "9013", FAULT_9013, FAULT_CPE_TYPE_SERVER,
							   "Unsupported protocol for file transfer" },
	[FAULT_CPE_DOWNLOAD_FAIL_MULTICAST_GROUP] = { "9014", FAULT_9014, FAULT_CPE_TYPE_SERVER,
						      "Download failure: unable to join multicast group" },
	[FAULT_CPE_DOWNLOAD_FAIL_CONTACT_SERVER] = { "9015", FAULT_9015, FAULT_CPE_TYPE_SERVER,
						     "Download failure: unable to contact file server" },
	[FAULT_CPE_DOWNLOAD_FAIL_ACCESS_FILE] = { "9016", FAULT_9016, FAULT_CPE_TYPE_SERVER,
						  "Download failure: unable to access file" },
	[FAULT_CPE_DOWNLOAD_FAIL_COMPLETE_DOWNLOAD] = { "9017", FAULT_9017, FAULT_CPE_TYPE_SERVER,
							"Download failure: unable to complete download" },
	[FAULT_CPE_DOWNLOAD_FAIL_FILE_CORRUPTED] = { "9018", FAULT_9018, FAULT_CPE_TYPE_SERVER,
						     "Download failure: file corrupted" },
	[FAULT_CPE_DOWNLOAD_FAIL_FILE_AUTHENTICATION] = { "9019", FAULT_9019, FAULT_CPE_TYPE_SERVER,
							  "Download failure: file authentication failure" },
	[FAULT_CPE_DOWNLOAD_FAIL_WITHIN_TIME_WINDOW] = { "9020", FAULT_9020, FAULT_CPE_TYPE_SERVER,
							 "Download failure: unable to complete download" },
	[FAULT_CPE_DUPLICATE_DEPLOYMENT_UNIT] = { "9026", FAULT_9026, FAULT_CPE_TYPE_SERVER,
						  "Duplicate deployment unit" },
	[FAULT_CPE_SYSTEM_RESOURCES_EXCEEDED] = { "9027", FAULT_9027, FAULT_CPE_TYPE_SERVER,
						  "System ressources exceeded" },
	[FAULT_CPE_UNKNOWN_DEPLOYMENT_UNIT] = { "9028", FAULT_9028, FAULT_CPE_TYPE_SERVER, "Unknown deployment unit" },
	[FAULT_CPE_INVALID_DEPLOYMENT_UNIT_STATE] = { "9029", FAULT_9029, FAULT_CPE_TYPE_SERVER,
						      "Invalid deployment unit state" },
	[FAULT_CPE_INVALID_DOWNGRADE_REJECTED] = { "9030", FAULT_9030, FAULT_CPE_TYPE_SERVER,
						   "Invalid deployment unit Update: Downgrade not permitted" },
	[FAULT_CPE_INVALID_UPDATE_VERSION_UNSPECIFIED] = { "9031", FAULT_9031, FAULT_CPE_TYPE_SERVER,
							   "Invalid deployment unit Update: Version not specified" },
	[FAULT_CPE_INVALID_UPDATE_VERSION_EXIST] = { "9031", FAULT_9032, FAULT_CPE_TYPE_SERVER,
						     "Invalid deployment unit Update: Version already exist" }
};

static void show_help(void)
{
	printf("Usage: icwmpd [OPTIONS]\n");
	printf(" -b, --boot-event                                    (CWMP daemon) Start CWMP with BOOT event\n");
	printf(" -g, --get-rpc-methods                               (CWMP daemon) Start CWMP with GetRPCMethods request to ACS\n");
	printf(" -c, --cli                              	     CWMP CLI\n");
	printf(" -h, --help                                          Display this help text\n");
	printf(" -v, --version                                       Display the version\n");
}

static void show_version()
{
#ifndef CWMP_REVISION
	fprintf(stdout, "\nVersion: %s\n\n", CWMP_VERSION);
#else
	fprintf(stdout, "\nVersion: %s revision %s\n\n", CWMP_VERSION, CWMP_REVISION);
#endif
}

int global_env_init(int argc, char **argv, struct env *env)
{
	int c, option_index = 0;

	while ((c = getopt_long(argc, argv, "bgchv", cwmp_long_options, &option_index)) != -1) {
		switch (c) {
		case 'b':
			env->boot = CWMP_START_BOOT;
			break;

		case 'g':
			env->periodic = CWMP_START_PERIODIC;
			break;
		case 'c':
			execute_cwmp_cli_command(argv[2], argv + 3);
			exit(0);
		case 'h':
			show_help();
			exit(0);

		case 'v':
			show_version();
			exit(0);
		}
	}
	return CWMP_OK;
}

/*
 * List dm_paramter
 */
void add_dm_parameter_to_list(struct list_head *head, char *param_name, char *param_val, char *param_type,
			      int notification, bool writable)
{
	struct cwmp_dm_parameter *dm_parameter;
	struct list_head *ilist;

	list_for_each (ilist, head) {
		int cmp;
		dm_parameter = list_entry(ilist, struct cwmp_dm_parameter, list);
		cmp = strcmp(dm_parameter->name, param_name);
		if (cmp == 0) {
			if (param_val && strcmp(dm_parameter->value, param_val) != 0) {
				free(dm_parameter->value);
				dm_parameter->value = strdup(param_val);
			}
			dm_parameter->notification = notification;
			return;
		} else if (cmp > 0) {
			break;
		}
	}
	dm_parameter = calloc(1, sizeof(struct cwmp_dm_parameter));
	_list_add(&dm_parameter->list, ilist->prev, ilist);
	if (param_name)
		dm_parameter->name = strdup(param_name);
	if (param_val)
		dm_parameter->value = strdup(param_val);

	dm_parameter->type = strdup(param_type ? param_type : "xsd:string");
	dm_parameter->notification = notification;
	dm_parameter->writable = writable;
}

void delete_dm_parameter_from_list(struct cwmp_dm_parameter *dm_parameter)
{
	list_del(&dm_parameter->list);
	free(dm_parameter->name);
	free(dm_parameter->value);
	free(dm_parameter->type);
	free(dm_parameter);
}

void cwmp_free_all_dm_parameter_list(struct list_head *list)
{
	while (list->next != list) {
		struct cwmp_dm_parameter *dm_parameter;
		dm_parameter = list_entry(list->next, struct cwmp_dm_parameter, list);
		delete_dm_parameter_from_list(dm_parameter);
	}
}

/*
 * List Fault parameter
 */
void cwmp_add_list_fault_param(char *param, int fault, struct list_head *list_set_value_fault)
{
	struct cwmp_param_fault *param_fault;
	if (param == NULL)
		param = "";

	param_fault = calloc(1, sizeof(struct cwmp_param_fault));
	list_add_tail(&param_fault->list, list_set_value_fault);
	param_fault->name = strdup(param);
	param_fault->fault = fault;
}

void cwmp_del_list_fault_param(struct cwmp_param_fault *param_fault)
{
	list_del(&param_fault->list);
	free(param_fault->name);
	free(param_fault);
}

void cwmp_free_all_list_param_fault(struct list_head *list_param_fault)
{
	while (list_param_fault->next != list_param_fault) {
		struct cwmp_param_fault *param_fault;
		param_fault = list_entry(list_param_fault->next, struct cwmp_param_fault, list);
		cwmp_del_list_fault_param(param_fault);
	}
}

int cwmp_asprintf(char **s, const char *format, ...)
{
	int size;
	char *str = NULL;
	va_list arg, argcopy;
	va_start(arg, format);
	va_copy(argcopy, arg);
	size = vsnprintf(NULL, 0, format, argcopy);
	if (size < 0) {
		va_end(argcopy);
		va_end(arg);
		return -1;
	}
	va_end(argcopy);
	str = (char *)calloc(sizeof(char), size + 1);
	vsnprintf(str, size + 1, format, arg);
	va_end(arg);
	*s = strdup(str);
	FREE(str);
	if (*s == NULL) {
		return -1;
	}
	return 0;
}

bool folder_exists(const char *path)
{
	struct stat folder_stat;

	return (stat(path, &folder_stat) == 0 && S_ISDIR(folder_stat.st_mode));
}

size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	size_t written = fwrite(ptr, size, nmemb, stream);
	return written;
}

void get_firewall_zone_name_by_wan_iface(char *if_wan, char **zone_name)
{
	struct uci_section *s;
	char *network = NULL;

	cwmp_uci_foreach_sections("firewall", "zone", UCI_STANDARD_CONFIG, s)
	{
		cwmp_uci_get_value_by_section_string(s, "network", &network);
		char *net = strtok(network, " ");
		while (net != NULL) {
			if (strcmp(net, if_wan) == 0) {
				cwmp_uci_get_value_by_section_string(s, "name", zone_name);
				icwmp_free(network);
				return;
			}
			net = strtok(NULL, " ");
		}
		icwmp_free(network);
	}
}

/*
 * Reboot
 */
void cwmp_reboot(char *command_key)
{
	cwmp_uci_set_varstate_value("cwmp", "cpe", "ParameterKey", command_key);
	cwmp_commit_package("cwmp", UCI_VARSTATE_CONFIG);

	cwmp_ubus_call("rpc-sys", "reboot", CWMP_UBUS_ARGS{ {} }, 0, NULL, NULL);
}

/*
 * FactoryReset
 */
void cwmp_factory_reset() //use the ubus rpc-sys factory
{
	cwmp_ubus_call("rpc-sys", "factory", CWMP_UBUS_ARGS{ {} }, 0, NULL, NULL);
}

long int get_file_size(char *file_name)
{
	FILE *fp = fopen(file_name, "r");

	if (fp == NULL) {
		CWMP_LOG(INFO, "File Not Found!");
		return -1;
	}

	fseek(fp, 0L, SEEK_END);
	long int res = ftell(fp);

	fclose(fp);

	return res;
}

int opkg_install_package(char *package_path)
{
	FILE *fp;
	char path[1035];
	char cmd[512];

	CWMP_LOG(INFO, "Apply downloaded config ...");

	int ret = snprintf(cmd, sizeof(cmd), "opkg --force-depends --force-maintainer install %s", package_path);
	if (ret < 0 || ret > 512)
		return -1;
	fp = popen(cmd, "r");
	if (fp == NULL) {
		CWMP_LOG(INFO, "Failed to run command");
		return -1;
	}

	/* Read the output a line at a time - output it. */
	while (fgets(path, sizeof(path), fp) != NULL) {
		if (strstr(path, "Installing") != NULL)
			return 0;
	}

	/* close */
	pclose(fp);
	return -1;
}

int copy(const char *from, const char *to)
{
	int fd_to, fd_from;
	char buf[4096];
	ssize_t nread;
	int saved_errno;

	fd_from = open(from, O_RDONLY);
	if (fd_from < 0)
		return -1;

	fd_to = open(to, O_WRONLY | O_CREAT | O_EXCL, 0666);
	if (fd_to < 0)
		goto out_error;

	while ((nread = read(fd_from, buf, sizeof buf)) > 0) {
		char *out_ptr = buf;
		ssize_t nwritten;

		do {
			nwritten = write(fd_to, out_ptr, nread);

			if (nwritten >= 0) {
				nread -= nwritten;
				out_ptr += nwritten;
			} else if (errno != EINTR) {
				goto out_error;
			}
		} while (nread > 0);
	}

	if (nread == 0) {
		if (close(fd_to) < 0) {
			fd_to = -1;
			goto out_error;
		}
		close(fd_from);

		/* Success! */
		return 0;
	}

out_error:
	saved_errno = errno;

	close(fd_from);
	if (fd_to >= 0)
		close(fd_to);

	errno = saved_errno;
	return -1;
}

bool file_exists(const char *path)
{
	struct stat buffer;

	return stat(path, &buffer) == 0;
}

int cwmp_get_fault_code(int fault_code)
{
	int i;

	for (i = 1; i < __FAULT_CPE_MAX; i++) {
		if (FAULT_CPE_ARRAY[i].ICODE == fault_code)
			break;
	}

	if (i == __FAULT_CPE_MAX)
		i = FAULT_CPE_INTERNAL_ERROR;

	return i;
}

int cwmp_get_fault_code_by_string(char *fault_code)
{
	int i;

	for (i = 1; i < __FAULT_CPE_MAX; i++) {
		if (strcmp(FAULT_CPE_ARRAY[i].CODE, fault_code) == 0)
			break;
	}

	if (i == __FAULT_CPE_MAX)
		i = FAULT_CPE_INTERNAL_ERROR;

	return i;
}

/*
 * Memory mgmt
 */

void *icwmp_malloc(size_t size)
{
	struct cwmp_mem *m = malloc(sizeof(struct cwmp_mem) + size);
	if (m == NULL)
		return NULL;
	list_add(&m->list, &cwmp_memory_list);
	return (void *)m->mem;
}

void *icwmp_calloc(int n, size_t size)
{
	struct cwmp_mem *m = calloc(n, sizeof(struct cwmp_mem) + size);
	if (m == NULL)
		return NULL;
	list_add(&m->list, &cwmp_memory_list);
	return (void *)m->mem;
}

void *icwmp_realloc(void *n, size_t size)
{
	struct cwmp_mem *m = NULL;
	if (n != NULL) {
		m = container_of(n, struct cwmp_mem, mem);
		list_del(&m->list);
	}
	struct cwmp_mem *new_m = realloc(m, sizeof(struct cwmp_mem) + size);
	if (new_m == NULL) {
		icwmp_free(m);
		return NULL;
	} else
		m = new_m;
	list_add(&m->list, &cwmp_memory_list);
	return (void *)m->mem;
}

char *icwmp_strdup(const char *s)
{
	size_t len = strlen(s) + 1;
	void *new = icwmp_malloc(len);
	if (new == NULL)
		return NULL;
	return (char *)memcpy(new, s, len);
}

int icwmp_asprintf(char **s, const char *format, ...)
{
	int size;
	char *str = NULL;
	va_list arg, argcopy;

	va_start(arg, format);
	va_copy(argcopy, arg);
	size = vsnprintf(NULL, 0, format, argcopy);
	va_end(argcopy);

	if (size < 0) {
		va_end(arg);
		return -1;
	}
	str = (char *)calloc(sizeof(char), size + 1);
	vsnprintf(str, size + 1, format, arg);
	va_end(arg);

	*s = icwmp_strdup(str);
	free(str);
	if (*s == NULL)
		return -1;
	return 0;
}

void icwmp_free(void *m)
{
	if (m == NULL)
		return;
	struct cwmp_mem *rm;
	rm = container_of(m, struct cwmp_mem, mem);
	list_del(&rm->list);
	free(rm);
}

void icwmp_cleanmem()
{
	struct cwmp_mem *mem;
	while (cwmp_memory_list.next != &cwmp_memory_list) {
		mem = list_entry(cwmp_memory_list.next, struct cwmp_mem, list);
		list_del(&mem->list);
		free(mem);
	}
}

/*
 * Services Management
 */
void icwmp_init_list_services()
{
	int i;

	nbre_services = 0;
	for (i = 0; i < MAX_NBRE_SERVICES; i++)
		list_services[i] = NULL;
}

int icwmp_add_service(char *service)
{
	if (nbre_services >= MAX_NBRE_SERVICES)
		return -1;
	list_services[nbre_services++] = strdup(service);
	return 0;
}

void icwmp_free_list_services()
{
	int i = 0;
	for (i = 0; i < nbre_services; i++) {
		FREE(list_services[i]);
	}
	nbre_services = 0;
}

void icwmp_restart_services()
{
	int i;

	for (i = 0; i < nbre_services; i++) {
		if (list_services[i] == NULL)
			continue;

		cwmp_ubus_call("uci", "commit",
			       CWMP_UBUS_ARGS{ { "config", { .str_val = list_services[i] }, UBUS_String } }, 1, NULL,
			       NULL);

		if (strcmp(list_services[i], "firewall") == 0) {
			g_firewall_restart = true;
		}
	}
	if (g_firewall_restart) {
			CWMP_LOG(INFO, "Initiating Firewall restart")
			cwmp_uci_set_varstate_value("cwmp", "cpe", "firewall_restart", "in_progress");
			cwmp_commit_package("cwmp", UCI_VARSTATE_CONFIG);
	}
	icwmp_free_list_services();
}

/*
 * Arguments validation
 */
bool icwmp_validate_string_length(char *arg, int max_length)
{
	if (arg != NULL && strlen(arg) > (size_t)max_length)
		return false;
	return true;
}

bool icwmp_validate_boolean_value(char *arg)
{
	if (!arg ||( strcmp(arg, "1") != 0 && strcmp(arg, "0") != 0 && strcmp(arg, "true") != 0 && strcmp(arg, "false") != 0))
		return false;
	return true;
}

bool icwmp_validate_unsignedint(char *arg)
{
	int arg_int;

	if(arg == NULL)
		return false;

	if (strcmp(arg, "0") == 0)
		arg_int = 0;
	else {
		arg_int = atoi(arg);
		if (arg_int == 0)
			return false;
	}
	return arg_int >= 0;
}

bool icwmp_validate_int_in_range(char *arg, int min, int max)
{
	int arg_int;

	if(arg == NULL)
		return false;

	if (strcmp(arg, "0") == 0)
		arg_int = 0;
	else {
		arg_int = atoi(arg);
		if (arg_int == 0)
			return false;
	}
	return arg_int >= min && arg_int <= max;
}

char *string_to_hex(const unsigned char *str, size_t size)
{
	size_t i;

	if (size == 0)
		return "";

	char* hex = (char*) calloc(size * 2 + 1, sizeof(char));

	if (!hex) {
		CWMP_LOG(ERROR, "Unable to allocate memory for hex string\n");
		return NULL;
	}

	for (i = 0; i < size; i++)
		sprintf(hex + (i * 2), "%02X", str[i]);

	return hex;
}

char *generate_random_string(size_t size)
{
	unsigned char *buf = NULL;
	char *hex = NULL;

	buf = (unsigned char *)calloc(size + 1, sizeof(unsigned char));
	if (buf == NULL) {
		CWMP_LOG(ERROR, "Unable to allocate memory for buf string\n");
		goto end;
	}

	int written = RAND_bytes(buf, size);
	if (written != 1) {
		printf("Failed to get random bytes");
		goto end;
	}

	hex = string_to_hex(buf, size);
	if (hex == NULL)
			goto end;

	hex[size] = '\0';

end:
	FREE(buf);
	return hex;
}


int copy_file(char *source_file, char *target_file)
{
	char ch;
	FILE *source, *target;
	source = fopen(source_file, "r");
	if (source == NULL) {
		CWMP_LOG(ERROR, "Not able to open the source file: %s\n", source_file);
		return -1;
	}
	target = fopen(target_file, "w");
	if (target == NULL) {
		fclose(source);
		CWMP_LOG(ERROR, "Not able to open the target file: %s\n", target_file);
		return -1;
	}

	ch = fgetc(source);
	while( feof(source) != EOF) {
		fputc(ch, target);
		ch = fgetc(source);
	}

	CWMP_LOG(ERROR, "File copied successfully.\n");
	fclose(source);
	fclose(target);
	return 0;
}
