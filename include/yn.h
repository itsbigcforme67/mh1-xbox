#ifndef YN_H
#define YN_H
/* Declarations for the yn.bin overlay (network setup screens). Unknown work structs are accessed
 * by byte offset with M2C_FIELD (identical code to a struct field access). */
#include "g_alias.h"
#include "types.h"

typedef long s64;
typedef unsigned long u64;
#define M2C_FIELD(ptr, type, off) (*(type)((u8 *)(ptr) + (off)))
#define NULL 0
/* G's work struct (ynw) has 4 more bytes at 0x28: every field from there to the netcnf work (0x1180, unmoved) is 4 higher.
 * YNO below shifts only up to 0x1100: which end is right is not checked (readability triage, 4 Nov 2026). */
#ifdef VERSION_MHG
#define YNO(o) (((o) >= 0x28 && (o) < 0x1100) ? (o) + 4 : (o))
#else
#define YNO(o) (o)
#endif
#define YNF(ptr, type, off) M2C_FIELD(ptr, type, YNO(off))

int strcmp();
char *strcpy();
int strlen();
void *memset();
int sprintf();
int strncmp();
#endif
