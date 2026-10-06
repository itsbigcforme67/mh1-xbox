#ifndef NETSYN_H
#define NETSYN_H
/* Network play sync (SLPM_654.95 0x001B9F70-0x001BD660): the PLW fields the net code touches (by offset, types from the matched loads)
 * and the per-kind packet layouts. A packet is cmd, len, 0, 0 followed by kind specific fields. */
#include "types.h"

/* net sync slots stored inside the work blocks */
typedef struct NPSLOT { s16 timer; u8 x2[2]; f32 x, y, z; } NPSLOT;   /* 0x10: pending position */
typedef struct NEMDUR { u8 x0, x1, cnt, x3, x4, x5, x6, x7; } NEMDUR;   /* 8 bytes per status slot */
typedef struct NEMACT {                                               /* 0x34: pending enemy action */
    s16 timer; u8 kind; u8 x3;
    f32 x, y, z;
    s16 ang[3];
    u8 b76, b77, x78, b79, b7A, b7B;
    s16 s7C;
    u8 b7E, x7F;
    u16 u80;
    u8 x82[0x34 - 0x22];
} NEMACT;

typedef struct NITEM { u16 id; s16 num; } NITEM;

typedef struct NPLV {
    u8     x00;               /* 0x000 */
    u8     x01;               /* 0x001 */
    u8  x002_[0xC - 0x2];
    u16    id;                /* 0x00C */
    u16    ang_y;             /* 0x00E */
    u8     x10;               /* 0x010 */
    u8     x11;               /* 0x011 */
    u8     x12;               /* 0x012 */
    u8  x013_[0x14 - 0x13];
    u8     x14;               /* 0x014 */
    u8     x15;               /* 0x015 */
    u8  x016_[0xA4 - 0x16];
    s32    xA4;               /* 0x0A4 */
    u8  x0A8_[0xAC - 0xA8];
    f32    posx;              /* 0x0AC */
    f32    posy;              /* 0x0B0 */
    f32    posz;              /* 0x0B4 */
    u8  x0B8_[0x302 - 0xB8];
    s16    vital;             /* 0x302 */
    u8  x304_[0x39A - 0x304];
    u16    cnt39A;            /* 0x39A */
    u8  x39C_[0x4D5 - 0x39C];
    u8     x4D5;              /* 0x4D5 */
    u8  x4D6_[0x56B - 0x4D6];
    u8     x56B;              /* 0x56B */
    u8     x56C;              /* 0x56C */
    u8  x56D_[0x570 - 0x56D];
    u16    x570;              /* 0x570 */
    u8  x572_[0x6AC - 0x572];
    u16    x6AC;              /* 0x6AC */
    u8  x6AE_[0x736 - 0x6AE];
    u8     x736;              /* 0x736 */
    u8  x737_[0x738 - 0x737];
    s8     x738;              /* 0x738 */
    u8  x739_[0x73A - 0x739];
    u16    x73A;              /* 0x73A */
    f32    x73C;              /* 0x73C */
    f32    x740;              /* 0x740 */
    f32    x744;              /* 0x744 */
    s16    x748;              /* 0x748 */
    u8  x74A_[0x792 - 0x74A];
    s16    x792;              /* 0x792 */
    u8  x794_[0x800 - 0x794];
    f32    x800;              /* 0x800 */
    f32    x804;              /* 0x804 */
    f32    x808;              /* 0x808 */
    u8  x80C_[0x818 - 0x80C];
    s16    x818;              /* 0x818 */
    u8  x81A_[0x880 - 0x81A];
    s8     x880;              /* 0x880 */
    u8  x881_[0x882 - 0x881];
    s16    x882;              /* 0x882 */
    u8  x884_[0x887 - 0x884];
    u8     x887;              /* 0x887 */
    u8  x888_[0x88A - 0x888];
    u16    x88A;              /* 0x88A */
    u8  x88C_[0x890 - 0x88C];
    f32    x890;              /* 0x890 */
    f32    x894;              /* 0x894 */
    f32    x898;              /* 0x898 */
    NPSLOT slot[2];           /* 0x89C */
    u8  x8BC_[0x8C3 - 0x8BC];
    u8     x8C3;              /* 0x8C3 */
    u8  x8C4_[0x8C9 - 0x8C4];
    s8     x8C9;              /* 0x8C9 */
    u8  x8CA_[0x904 - 0x8CA];
    u16    x904;              /* 0x904 */
    s16    x906;              /* 0x906 */
    u8  x908_[0x909 - 0x908];
    u8     x909;              /* 0x909 */
    u8     x90A;              /* 0x90A */
    u8  x90B_[0x90E - 0x90B];
    s8     x90E;              /* 0x90E */
    u8  x90F_[0x91F - 0x90F];
    s8     x91F;              /* 0x91F */
} NPLV;

