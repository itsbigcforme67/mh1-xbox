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

/* One slot of the item pouch (PLW.item[20]): item id (index into Item_data) and count. */
typedef struct PL_ITEM {
    u16 id;
    s16 num;
} PL_ITEM;

typedef struct PL_HAND {
    u8 _pad00[0x70];
    f32 pos[3];         /* 0x70 */
} PL_HAND;

/* pl_prog_tbl entry: handlers called through PLW.prog (offsets 0 and 0xC seen
 * in pl_work_clr; the rest is not decoded yet). */
typedef struct PLPROG {
    void (*init)(struct PLW *);   /* 0x00 */
    u8 _pad04[0x8];
    void (*init2)(struct PLW *);  /* 0x0C */
} PLPROG;

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
    u8    work011;           /* 0x011 */
    u8    flag12;        /* 0x012 */
    u8 _pad013[0x1];
    u8    flag14;            /* 0x014 */
    u8    flag15;            /* 0x015 */
    u8 _pad016[0x6];
    u8    work01C;           /* 0x01C */
    u8    work01D;           /* 0x01D */
    s8    work01E;           /* 0x01E */
    u8 _pad01F[0x41];
    u8    rot[0x18];     /* 0x060 rotation matrix (start; extent unknown); flvecApplyMat33 (shell00_i) */
    u8 _pad078[0xA0 - 0x78];
    s32   ang[3];        /* 0x0A0 rotation, 0x10000 = 360 degrees (set05_m, as EMW) */
    f32   pos[3];        /* 0x0AC world position (set16_m, shell00_set) */
    f32   scl[3];            /* 0x0B8 scale (pl_init_sub: 1.0 each) */
    u8 _pad0C4[0x94];
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
    u8 _pad2FE[0x2];
    s16   work300;           /* 0x300 */
    s16   vital;         /* 0x302 hit points (Pl_damage_sub) */
    u8 _pad304[0x48];
    u8    work34C;           /* 0x34C */
    u8 _pad34D[0x3];
    s8    work350;           /* 0x350 */
    s8    work351;           /* 0x351 */
    s8    work352[6];        /* 0x352 parts/armor ids? */
    u8 _pad358[0x8];
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
    u16   work398;           /* 0x398 */
    u16   cnt39A;        /* 0x39A every 3rd hit applies ailments (shell00_i) */
    s32   work39C;       /* 0x39C */
    f32   work3A0;           /* 0x3A0 */
    u8 _pad3A4[0x4];
    s32   work3A8;           /* 0x3A8 */
    u8 _pad3AC[0x4];
    void *x3B0;          /* 0x3B0 player marked by eft26 (eft26_m); type unknown */
    s32   work3B4[6];    /* 0x3B4 */
    struct PLPROG *prog; /* 0x3CC table of state handlers (pl_work_clr); see PLPROG */
    s8    work3D0;       /* 0x3D0 */
    s8    work3D1;       /* 0x3D1 */
    u8 _pad3D2[0x3EC - 0x3D2];
    u16   dm_ang;        /* 0x3EC direction the hit came from (Guard_dir_ck) */
    u16   dm_pow;        /* 0x3EE hit strength (pl_guard_set, Pl_damage_sub) */
    u16   dm_type;       /* 0x3F0 kind of hit, 10 = no knockback (Pl_damage_sub) */
    u8 _pad3F2[0x2];
    s8    work3F4;       /* 0x3F4 */
    u8 _pad3F5[0x14];
    u8    work409;           /* 0x409 */
    u8    x40A;          /* 0x40A shell00 hits count while set (cont_add) */
    u8 _pad40B;
    u16   work40C;           /* 0x40C */
    u16   work40E;           /* 0x40E */
    u8 _pad410[0x2C];
    s16   work43C;       /* 0x43C */
    s16   x43E;          /* 0x43E stun ("piyo") gauge (Pl_damage_sub) */
    u8    work440;           /* 0x440 */
    u8 _pad441[0x93];
    s8    work4D4;           /* 0x4D4 */
    u8    work4D5;           /* 0x4D5 */
    s16   work4D6;       /* 0x4D6 */
    u8 _pad4D8[0x5];
    u8    work4DD;           /* 0x4DD */
    u8 _pad4DE[0x2];
    s16   work4E0;       /* 0x4E0 */
    u8 _pad4E2[0x1];
    s8    work4E3;       /* 0x4E3 */
    u8 _pad4E4[0x80];
    void *work564;           /* 0x564 */
    s16   work568;           /* 0x568 */
    u8    work56A;           /* 0x56A */
    u8    work56B;           /* 0x56B */
    u8    ammo_type;     /* 0x56C shot type fired (shell06_set) */
    u8    work56D;           /* 0x56D */
    u8    work56E;           /* 0x56E */
    u8    work56F;           /* 0x56F */
    u16   work570;           /* 0x570 */
    s16   work572;           /* 0x572 */
    u8 _pad574[0x38];
    f32   x5AC;          /* 0x5AC ground height (eft21_i, as EMW) */
    u8 _pad5B0[0x4C];
    u32   work5FC;           /* 0x5FC */
    u8 _pad600[0x4];
    u8    flag604;       /* 0x604 */
    u8 _pad605[0x3];
    s16   work608;           /* 0x608 */
    s16   work60A;           /* 0x60A */
    u8 _pad60C[0x4];
    s16   x610;          /* 0x610 shell00 hits count while set (cont_add) */
    s8    work612;           /* 0x612 */
    u8 _pad613[0x2];
    u8    work615;       /* 0x615 non-zero: weapon in the other hand (eft05) */
    u8    work616;           /* 0x616 */
    u8 _pad617[0x8D];
    s8    work6A4;           /* 0x6A4 */
    s8    work6A5;           /* 0x6A5 */
    u16   work6A6;           /* 0x6A6 */
    s8    work6A8;           /* 0x6A8 */
    s8    work6A9;           /* 0x6A9 */
    u16   work6AA;           /* 0x6AA */
    u8 _pad6AC[0x74];
    s8    work720[4];    /* 0x720 */
    s16   work724[4];    /* 0x724 */
    u16   work72C[4];    /* 0x72C (retyped from s16: blend_set) */
    u8 _pad734[0x2];
    u8    stg;           /* 0x736 */
    u8 _pad737[0x1];
    u8    x738;          /* 0x738 cleared on death (Pl_die_set) */
    u8 _pad739;
    s16   work73A;           /* 0x73A */
    f32   work73C;           /* 0x73C */
    f32   work740;           /* 0x740 */
    f32   work744;           /* 0x744 */
    s16   stamina;       /* 0x748 guarding needs 75 or more (pl_guard_ck); a guess */
    s16   work74A;           /* 0x74A */
    u8 _pad74C[0x4];
    s16   work750;           /* 0x750 */
    u8 _pad752[0xE];
    s16   work760;           /* 0x760 */
    u8 _pad762;
    u8    work763;           /* 0x763 */
    u8    work764;           /* 0x764 */
    u8 _pad765;
    s16   dm_vital;      /* 0x766 damage to take (Pl_damage_sub) */
    u8 _pad768[0x790 - 0x768];
    s16   vital_red;     /* 0x790 red part of the life bar (Pl_damage_sub) */
    s16   work792;           /* 0x792 */
    u8 _pad794[0x4];
    f32   work798;           /* 0x798 */
    u8 _pad79C[0xE];
    s16   x7AA;          /* 0x7AA ailment gauges: x7AA/x7AC/x7AE sleep?, */
    s16   x7AC;          /* 0x7AC  x7B2/x7B4, x7BA/x7BC poison, x7C4/x7C6 */
    s16   x7AE;          /* 0x7AE  (Pl_damage_sub, Pl_die_set); names are guesses */
    u8 _pad7B0[0x2];
    s16   x7B2;          /* 0x7B2 */
    s16   x7B4;          /* 0x7B4 */
    u8 _pad7B6[0x4];
    s16   x7BA;          /* 0x7BA */
    s16   x7BC;          /* 0x7BC */
    s16   work7BE;           /* 0x7BE */
    u8 _pad7C0[0x4];
    s16   x7C4;          /* 0x7C4 */
    s16   x7C6;          /* 0x7C6 */
    u8 _pad7C8[0xC];
    s8    work7D4;           /* 0x7D4 */
    s8    work7D5;           /* 0x7D5 */
    u8    work7D6;           /* 0x7D6 */
    u8 _pad7D7;
    f32   atk_rate;      /* 0x7D8 shot power (shell06_get_weaopn_data) */
    f32   work7DC;           /* 0x7DC */
    u8 _pad7E0[0xD];
    u8    work7ED;           /* 0x7ED */
    u8    work7EE;           /* 0x7EE */
    u8 _pad7EF[0x11];
    s32   work800;           /* 0x800 */
    s32   work804;           /* 0x804 */
    s32   work808;           /* 0x808 */
    u8 _pad80C[0xC];
    s16   work818;           /* 0x818 */
    s16   work81A;           /* 0x81A */
    s8    work81C;           /* 0x81C */
    u8 _pad81D;
    u8    work81E;           /* 0x81E */
    u8    work81F;           /* 0x81F */
    u8 _pad820[0x8];
    PL_ITEM item[20];        /* 0x828 item pouch, 20 slots (Pl_item_charge, item_sel_sub); the ammo slot is picked by Pl_shell_set */
    u8 _pad878[0x4];
    s16   work87C;       /* 0x87C */
    s16   work87E;           /* 0x87E */
    u8 _pad880;
    u8    x881;          /* 0x881 bite timer (eft23 fishing) */
    s16   work882;       /* 0x882 */
    s16   work884;           /* 0x884 */
    u8    work886;           /* 0x886 */
    s8    work887;           /* 0x887 */
    u16   work888;           /* 0x888 */
    s16   work88A;           /* 0x88A */
    u8    work88C;       /* 0x88C */
    s8    work88D;           /* 0x88D */
    u16   work88E;           /* 0x88E */
    u8 _pad890[0x2C];
    s16   work8BC;           /* 0x8BC */
    u8    work8BE;           /* 0x8BE */
    u8    work8BF;           /* 0x8BF */
    s16   work8C0;           /* 0x8C0 */
    u8    work8C2;       /* 0x8C2 */
    u8    work8C3;           /* 0x8C3 */
    s8    work8C4;           /* 0x8C4 */
    s8    work8C5;           /* 0x8C5 */
    u8    work8C6;           /* 0x8C6 */
    s8    work8C7;           /* 0x8C7 */
    s8    work8C8;           /* 0x8C8 */
    s8    work8C9;           /* 0x8C9 */
    u8 _pad8CA[0x2];
    s16   work8CC;           /* 0x8CC */
    s16   work8CE;           /* 0x8CE */
    u8 _pad8D0;
    u8    work8D1;           /* 0x8D1 */
    u8    work8D2;           /* 0x8D2 */
    u8 _pad8D3;
    char  name[0x14];    /* 0x8D4 player name (set01_i) */
    u16   fish_time;     /* 0x8E8 time to land the hooked fish (eft23) */
    u16   x8EA;          /* 0x8EA non-zero: bait still on (eft23) */
    u8 _pad8EC;
    s8    work8ED;           /* 0x8ED */
    u8 _pad8EE[0x2];
    s8    work8F0;       /* 0x8F0 */
    u8 _pad8F1;
    u8    work8F2;           /* 0x8F2 */
    s8    work8F3;           /* 0x8F3 */
    u8 _pad8F4[0x14];
    s8    work908;           /* 0x908 */
    u8 _pad909[0x3];
    s16   work90C;           /* 0x90C */
    s8    work90E;           /* 0x90E */
    u8 _pad90F[0x8];
    s8    work917;           /* 0x917 */
    u16   work918;           /* 0x918 */
    u16   work91A;           /* 0x91A */
    u16   work91C;           /* 0x91C */
    s8    work91E;           /* 0x91E */
    s8    work91F;           /* 0x91F */
    u8 _pad920[0x10];
    u16   work930;           /* 0x930 */
    u8 _pad932[0x2];
    s16   work934;           /* 0x934 */
    u8    work936;           /* 0x936 */
    u8 _pad937[0xC9];
} PLW;

extern PLW player_work[];

#endif
