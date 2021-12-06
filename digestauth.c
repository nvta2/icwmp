/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *  HTTP digest auth functions: originally imported from libmicrohttpd
 *
 *	Copyright (C) 2013 Oussama Ghorbel <oussama.ghorbel@pivasoftware.com>
 *					   Omar Kallel <omar.kallel@pivasoftware.com>
 *
 */

#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <limits.h>

#include "common.h"
#include "digestauth.h"
#include "md5.h"
#include "log.h"

#define HASH_MD5_HEX_LEN (2 * MD5_DIGEST_SIZE)

/**
 * Maximum length of a username for digest authentication.
 */
#define MAX_USERNAME_LENGTH 1024

/**
 * Maximum length of a realm for digest authentication.
 */
#define MAX_REALM_LENGTH 1024

/**
 * Maximum length of the response in digest authentication.
 */
#define MAX_AUTH_RESPONSE_LENGTH 1024

#define MAX_NONCE_LENGTH 1024

char *nonce_privacy_key = NULL;

int generate_nonce_priv_key()
{
	nonce_privacy_key = generate_random_string(28);
	if (nonce_privacy_key == NULL)
		return CWMP_GEN_ERR;
	return CWMP_OK;
}

static time_t mhd_monotonic_time(void)
{
	struct timespec ts;
	if (clock_gettime(CLOCK_MONOTONIC, &ts) == 0)
		return ts.tv_sec;
	return time(NULL);
}

/**
 * convert bin to hex
 *
 * @param bin binary data
 * @param len number of bytes in bin
 * @param hex pointer to len*2+1 bytes
 */
static void cvthex(const unsigned char *bin, size_t len, char *hex)
{
	size_t i;

	for (i = 0; i < len; ++i) {
		unsigned int j;
		j = (bin[i] >> 4) & 0x0f;
		hex[i * 2] = j <= 9 ? (j + '0') : (j + 'a' - 10);
		j = bin[i] & 0x0f;
		hex[i * 2 + 1] = j <= 9 ? (j + '0') : (j + 'a' - 10);
	}
	hex[len * 2] = '\0';
}

/**
 * Calculate the server nonce so that it mitigates replay attacks
 * The current format of the nonce is ...
 * H(timestamp ":" method ":" random ":" uri ":" realm) + Hex(timestamp)
 *
 * @param nonce_time The amount of time in seconds for a nonce to be invalid
 * @param method HTTP method
 * @param rnd A pointer to a character array for the random seed
 * @param rnd_size The size of the random seed array
 * @param uri HTTP URI (in MHD, without the arguments ("?k=v")
 * @param realm A string of characters that describes the realm of auth.
 * @param nonce A pointer to a character array for the nonce to put in
 */
static void calculate_nonce(uint32_t nonce_time, const char *method, const char *rnd, unsigned int rnd_size, const char *uri, const char *realm, char *nonce)
{
	struct MD5Context md5;
	unsigned char timestamp[4];
	unsigned char tmpnonce[MD5_DIGEST_SIZE];
	char timestamphex[sizeof(timestamp) * 2 + 1];

	md5_init(&md5);
	timestamp[0] = (nonce_time & 0xff000000) >> 0x18;
	timestamp[1] = (nonce_time & 0x00ff0000) >> 0x10;
	timestamp[2] = (nonce_time & 0x0000ff00) >> 0x08;
	timestamp[3] = (nonce_time & 0x000000ff);
	md5_update(&md5, timestamp, 4);
	md5_update(&md5, ":", 1);
	md5_update(&md5, method, strlen(method));
	md5_update(&md5, ":", 1);
	if (rnd_size > 0)
		md5_update(&md5, rnd, rnd_size);
	md5_update(&md5, ":", 1);
	md5_update(&md5, uri, strlen(uri));
	md5_update(&md5, ":", 1);
	md5_update(&md5, realm, strlen(realm));
	md5_final(tmpnonce, &md5);
	cvthex(tmpnonce, sizeof(tmpnonce), nonce);
	cvthex(timestamp, 4, timestamphex);
	strncat(nonce, timestamphex, 8);
}

