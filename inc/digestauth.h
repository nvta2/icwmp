/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *  HTTP digest auth functions: originally imported from libmicrohttpd
 *
 *	Copyright (C) 2013 Oussama Ghorbel <oussama.ghorbel@pivasoftware.com>
 *
 */

#ifndef DIGESTAUTH_H_
#define DIGESTAUTH_H_

/**
 * MHD-internal return code for "YES".
 */
#define MHD_YES 1

/**
 * MHD-internal return code for "NO".
 */
#define MHD_NO 0

/**
 * MHD digest auth internal code for an invalid nonce.
 */
#define MHD_INVALID_NONCE -1

extern char *nonce_privacy_key;

int http_digest_auth_fail_response(FILE *fp, const char *http_method, const char *url, const char *realm, const char *opaque);

int http_digest_auth_check(const char *http_method, const char *url, const char *header, const char *realm, const char *username, const char *password, unsigned int nonce_timeout);

void generate_nonce_priv_key();

#endif /* DIGESTAUTH_H_ */
