/* Near-match, not linked: fl pad layer part 3 (SLPM_654.95 0x0018E6B0-0x0018E810): flPADGetALL (NEAR-MATCH, not linked: 4-5 of 86 instructions differ, load order of the stick byte pairs), reads both ports from the tarpad layer into the 0x88 byte
 * flpad_root records. */
#include "types.h"

typedef struct V3 { f32 x, y, z; } V3;
typedef struct S2 { s8 a, b; } S2;

typedef struct TPAD {
    u8 b0, b1;
    u16 h2;
    f32 f4;
    u32 now;
    S2 stick[8];
    V3 va, vb;
} TPAD;

typedef struct FPAD {
    u8 b0, b1;
    u16 h2;
    f32 f4;
    u32 now, old, press;
    u8 x14[0x1C - 0x14];
    S2 stick[8];
    V3 va, vb;
    s32 x44;
} FPAD;

extern TPAD tarpad_root[2];
extern FPAD *flpad_adr[2];
extern u8 NumOfValidPads;

void tarPADRead(void);
void flupdate_pad_button_data(FPAD *, u32);
void flupdate_pad_on_cnt(FPAD *);
void flPADACRConf(void);

void flPADGetALL(void) {
    s16 i;
    TPAD *t;
    S2 *s;
    S2 *d;
    int j;
    s8 x, y;
    FPAD *f;
    int off;

    tarPADRead();
    NumOfValidPads = 0;
    i = 0;
    t = tarpad_root;
    off = 0;
    do {
        ((FPAD *)((u8 *)flpad_adr[0] + off))->b0 = t->b0;
        ((FPAD *)((u8 *)flpad_adr[0] + off))->b1 = t->b1;
        ((FPAD *)((u8 *)flpad_adr[0] + off))->h2 = t->h2;
        ((FPAD *)((u8 *)flpad_adr[0] + off))->f4 = t->f4;
        f = (FPAD *)((u8 *)flpad_adr[0] + off);
        if (f->h2 != 0 && f->h2 != 0x8000) {
            NumOfValidPads++;
        }
        s = t->stick;
        d = f->stick;
        f->va = t->va;
        f->vb = t->vb;
        j = 8;
        do {
            j--;
            x = s->a;
            y = s->b;
            d->a = x;
            d->b = y;
            s++;
            d++;
        } while (j > 0);
        flupdate_pad_button_data(f, t->now);
        flupdate_pad_on_cnt((FPAD *)((u8 *)flpad_adr[0] + off));
        i++;
        t++;
        f = (FPAD *)((u8 *)flpad_adr[0] + off);
        off += 0x88;
        f->x44 = f->press;
    } while (i < 2);
    flPADACRConf();
}
