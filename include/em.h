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
    u8 _pad34[0x10];
    struct EM_MOTW *mot0; /* 0x44 motion of layer 0 (mot_miration_ret) */
    u8 *mtx;            /* 0x48 skin matrix list */
    u8 _pad4C[8];
    struct EM_MOTW *mot1; /* 0x54 blended motion (mot_miration_ret) */
} EM_MDL;

/* Per-stage point list (EM_AREA+8, em08_senkai_pos_no): entries until
 * stg == -1; pos points to 4 positions. */
typedef struct EM_STG_POS {
    s16 stg;            /* 0x0 stage number, -1 ends the list (also used as default) */
    s16 num;            /* 0x2 entry count (EMW.area->x18 lists, target_kind_set) */
    f32 (*pos)[3];      /* 0x4 */
} EM_STG_POS;

typedef struct EM_AREA {
    EM_STG_POS *x0;     /* 0x0 lists picked by EMW.x828 (cmd_target_kind_set) */
    EM_STG_POS *x4;     /* 0x4 */
    EM_STG_POS *stg_pos; /* 0x8 */
    EM_STG_POS *xC;     /* 0xC */
    EM_STG_POS *x10;    /* 0x10 */
    EM_STG_POS *x14;    /* 0x14 */
    EM_STG_POS *x18;    /* 0x18 routes: pos is really an EM_ROUTE list */
} EM_AREA;

/* One hagitori (carve) slot of a monster (EMW+0x308, 8 entries). */
typedef struct EM_HAGI {
    s16 hp;             /* 0x0 remaining hit points of the part */
    u8 cnt;             /* 0x2 times the part was broken, capped at 99 */
    u8 _pad3[5];
} EM_HAGI;

