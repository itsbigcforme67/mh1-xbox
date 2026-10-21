/* tarpad layer (SLPM_654.95 0x00194DF0-0x00194FF4): tarPADDestroy, tarPADFixedAnalogSelectSwitch, flPADConfigSetACRtoXX, tarPADRead, ps2PADWorkClear.
 * tarpad_root is a 0x34 byte record per port (+6 slot byte, +8 button bits, +0x1C / +0x28 the two analog stick vectors). */
#include "g_alias.h"
#include "types.h"
#ifndef __MWERKS__
unsigned char lever_analog_to_digital(void *);   /* (declared before its first use) */
#endif

typedef struct TARPAD {
    u8 x0[6];
    u8 x6;
    u8 x7;
    u32 bits;
    u8 xC[0x1C - 0xC];
    u8 stickL[0xC];
    u8 stickR[0xC];
} TARPAD;

typedef struct PS2SLOT {
    u8 x0, x1, x2, x3, x4;
    u8 x5;
    u8 x6, x7;
    void *buf;
    u8 xC[0x18 - 0xC];
} PS2SLOT;

typedef struct PS2CFG {
    s16 x0;
    s16 lx;
    s16 ly;
    s16 x6;
} PS2CFG;

extern TARPAD tarpad_root[2];
extern PS2SLOT ps2slot[2];
extern PS2CFG ps2pad_config[2];
extern u8 flPadFixedAnalogSelectSwitch[2];
extern u8 flPadFASS[2];
extern u8 etclever_wrong_data[];
extern u8 ps2pad_state[];
extern u8 ps2pad_backup[];
extern u8 ps2pad_clear[];

int PADRead_for_PS2(int);
void update_pad_stick_dir(void *, s16);
#ifdef __MWERKS__
int lever_analog_to_digital(void *);
#else
unsigned char lever_analog_to_digital(void *);   /* the real return type (tools/check_kr_returns.py) */
#endif
void PADDeviceDestroy(void);
void ps2PADWorkClear(void);
void flpad_ram_clear(s32 *, int);

#if !defined(G_PART) || G_PART == 1
void tarPADDestroy(void) {
    ps2PADWorkClear();
    PADDeviceDestroy();
}
#endif

#if !defined(G_PART) || G_PART == 1
void tarPADFixedAnalogSelectSwitch(s8 v) {
    flPadFixedAnalogSelectSwitch[0] = v;
#ifndef MHG_PS2
    flPadFixedAnalogSelectSwitch[1] = v;    /* G has one pad here */
#endif
}
#endif

#if !defined(G_PART)
void flPADConfigSetACRtoXX(int port, s16 a, s16 b, s16 c) {
    ps2pad_config[port].x0 = a;
    ps2pad_config[port].lx = b;
    ps2pad_config[port].ly = c;
}
#endif

#if !defined(G_PART)
void tarPADRead(void) {
    int i;
    TARPAD *t;
    PS2SLOT *s;
    PS2CFG *c;
    u8 *fs;
    u8 *fa;
    int l, r;

    i = 0;
    t = tarpad_root;
    s = ps2slot;
    c = ps2pad_config;
    fs = flPadFixedAnalogSelectSwitch;
    fa = flPadFASS;
    do {
        if (PADRead_for_PS2(i) != 0) {
            t->x6 = s->x5;
            t->bits = (t->bits & 0xFFF0) | etclever_wrong_data[t->bits & 0xF];
            update_pad_stick_dir(t->stickL, c->lx);
            update_pad_stick_dir(t->stickR, c->ly);
            l = lever_analog_to_digital(t->stickL) & 0xFF;
            r = lever_analog_to_digital(t->stickR) & 0xFF;
            t->bits |= l << 16;
            t->bits |= r << 20;
        }
        *fa = *fs;
        i++;
        t++;
        s++;
        c++;
        fs++;
        fa++;
    } while (i < 2);
}
#endif

#if !defined(G_PART) || G_PART == 2
void ps2PADWorkClear(void) {
    flpad_ram_clear((s32 *)tarpad_root, GV(0x68, 0x34));
    flpad_ram_clear((s32 *)ps2pad_state, GV(0x40, 0x20));
    flpad_ram_clear((s32 *)ps2pad_backup, GV(0x40, 0x20));
    flpad_ram_clear((s32 *)ps2pad_clear, 0x20);
}
#endif
