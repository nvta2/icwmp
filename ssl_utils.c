/*
 * ssl_utils.c: Utility functions with ssl
 *
 * Copyright (C) 2022 iopsys Software Solutions AB. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */

#include <wolfssl/openssl/ssl.h>
#include <wolfssl/openssl/hmac.h>

#include "common.h"
#include "log.h"

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

void message_compute_signature(char *msg_out, char *signature)
{
	int i;
	int result_len = 20;
	unsigned char *result;
	struct cwmp *cwmp = cwmp_main;
	struct config *conf;
	conf = &(cwmp->conf);
	/*	unsigned char *HMAC(const EVP_MD *evp_md, const void *key, int key_len,
	                    const unsigned char *d, size_t n, unsigned char *md,
	                    unsigned int *md_len);*/
	result = HMAC(EVP_sha1(), conf->acs_passwd, strlen(conf->acs_passwd), (unsigned char *)msg_out, strlen(msg_out), NULL, NULL);
	for (i = 0; i < result_len; i++) {
		sprintf(&(signature[i * 2]), "%02X", result[i]);
	}
	signature[i * 2] = '\0';
	FREE(result);
}