typedef struct NEMV {
    u8  x000_[0x2 - 0x0];
    u8     x02;               /* 0x002 */
    u8  x003_[0x4 - 0x3];
    u8     x04;               /* 0x004 */
    u8  x005_[0x8 - 0x5];
    s32    x08;               /* 0x008 */
    u16    x0C;               /* 0x00C */
    u8  x00E_[0x14 - 0xE];
    u8     x14;               /* 0x014 */
    u8     x15;               /* 0x015 */
    u8  x016_[0xA0 - 0x16];
    s32    xA0;               /* 0x0A0 */
    s32    xA4;               /* 0x0A4 */
    s32    xA8;               /* 0x0A8 */
    f32    posx;              /* 0x0AC */
    f32    posy;              /* 0x0B0 */
    f32    posz;              /* 0x0B4 */
    u8  x0B8_[0x302 - 0xB8];
    s16    x302;              /* 0x302 */
    u8  x304_[0x308 - 0x304];
    NEMDUR x308[8];           /* 0x308 */
    u8  x348_[0x39A - 0x348];
    u16    x39A;              /* 0x39A */
    u8  x39C_[0x45C - 0x39C];
    u8     x45C;              /* 0x45C */
    s8     x45D;              /* 0x45D */
    u8  x45E_[0x462 - 0x45E];
    s16    x462;              /* 0x462 */
    u8  x464_[0x488 - 0x464];
    u8     x488;              /* 0x488 */
    u8     x489;              /* 0x489 */
    u8  x48A_[0x56A - 0x48A];
    u8     x56A;              /* 0x56A */
    u8  x56B_[0x572 - 0x56B];
    s16    x572;              /* 0x572 */
    u8  x574_[0x736 - 0x574];
    u8     x736;              /* 0x736 */
    u8  x737_[0x794 - 0x737];
    u8     x794;              /* 0x794 */
    u8     x795;              /* 0x795 */
    u8  x796_[0x797 - 0x796];
    u8     x797;              /* 0x797 */
    u8  x798_[0x7A8 - 0x798];
    u8     x7A8;              /* 0x7A8 */
    u8     x7A9;              /* 0x7A9 */
    u8  x7AA_[0x7B8 - 0x7AA];
    s16    x7B8;              /* 0x7B8 */
    u8  x7BA_[0x7C0 - 0x7BA];
    s16    x7C0;              /* 0x7C0 */
    u8  x7C2_[0x7D3 - 0x7C2];
    u8     x7D3;              /* 0x7D3 */
    u8  x7D4_[0x827 - 0x7D4];
    u8     x827;              /* 0x827 */
    u8     x828;              /* 0x828 */
    u8     x829;              /* 0x829 */
    u8  x82A_[0x86F - 0x82A];
    u8     x86F;              /* 0x86F */
    u8  x870_[0x87D - 0x870];
    u8     x87D;              /* 0x87D */
    u8     x87E;              /* 0x87E */
    u8  x87F_[0x881 - 0x87F];
    u8     x881;              /* 0x881 */
    u8     x882;              /* 0x882 */
    u8     x883;              /* 0x883 */
    s8     x884;              /* 0x884 */
    s8     x885;              /* 0x885 */
    u8  x886_[0x888 - 0x886];
    u8     x888;              /* 0x888 */
    u8  x889_[0x88B - 0x889];
    u8     x88B;              /* 0x88B */
    u8  x88C_[0x88E - 0x88C];
    u8     x88E;              /* 0x88E */
    u8  x88F_[0x8B0 - 0x88F];
    s16    x8B0;              /* 0x8B0 */
    u8  x8B2_[0x8B6 - 0x8B2];
    u8     x8B6;              /* 0x8B6 */
    u8  x8B7_[0x8C3 - 0x8B7];
    u8     x8C3;              /* 0x8C3 */
    u8  x8C4_[0x949 - 0x8C4];
    u8     x949;              /* 0x949 */
    u8  x94A_[0x94E - 0x94A];
    s16    x94E;              /* 0x94E */
    u8  x950_[0x954 - 0x950];
    u16    x954;              /* 0x954 */
    u8  x956_[0x960 - 0x956];
    NEMACT act[2];            /* 0x960 */
    u8  x9C8_[0x9D8 - 0x9C8];
    s8     x9D8;              /* 0x9D8 */
    u8  x9D9_[0x9ED - 0x9D9];
    u8     x9ED;              /* 0x9ED */
    u8  x9EE_[0xA00 - 0x9EE];
    s8     xA00;              /* 0xA00 */
} NEMV;