/**
 * Lookup subvalue off of the HTTP Authorization header.
 *
 * A description of the input format for 'data' is at
 * http://en.wikipedia.org/wiki/Digest_access_authentication
 *
 *
 * @param dest where to store the result (possibly truncated if
 *             the buffer is not big enough).
 * @param size size of dest
 * @param data pointer to the Authorization header
 * @param key key to look up in data
 * @return size of the located value, 0 if otherwise
 */
static int lookup_sub_value(char *dest, size_t size, const char *data, const char *key)
{
	size_t keylen;
	size_t len;
	const char *ptr;
	const char *eq;
	const char *q2;
	const char *qn;

	unsigned int diff;
	if (0 == size)
		return 0;
	keylen = strlen(key);
	ptr = data;
	while ('\0' != *ptr) {
		const char *q1;
		if (NULL == (eq = strchr(ptr, '=')))
			return 0;
		q1 = eq + 1;
		while (' ' == *q1)
			q1++;
		if ('\"' != *q1) {
			q2 = strchr(q1, ',');
			qn = q2;
		} else {
			q1++;
			q2 = strchr(q1, '\"');
			if (NULL == q2)
				return 0; /* end quote not found */
			qn = q2 + 1;
		}
		if ((0 == strncasecmp(ptr, key, keylen)) && (eq == &ptr[keylen])) {
			if (NULL == q2) {
				len = strlen(q1) + 1;
				if (size > len)
					size = len;
				size--;
				strncpy(dest, q1, size);
				dest[size] = '\0';
				return size;
			} else {
				diff = (q2 - q1) + 1;
				if (size > diff)
					size = diff;
				size--;
				memcpy(dest, q1, size);
				dest[size] = '\0';
				return size;
			}
		}
		if (NULL == qn)
			return 0;
		ptr = strchr(qn, ',');
		if (NULL == ptr)
			return 0;
		ptr++;
		while (' ' == *ptr)
			ptr++;
	}
	return 0;
}

/**
 * calculate H(A1) as per RFC2617 spec and store the
 * result in 'sessionkey'.
 *
 * @param alg The hash algorithm used, can be "md5" or "md5-sess"
 * @param username A `char *' pointer to the username value
 * @param realm A `char *' pointer to the realm value
 * @param password A `char *' pointer to the password value
 * @param nonce A `char *' pointer to the nonce value
 * @param cnonce A `char *' pointer to the cnonce value
 * @param sessionkey pointer to buffer of HASH_MD5_HEX_LEN+1 bytes
 */
static void digest_calc_ha1(const char *alg, const char *username, const char *realm, const char *password, const char *nonce, const char *cnonce, char *sessionkey)
{
	struct MD5Context md5;
	unsigned char ha1[MD5_DIGEST_SIZE];

	md5_init(&md5);
	md5_update(&md5, username, strlen(username));
	md5_update(&md5, ":", 1);
	md5_update(&md5, realm, strlen(realm));
	md5_update(&md5, ":", 1);
	md5_update(&md5, password, strlen(password));
	md5_final(ha1, &md5);
	if (0 == strcasecmp(alg, "md5-sess")) {
		md5_init(&md5);
		md5_update(&md5, ha1, sizeof(ha1));
		md5_update(&md5, ":", 1);
		md5_update(&md5, nonce, strlen(nonce));
		md5_update(&md5, ":", 1);
		md5_update(&md5, cnonce, strlen(cnonce));
		md5_final(ha1, &md5);
	}
	cvthex(ha1, sizeof(ha1), sessionkey);
}

/**
 * Calculate request-digest/response-digest as per RFC2617 spec
 *
 * @param ha1 H(A1)
 * @param nonce nonce from server
 * @param noncecount 8 hex digits
 * @param cnonce client nonce
 * @param qop qop-value: "", "auth" or "auth-int"
 * @param method method from request
 * @param uri requested URL
 * @param hentity H(entity body) if qop="auth-int"
 * @param response request-digest or response-digest
 */
