/* Near-match work file: flPADACRConf (SLPM_654.95 0x0018E810-0x0018EEC4): per port, copies the first 8 bytes of the pad record from the
 * "current" buffer (flpad_adr[0]) to the "work" buffer (flpad_adr[1]), flips the digital buttons and analog depth bytes with the per-port
 * remap tables (flpad_config: 24 button map bytes + three flip-mode bytes at +0x20/+0x21/+0x22), rebuilds the button word, then mirrors the two
 * sticks according to the flip mode (1 = x, 2 = y, 3 = both) and updates their angle/magnitude. Names are guesses. */
#include "types.h"

typedef struct STICK {
    s16 x;              /* 0x00 */
    s16 y;              /* 0x02 */
    s16 mag;            /* 0x04 */
    s16 deg;            /* 0x06 */
    f32 rad;            /* 0x08 */
} STICK;

typedef struct FPAD {
    u8 b0, b1;
    u16 h2;
    f32 f4;
    u32 now, old, press;
    u8 x14[0x1C - 0x14];
    u8 dep[16];         /* 0x1C analog depth bytes */
    STICK l;            /* 0x2C */
    STICK r;            /* 0x38 */
    s32 x44;
} FPAD;

typedef struct PADCFG2 {
    u8 map[24];         /* 0x00 */
    u8 x18[8];
    u8 dflip;           /* 0x20 depth flip table index */
    u8 lflip;           /* 0x21 left stick flip mode */
    u8 rflip;           /* 0x22 right stick flip mode */
    u8 x23[0x2C - 0x23];
} PADCFG2;

extern FPAD *flpad_adr[2];
extern PADCFG2 flpad_config[2];
extern u32 flpad_io_map[];
extern u8 fllever_flip_data[][16];
extern u8 fllever_depth_flip_data[][4];

void flupdate_pad_button_data(FPAD *, u32);
void flupdate_pad_on_cnt(FPAD *);
void flupdate_pad_stick_dir(STICK *);
void padconf_setup_depth(u8 *, u8, u32);

