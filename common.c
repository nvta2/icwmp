/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2013-2020 iopsys Software Solutions AB
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

#include "common.h"
#include "cwmp_uci.h"
#include "ubus.h"
#include "log.h"
#include "cwmp_cli.h"
#include "cwmp_du_state.h"

char *commandKey = NULL;
long int flashsize = 256000000;
static unsigned long int next_rand_seed = 1;
struct cwmp cwmp_main = { 0 };

LIST_HEAD(cwmp_memory_list);

struct cwmp_mem {
	struct list_head list;
	char mem[0];
};

struct option cwmp_long_options[] = { { "boot-event", no_argument, NULL, 'b' }, { "get-rpc-methods", no_argument, NULL, 'g' }, { "command-input", no_argument, NULL, 'c' }, { "help", no_argument, NULL, 'h' }, { "version", no_argument, NULL, 'v' }, { NULL, 0, NULL, 0 } };

struct FAULT_CPE FAULT_CPE_ARRAY[] = {[FAULT_CPE_METHOD_NOT_SUPPORTED] = { "9000", FAULT_9000, FAULT_CPE_TYPE_SERVER, "Method not supported" },
				      [FAULT_CPE_REQUEST_DENIED] = { "9001", FAULT_9001, FAULT_CPE_TYPE_SERVER, "Request denied (no reason specified)" },
				      [FAULT_CPE_INTERNAL_ERROR] = { "9002", FAULT_9002, FAULT_CPE_TYPE_SERVER, "Internal error" },
				      [FAULT_CPE_INVALID_ARGUMENTS] = { "9003", FAULT_9003, FAULT_CPE_TYPE_CLIENT, "Invalid arguments" },
				      [FAULT_CPE_RESOURCES_EXCEEDED] = { "9004", FAULT_9004, FAULT_CPE_TYPE_SERVER, "Resources exceeded" },
				      [FAULT_CPE_INVALID_PARAMETER_NAME] = { "9005", FAULT_9005, FAULT_CPE_TYPE_CLIENT, "Invalid parameter name" },
				      [FAULT_CPE_INVALID_PARAMETER_TYPE] = { "9006", FAULT_9006, FAULT_CPE_TYPE_CLIENT, "Invalid parameter type" },
				      [FAULT_CPE_INVALID_PARAMETER_VALUE] = { "9007", FAULT_9007, FAULT_CPE_TYPE_CLIENT, "Invalid parameter value" },
				      [FAULT_CPE_NON_WRITABLE_PARAMETER] = { "9008", FAULT_9008, FAULT_CPE_TYPE_CLIENT, "Attempt to set a non-writable parameter" },
				      [FAULT_CPE_NOTIFICATION_REJECTED] = { "9009", FAULT_9009, FAULT_CPE_TYPE_SERVER, "Notification request rejected" },
				      [FAULT_CPE_DOWNLOAD_FAILURE] = { "9010", FAULT_9010, FAULT_CPE_TYPE_SERVER, "Download failure" },
				      [FAULT_CPE_UPLOAD_FAILURE] = { "9011", FAULT_9011, FAULT_CPE_TYPE_SERVER, "Upload failure" },
				      [FAULT_CPE_FILE_TRANSFER_AUTHENTICATION_FAILURE] = { "9012", FAULT_9012, FAULT_CPE_TYPE_SERVER, "File transfer server authentication failure" },
				      [FAULT_CPE_FILE_TRANSFER_UNSUPPORTED_PROTOCOL] = { "9013", FAULT_9013, FAULT_CPE_TYPE_SERVER, "Unsupported protocol for file transfer" },
				      [FAULT_CPE_DOWNLOAD_FAIL_MULTICAST_GROUP] = { "9014", FAULT_9014, FAULT_CPE_TYPE_SERVER, "Download failure: unable to join multicast group" },
				      [FAULT_CPE_DOWNLOAD_FAIL_CONTACT_SERVER] = { "9015", FAULT_9015, FAULT_CPE_TYPE_SERVER, "Download failure: unable to contact file server" },
				      [FAULT_CPE_DOWNLOAD_FAIL_ACCESS_FILE] = { "9016", FAULT_9016, FAULT_CPE_TYPE_SERVER, "Download failure: unable to access file" },
				      [FAULT_CPE_DOWNLOAD_FAIL_COMPLETE_DOWNLOAD] = { "9017", FAULT_9017, FAULT_CPE_TYPE_SERVER, "Download failure: unable to complete download" },
				      [FAULT_CPE_DOWNLOAD_FAIL_FILE_CORRUPTED] = { "9018", FAULT_9018, FAULT_CPE_TYPE_SERVER, "Download failure: file corrupted" },
				      [FAULT_CPE_DOWNLOAD_FAIL_FILE_AUTHENTICATION] = { "9019", FAULT_9019, FAULT_CPE_TYPE_SERVER, "Download failure: file authentication failure" },
				      [FAULT_CPE_DOWNLOAD_FAIL_WITHIN_TIME_WINDOW] = { "9020", FAULT_9020, FAULT_CPE_TYPE_SERVER, "Download failure: unable to complete download" },
				      [FAULT_CPE_DUPLICATE_DEPLOYMENT_UNIT] = { "9026", FAULT_9026, FAULT_CPE_TYPE_SERVER, "Duplicate deployment unit" },
				      [FAULT_CPE_SYSTEM_RESOURCES_EXCEEDED] = { "9027", FAULT_9027, FAULT_CPE_TYPE_SERVER, "System ressources exceeded" },
				      [FAULT_CPE_UNKNOWN_DEPLOYMENT_UNIT] = { "9028", FAULT_9028, FAULT_CPE_TYPE_SERVER, "Unknown deployment unit" },
				      [FAULT_CPE_INVALID_DEPLOYMENT_UNIT_STATE] = { "9029", FAULT_9029, FAULT_CPE_TYPE_SERVER, "Invalid deployment unit state" },
				      [FAULT_CPE_INVALID_DOWNGRADE_REJECTED] = { "9030", FAULT_9030, FAULT_CPE_TYPE_SERVER, "Invalid deployment unit Update: Downgrade not permitted" },
				      [FAULT_CPE_INVALID_UPDATE_VERSION_UNSPECIFIED] = { "9031", FAULT_9031, FAULT_CPE_TYPE_SERVER, "Invalid deployment unit Update: Version not specified" },
				      [FAULT_CPE_INVALID_UPDATE_VERSION_EXIST] = { "9031", FAULT_9032, FAULT_CPE_TYPE_SERVER, "Invalid deployment unit Update: Version already exist" } };

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
void add_dm_parameter_to_list(struct list_head *head, char *param_name, char *param_val, char *param_type, int notification, bool writable)
{
	struct cwmp_dm_parameter *dm_parameter;
	struct list_head *ilist;
	int cmp;
	list_for_each (ilist, head) {
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
	if (param_type)
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
	struct cwmp_dm_parameter *dm_parameter;
	while (list->next != list) {
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
	struct cwmp_param_fault *param_fault;
	while (list_param_fault->next != list_param_fault) {
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

struct transfer_status {
	CURL *easy;
	int halted;
	int counter; /* count write callback invokes */
	int please; /* number of times xferinfo is called while halted */
};

void get_firewall_zone_name_by_wan_iface(char *if_wan, char **zone_name)
{
	struct uci_section *s;
	char *network = NULL;

	cwmp_uci_init(UCI_STANDARD_CONFIG);
	cwmp_uci_foreach_sections("firewall", "zone", s)
	{
		cwmp_uci_get_value_by_section_string(s, "network", &network);
		char *net = strtok(network, " ");
		while (net != NULL) {
			if (strcmp(net, if_wan) == 0) {
				cwmp_uci_get_value_by_section_string(s, "name", zone_name);
				icwmp_free(network);
				goto end;
			}
			net = strtok(NULL, " ");
		}
		icwmp_free(network);
	}

end:
	cwmp_uci_exit();
}

/*
 * updated firewall.cwmp file
 */
int update_firewall_cwmp_file(int port, char *zone_name, char *ip_addr, int ip_type)
{
	FILE *fp;

	remove(FIREWALL_CWMP);
	fp = fopen(FIREWALL_CWMP, "a");
	if (fp == NULL)
		return -1;
	fprintf(fp, "zone_name=%s\n", zone_name);
	fprintf(fp, "port=%d\n", port);
	fprintf(fp, "if [ \"$zone_name\" = \"\" ]; then\n");
	fprintf(fp, "	exit 0\n");
	fprintf(fp, "elif [ \"$zone_name\" = \"icwmp\" ]; then\n");
	fprintf(fp, "	iptables -nL zone_icwmp_input 2> /dev/null\n");
	fprintf(fp, "	if [ $? != 0 ]; then\n");
	fprintf(fp, "		iptables -N zone_icwmp_input\n");
	fprintf(fp, "		iptables -t filter -A INPUT -j zone_icwmp_input\n");
	fprintf(fp, "		iptables -I zone_icwmp_input -p tcp --dport $port -j REJECT\n");
	fprintf(fp, "	else\n");
	fprintf(fp, "		iptables -F zone_icwmp_input\n");
	fprintf(fp, "		iptables -I zone_icwmp_input -p tcp --dport $port -j REJECT\n");
	fprintf(fp, "	fi\n");
	fprintf(fp, "else\n");
	fprintf(fp, "	iptables -F zone_icwmp_input 2> /dev/null\n");
	fprintf(fp, "	iptables -t filter -D INPUT -j zone_icwmp_input 2> /dev/null\n");
	fprintf(fp, "	iptables -X zone_icwmp_input 2> /dev/null\n");
	fprintf(fp, "fi\n");
	if (ip_type == 0)
		fprintf(fp, "iptables -I zone_%s_input -p tcp -s %s --dport %d -j ACCEPT -m comment --comment=\"Open ACS port\"\n", zone_name, ip_addr, port);
	else
		fprintf(fp, "ip6tables -I zone_%s_input -p tcp -s %s --dport %d -j ACCEPT -m comment --comment=\"Open ACS port\"\n", zone_name, ip_addr, port);
	fclose(fp);
	return 0;
}

/*
 * Reboot
 */
void cwmp_reboot(char *command_key)
{
	uci_set_value(UCI_ACS_PARAMETERKEY_PATH, command_key, CWMP_CMD_SET);
	cwmp_commit_package("cwmp");

	sync();
	reboot(RB_AUTOBOOT);
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

	snprintf(cmd, sizeof(cmd), "opkg --force-depends --force-maintainer install %s", package_path);
	if (cmd == NULL)
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

int icwmp_rand(void) // RAND_MAX assumed to be 32767
{
	next_rand_seed = next_rand_seed * 1103515245 + 12345;
	return (unsigned int)(next_rand_seed / 65536) % 32768;
}

void icwmp_srand(unsigned int seed) //
{
	next_rand_seed = seed;
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
	va_list arg;

	va_start(arg, format);
	size = vasprintf(&str, format, arg);
	va_end(arg);

	if (size < 0 || str == NULL)
		return -1;

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
