#ifndef TYPES_H
#define TYPES_H
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;
typedef int s32;
typedef float f32;
/* a constant MH2 changed (docs/mh2.md): MH1 and G compile the first value */
#ifdef VERSION_MH2
#define MH2V(old, mh2) (mh2)
#else
#define MH2V(old, mh2) (old)
#endif
#endif