static void digest_calc_response(const char *ha1, const char *nonce, const char *noncecount, const char *cnonce, const char *qop, const char *method, const char *uri, char *response)
{
	struct MD5Context md5;
	unsigned char ha2[MD5_DIGEST_SIZE];
	unsigned char resphash[MD5_DIGEST_SIZE];
	char ha2hex[HASH_MD5_HEX_LEN + 1];

	md5_init(&md5);
	md5_update(&md5, method, strlen(method));
	md5_update(&md5, ":", 1);
	md5_update(&md5, uri, strlen(uri));
	md5_final(ha2, &md5);
	cvthex(ha2, MD5_DIGEST_SIZE, ha2hex);
	md5_init(&md5);
	/* calculate response */
	md5_update(&md5, ha1, HASH_MD5_HEX_LEN);
	md5_update(&md5, ":", 1);
	md5_update(&md5, nonce, strlen(nonce));
	md5_update(&md5, ":", 1);
	if ('\0' != *qop) {
		md5_update(&md5, noncecount, strlen(noncecount));
		md5_update(&md5, ":", 1);
		md5_update(&md5, cnonce, strlen(cnonce));
		md5_update(&md5, ":", 1);
		md5_update(&md5, qop, strlen(qop));
		md5_update(&md5, ":", 1);
	}
	md5_update(&md5, ha2hex, HASH_MD5_HEX_LEN);
	md5_final(resphash, &md5);
	cvthex(resphash, sizeof(resphash), response);
}

/**
 * make response to request authentication from the client
 *
 * @param fp
 * @param http_method
 * @param url
 * @param realm the realm presented to the client
 * @param opaque string to user for opaque value
 * @return MHD_YES on success, MHD_NO otherwise
 */

int http_digest_auth_fail_response(FILE *fp, const char *http_method, const char *url, const char *realm, const char *opaque)
{
	size_t hlen, nonce_key_len = 0;
	char nonce[HASH_MD5_HEX_LEN + 9];

	/* Generating the server nonce */
	nonce_key_len = nonce_privacy_key ? strlen(nonce_privacy_key) : 0;
	calculate_nonce((uint32_t)mhd_monotonic_time(), http_method, nonce_privacy_key, nonce_key_len, url, realm, nonce);

	/* Building the authentication header */
	hlen = snprintf(NULL, 0, "Digest realm=\"%s\",qop=\"auth\",nonce=\"%s\",opaque=\"%s\"", realm, nonce, opaque);
	{
		char header[hlen + 1];

		snprintf(header, sizeof(header), "Digest realm=\"%s\",qop=\"auth\",nonce=\"%s\",opaque=\"%s\"", realm, nonce, opaque);

		DD(DEBUG, "%s: header: %s", __FUNCTION__, header);

		fputs("WWW-Authenticate: ", fp);
		fputs(header, fp);
		return MHD_YES;
	}
	return MHD_NO;
}

/**
 * Authenticates the authorization header sent by the client
 *
 * @param http_method
 * @param url
 * @param header: pointer to the position just after the string "Authorization: Digest "
 * @param realm The realm presented to the client
 * @param username The username needs to be authenticated
 * @param password The password used in the authentication
 * @param nonce_timeout The amount of time for a nonce to be
 * 			invalid in seconds
 * @return MHD_YES if authenticated, MHD_NO if not,
 * 			MHD_INVALID_NONCE if nonce is invalid
 */
