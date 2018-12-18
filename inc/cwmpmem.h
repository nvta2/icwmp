/*	Data Model in C
 *	Copyright (C) 2016 PIVA SOFTWARE <www.pivasoftware.com> - All Rights Reserved
 *		Author: MOHAMED Kallel <mohamed.kallel@pivasoftware.com>
 *		Author: ANIS Ellouze <anis.ellouze@pivasoftware.com>
 *	This software is under the terms of the commercial license attached with this source code
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <libubox/list.h>
#include "smmem.h"
#ifndef __CWMPMEM_H
#define __CWMPMEM_H

#ifdef CWMP_WITH_MEMLEACKSEC
extern struct list_head cwmpmem_ctx;
#ifdef CWMP_WITH_MEMTRACK
/*
#define cwmpmalloc(x) __smmalloc(__FILE__, __func__, __LINE__, &cwmpmem_ctx, x)
#define cwmpcalloc(n, x) __smcalloc(__FILE__, __func__, __LINE__, &cwmpmem_ctx, n, x)
#define cwmprealloc(m, x) __smrealloc(__FILE__, __func__, __LINE__, &cwmpmem_ctx, m, x)
#define cwmpstrdup(x) __smstrdup(__FILE__, __func__, __LINE__, &cwmpmem_ctx, x)
#define cwmpasprintf(s, format, ...) __smasprintf(__FILE__, __func__, __LINE__, &cwmpmem_ctx, s, format, ## __VA_ARGS__)
#define cwmpastrcat(s, b, m) __smastrcat(__FILE__, __func__, __LINE__, &cwmpmem_ctx, s, b, m)
#define cwmpfree(x) smfree(x)
#define cwmpcleanmem() smcleanmem(&cwmpmem_ctx)
#define cwmpmxmlSaveAllocString(a, b) __smmxmlSaveAllocString(__FILE__, __func__, __LINE__, &cwmpmem_ctx, a, b)
*/
/*************/
#define ctx_malloc(ctx, x) __smmalloc(__FILE__, __func__, __LINE__, &((ctx)->head_mem), x)
#define ctx_calloc(ctx, n, x) __smcalloc(__FILE__, __func__, __LINE__, &((ctx)->head_mem), n, x)
#define ctx_realloc(ctx, m, x) __smrealloc(__FILE__, __func__, __LINE__, &((ctx)->head_mem), m, x)
#define ctx_strdup(ctx, x) __smstrdup(__FILE__, __func__, __LINE__, &((ctx)->head_mem), x)
#define ctx_asprintf(ctx, s, format, ...) __smasprintf(__FILE__, __func__, __LINE__, &((ctx)->head_mem), s, format, ## __VA_ARGS__)
#define ctx_astrcat(ctx, s, b, m) __smastrcat(__FILE__, __func__, __LINE__, &((ctx)->head_mem), s, b, m)
#define ctx_free(x) smfree(x)
#define ctx_cleanmem(ctx) smcleanmem(&((ctx)->head_mem))
#define ctx_mxmlSaveAllocString(ctx, a, b) __smmxmlSaveAllocString(__FILE__, __func__, __LINE__, &((ctx)->head_mem), a, b)
#else
/*
#define cwmpmalloc(x) __smmalloc(&cwmpmem_ctx, x)
#define cwmpcalloc(n, x) __smcalloc(&cwmpmem_ctx, n, x)
#define cwmprealloc(m, x) __smrealloc(&cwmpmem_ctx, m, x)
#define cwmpstrdup(x) __smstrdup(&cwmpmem_ctx, x)
#define cwmpasprintf(s, format, ...) __smasprintf(&cwmpmem_ctx, s, format, ## __VA_ARGS__)
#define cwmpastrcat(s, b, m) __smastrcat(&cwmpmem_ctx, s, b, m)
#define cwmpfree(x) smfree(x)
#define cwmpcleanmem() smcleanmem(&cwmpmem_ctx)
#define cwmpmxmlSaveAllocString(a, b) __smmxmlSaveAllocString(&cwmpmem_ctx, a, b)
*/
/*************/
#define ctx_malloc(ctx, x) __smmalloc(&((ctx)->head_mem), x)
#define ctx_calloc(ctx, n, x) __smcalloc(&((ctx)->head_mem), n, x)
#define ctx_realloc(ctx, m, x) __smrealloc(&((ctx)->head_mem), m, x)
#define ctx_strdup(ctx, x) __smstrdup(&((ctx)->head_mem), x)
#define ctx_asprintf(ctx, s, format, ...) __smasprintf(&((ctx)->head_mem), s, format, ## __VA_ARGS__)
#define ctx_astrcat(ctx, s, b, m) __smastrcat(&((ctx)->head_mem), s, b, m)
#define ctx_free(x) smfree(x)
#define ctx_cleanmem(ctx) smcleanmem(&((ctx)->head_mem))
#define ctx_mxmlSaveAllocString(ctx, a, b) __smmxmlSaveAllocString(&((ctx)->head_mem), a, b)
#endif /*CWMP_WITH_MEMTRACK*/
#else
/*
#define cwmpmalloc(x) malloc(x)
#define cwmpcalloc(n, x) calloc(n, x)
#define __smstrdup(m, x) strdup(x)
#define cwmpstrdup(x) strdup(x)
#define cwmpasprintf(s, format, ...) __smasprintf(NULL, s, format, ## __VA_ARGS__)
#define cwmpastrcat(s, b, m) __smastrcat(NULL, s, b, m)
#define cwmpfree(x) free(x)
#define cwmpcleanmem() sm_empty_func()
#define ctx_mxmlSaveAllocString(a, b) mxmlSaveAllocString(a, b)
*/
#endif /*CWMP_WITH_MEMLEACKSEC*/

#define CTXFREE(x) do { smfree(x); x = NULL; } while (0);
#endif //__CWMPMEM_H
