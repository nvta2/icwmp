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
#include <microxml.h>

#ifndef __SMMEM_H
#define __SMMEM_H

static inline void sm_empty_func()
{
}

#define CWMP_WITH_MEMLEACKSEC 1
//#define CWMP_WITH_MEMTRACK 1

#ifndef CWMP_WITH_MEMLEACKSEC
#undef CWMP_WITH_MEMTRACK
#endif

#ifdef CWMP_WITH_MEMLEACKSEC
struct smmem {
	struct list_head list;
#ifdef CWMP_WITH_MEMTRACK
	char *file;
	char *func;
	int line;
	int size;
#endif /*CWMP_WITH_MEMTRACK*/
	char mem[0];
};

#endif

static inline void *__smmalloc
(
#ifdef CWMP_WITH_MEMTRACK
const char *file, const char *func, int line,
#endif /*CWMP_WITH_MEMTRACK*/
struct list_head *chead, size_t size
)
{
	struct smmem *m = malloc(sizeof(struct smmem) + size);
	if (m == NULL) return NULL;
	list_add(&m->list, chead);
#ifdef CWMP_WITH_MEMTRACK
	m->file = (char *)file;
	m->func = (char *)func;
	m->line = line;
	m->size = size;
#endif /*CWMP_WITH_MEMTRACK*/
	return (void *)m->mem;
}

static inline void *__smrealloc
(
#ifdef CWMP_WITH_MEMTRACK
const char *file, const char *func, int line,
#endif /*CWMP_WITH_MEMTRACK*/
struct list_head *chead, void *old, size_t size
)
{
	struct smmem *m = NULL;
	if (old != NULL) {
		m = container_of(old, struct smmem, mem);
		list_del(&m->list);
	}
	m = realloc(m, sizeof(struct smmem) + size);
	if (m == NULL) return NULL;
	list_add(&m->list, chead);
#ifdef CWMP_WITH_MEMTRACK
	m->file = (char *)file;
	m->func = (char *)func;
	m->line = line;
	m->size = size;
#endif /*CWMP_WITH_MEMTRACK*/
	return (void *)m->mem;
}

static inline char *__smstrdup
(
#ifdef CWMP_WITH_MEMTRACK
const char *file, const char *func, int line,
#endif /*CWMP_WITH_MEMTRACK*/
struct list_head *chead, const char *s
)
{
	size_t len = strlen(s) + 1;
#ifdef CWMP_WITH_MEMTRACK
	void *new = __smmalloc(file, func, line, chead, len);
#else
	void *new = __smmalloc(chead, len);
#endif /*CWMP_WITH_MEMTRACK*/
	if (new == NULL) return NULL;
	return (char *) memcpy(new, s, len);
}

static inline void smfree(void *m)
{
	if (m == NULL) return;
	struct smmem *rm;
	rm = container_of(m, struct smmem, mem);
	list_del(&rm->list);
	free(rm);
}

static inline void smcleanmem(struct list_head *chead)
{
	struct smmem *smm;
	while (chead->next != chead) {
		smm = list_entry(chead->next, struct smmem, list);
#ifdef CWMP_WITH_MEMTRACK
		//fprintf(stderr, "Allocated memory in {%s, %s(), line %d} is not freed\n", smm->file, smm->func, smm->line);
#endif /*CWMP_WITH_MEMTRACK*/
		list_del(&smm->list);
		free(smm);
	}
}


static inline void smprintmem(struct list_head *chead)
{
	struct smmem *smm;
#ifdef CWMP_WITH_MEMTRACK
	fprintf(stderr,"\n****************************************************************************\n");
	list_for_each_entry(smm, chead, list) {
		fprintf(stderr, "Allocated Memory ====> (%p) - size : (%d) <==== in {%s, %s(), line %d}\n", smm->mem, smm->size, smm->file, smm->func, smm->line);
	}
	fprintf(stderr,"\n****************************************************************************\n");
	fflush(stderr);
#endif /*CWMP_WITH_MEMTRACK*/
}

static inline int __smasprintf
(
#ifdef CWMP_WITH_MEMTRACK
const char *file, const char *func, int line,
#endif /*CWMP_WITH_MEMTRACK*/
struct list_head *chead, char **s, const char *format, ...
)
{
	char *buf = NULL;
	va_list arg;
	int ret;

	va_start(arg,format);
	ret = vasprintf(&buf, format, arg);
	va_end(arg);
	if (buf == NULL){
		*s = NULL;
		return -1;
	}


#ifdef CWMP_WITH_MEMTRACK
	*s = __smstrdup(file, func, line, chead, buf);
#else
	*s = __smstrdup(chead, buf);
#endif /*CWMP_WITH_MEMTRACK*/
	free(buf);
	if (*s == NULL) return -1;
	return 0;
}

static inline int __smastrcat
(
#ifdef CWMP_WITH_MEMTRACK
const char *file, const char *func, int line,
#endif /*CWMP_WITH_MEMTRACK*/
struct list_head *chead, char **s, char *obj, char *lastname
)
{
	char buf[512];
	int olen = strlen(obj);
	memcpy(buf, obj, olen);
	int llen = strlen(lastname) + 1;
	memcpy(buf + olen, lastname, llen);
#ifdef CWMP_WITH_MEMTRACK
	*s = __smstrdup(file, func, line, chead, buf);
#else
	*s = __smstrdup(chead, buf);
#endif /*CWMP_WITH_MEMTRACK*/
	if (*s == NULL) return -1;
	return 0;	
}

static inline void *__smcalloc
(
#ifdef CWMP_WITH_MEMTRACK
const char *file, const char *func, int line,
#endif /*CWMP_WITH_MEMTRACK*/
struct list_head *chead, int n, size_t size
)
{
	struct smmem *m = calloc(n, sizeof(struct smmem) + size);
	if (m == NULL) return NULL;
	list_add(&m->list, chead);
#ifdef CWMP_WITH_MEMTRACK
	m->file = (char *)file;
	m->func = (char *)func;
	m->line = line;
	m->size = size;
#endif /*CWMP_WITH_MEMTRACK*/
	return (void *)m->mem;
}


static inline void  *__smmxmlSaveAllocString(
#ifdef CWMP_WITH_MEMTRACK
const char *file, const char *func, int line,
#endif /*CWMP_WITH_MEMTRACK*/
struct list_head *chead, mxml_node_t *node_t , mxml_save_cb_t callback
)
{
	char *str = mxmlSaveAllocString(node_t, callback);
	int len = strlen(str)+1;
	struct smmem *m = calloc(1, sizeof(struct smmem) + len);
	if (m == NULL) return NULL;
	list_add(&m->list, chead);
	strcpy((char *)m->mem, str);
	free(str);
#ifdef CWMP_WITH_MEMTRACK
	m->file = (char *)file;
	m->func = (char *)func;
	m->line = line;
	m->size = len;
#endif /*CWMP_WITH_MEMTRACK*/
	return (void *)m->mem;
}
#endif //__SMMEM_H