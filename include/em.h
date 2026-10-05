#ifndef EM_H
#define EM_H
/* Monster work: em_work[], 0xA10 bytes per monster. Shares its start with
 * PLW (id at 0xC, char0 at 0x2DC, stg at 0x736): probably a common header.
 * Offsets from matched code (shell18.c). */
#include "types.h"

typedef struct VEC3 {
    f32 x, y, z;
} VEC3;


/* Monster model work (EMW+0x50C). */
typedef struct EM_MDL {
    u8 flag;            /* 0x00 */
    u8 _pad01[0x23];
    u8 *bone;           /* 0x24 bone matrices (byte offsets: 0x4330 tail root) */
    u8 _pad28[8];
    struct CLAY *clay;  /* 0x30 */
    u8 _pad34[0x14];
    u8 *mtx;            /* 0x48 skin matrix list */
} EM_MDL;

typedef struct EMW {
    u8 be_flag;         /* 0x000 alive; shells end when it clears (shell11_m) */
    u8 x01;             /* 0x001 (set10_m) */
    u8 kind;            /* 0x002 monster kind (set10_m checks 7) */
    u8 _pad003;
    u8 x04;             /* 0x004 shells end when >= 2 (shell02_m) */
    u8 _pad005[2];
    u8 x07;             /* 0x007 3 ends attached effects (eft07_m) */
    u8 _pad008[0x0C - 0x08];
    u16 id;             /* 0x00C */
    u8 _pad00E[2];
    u8 x10;             /* 0x010 */
    u8 _pad011[0x14 - 0x11];
    u8 mode;            /* 0x014 4/5 end attached shells (shell19_m) */
    u8 x15;             /* 0x015 sub-mode (eft09_m) */
    u8 _pad016[0x19 - 0x16];
    u8 x19;             /* 0x019 cleared when a shell is spawned */
    u8 _pad01A[0xA0 - 0x1A];
    s32 ang[3];         /* 0x0A0 rotation, 0x10000 = 360 degrees (shell14_trans) */
    f32 pos[3];         /* 0x0AC world position (set20_m, as PLW) */
    f32 scale[3];       /* 0x0B8 model scale (eft09_t) */
    u8 _pad0C4[0x2DC - 0xC4];
    u16 char0;          /* 0x2DC current animation (as PLW) */
    u8 _pad2DE[0x2E4 - 0x2DE];
    u16 act_tm0;        /* 0x2E4 */
    u16 act_tm1;        /* 0x2E6 */
    u8 _pad2E8[0x2EC - 0x2E8];
    s16 blend0;         /* 0x2EC */
    s16 blend1;         /* 0x2EE */
    u8 _pad2F0[0x34F - 0x2F0];
    u8 mdl_no;          /* 0x34F model number (eft09_t: texture and matrix list) */
    u8 _pad350[0x388 - 0x350];
    u8 x388;            /* 0x388 non-zero keeps set20's gate shut */
    u8 _pad389[0x444 - 0x389];
    u8 ex[0x50C - 0x444]; /* 0x444 per-monster work: each emNN.c lays out its own
                         * struct here (EM07W...). The end is a guess. */
    struct EM_MDL *mdl; /* 0x50C model work */
    u8 _pad510[0x5AC - 0x510];
    f32 x5AC;           /* 0x5AC height used for set20's shell */
    u8 _pad5B0[0x736 - 0x5B0];
    u8 stg;             /* 0x736 */
    u8 _pad737[0x878 - 0x737];
    struct EFTW *tail;  /* 0x878 cut-tail effect (eft09_set) */
    u8 _pad87C[0x881 - 0x87C];
    u8 x881;            /* 0x881 has a target (target position at 0x934) */
    u8 _pad882[0x884 - 0x882];
    s8 x884;            /* 0x884 state flags picking eft19's model */
    s8 x885;            /* 0x885 */
    u8 _pad886[0x8B6 - 0x886];
    u8 x8B6;            /* 0x8B6 eyes shown (eft07) */
    u8 _pad8B7[0x8C3 - 0x8B7];
    u8 x8C3;            /* 0x8C3 0: em_cdm_act_flag_ck runs before an action is set */
    u8 _pad8C4[0x930 - 0x8C4];
    f32 act_spd;        /* 0x930 animation speed, 1.0 set by every em*_act_set (guess) */
    f32 tgt_pos[3];     /* 0x934 target position (CalcDistanceXZ/Em_Calc_angY from pos) */
    u8 _pad940[0x959 - 0x940];
    u8 x959;            /* 0x959 trapped: 6 pitfall, 9 shock (shell12_m) */
    u8 _pad95A[0x9EA - 0x95A];
    s8 x9EA;            /* 0x9EA trap state (shell12_m) */
    u8 _pad9EB[0xA10 - 0x9EB];
} EMW;

extern EMW em_work[];

#endif