void flPADACRConf(void) {
    u8 tmp[16];
    u32 mask;
    s16 p;
    PADCFG2 *c;
    u32 bits;
    int off;
    s16 k;
    s16 j;
    u32 now;
    u8 a;
    FPAD *f0;

    p = 0;
    c = flpad_config;
    off = 0;
    do {
        ((FPAD *)((u8 *)flpad_adr[1] + off))->b0 = ((FPAD *)((u8 *)flpad_adr[0] + off))->b0;
        ((FPAD *)((u8 *)flpad_adr[1] + off))->b1 = ((FPAD *)((u8 *)flpad_adr[0] + off))->b1;
        ((FPAD *)((u8 *)flpad_adr[1] + off))->h2 = ((FPAD *)((u8 *)flpad_adr[0] + off))->h2;
        ((FPAD *)((u8 *)flpad_adr[1] + off))->f4 = ((FPAD *)((u8 *)flpad_adr[0] + off))->f4;
        f0 = (FPAD *)((u8 *)flpad_adr[0] + off);
        now = f0->now;
        mask = (now & 0xFFF0) | fllever_flip_data[c->dflip][now & 0xF] | (fllever_flip_data[c->lflip][(now >> 16) & 0xF] << 16) |
               (fllever_flip_data[c->rflip][(now >> 20) & 0xF] << 20);
        tmp[0] = f0->dep[fllever_depth_flip_data[c->dflip][0]];
        tmp[1] = f0->dep[fllever_depth_flip_data[c->dflip][1]];
        tmp[2] = f0->dep[fllever_depth_flip_data[c->dflip][2]];
        tmp[3] = f0->dep[fllever_depth_flip_data[c->dflip][3]];
        for (j = 4; j < 16; j++) {
            tmp[j] = f0->dep[j];
        }
        for (j = 0; j < 16; j++) {
            ((FPAD *)((u8 *)flpad_adr[1] + off))->dep[j] = 0;
        }
        bits = 0;
        for (k = 0; k < 24; k++) {
            if (mask & flpad_io_map[k]) {
                bits |= flpad_io_map[c->map[k]];
            }
            a = c->map[k];
            if (a < 0x10) {
                f0 = (FPAD *)((u8 *)flpad_adr[1] + off);
                if (f0->dep[a] < tmp[k]) {
                    f0->dep[a] = tmp[k];
                }
            } else if (a >= 0x19) {
                padconf_setup_depth(((FPAD *)((u8 *)flpad_adr[1] + off))->dep, tmp[k], flpad_io_map[a]);
            }
        }
        flupdate_pad_button_data((FPAD *)((u8 *)flpad_adr[1] + off), bits);
        flupdate_pad_on_cnt((FPAD *)((u8 *)flpad_adr[1] + off));
        f0 = (FPAD *)((u8 *)flpad_adr[1] + off);
        f0->x44 = f0->press;
        ((FPAD *)((u8 *)flpad_adr[1] + off))->l = ((FPAD *)((u8 *)flpad_adr[0] + off))->l;
        switch (c->lflip) {
        case 1:
            ((FPAD *)((u8 *)flpad_adr[1] + off))->l.x = -((FPAD *)((u8 *)flpad_adr[1] + off))->l.x;
            ((FPAD *)((u8 *)flpad_adr[1] + off))->l.deg = 540 - ((FPAD *)((u8 *)flpad_adr[1] + off))->l.deg;
            break;
        case 2:
            ((FPAD *)((u8 *)flpad_adr[1] + off))->l.y = -((FPAD *)((u8 *)flpad_adr[1] + off))->l.y;
            ((FPAD *)((u8 *)flpad_adr[1] + off))->l.deg = 360 - ((FPAD *)((u8 *)flpad_adr[1] + off))->l.deg;
            break;
        case 3:
            ((FPAD *)((u8 *)flpad_adr[1] + off))->l.x = -((FPAD *)((u8 *)flpad_adr[1] + off))->l.x;
            ((FPAD *)((u8 *)flpad_adr[1] + off))->l.y = -((FPAD *)((u8 *)flpad_adr[1] + off))->l.y;
            ((FPAD *)((u8 *)flpad_adr[1] + off))->l.deg += 180;
            break;
        }
        ((FPAD *)((u8 *)flpad_adr[1] + off))->r = ((FPAD *)((u8 *)flpad_adr[0] + off))->r;
        switch (c->rflip) {
        case 1:
            ((FPAD *)((u8 *)flpad_adr[1] + off))->r.x = -((FPAD *)((u8 *)flpad_adr[1] + off))->r.x;
            ((FPAD *)((u8 *)flpad_adr[1] + off))->r.deg = 540 - ((FPAD *)((u8 *)flpad_adr[1] + off))->r.deg;
            break;
        case 2:
            ((FPAD *)((u8 *)flpad_adr[1] + off))->r.y = -((FPAD *)((u8 *)flpad_adr[1] + off))->r.y;
            ((FPAD *)((u8 *)flpad_adr[1] + off))->r.deg = 360 - ((FPAD *)((u8 *)flpad_adr[1] + off))->r.deg;
            break;
        case 3:
            ((FPAD *)((u8 *)flpad_adr[1] + off))->r.x = -((FPAD *)((u8 *)flpad_adr[1] + off))->r.x;
            ((FPAD *)((u8 *)flpad_adr[1] + off))->r.y = -((FPAD *)((u8 *)flpad_adr[1] + off))->r.y;
            ((FPAD *)((u8 *)flpad_adr[1] + off))->r.deg += 180;
            break;
        }
        ((FPAD *)((u8 *)flpad_adr[1] + off))->l.deg = ((FPAD *)((u8 *)flpad_adr[1] + off))->l.deg % 360;
        ((FPAD *)((u8 *)flpad_adr[1] + off))->r.deg = ((FPAD *)((u8 *)flpad_adr[1] + off))->r.deg % 360;
        flupdate_pad_stick_dir(&((FPAD *)((u8 *)flpad_adr[1] + off))->l);
        flupdate_pad_stick_dir(&((FPAD *)((u8 *)flpad_adr[1] + off))->r);
        p++;
        off += 0x88;
        c++;
    } while (p < 2);
}
