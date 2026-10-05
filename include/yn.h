#ifndef YN_H
#define YN_H
/* Declarations for the yn.bin overlay (network setup screens). Unknown work structs are accessed
 * by byte offset with M2C_FIELD (identical code to a struct field access). */
#include "types.h"

typedef long s64;
typedef unsigned long u64;
#define M2C_FIELD(ptr, type, off) (*(type)((u8 *)(ptr) + (off)))
#define NULL 0

int strcmp();
char *strcpy();
int strlen();
void *memset();
int sprintf();
int strncmp();
#endif
