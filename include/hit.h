#ifndef HIT_H
#define HIT_H
/* Hit detection (src/main/hit). Views of the structures the hit code uses,
 * fields named by offset where their meaning is not known yet. HCHR is the
 * header shared by PLW and EMW (players 0xA00, monsters 0xA10 bytes); HSHL
 * is the hit side of a shell (SHLW). HBODY is one 0x28-byte hit volume of a
 * body table: type 0x7D-0x7F are special records, -1 ends the table.
 * Offsets from the asm of f_hit; names are guesses unless noted. */
#include "types.h"

typedef struct HBODY {
    s16 type;             /* 0x000 */
    s16 mask;             /* 0x002 */
    s16 num;              /* 0x004 */
    s16 part;             /* 0x006 */
    u8 flag;              /* 0x008 */
    u8 _pad009[0x28 - 0x9];
} HBODY;

typedef struct HCHR {
    u8 be_flag;           /* 0x000 */
    u8 x01;               /* 0x001 */
    u8 kind;              /* 0x002 */
    u8 _pad003[0xC - 0x3];
    u16 id;               /* 0x00C */
    u8 _pad00E[0x10 - 0xE];
    u8 x10;               /* 0x010 */
    u8 _pad011[0x14 - 0x11];
    u8 mode;              /* 0x014 */
    u8 _pad015[0x18 - 0x15];
    u8 x18;               /* 0x018 */
    u8 x19;               /* 0x019 */
    u8 _pad01A[0xA4 - 0x1A];
    u16 ang_y;            /* 0x0A4 */
    u8 _pad0A6[0xAC - 0xA6];
    f32 pos[3];           /* 0x0AC */
    u8 _pad0B8[0x302 - 0xB8];
    s16 x302;             /* 0x302 */
    u8 _pad304[0x388 - 0x304];
    u8 x388;              /* 0x388 */
    u8 _pad389[0x38D - 0x389];
    u8 dm_flag;           /* 0x38D */
    u8 dm_part;           /* 0x38E */
    u8 _pad38F[0x3B0 - 0x38F];
    void *x3B0;           /* 0x3B0 */
    u8 _pad3B4[0x3D0 - 0x3B4];
    u8 x3D0;              /* 0x3D0 */
    u8 x3D1;              /* 0x3D1 */
    u8 _pad3D2[0x3E3 - 0x3D2];
    u8 x3E3;              /* 0x3E3 */
    u8 _pad3E4[0x3EC - 0x3E4];
    u16 dm_ang;           /* 0x3EC */
    s16 dm_pow;           /* 0x3EE */
    s16 dm_type;          /* 0x3F0 */
    u8 _pad3F2[0x3F8 - 0x3F2];
    u8 hit_id[16];        /* 0x3F8 */
    u8 _pad408[0x409 - 0x408];
    u8 x409;              /* 0x409 */
    u8 x40A;              /* 0x40A */
    u8 _pad40B[0x40C - 0x40B];
    u16 x40C;             /* 0x40C */
    u8 _pad40E[0x410 - 0x40E];
    u8 hit_id_no;         /* 0x410 */
    u8 _pad411[0x412 - 0x411];
    u8 x412;              /* 0x412 */
    u8 _pad413[0x41C - 0x413];
    void *x41C;           /* 0x41C */
    u8 _pad420[0x430 - 0x420];
    f32 dm_pos[3];        /* 0x430 */
    u8 _pad43C[0x4D8 - 0x43C];
    struct HSHL *dm_shl;         /* 0x4D8 */
    u8 _pad4DC[0x56A - 0x4DC];
    u8 x56A;              /* 0x56A */
    u8 _pad56B[0x572 - 0x56B];
    s16 x572;             /* 0x572 */
    u8 _pad574[0x610 - 0x574];
    s16 x610;             /* 0x610 */
    u8 _pad612[0x6A4 - 0x612];
    u8 x6A4;              /* 0x6A4 */
    u8 _pad6A5[0x6A8 - 0x6A5];
    u8 x6A8;              /* 0x6A8 */
    u8 _pad6A9[0x736 - 0x6A9];
    u8 stg;               /* 0x736 */
    u8 _pad737[0x748 - 0x737];
    s16 x748;             /* 0x748 */
    u8 _pad74A[0x762 - 0x74A];
    u8 x762;              /* 0x762 */
    u8 _pad763[0x766 - 0x763];
    s16 dm_val[8];        /* 0x766 */
    u8 _pad776[0x778 - 0x776];
    s16 dm_pl[4];         /* 0x778 */
    u8 _pad780[0x788 - 0x780];
    u8 dm_kind[8];        /* 0x788 */
    u8 _pad790[0x7A0 - 0x790];
    void *x7A0;           /* 0x7A0 */
    void *x7A4;           /* 0x7A4 */
    u8 _pad7A8[0x7AE - 0x7A8];
    s16 x7AE;             /* 0x7AE */
    u8 _pad7B0[0x7B4 - 0x7B0];
    s16 x7B4;             /* 0x7B4 */
    u8 _pad7B6[0x7BA - 0x7B6];
    s16 x7BA;             /* 0x7BA */
    s16 x7BC;             /* 0x7BC */
    u8 _pad7BE[0x7C6 - 0x7BE];
    s16 x7C6;             /* 0x7C6 */
    u8 _pad7C8[0x7CE - 0x7C8];
    s16 x7CE;             /* 0x7CE */
    u8 _pad7D0[0x7D4 - 0x7D0];
    u8 x7D4;              /* 0x7D4 */
    u8 x7D5;              /* 0x7D5 */
    u8 _pad7D6[0x7D8 - 0x7D6];
    f32 atk_rate;         /* 0x7D8 */
    f32 x7DC;             /* 0x7DC */
    u8 _pad7E0[0x887 - 0x7E0];
    u8 x887;              /* 0x887 */
    u8 _pad888[0x8C3 - 0x888];
    u8 x8C3;              /* 0x8C3 */
    u8 _pad8C4[0x920 - 0x8C4];
    f32 resist[4];        /* 0x920 */
    u8 _pad930[0x948 - 0x930];
    u8 x948;              /* 0x948 */
    u8 _pad949[0xA00 - 0x949];
} HCHR;