int http_digest_auth_check(const char *http_method, const char *url, const char *header, const char *realm, const char *username, const char *password, unsigned int nonce_timeout)
{
	size_t len;
	char *end;
	char nonce[MAX_NONCE_LENGTH];
	size_t left; /* number of characters left in 'header' for 'uri' */

	DD(DEBUG, "%s: header: %s", __FUNCTION__, header);

	left = strlen(header);

	{
		char un[MAX_USERNAME_LENGTH];

		len = lookup_sub_value(un, sizeof(un), header, "username");
		if (0 != strcmp(username, un))
			return MHD_NO;
		left -= strlen("username") + len;
	}

	{
		char r[MAX_REALM_LENGTH];

		len = lookup_sub_value(r, sizeof(r), header, "realm");
		if ((0 == len) || (0 != strcmp(realm, r)))
			return MHD_NO;
		left -= strlen("realm") + len;
	}

	if (0 == (len = lookup_sub_value(nonce, sizeof(nonce), header, "nonce")))
		return MHD_NO;
	left -= strlen("nonce") + len;

	{
		char uri[left];
		char cnonce[MAX_NONCE_LENGTH];
		char qop[15]; /* auth,auth-int */
		char nc[20];
		char response[MAX_AUTH_RESPONSE_LENGTH];
		char ha1[HASH_MD5_HEX_LEN + 1];
		char respexp[HASH_MD5_HEX_LEN + 1];
		char noncehashexp[HASH_MD5_HEX_LEN + 9];
		uint32_t nonce_time;
		unsigned long int nci;
		uint32_t t;
		size_t nonce_key_len = 0;


		if (0 == lookup_sub_value(uri, sizeof(uri), header, "uri"))
			return MHD_NO;

		/* 8 = 4 hexadecimal numbers for the timestamp */
		nonce_time = strtoul(nonce + len - 8, (char **)NULL, 16);
		t = (uint32_t)mhd_monotonic_time();
		/*
		 * First level vetting for the nonce validity if the timestamp
		 * attached to the nonce exceeds `nonce_timeout' then the nonce is
		 * invalid.
		 */
		if ((t > nonce_time + nonce_timeout) || (nonce_time + nonce_timeout < nonce_time)) {
			CWMP_LOG(ERROR, "Timestamp attached to the nonce exceeds");
			return MHD_NO;
		}

		if (0 != strncmp(uri, url, strlen(url))) {
			DD(DEBUG, "Authentication failed: URI does not match.");
			return MHD_NO;
		}
		if (nonce_privacy_key == NULL) {
			if (generate_nonce_priv_key() != CWMP_OK)
				return MHD_INVALID_NONCE;
		}
		nonce_key_len = strlen(nonce_privacy_key);
		calculate_nonce(nonce_time, http_method, nonce_privacy_key, nonce_key_len, url, realm, noncehashexp);
		
		/*
		 * Second level vetting for the nonce validity
		 * if the timestamp attached to the nonce is valid
		 * and possibly fabricated (in case of an attack)
		 * the attacker must also know the random seed to be
		 * able to generate a "sane" nonce, which if he does
		 * not, the nonce fabrication process going to be
		 * very hard to achieve.
		 */

		if (0 != strcmp(nonce, noncehashexp)) {
			CWMP_LOG(ERROR, "Nonce value is valid and possibly fabricated");
			return MHD_NO;
		}

		if ((0 == lookup_sub_value(cnonce, sizeof(cnonce), header, "cnonce")) || (0 == lookup_sub_value(qop, sizeof(qop), header, "qop")) || ((0 != strcmp(qop, "auth")) && (0 != strcmp(qop, ""))) || (0 == lookup_sub_value(nc, sizeof(nc), header, "nc")) ||
		    (0 == lookup_sub_value(response, sizeof(response), header, "response"))) {
			DD(DEBUG, "Authentication failed, invalid format.");
			return MHD_NO;
		}
		nci = strtoul(nc, &end, 16);
		if (('\0' != *end) || ((LONG_MAX == nci) && (ERANGE == errno))) {
			DD(DEBUG, "Authentication failed, invalid format.");
			return MHD_NO; /* invalid nonce format */
		}

		/*
		 * Checking if that combination of nonce and nc is sound
		 * and not a replay attack attempt. Also adds the nonce
		 * to the nonce-nc map if it does not exist there.
		 */

		digest_calc_ha1("md5", username, realm, password, nonce, cnonce, ha1);
		digest_calc_response(ha1, nonce, nc, cnonce, qop, http_method, uri, respexp);
		return (0 == strcmp(response, respexp)) ? MHD_YES : MHD_NO;
	}
}
