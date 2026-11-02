#ifndef GAME_MH2_H
#define GAME_MH2_H
/* MH2 game_w (0x6ADCB0): G's order with blocks grown. Offsets read from the relocations of near-twin functions: the MH1
 * and G offset of each field is in the comment. Fields marked "?" were not seen in MH2 code: placed by their neighbours. */
typedef struct GAME_W {
    u8 mode;            /* 0x000 */
    u8 step;            /* 0x001 */
    u8 sub;             /* 0x002 */
    u8 x03;             /* 0x003 */
    s16 x04;            /* 0x004 */
    s16 x06;            /* 0x006 */
    s16 x08;            /* 0x008 */
    s16 x0A;            /* 0x00A */
    u8 _pad00C[0xD - 0xC];
    u8 pad_on;          /* 0x00D */
    u8 _pad00E[0xF - 0xE];
    u8 x0F;             /* 0x00F */
    u8 x10;             /* 0x010 */
    u8 x11;             /* 0x011 */
    u8 x12;             /* 0x012 */
    u8 _pad013[0x14 - 0x13];
    u8 stage;           /* 0x014 */
    u8 x15;             /* 0x015 ? */
    u8 _pad016[0x20 - 0x16];
    u8 x1E;             /* 0x020 (MH1 0x01E) */
    u8 _pad021;
    u8 port[2];         /* 0x022 (MH1 0x020) */
    u8 _pad024[0x26 - 0x24];
    u8 sw_mask;         /* 0x026 (MH1 0x024) */
    u8 _pad027[0x2A - 0x27];
    u8 x28[4];          /* 0x02A (MH1 0x028) */
    u16 quest;          /* 0x02E (MH1 0x02C) */
    u8 x2E;             /* 0x030 (MH1 0x02E) */
    u8 x2F;             /* 0x031 (MH1 0x02F) */
    u8 x30[4];          /* 0x032 (MH1 0x030) */
    u8 _pad036[0x3C - 0x36];
    u8 master;          /* 0x03C (MH1 0x0D1, G 0x03A) */
    u8 x0D2;            /* 0x03D (MH1 0x0D2) */
    u8 pl_num;          /* 0x03E (MH1 0x0D3) */
    u8 hit_id;          /* 0x03F (MH1 0x0D4) */
    u8 x0D5;            /* 0x040 (MH1 0x0D5) */
    u8 x1DC;            /* 0x041 (MH1 0x1DC) */
    V3S x40[10];        /* 0x042 ? (MH1 0x040, 4 entries; G 10) */
    u8 x70[4];          /* 0x07E ? (MH1 0x070) */
    u8 _pad082[0x88 - 0x82];
    s8 x78[4];          /* 0x088 ? (MH1 0x078) */
    u8 _pad08C[0xF0 - 0x8C];
    struct EFT_MDLW *area_mdlw[8]; /* 0x0F0 ? (MH1 0x0A8, G 0x0B0) */
    u8 x80[4];          /* 0x110 (MH1 0x080, G 0x0D0) */
    u8 _pad114[0x12C - 0x114];
    s16 xCC;            /* 0x12C (MH1 0x0CC) */
    s16 xCE;            /* 0x12E (MH1 0x0CE) */
    u8 x0D0;            /* 0x130 (MH1 0x0D0) */
    u8 x1DD;            /* 0x131 ? (MH1 0x1DD) */
    u8 info_seq;        /* 0x132 ? (MH1 0x1DE) */
    u8 info_now;        /* 0x133 (MH1 0x1DF) */
    u8 _pad134[0x138 - 0x134];
    s16 x216;           /* 0x138 ? (MH1 0x216) */
    s16 x218;           /* 0x13A ? (MH1 0x218) */
    u8 shl10_num;       /* 0x13C (MH1 0x210) */
    u8 meat_num;        /* 0x13D (MH1 0x211) */
    u8 trap_num;        /* 0x13E (MH1 0x212) */
    s8 x21D;            /* 0x13F (MH1 0x21D) */
    PL_ITEM reward_item[48]; /* 0x140 ? (MH1 0x128, 32 entries) */
    s32 x1A8;           /* 0x200 ? (MH1 0x1A8) */
    s32 x1AC;           /* 0x204 ? (MH1 0x1AC) */
    u8 x1B2;            /* 0x208 ? (MH1 0x1B2) */
    u8 flag1B3;         /* 0x209 (MH1 0x1B3) */
    u8 gate_open;       /* 0x20A ? (MH1 0x1E6) */
    u8 x1E7;            /* 0x20B ? (MH1 0x1E7) */
    u8 _pad20C[0x224 - 0x20C];
    u8 pl_state[8];     /* 0x224 (MH1 0x208) */
    u8 _pad22C[0x26E - 0x22C];
    u8 x21B;            /* 0x26E (MH1 0x21B) */
    u8 _pad26F;
    u8 x21A;            /* 0x270 ? (MH1 0x21A) */
    u8 _pad271;
    s8 x213;            /* 0x272 ? (MH1 0x213) */
    s8 x214;            /* 0x273 ? (MH1 0x214) */
    s8 x215;            /* 0x274 ? (MH1 0x215) */
    u8 info_stop;       /* 0x275 (MH1 0x21F) */
    u8 x1E8[4][8];      /* 0x276 (MH1 0x1E8) */
    u8 _pad296[0x2C0 - 0x296];
} GAME_W;
#endif
