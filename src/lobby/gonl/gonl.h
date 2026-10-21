/* gonl.h - common declarations of the G online PC port's generated lobby C (src/lobby/gonl/gen, tools/mhg_gonl.py;
 * docs/network.md 3.12). m2c's drafts use these types and helpers; every file declares the rest itself. */
#ifndef GONL_H
#define GONL_H
#include "types.h"
typedef long long s64;
typedef double f64;
typedef unsigned long long u64;
#ifndef NULL
#define NULL ((void *)0)
#endif
/* m2c's helpers that can survive the conversion */
typedef struct { u32 w[4]; } s128;   /* m2c: a 128-bit register (lq / sq) */
#define M2C_ERROR(x) 0
#define M2C_BITWISE(T, x) ((union { __typeof__(x) a; T b; }){ .a = (x) }.b)   /* the bits of x as a T (mfc1 / mtc1) */
#define M2C_CARRY 0
#endif
