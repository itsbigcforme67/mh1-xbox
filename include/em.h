#ifndef EM_H
#define EM_H
/* Monster work: em_work[], 0xA10 bytes per monster. Shares its start with
 * PLW (id at 0xC, char0 at 0x2DC, stg at 0x736): probably a common header.
 * Offsets from matched code (shell18.c). */
#include "types.h"

typedef struct VEC3 {
    f32 x, y, z;
} VEC3;


typedef struct EMW {
    u8 be_flag;         /* 0x000 alive; shells end when it clears (shell11_m) */
    u8 x01;             /* 0x001 (set10_m) */
    u8 kind;            /* 0x002 monster kind (set10_m checks 7) */
    u8 _pad003;
    u8 x04;             /* 0x004 shells end when >= 2 (shell02_m) */
    u8 _pad005[0x0C - 0x05];
    u16 id;             /* 0x00C */
    u8 _pad00E[2];
    u8 x10;             /* 0x010 */
    u8 _pad011[0x14 - 0x11];
    u8 mode;            /* 0x014 4/5 end attached shells (shell19_m) */
    u8 _pad015[0x19 - 0x15];
    u8 x19;             /* 0x019 cleared when a shell is spawned */
    u8 _pad01A[0xA0 - 0x1A];
    s32 ang[3];         /* 0x0A0 rotation, 0x10000 = 360 degrees (shell14_trans) */
    f32 pos[3];         /* 0x0AC world position (set20_m, as PLW) */
    u8 _pad0B8[0x2DC - 0xB8];
    u16 char0;          /* 0x2DC current animation (as PLW) */
    u8 _pad2DE[0x2E4 - 0x2DE];
    u16 act_tm0;        /* 0x2E4 */
    u16 act_tm1;        /* 0x2E6 */
    u8 _pad2E8[0x2EC - 0x2E8];
    s16 blend0;         /* 0x2EC */
    s16 blend1;         /* 0x2EE */
    u8 _pad2F0[0x388 - 0x2F0];
    u8 x388;            /* 0x388 non-zero keeps set20's gate shut */
    u8 _pad389[0x5AC - 0x389];
    f32 x5AC;           /* 0x5AC height used for set20's shell */
    u8 _pad5B0[0x736 - 0x5B0];
    u8 stg;             /* 0x736 */
    u8 _pad737[0xA10 - 0x737];
} EMW;

extern EMW em_work[];

#endif