typedef struct HSHL {
    u8 be_flag;           /* 0x000 */
    u8 _pad001[0x4 - 0x1];
    u8 mode;              /* 0x004 */
    u8 mode2;             /* 0x005 */
    u8 _pad006[0x8 - 0x6];
    u8 x08;               /* 0x008 */
    u8 _pad009[0xA - 0x9];
    u8 no;                /* 0x00A */
    u8 hit_mode;          /* 0x00B */
    u8 _pad00C[0x10 - 0xC];
    struct HSHL *next;           /* 0x010 */
    u8 _pad014[0x1E - 0x14];
    u8 x1E;               /* 0x01E */
    u8 _pad01F[0x30 - 0x1F];
    f32 pos2[3];          /* 0x030 */
    f32 pos0[3];          /* 0x03C */
    u8 _pad048[0x60 - 0x48];
    u8 hit_time;          /* 0x060 */
    u8 hit_wait;          /* 0x061 */
    u8 pow;               /* 0x062 */
    u8 atk_type;          /* 0x063 */
    u8 _pad064[0x65 - 0x64];
    u8 x65;               /* 0x065 */
    s8 x66;               /* 0x066 */
    u8 _pad067[0x68 - 0x67];
    u8 x68;               /* 0x068 */
    u8 x69;               /* 0x069 */
    u8 _pad06A[0x6C - 0x6A];
    u8 ailment;           /* 0x06C */
    u8 ailment_val;       /* 0x06D */
    u8 se;                /* 0x06E */
    u8 mark;              /* 0x06F */
    u8 x70;               /* 0x070 */
    u8 _pad071[0x72 - 0x71];
    u8 hit_cnt;           /* 0x072 */
    u8 x73;               /* 0x073 */
    u8 x74;               /* 0x074 */
    u8 x75;               /* 0x075 */
    u8 x76;               /* 0x076 */
    u8 _pad077[0x7A - 0x77];
    u8 own_em;            /* 0x07A */
    u8 x7B;               /* 0x07B */
    u8 _pad07C[0x88 - 0x7C];
    HBODY *body;           /* 0x088 */
    HBODY *body2;          /* 0x08C */
    u8 _pad090[0x9C - 0x90];
    struct HCHR *hit_chr;        /* 0x09C */
    HBODY *hit_body;       /* 0x0A0 */
    u8 _pad0A4[0xB4 - 0xA4];
    u8 xB4;               /* 0x0B4 */
    u8 hit_id;            /* 0x0B5 */
    u8 _pad0B6[0xC4 - 0xB6];
    void *xC4;            /* 0x0C4 */
    u16 ang;              /* 0x0C8 */
    u8 stg;               /* 0x0CA */
    u8 xCB;               /* 0x0CB */
} HSHL;

/* A packed capsule (hit_cap_pk); +0x28 is the point dm_vec_calc uses. */
/* A packed capsule (hit_cap_pk). */
typedef struct HPK {
    f32 p0[3];          /* 0x00 */
    f32 p1[3];          /* 0x0C */
    f32 r;              /* 0x18 */
    f32 dir[3];         /* 0x1C p1 - p0 */
    f32 c[3];           /* 0x28 middle */
    f32 cr;             /* 0x34 bounding radius */
    u8 _pad38[0x40 - 0x38];
} HPK;

typedef struct HLINE {
    f32 p0[3];          /* 0x00 */
    f32 p1[3];          /* 0x0C */
    f32 dir[3];         /* 0x18 */
    f32 mid[3];         /* 0x24 */
    f32 half;           /* 0x30 */
} HLINE;

typedef struct HCAP {
    f32 p[2][3];
    f32 r;
    u8 _pad1C[4];
} HCAP;

typedef struct HSPH {
    f32 c[3];
    f32 r;
} HSPH;

#endif
