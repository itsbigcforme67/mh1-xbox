#ifndef MAIN_H
#define MAIN_H
/* Common header for m2c-derived near-match C in main.bin (tools/draft2c.py output). Unknown work
 * structs are accessed by byte offset with M2C_FIELD (identical code to a struct field access). */
#include "types.h"

typedef long s64;
typedef unsigned long u64;
#define M2C_FIELD(ptr, type, off) (*(type)((u8 *)(ptr) + (off)))
#define NULL 0

int strcmp();
char *strcpy();
int strlen();
void *memset();
void *memcpy();
int sprintf();
int strncmp();
#endif
