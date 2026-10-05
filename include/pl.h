#ifndef PL_H
#define PL_H
/* Player work: player_work[], 0xA00 bytes per player.
 * Only fields seen in matched code are named; names ending in an offset
 * (work2F4) are placeholders until their meaning is known. Every offset
 * here comes from code that byte-matches (see the file noted per group).
 * First filled from the pl file at 0x14F030 (src/main/pl/pl_normal.c). */
#include "types.h"

/* Pad/switch state at PLW+0x364, written by sw_set_sub. */
typedef struct PLSW {
    u16 now;            /* 0x00 */
    u16 old;            /* 0x02 */
    u16 trg;            /* 0x04 */
    u16 trg_old;        /* 0x06 */
    u16 pad08[4];       /* 0x08 */
    s16 chg;            /* 0x10 */
    u16 pad12;          /* 0x12 */
    u16 an_now;         /* 0x14 */
    u16 an_old;         /* 0x16 */
    u16 an_trg;         /* 0x18 */
    u16 an_trg_old;     /* 0x1A */
    u16 ang[2];         /* 0x1C */
    u16 pow[2];         /* 0x20 */
} PLSW;

typedef struct PL_HAND {
    u8 _pad00[0x70];
    f32 pos[3];         /* 0x70 */
} PL_HAND;