typedef struct EMW {
    u8 be_flag;         /* 0x000 alive; shells end when it clears (shell11_m) */
    u8 x01;             /* 0x001 (set10_m) */
    u8 kind;            /* 0x002 monster kind (set10_m checks 7) */
    u8 _pad003[0x4 - 0x3];
    u8 x04;             /* 0x004 shells end when >= 2 (shell02_m) */
    u8 x05;             /* 0x005 step within the current action (em29 dm00/move05) */
    u8 x06;             /* 0x006 cleared when em10 starts talking (act 4) */
    u8 x07;             /* 0x007 3 ends attached effects (eft07_m) */
    s32 work08;         /* 0x008 (as PLW; em08 stores a turn time here) */
    u16 id;             /* 0x00C */
    s16 x0E;            /* 0x00E facing the monster turns toward (em10_turn_sub) */
    u8 x10;             /* 0x010 */
    u8 x11;             /* 0x011 small-size variant flag (em03_init: scale 0.85) */
    u8 _pad012[0x13 - 0x12];
    u8 x13;             /* 0x013 spawn slot (em29_init places the monster by it) */
    u8 mode;            /* 0x014 4/5 end attached shells (shell19_m) */
    u8 x15;             /* 0x015 sub-mode (eft09_m) */
    u8 mode_old;        /* 0x016 mode copied each frame (em29 move 0) */
    u8 x15_old;         /* 0x017 */
    u8 _pad018[0x19 - 0x18];
    u8 x19;             /* 0x019 cleared when a shell is spawned */
    u8 _pad01A[0x1B - 0x1A];
    u8 type;            /* 0x01B variant, row of the monster's type table (em29) */
    u8 _pad01C[0x20 - 0x1C];
    f32 mat[4][4];      /* 0x020 rotation matrix, rebuilt from ang by cpRotMatrix (em18) */
    u8 _pad060[0xA0 - 0x60];
    s32 ang[3];         /* 0x0A0 rotation, 0x10000 = 360 degrees (shell14_trans) */
    f32 pos[3];         /* 0x0AC world position (set20_m, as PLW) */
    f32 scale[3];       /* 0x0B8 model scale (eft09_t) */
    u8 _pad0C4[0x194 - 0xC4];
    s32 x194;           /* 0x194 0: em_char_set may start a new animation (em17_senkai_sub) */
    u8 _pad198[0x19C - 0x198];
    f32 x19C;           /* 0x19C time used by em02_fly_adjy (0 or 1: start value) */
    f32 chr_spd0;       /* 0x1A0 frame step (as PLW); divides the fly_adjy2 tables */
    u8 _pad1A4[0x1A8 - 0x1A4];
    f32 x1A8;           /* 0x1A8 (em04 turn: frame count of the current motion?) */
    s32 x1AC;           /* 0x1AC */
    f32 x1B0;           /* 0x1B0 */
    u8 _pad1B4[0x1C4 - 0x1B4];
    s32 x1C4;           /* 0x1C4 non-zero: fly_adjy2 uses time 0 */
    u8 _pad1C8[0x1CC - 0x1C8];
    f32 x1CC;           /* 0x1CC */
    u8 _pad1D0[0x2D4 - 0x1D0];
    u16 x2D4;           /* 0x2D4 em10: idle pose (0 stand, 1/2 others) */
    u16 x2D6;           /* 0x2D6 em10: message page */
    u16 x2D8;           /* 0x2D8 em10: message number (row of talk_tbl) */
    u16 x2DA;           /* 0x2DA em10: message timer, capped at 300 */
    u16 char0;          /* 0x2DC current animation (as PLW) */
    u16 x2DE;           /* 0x2DE animation of layer 1 (char0 is layer 0; em01 reset_char_set) */
    u16 x2E0;           /* 0x2E0 animation of layer 2 */
    u8 _pad2E2[0x2E4 - 0x2E2];
    u16 act_tm0;        /* 0x2E4 */
    u16 act_tm1;        /* 0x2E6 */
    u8 _pad2E8[0x2EC - 0x2E8];
    s16 blend0;         /* 0x2EC */
    s16 blend1;         /* 0x2EE */
    u8 _pad2F0[0x300 - 0x2F0];
    u16 x300;           /* 0x300 number of animation layers (char0/act_tm0/blend0 are [0] of arrays) */
    s16 x302;           /* 0x302 compared with 10% of x792 (hit points? guess) */
    u8 _pad304[0x308 - 0x304];
    EM_HAGI hagi[8];    /* 0x308 */
    u8 _pad348[0x34F - 0x348];
    u8 mdl_no;          /* 0x34F model number (eft09_t: texture and matrix list) */
    u8 _pad350[0x388 - 0x350];
    u8 x388;            /* 0x388 non-zero keeps set20's gate shut */
    u8 _pad389[0x38D - 0x389];
    u8 dm_flag;         /* 0x38D set when hit this frame (as PLW; Em_Dmg_Sys) */
    u8 x38E;            /* 0x38E (u8: lbu in em01_main, damage part index) */
    u8 _pad38F[0x390 - 0x38F];
    s32 x390;           /* 0x390 */
    s32 x394;           /* 0x394 */
    u8 _pad398[0x39A - 0x398];
    u16 x39A;           /* 0x39A em16 acts only when it is even */
    s32 x39C;           /* 0x39C action timer (em04 act 3: waits until >= 240) */
    u8 _pad3A0[0x3A4 - 0x3A0];
    s32 horm_ang;       /* 0x3A4 angle to turn toward (emNN_horm_init, em10 act 10) */
    u8 _pad3A8[0x3AC - 0x3A8];
    s32 x3AC;           /* 0x3AC cleared by em_eye_search_set */
    struct PLW *x3B0;   /* 0x3B0 target player (em15 fly 33 / atk 2) */
    f32 rate_x;         /* 0x3B4 rate/jump vector x (em10 act 2); with adj_y, adj_z it is a
                         * f32[3] (em19_rate_add_calc copies all three) */
    f32 adj_y;          /* 0x3B8 fly height correction per frame (fly_adjy2_suby) */
    f32 adj_z;          /* 0x3BC (fly_adjy2_subz) */
    f32 x3C0[3];        /* 0x3C0 em19: copy of the 0x3B4 vector, flipped to wobble */
    u8 _pad3CC[0x3EC - 0x3CC];
    u16 dm_ang;         /* 0x3EC direction the hit came from (as PLW, em04 dm00) */
    u8 _pad3EE[0x3F0 - 0x3EE];
    u16 x3F0;           /* 0x3F0 */
    u8 _pad3F2[0x3F4 - 0x3F2];
    u8 x3F4;            /* 0x3F4 cleared by em19 demo/revival */
    u8 _pad3F5[0x40C - 0x3F5];
    s16 x40C;           /* 0x40C set to 10 while dying (em29 move 5) */
    s16 x40E;           /* 0x40E */
    u8 _pad410[0x416 - 0x410];
    s16 x416;           /* 0x416 default blend (em_char_set) */
    u8 _pad418[0x444 - 0x418];
    u8 ex[0x50C - 0x444]; /* 0x444 per-monster work: each emNN.c lays out its own
                         * struct here (EM07W...). The end is a guess. */
    struct EM_MDL *mdl; /* 0x50C model work */
    u8 _pad510[0x56A - 0x510];
    u8 x56A;            /* 0x56A cleared when em19 dies */
    u8 _pad56B[0x5A0 - 0x56B];
    f32 x5A0[3];        /* 0x5A0 */
    f32 x5AC;           /* 0x5AC height used for set20's shell */
    u8 _pad5B0[0x5C0 - 0x5B0];
    f32 uv[4][3];       /* 0x5C0 texture scroll per slot (x, y, unused) */
    u16 uvtm[4];        /* 0x5F0 slot timer, 0xFFFF idle */
    u8 uvty[4];         /* 0x5F8 slot type, 0xFF none */
    u8 _pad5FC[0x60C - 0x5FC];
    u16 neck_tgt;       /* 0x60C neck target angle, relative (em_neck_move_sub) */
    u16 neck_ang;       /* 0x60E current neck angle */
    u8 _pad610[0x616 - 0x610];
    u8 x616;            /* 0x616 player number (em18 mov01 follows player_work[x616]) */
    s8 x617;            /* 0x617 -1: no ... (em08_fly_act_set) */
    u8 _pad618[0x6E0 - 0x618];
    u16 x6E0;           /* 0x6E0 em10: item given in a trade */
    u16 x6E2;           /* 0x6E2 em10: item taken in a trade */
    struct EM_SEARCH * search;/* 0x6E4 */
    u32 neck[4];        /* 0x6E8 per-joint neck angles (neck_ang_set) */
    u16 neck_spd;       /* 0x6F8 */
    u16 neck_lock;      /* 0x6FA target latched while turning */
    u8 neck_st;         /* 0x6FC 0 off 1 start 2 turning 3 settled */
    s8 x6FD;            /* 0x6FD */
    u8 x6FE;            /* 0x6FE 1 = next em_char_set uses canmot_data_tbl */
    u8 x6FF;            /* 0x6FF non-zero: main_sub runs twice this frame (em29_main) */
    f32 x700[3];        /* 0x700 */
    u8 _pad70C[0x70E - 0x70C];
    u16 x70E;           /* 0x70E */
    u8 _pad710[0x734 - 0x710];
    u8 x734;            /* 0x734 3: monster takes commands (em29_main) */
    u8 _pad735[0x736 - 0x735];
    u8 stg;             /* 0x736 */
    u8 _pad737[0x73A - 0x737];
    s16 x73A;           /* 0x73A */
    f32 home[3];        /* 0x73C start position (em10_init) */
    u8 _pad748[0x74C - 0x748];
    u32 x74C;           /* 0x74C flags; 0xF000000F stops fly_adjz2 (em16) */
    u8 _pad750[0x754 - 0x750];
    f32 x754[3];        /* 0x754 position the turn toward a player starts from (em02_senkai_player) */
    u8 _pad760[0x762 - 0x760];
    s8 x762;            /* 0x762 */
    u8 _pad763[0x765 - 0x763];
    u8 x765;            /* 0x765 (em02_init) */
    s16 dmg[8];         /* 0x766 */
    s16 x776;           /* 0x776 */
    s16 x778[4];        /* 0x778 */
    s16 x780[4];        /* 0x780 */
    u8 x788[8];         /* 0x788 */
    u8 _pad790[0x792 - 0x790];
    s16 x792;           /* 0x792 maximum of x302? (guess) */
    s8 x794;            /* 0x794 */
    s8 x795;            /* 0x795 */
    u8 _pad796[0x797 - 0x796];
    u8 x797;            /* 0x797 */
    f32 x798;           /* 0x798 fade 0..1 at the end of em18 mov03 (alpha?) */
    u8 _pad79C[0x7A0 - 0x79C];
    struct PLW *x7A0;   /* 0x7A0 player the monster follows (em09 mov04, atk01) */
    struct EMW *x7A4;   /* 0x7A4 (em09_status_ck reads its kind) */
    u8 x7A8;            /* 0x7A8 */
    u8 x7A9;            /* 0x7A9 */
    u8 _pad7AA[0x7B0 - 0x7AA];
    s16 sleep_tol;      /* 0x7B0 */
    s16 x7B2;           /* 0x7B2 */
    s16 x7B4;           /* 0x7B4 */
    s16 x7B6;           /* 0x7B6 */
    s16 poison_tol;     /* 0x7B8 */
    s16 x7BA;           /* 0x7BA */
    s16 x7BC;           /* 0x7BC */
    s16 x7BE;           /* 0x7BE */
    s16 x7C0;           /* 0x7C0 */
    s16 mahi_tol;       /* 0x7C2 */
    s16 x7C4;           /* 0x7C4 */
    s16 x7C6;           /* 0x7C6 */
    s16 x7C8;           /* 0x7C8 */
    s16 sleep2_tol;     /* 0x7CA */
    s16 x7CC;           /* 0x7CC */
    s16 x7CE;           /* 0x7CE */
    s16 x7D0;           /* 0x7D0 */
    u8 x7D2;            /* 0x7D2 */
    u8 taisei;          /* 0x7D3 */
    u8 _pad7D4[0x7D6 - 0x7D4];
    u8 x7D6;            /* 0x7D6 2 while dying (em19) */
    u8 _pad7D7[0x7D8 - 0x7D7];
    f32 x7D8;           /* 0x7D8 */
    f32 x7DC;           /* 0x7DC */
    f32 x7E0;           /* 0x7E0 em08: depth below the water surface (450 big / 250 small) */
    f32 x7E4;           /* 0x7E4 em08: water surface height */
    u8 x7E8;            /* 0x7E8 0: em21 falls back to act 0/1 on its own stage */
    u8 x7E9;            /* 0x7E9 0: em14 fly action 0 becomes act 0/3 */
    u8 _pad7EA[0x7EE - 0x7EA];
    u8 x7EE;            /* 0x7EE (em02_init: 15) */
    u8 x7EF;            /* 0x7EF */
    u8 x7F0;            /* 0x7F0 */
    u8 x7F1;            /* 0x7F1 */
    u8 x7F2;            /* 0x7F2 */
    u8 _pad7F3[0x810 - 0x7F3];
    f32 x810;           /* 0x810 */
    f32 x814;           /* 0x814 */
    f32 x818;           /* 0x818 compared with x8C4[x883] (em16_act_act_set) */
    f32 x81C;           /* 0x81C */
    u8 _pad820[0x827 - 0x820];
    u8 x827;            /* 0x827 */
    u8 x828;            /* 0x828 */
    u8 x829;            /* 0x829 em08_senkai_pos_no result */
    u8 _pad82A[0x839 - 0x82A];
    u8 x839;            /* 0x839 set by em14 move action 1 */
    u8 _pad83A[0x83B - 0x83A];
    u8 x83B;            /* 0x83B */
    u8 _pad83C[0x844 - 0x83C];
    s8 x844;            /* 0x844 */
    u8 _pad845[0x84D - 0x845];
    u8 x84D;            /* 0x84D (em15 fly 11) */
    s8 x84E;            /* 0x84E */
    u8 _pad84F[0x86D - 0x84F];
    u8 x86D;            /* 0x86D */
    u8 x86E;            /* 0x86E */
    u8 x86F;            /* 0x86F */
    u8 _pad870[0x876 - 0x870];
    u8 x876;            /* 0x876 joint of the hagi pick point (Em_hagi_point_set), 0 = none */
    u8 _pad877;
    struct EFTW *tail;  /* 0x878 cut-tail effect (eft09_set) */
    u8 x87C;            /* 0x87C player id this monster holds/targets (pl_mv083 compares it with PLW.id) */
    u8 x87D;            /* 0x87D bit mask of the hagitori parts broken by this hit (Em_Dmg_Sys) */
    u8 x87E;            /* 0x87E same, accumulated */
    s8 x87F;            /* 0x87F (s8) non-zero: monster state checked in basic_com_ck */
    u8 _pad880[0x881 - 0x880];
    u8 x881;            /* 0x881 target kind, 0 none (1 and 7 seen; 0x934 = its position) */
    u8 x882;            /* 0x882 */
    u8 x883;            /* 0x883 index into x8C4, 0xFF none */
    s8 x884;            /* 0x884 state flags picking eft19's model */
    s8 x885;            /* 0x885 */
    s16 x886;           /* 0x886 */
    u8 x888;            /* 0x888 */
    u8 x889;            /* 0x889 */
    u8 x88A;            /* 0x88A */
    u8 x88B;            /* 0x88B */
    u8 x88C;            /* 0x88C */
    s8 x88D;            /* 0x88D (s8: lb in Em_hagi_point_cnt_ck) hagi pick point index, -1 none */
    u8 x88E;            /* 0x88E */
    u8 x88F;            /* 0x88F */
    s16 x890[4];        /* 0x890 */
    s32 thirst;         /* 0x898 */
    s32 hungry;         /* 0x89C */
    s32 x8A0;           /* 0x8A0 */
    s32 thirst_max;     /* 0x8A4 */
    s32 hungry_max;     /* 0x8A8 */
    s32 x8AC;           /* 0x8AC */
    s16 x8B0;           /* 0x8B0 */
    s16 x8B2;           /* 0x8B2 */
    s16 x8B4;           /* 0x8B4 */
    u8 x8B6;            /* 0x8B6 eyes shown (eft07) */
    u8 x8B7;            /* 0x8B7 */
    u8 x8B8;            /* 0x8B8 */
    u8 x8B9;            /* 0x8B9 non-zero: monster is grabbing a player (pl_mv083) */
    s8 x8BA;            /* 0x8BA */
    s8 x8BB;            /* 0x8BB (s8: Em_Damage_Stock) set to 10 while ex+4 is non-zero (em29_main) */
    u8 x8BC;            /* 0x8BC */
    u8 x8BD;            /* 0x8BD 1 while paralysed (em19 dm02) */
    u8 _pad8BE[0x8BF - 0x8BE];
    u8 x8BF;            /* 0x8BF */
    s8 x8C0;            /* 0x8C0 */
    s8 x8C1;            /* 0x8C1 */
    u8 x8C2;            /* 0x8C2 */
    u8 x8C3;            /* 0x8C3 0: em_cdm_act_flag_ck runs before an action is set */
    f32 x8C4[4];        /* 0x8C4 indexed by x883 (size a guess) */
    f32 x8D4[4];        /* 0x8D4 per player (indexed by x617; em01 atk 4), size a guess */
    f32 x8E4[4];        /* 0x8E4 */
    s32 x8F4[4];        /* 0x8F4 */
    u16 x904[4];        /* 0x904 (u16: em_eye_search_set) angle to each player */
    u16 x90C[4];        /* 0x90C */
    u8 x914;            /* 0x914 */
    u8 x915;            /* 0x915 */
    u8 x916;            /* 0x916 */
    u8 x917;            /* 0x917 */
    s32 x918[4];        /* 0x918 */
    s8 x928;            /* 0x928 */
    s8 x929;            /* 0x929 */
    u8 _pad92A[0x92F - 0x92A];
    u8 x92F;            /* 0x92F */
    f32 act_spd;        /* 0x930 animation speed, 1.0 set by every em*_act_set (guess) */
    f32 tgt_pos[3];     /* 0x934 target position (CalcDistanceXZ/Em_Calc_angY from pos) */
    struct EM_AREA *area; /* 0x940 per-stage data (em08_senkai_pos_no) */
    struct EMW *x944;   /* 0x944 (em12 demo: the monster / player it watches) */
    u8 x948;            /* 0x948 */
    u8 x949;            /* 0x949 */
    s16 stay_tm;        /* 0x94A from emNN_stay_timer_tbl[stg] (local_area_move_init) */
    s16 runaway_tm;     /* 0x94C from emNN_runaway_timer_tbl[stg] */
    s16 x94E;           /* 0x94E (em12 dm04 copies it to work08) */
    u8 x950;            /* 0x950 */
    u8 x951;            /* 0x951 */
    u8 x952;            /* 0x952 */
    u8 x953;            /* 0x953 damage kind: 1, 8 or 9 selects the hagitori loop (Em_Dmg_Sys) */
    u16 x954;           /* 0x954 */
    u8 _pad956;
    u8 x957;            /* 0x957 set when part 8 breaks (Em_Dmg_Sys) */
    s8 x958;            /* 0x958 */
    u8 x959;            /* 0x959 trapped: 6 pitfall, 9 shock (shell12_m) */
    s8 x95A;            /* 0x95A */
    u8 x95B;            /* 0x95B 1: boss (em16 init sets x9E1 and the boss work) */
    u8 x95C;            /* 0x95C 2 while dying (em19) */
    s8 x95D;            /* 0x95D (em14_sasari_ck) */
    u16 x95E;           /* 0x95E */
    u8 _pad960[0x9C8 - 0x960];
    f32 x9C8[3];        /* 0x9C8 */
    struct EMW * boss;  /* 0x9D4 */
    u8 _pad9D8[0x9D9 - 0x9D8];
    u8 x9D9;            /* 0x9D9 */
    u8 _pad9DA[0x9E1 - 0x9DA];
    s8 x9E1;            /* 0x9E1 (em18/em29_init: 5; Em_Yobi_Ck: non-zero = no call for help) */
    u8 x9E2;            /* 0x9E2 */
    u8 x9E3;            /* 0x9E3 (u8: em_escape_action_ck) */
    u8 x9E4;            /* 0x9E4 (u8: em_escape_action_ck) */
    u8 x9E5;            /* 0x9E5 */
    u8 x9E6;            /* 0x9E6 (u8: em_escape_action_ck) */
    u8 x9E7;            /* 0x9E7 (u8: em_escape_action_ck) */
    u8 x9E8;            /* 0x9E8 (u8: em_escape_action_ck) */
    u8 x9E9;            /* 0x9E9 */
    s8 x9EA;            /* 0x9EA trap state (shell12_m) */
    u8 _pad9EB[0x9EC - 0x9EB];
    u8 x9EC;            /* 0x9EC */
    s8 x9ED;            /* 0x9ED */
    s8 range_no;        /* 0x9EE */
    u8 _pad9EF[0x9F1 - 0x9EF];
    s8 x9F1;            /* 0x9F1 */
    u8 _pad9F2[0x9F3 - 0x9F2];
    u8 x9F3;            /* 0x9F3 0: em15 fly 24 falls back to act 0/7 */
    u8 _pad9F4[0xA10 - 0x9F4];
} EMW;

extern EMW em_work[];

#endif
