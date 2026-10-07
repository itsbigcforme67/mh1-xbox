/*
 * rt_memstat.h - forced include (-include) for the port's own C (src/pc):
 * malloc/calloc/realloc/free go through rt_memstat.c, which counts
 * live bytes per category (by the allocating source file, or a category the
 * caller sets around a load). RT_MEM=1 prints the table at checkpoints.
 * The decompiled game C is not wrapped: it uses fixed work areas, counted
 * from the object files' .data/.bss (tools/pc_memstat.py).
 */
#ifndef RT_MEMSTAT_H
#define RT_MEMSTAT_H
#include <stdlib.h>
void *rt_ms_malloc(size_t n, const char *file);
void *rt_ms_calloc(size_t k, size_t n, const char *file);
void *rt_ms_realloc(void *p, size_t n, const char *file);
void rt_ms_free(void *p);
/* A file that includes a library header using these names otherwise (a
 * struct member or callback called free, e.g. libmpeg2) can opt out with
 * -DRT_MEMSTAT_NO_MACROS; its allocations are then not counted. */
#if !defined(RT_MEMSTAT_IMPL) && !defined(RT_MEMSTAT_NO_MACROS)
#define malloc(n) rt_ms_malloc((n), __FILE__)
#define calloc(k, n) rt_ms_calloc((k), (n), __FILE__)
#define realloc(p, n) rt_ms_realloc((p), (n), __FILE__)
#define free(p) rt_ms_free(p)
#endif
/* a category for allocations made until the matching pop (e.g. a file read
 * that becomes sound-pack data); NULL = by source file */
const char *rt_ms_push(const char *cat);
void rt_ms_pop(const char *prev);
/* counters the backends add to directly (GPU-side copies) */
void rt_ms_add(const char *cat, long bytes);
void rt_ms_report(const char *where);
/* RT_STACK=1: stack depth watermark of the main thread (rt_memstat.c) */
void rt_stack_paint(void);
void rt_stack_report(const char *where);
#endif
