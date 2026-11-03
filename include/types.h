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
#define MH2_EM(o) MH2_OFS_EMW(o)  /* for pads of local copies of EMW */
typedef u16 stg_t;              /* stage number: 16 bits in MH2 */
typedef u16 qno_t;              /* quest number in copies of quest_w's head: lhu in MH2 */
#include "mh2_lay_ofs.h"
#else
#define MH2_OFS(p, o) (o)
#define MH2V(mh1, mh2) (mh1)
#define MH2_EM(o) (o)
typedef u8 stg_t;
typedef s16 qno_t;
#define QWO(o) (o)
#endif
#endif
