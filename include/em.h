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

/* Per-stage point list (EM_AREA+8, em08_senkai_pos_no): entries until
 * stg == -1; pos points to 4 positions. */
typedef struct EM_STG_POS {
    s16 stg;            /* 0x0 stage number, -1 ends the list (also used as default) */
    u8 _pad2[2];
    f32 (*pos)[3];      /* 0x4 */
} EM_STG_POS;

typedef struct EM_AREA {
    u8 _pad00[8];
    EM_STG_POS *stg_pos; /* 0x8 */
} EM_AREA;

typedef struct EMW {
    u8 be_flag;         /* 0x000 alive; shells end when it clears (shell11_m) */
    u8 x01;             /* 0x001 (set10_m) */
    u8 kind;            /* 0x002 monster kind (set10_m checks 7) */
    u8 _pad003;
    u8 x04;             /* 0x004 shells end when >= 2 (shell02_m) */
    u8 _pad005[2];
    u8 x07;             /* 0x007 3 ends attached effects (eft07_m) */
    s32 work08;         /* 0x008 (as PLW; em08 stores a turn time here) */
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
    u8 _pad0C4[0x194 - 0xC4];
    s32 x194;           /* 0x194 0: em_char_set may start a new animation (em17_senkai_sub) */
    u8 _pad198[0x19C - 0x198];
    f32 x19C;           /* 0x19C time used by em02_fly_adjy (0 or 1: start value) */
    f32 chr_spd0;       /* 0x1A0 frame step (as PLW); divides the fly_adjy2 tables */
    u8 _pad1A4[0x1C4 - 0x1A4];
    s32 x1C4;           /* 0x1C4 non-zero: fly_adjy2 uses time 0 */
    u8 _pad1C8[0x2DC - 0x1C8];
    u16 char0;          /* 0x2DC current animation (as PLW) */
    u8 _pad2DE[0x2E4 - 0x2DE];
    u16 act_tm0;        /* 0x2E4 */
    u16 act_tm1;        /* 0x2E6 */
    u8 _pad2E8[0x2EC - 0x2E8];
    s16 blend0;         /* 0x2EC */
    s16 blend1;         /* 0x2EE */
    u8 _pad2F0[0x302 - 0x2F0];
    s16 x302;           /* 0x302 compared with 10% of x792 (hit points? guess) */
    u8 _pad304[0x34F - 0x304];
    u8 mdl_no;          /* 0x34F model number (eft09_t: texture and matrix list) */
    u8 _pad350[0x388 - 0x350];
    u8 x388;            /* 0x388 non-zero keeps set20's gate shut */
    u8 _pad389[0x39A - 0x389];
    u16 x39A;           /* 0x39A em16 acts only when it is even */
    u8 _pad39C[0x3B0 - 0x39C];
    struct PLW *x3B0;   /* 0x3B0 target player (em15 fly 33 / atk 2) */
    u8 _pad3B4[4];
    f32 adj_y;          /* 0x3B8 fly height correction per frame (fly_adjy2_suby) */
    f32 adj_z;          /* 0x3BC (fly_adjy2_subz) */
    u8 _pad3C0[0x444 - 0x3C0];
    u8 ex[0x50C - 0x444]; /* 0x444 per-monster work: each emNN.c lays out its own
                         * struct here (EM07W...). The end is a guess. */
    struct EM_MDL *mdl; /* 0x50C model work */
    u8 _pad510[0x5AC - 0x510];
    f32 x5AC;           /* 0x5AC height used for set20's shell */
    u8 _pad5B0[0x617 - 0x5B0];
    s8 x617;            /* 0x617 -1: no ... (em08_fly_act_set) */
    u8 _pad618[0x736 - 0x618];
    u8 stg;             /* 0x736 */
    u8 _pad737[0x74C - 0x737];
    u32 x74C;           /* 0x74C flags; 0xF000000F stops fly_adjz2 (em16) */
    u8 _pad750[0x754 - 0x750];
    f32 x754[3];        /* 0x754 position the turn toward a player starts from (em02_senkai_player) */
    u8 _pad760[0x792 - 0x760];
    s16 x792;           /* 0x792 maximum of x302? (guess) */
    u8 _pad794[0x7E8 - 0x794];
    u8 x7E8;            /* 0x7E8 0: em21 falls back to act 0/1 on its own stage */
    u8 x7E9;            /* 0x7E9 0: em14 fly action 0 becomes act 0/3 */
    u8 _pad7EA[0x7EE - 0x7EA];
    u8 x7EE;            /* 0x7EE bit per player that has noticed this monster (em_ninshiki_ck) */
    u8 _pad7EF[0x818 - 0x7EF];
    f32 x818;           /* 0x818 compared with x8C4[x883] (em16_act_act_set) */
    u8 _pad81C[0x827 - 0x81C];
    u8 x827;            /* 0x827 */
    u8 x828;            /* 0x828 */
    u8 x829;            /* 0x829 em08_senkai_pos_no result */
    u8 _pad82A[0x839 - 0x82A];
    u8 x839;            /* 0x839 set by em14 move action 1 */
    u8 _pad83A[0x84D - 0x83A];
    u8 x84D;            /* 0x84D (em15 fly 11) */
    u8 _pad84E[0x878 - 0x84E];
    struct EFTW *tail;  /* 0x878 cut-tail effect (eft09_set) */
    u8 _pad87C[0x881 - 0x87C];
    u8 x881;            /* 0x881 target kind, 0 none (1 and 7 seen; 0x934 = its position) */
    u8 x882;            /* 0x882 */
    u8 x883;            /* 0x883 index into x8C4, 0xFF none */
    s8 x884;            /* 0x884 state flags picking eft19's model */
    s8 x885;            /* 0x885 */
    u8 _pad886[0x888 - 0x886];
    u8 x888;            /* 0x888 */
    u8 _pad889[0x88F - 0x889];
    u8 x88F;            /* 0x88F bit per player that can notice this monster (em_ninshiki_ck) */
    u8 _pad890[0x8B6 - 0x890];
    u8 x8B6;            /* 0x8B6 eyes shown (eft07) */
    u8 _pad8B7[0x8C3 - 0x8B7];
    u8 x8C3;            /* 0x8C3 0: em_cdm_act_flag_ck runs before an action is set */
    f32 x8C4[4];        /* 0x8C4 indexed by x883 (size a guess) */
    f32 x8D4[4];        /* 0x8D4 per player (indexed by x617; em01 atk 4), size a guess */
    u8 _pad8E4[0x930 - 0x8E4];
    f32 act_spd;        /* 0x930 animation speed, 1.0 set by every em*_act_set (guess) */
    f32 tgt_pos[3];     /* 0x934 target position (CalcDistanceXZ/Em_Calc_angY from pos) */
    struct EM_AREA *area; /* 0x940 per-stage data (em08_senkai_pos_no) */
    u8 _pad944[0x959 - 0x944];
    u8 x959;            /* 0x959 trapped: 6 pitfall, 9 shock (shell12_m) */
    u8 _pad95A[0x9EA - 0x95A];
    s8 x9EA;            /* 0x9EA trap state (shell12_m) */
    u8 _pad9EB[0x9EC - 0x9EB];
    u8 x9EC;            /* 0x9EC non-zero: counts for em_ninshiki_ck */
    u8 _pad9ED[0x9F3 - 0x9ED];
    u8 x9F3;            /* 0x9F3 0: em15 fly 24 falls back to act 0/7 */
    u8 _pad9F4[0xA10 - 0x9F4];
} EMW;

extern EMW em_work[];

#endif