typedef struct NGW {
    u8     mode;              /* 0x000 */
    u8     step;              /* 0x001 */
    u8  x002_[0xD1 - 0x2];
    u8     master;            /* 0x0D1 */
    u8  x0D2_[0xD3 - 0xD2];
    u8     pl_num;            /* 0x0D3 */
    u8  x0D4_[0xD6 - 0xD4];
    u8     xD6;               /* 0x0D6 */
    u8     xD7;               /* 0x0D7 */
    u8     xD8[4];            /* 0x0D8 */
    u8  x0DC_[0xE0 - 0xDC];
    u8     xE0[4];            /* 0x0E0 */
    u8  x0E4_[0xE8 - 0xE4];
    u8     xE8[8];            /* 0x0E8 */
    u8  x0F0_[0xF8 - 0xF0];
    u8     xF8[8];            /* 0x0F8 */
    u8  x100_[0x108 - 0x100];
    u8     x108[8];           /* 0x108 */
    u8     x110[4];           /* 0x110 */
    u8  x114_[0x124 - 0x114];
    s32    x124;              /* 0x124 */
    NITEM  item[32];          /* 0x128 */
    s32    x1A8[2];           /* 0x1A8 */
    s16    x1B0;              /* 0x1B0 */
    u8  x1B2_[0x1E2 - 0x1B2];
    u16    x1E2;              /* 0x1E2 */
    u16    x1E4;              /* 0x1E4 */
    u8  x1E6_[0x1E8 - 0x1E6];
    s8     x1E8[4][8];        /* 0x1E8 */
    u8     pl_state[8];       /* 0x208 */
    u8  x210_[0x21B - 0x210];
    u8     x21B;              /* 0x21B */
    u8  x21C_[0x21D - 0x21C];
    s8     x21D;              /* 0x21D */
    u8     x21E;              /* 0x21E */
} NGW;

int Online_ck();
int Now_Game_ck(void);
int Pl_master_ck(void *);
void net_plpos_set(NPLV *, f32 *);
int AQ_data_put(int, u8 *, int);

typedef union NPLPK {
    struct {
        u8 cmd, len, x2, x3;
        u8 b4;
        u8 b5;
        s16 s6;
        f32 f8, fC, f10;
        u16 u14;
        u8 b16, b17, b18, b19;
        u16 u1A;
        s16 s1C, s1E;
        s8 c20;
        u8 b21, b22;
        s8 c23;
        s16 s24, s26;
        u16 u28, u2A;
        u8 b2C;
    } a;
    struct {
        u8 cmd, len, x2, x3;
        f32 f4, f8, fC;
        u16 u10;
        s16 s12, s14;
        u8 b16, b17;
        s16 s18, s1A;
    } b;
    struct {
        u8 cmd, len, x2, x3;
        f32 f4, f8, fC;
        u16 u10;
        u8 b12, b13;
    } c;
    struct {
        u8 cmd, len, x2, x3;
        u8 b4, b5;
        s16 s6;
        f32 f8, fC, f10;
        u16 u14;
        u8 b16, b17, b18, b19;
        u16 u1A;
        u8 b1C;
        u16 u1E, u20, u22, u24;
    } d;
    struct {
        u8 cmd, len, x2, x3;
        u8 b4, b5, b6, b7;
        u16 u8;
        s16 sA;
    } e;
    u8 pad[0x40];
} NPLPK;


/* received payload (packet + 4), layouts by kind */
typedef union NPLRX {
    struct {
        u8 b0, b1;
        u16 flags;
        f32 f4, f8, fC;
        u16 u10;
        u8 b12, b13, b14, b15;
        u16 u16_;
        s16 s18, s1A;
        u8 b1C, b1D, b1E;
        s8 c1F;
        s16 s20, s22;
        u16 u24, u26;
        u8 b28;
    } a;
    struct {
        f32 f0, f4, f8;
        u16 uC;
        s16 sE, s10;
        u8 b12, b13;
        s16 s14, s16_;
    } b;
    struct {
        f32 f0, f4, f8;
        u16 uC;
        u8 bE, bF;
    } c;
    struct {
        u8 b0, b1;
        u16 flags;
        f32 f4, f8, fC;
        u16 u10;
        u8 b12, b13, b14, b15;
        u16 u16_;
        u8 b18, b19;
        u16 u1A, u1C, u1E;
        s16 s20;
    } d;
    struct {
        u8 b0, b1, b2, b3;
        u16 u4;
        s16 s6;
    } e;
} NPLRX;

extern u8 player_work[];
int act_ck(void *, int, int);
int Pl_item_num_ck(void *, u16);
void Pl_item_stack(void *, u16, int);
void Pl_act_set(void *, int, int, int);
void Pl_adj_calc(void *, int, int);
void pl_init_sub(void *);
void Oki_item_set(void *);
void Ana_item_set(void *);
void Fue_item_set(void *);
void Taru_item_set(void *);
void Basic_item_set(void *);
void set01_set(int, int, int);
void net_send_pl(NPLV *, u8, s16);

#endif
