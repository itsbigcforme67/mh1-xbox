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
    u8 _pad05[0x0A - 0x05];
    u8 em_no;           /* 0x0A owner monster number */
    u8 xB;              /* 0x0B */
    u8 _pad0C[0x14 - 0x0C];
    s32 x14;            /* 0x14 */
    struct SENKO *senko;    /* 0x18 flash effect (shell17) */
    u8 _pad1C[0x20 - 0x1C];
    void (*move)(struct SHLW *);    /* 0x20 */
    VEC3 pos;           /* 0x24 */
    VEC3 pos2;          /* 0x30 */
    u8 _pad3C[0x60 - 0x3C];
    u8 x60;             /* 0x60 */
    u8 x61;             /* 0x61 */
    u8 _pad62[0x6A - 0x62];
    u8 body;            /* 0x6A */
    u8 _pad6B[0x78 - 0x6B];
    u16 flag;           /* 0x78 shell_flag_set/ck */
    u8 x7A;             /* 0x7A */
    u8 _pad7B;
    s16 char0;          /* 0x7C owner's animation when spawned */
    u8 _pad7E[0x88 - 0x7E];
    s32 x88;            /* 0x88 */
    s32 x8C;            /* 0x8C */
    u8 _pad90[0x94 - 0x90];
    void *owner;        /* 0x94 */
    u8 _pad98[0xBC - 0x98];
    struct PRIM *prim;  /* 0xBC */
    s16 prim_no;        /* 0xC0 */
    u8 _padC2[0xC8 - 0xC2];
    s16 xC8;            /* 0xC8 */
    u8 stg;             /* 0xCA */
} SHLW;

SHLW *pull_shell_work(int);
void push_shell_work(SHLW *);
int shell_flag_set(void *, int);

#endif
