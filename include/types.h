#ifndef TYPES_H
#define TYPES_H
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;
typedef int s32;
typedef float f32;
/* C that reaches a work struct by number gives MH1's offset; only MH2 moved them (include/mh2_ofs.h is generated) */
#ifdef VERSION_MH2
#include "mh2_ofs.h"
#define MH2V(mh1, mh2) (mh2)    /* a constant MH2 changed */
#else
#define MH2_OFS(p, o) (o)
#define MH2V(mh1, mh2) (mh1)
#endif
#endif