typedef struct PLW {
    u8    be_flag;       /* 0x000 in use (set05_m) */
    u8    x01;           /* 0x001 (set05_m) */
    u8    kind;          /* 0x002 0/3/4 can guard (pl_guard_ck) */
    u8 _pad003[0x1];
    s32   work04;        /* 0x004 */
    s32   work08;        /* 0x008 */
    u16   id;            /* 0x00C */
    s16   ang_y;         /* 0x00E copy of ang[1] (Pl_damage_sub) */
    u8    x10;           /* 0x010 copied to shells (shell03_set) */
    u8 _pad011;
    u8    flag12;        /* 0x012 */
    u8 _pad013[0x1];
    s8    flag14;        /* 0x014 */
    s8    flag15;        /* 0x015 */
    u8 _pad016[0x4A];
    u8    rot[0x18];     /* 0x060 rotation matrix (start; extent unknown), used
                            with flvecApplyMat33 (shell00_i) */
    u8 _pad078[0xA0 - 0x78];
    s32   ang[3];        /* 0x0A0 rotation, 0x10000 = 360 degrees (set05_m, as EMW) */
    f32   pos[3];        /* 0x0AC world position (set16_m, shell00_set) */
    u8 _pad0B8[0x158 - 0xB8];
    struct PL_HAND *hand;  /* 0x158 thrown items start from hand->pos (shell03_set) */
    u8 _pad15C[0x198 - 0x15C];
    s32   chr_no0;       /* 0x198 */
    u8 _pad19C[0x4];
    f32   chr_spd0;      /* 0x1A0 */
    u8 _pad1A4[0x44];
    s32   chr_no1;       /* 0x1E8 */
    u8 _pad1EC[0x4];
    f32   chr_spd1;      /* 0x1F0 */
    u8 _pad1F4[0xE8];
    u16   char0;         /* 0x2DC */
    u16   char1;         /* 0x2DE */
    u8 _pad2E0[0x4];
    u16   act_tm0;       /* 0x2E4 */
    s16   act_tm1;       /* 0x2E6 */
    u8 _pad2E8[0x4];
    s16   blend0;        /* 0x2EC */
    s16   blend1;        /* 0x2EE */
    u8 _pad2F0[0x4];
    s16   work2F4;       /* 0x2F4 */
    u8 _pad2F6[0x2];
    s8    work2F8;       /* 0x2F8 */
    u8 _pad2F9[0x3];
    s16   work2FC;       /* 0x2FC */
    u8 _pad2FE[0x302 - 0x2FE];
    s16   vital;         /* 0x302 hit points (Pl_damage_sub) */
    u8 _pad304[0x360 - 0x304];
    u16   wpn_kind;      /* 0x360 gun type, row of D_3367B2 (shell06) */
    u16   wpn_ammo;      /* 0x362 loaded ammo; low nibble = Gun_Grow_Up_DATA row (shell06) */
    PLSW  sw;            /* 0x364 */
    u8    st;            /* 0x388 */
    u8 _pad389[0x1];
    s8    work38A;       /* 0x38A */
    u8 _pad38B[0x2];
    u8    dm_flag;       /* 0x38D set when hit this frame (Pl_damage_sub) */
    u8 _pad38E[0x1];
    u8    sw_cfg;        /* 0x38F bit 0: swap buttons 0xC00 (get_sw) */
    s32   act_flag;      /* 0x390 */
    s32   work394;       /* 0x394 */
    s16   work398;       /* 0x398 */
    u16   cnt39A;        /* 0x39A every 3rd hit applies ailments (shell00_i) */
    s32   work39C;       /* 0x39C */
    u8 _pad3A0[0x10];
    void *x3B0;          /* 0x3B0 player marked by eft26 (eft26_m); type unknown */
    s32   work3B4[6];    /* 0x3B4 */
    u8 _pad3CC[0x4];
    s8    work3D0;       /* 0x3D0 */
    s8    work3D1;       /* 0x3D1 */
    u8 _pad3D2[0x3EC - 0x3D2];
    u16   dm_ang;        /* 0x3EC direction the hit came from (Guard_dir_ck) */
    u16   dm_pow;        /* 0x3EE hit strength (pl_guard_set, Pl_damage_sub) */
    u16   dm_type;       /* 0x3F0 kind of hit, 10 = no knockback (Pl_damage_sub) */
    u8 _pad3F2[0x2];
    s8    work3F4;       /* 0x3F4 */
    u8 _pad3F5[0x15];
    u8    x40A;          /* 0x40A shell00 hits count while set (cont_add) */
    u8 _pad40B;
    s16   work40C;       /* 0x40C */
    u8 _pad40E[0x2E];
    s16   work43C;       /* 0x43C */
    s16   x43E;          /* 0x43E stun ("piyo") gauge (Pl_damage_sub) */
    s8    work440;       /* 0x440 */
    u8 _pad441[0x95];
    s16   work4D6;       /* 0x4D6 */
    u8 _pad4D8[0x5];
    s8    work4DD;       /* 0x4DD */
    u8 _pad4DE[0x2];
    s16   work4E0;       /* 0x4E0 */
    u8 _pad4E2[0x1];
    s8    work4E3;       /* 0x4E3 */
    u8 _pad4E4[0x87];
    s8    work56B;       /* 0x56B */
    u8    ammo_type;     /* 0x56C shot type fired (shell06_set) */
    u8 _pad56D[0x5AC - 0x56D];
    f32   x5AC;          /* 0x5AC ground height (eft21_i, as EMW) */
    u8 _pad5B0[0x604 - 0x5B0];
    u8    flag604;       /* 0x604 */
    u8 _pad605[0x0B];
    s16   x610;          /* 0x610 shell00 hits count while set (cont_add) */
    u8 _pad612[0x3];
    u8    work615;       /* 0x615 non-zero: weapon in the other hand (eft05) */
    u8 _pad616[0x10A];
    s8    work720[4];    /* 0x720 */
    s16   work724[4];    /* 0x724 */
    s16   work72C[4];    /* 0x72C */
    u8 _pad734[0x2];
    u8    stg;           /* 0x736 */
    u8 _pad737[0x1];
    u8    x738;          /* 0x738 cleared on death (Pl_die_set) */
    u8 _pad739;
    u16   x73A;          /* 0x73A next stage number? (game2) */
    u8 _pad73C[0x748 - 0x73C];
    s16   stamina;       /* 0x748 guarding needs 75 or more (pl_guard_ck); a guess */
    u8 _pad74A[0x766 - 0x74A];
    s16   dm_vital;      /* 0x766 damage to take (Pl_damage_sub) */
    u8 _pad768[0x790 - 0x768];
    s16   vital_red;     /* 0x790 red part of the life bar (Pl_damage_sub) */
    u8 _pad792[0x7AA - 0x792];
    s16   x7AA;          /* 0x7AA ailment gauges: x7AA/x7AC/x7AE sleep?, */
    s16   x7AC;          /* 0x7AC  x7B2/x7B4, x7BA/x7BC poison, x7C4/x7C6 */
    s16   x7AE;          /* 0x7AE  (Pl_damage_sub, Pl_die_set); names are guesses */
    u8 _pad7B0[0x2];
    s16   x7B2;          /* 0x7B2 */
    s16   x7B4;          /* 0x7B4 */
    u8 _pad7B6[0x4];
    s16   x7BA;          /* 0x7BA */
    s16   x7BC;          /* 0x7BC */
    u8 _pad7BE[0x6];
    s16   x7C4;          /* 0x7C4 */
    s16   x7C6;          /* 0x7C6 */
    u8 _pad7C8[0x7D8 - 0x7C8];
    f32   atk_rate;      /* 0x7D8 shot power (shell06_get_weaopn_data) */
    u8 _pad7DC[0x87C - 0x7DC];
    s16   work87C;       /* 0x87C */
    u8 _pad87E[0x3];
    u8    x881;          /* 0x881 bite timer (eft23 fishing) */
    s16   work882;       /* 0x882 */
    u8 _pad884[0x8];
    u8    work88C;       /* 0x88C */
    u8 _pad88D[0x35];
    u8    work8C2;       /* 0x8C2 */
    u8 _pad8C3[0x11];
    char  name[0x14];    /* 0x8D4 player name (set01_i) */
    u16   fish_time;     /* 0x8E8 time to land the hooked fish (eft23) */
    u16   x8EA;          /* 0x8EA non-zero: bait still on (eft23) */
    u8 _pad8EC[4];
    s8    work8F0;       /* 0x8F0 */
    u8 _pad8F1[0x10F];
} PLW;

extern PLW player_work[];

#endif
