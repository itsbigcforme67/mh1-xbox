#ifndef SHELL_H
#define SHELL_H
/* Shells: projectiles and attack hit objects (shell00..shell23), handed out
 * by pull_shell_work(). Same mode-machine idea as set objects. Offsets from
 * matched code (shell18.c). */
#include "types.h"
#include "em.h"

struct PRIM;

typedef struct SHLW {
    u8 pad0;            /* 0x00 */
    u8 be_flag;         /* 0x01 */
    u8 type;            /* 0x02 */
    u8 arg;             /* 0x03 */
    u8 mode;            /* 0x04 */
    u8 x05;             /* 0x05 set to 1 by shell01_set2/3 */
    u8 x06;             /* 0x06 shell14: sound timer */
    u8 x07;             /* 0x07 shell10: counted in game_w.shl10_num; shell14: flicker */
    u8 x08;             /* 0x08 */
    u8 x09;             /* 0x09 */
    u8 em_no;           /* 0x0A owner monster number */
    u8 xB;              /* 0x0B */
    struct SHLW *prev;  /* 0x0C live list (shell_w_top is the head) */
    struct SHLW *next;  /* 0x10 */
    void (*trans)(struct SHLW *);   /* 0x14 draw callback, 0 = none */
    struct SENKO *senko;    /* 0x18 flash effect (shell17) */
    u8 heap_pos;        /* 0x1C work heap block (pull_shell_work) */
    u8 heap_n;          /* 0x1D */
    u8 x1E;             /* 0x1E */
    u8 _pad1F;
    void (*move)(struct SHLW *);    /* 0x20 */
    s32 ang[3];         /* 0x24 owner's rotation at spawn */
    VEC3 pos2;          /* 0x30 */
    VEC3 pos0;          /* 0x3C start position (Eft18_set3 sizes its effect by the distance) */
    f32 rate[3];        /* 0x48 velocity (shell03); shell14 keeps its scale in rate[0] */
    f32 rate_g[3];      /* 0x54 acceleration, added by shell_rate_add_g */
    u8 x60;             /* 0x60 */
    u8 x61;             /* 0x61 */
    u8 x62;             /* 0x62 attack power (shell06 scales it by range) */
    u8 x63;             /* 0x63 */
    u8 _pad64[0x6A - 0x64];
    u8 body;            /* 0x6A */
    u8 _pad6B;
    u8 ailment;         /* 0x6C bits set from Get_atk_value 0-6 (shell00_i) */
    s8 ailment_val;     /* 0x6D */
    u8 x6E;             /* 0x6E hit sound (shell06) */
    u8 x6F;             /* 0x6F hit mark (shell06) */
    u8 _pad70[0x78 - 0x70];
    u16 flag;           /* 0x78 shell_flag_set/ck */
    u8 x7A;             /* 0x7A */
    u8 x7B;             /* 0x7B delay counter: move_shell counts it down before running move */
    s16 char0;          /* 0x7C owner's animation when spawned */
    s16 x7E;            /* 0x7E */
    u8 _pad80[0x88 - 0x80];
    s32 x88;            /* 0x88 */
    s32 x8C;            /* 0x8C */
    u8 _pad90[0x94 - 0x90];
    void *owner;        /* 0x94 */
    u8 _pad98[4];
    EMW *x9C;           /* 0x9C monster caught (shell12) */
    s32 xA0;            /* 0xA0 */
    u8 _padA4[0xB4 - 0xA4];
    u8 xB4;             /* 0xB4 */
    u8 _padB5[0xB8 - 0xB5];
    u16 xB8;            /* 0xB8 shell00: owner's hit counter at spawn; shell06: spread seed */
    u8 _padBA[2];
    struct PRIM *prim;  /* 0xBC */
    s16 prim_no;        /* 0xC0 */
    u8 _padC2[0xC4 - 0xC2];
    s32 xC4;            /* 0xC4 cleared by pull_shell_work */
    u16 xC8;            /* 0xC8 */
    u8 stg;             /* 0xCA */
    u8 _padCB;
    u8 xCC;             /* 0xCC shell05: passed to set4 */
    u8 _padCD[0xD4 - 0xCD];
} SHLW;

SHLW *pull_shell_work(int);
void push_shell_work(SHLW *);
int shell_flag_set(struct SHLW *, int);

#endif
