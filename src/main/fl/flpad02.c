/* fl pad layer part 2 (SLPM_654.95 0x0018EED0-0x0018F0A8): padconf_setup_depth, flupdate_pad_stick_dir, flupdate_pad_button_data,
 * flupdate_pad_on_cnt, flPADFixedAnalogSelectSwitch. */
#include "types.h"

extern u32 flpad_io_map[];
double atan2(double, double);
void tarPADFixedAnalogSelectSwitch(int);

void padconf_setup_depth(u8 *d, u8 depth, u32 mask) {
    int i;
    u32 *m;

    i = 0;
    m = flpad_io_map;
    for (;;) {
        if (mask & *m) {
            if (d[i] < depth) {
                d[i] = depth;
            }
            mask ^= *m;
            if (mask == 0) {
                break;
            }
        }
        i++;
        m++;
        if (i >= 16) {
            break;
        }
    }
}

typedef struct PADST {
    s16 x, y;
    u8 pad4[4];
    f32 ang;
} PADST;

void flupdate_pad_stick_dir(PADST *p) {
    f32 a;
    s16 x = p->x;
    s16 y = p->y;

    if ((y | x) == 0) {
        a = 0.0f;
    } else {
        a = atan2(-y, x);
        if (a < 0.0f) {
            a += 6.2831855f;
        }
    }
    p->ang = a;
}

typedef struct PADBT {
    u8 pad0[8];
    u32 now, old, press, release, change;
} PADBT;

void flupdate_pad_button_data(PADBT *p, u32 v) {
    p->old = p->now;
    p->now = v;
    p->press = p->now & (p->old ^ p->now);
    p->release = p->old & (p->old ^ p->now);
    p->change = p->press | p->release;
}

void flupdate_pad_on_cnt(u8 *p) {
    s16 i;
    u32 *m;
    u8 *c;

    i = 0;
    m = flpad_io_map;
    c = p;
    do {
        if (*(u32 *)(p + 8) & *m) {
            if (c[0x48] != 0xFF) {
                c[0x48] = c[0x48] + 1;
            }
        } else {
            *(u16 *)(c + 0x48) = 0;
        }
        m++;
        i++;
        c += 2;
    } while (i < 24);
}

void flPADFixedAnalogSelectSwitch(int a) {
    tarPADFixedAnalogSelectSwitch(a);
}
