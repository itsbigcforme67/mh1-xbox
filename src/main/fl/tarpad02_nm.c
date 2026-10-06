/* tarPADInit (SLPM_654.95 0x00194B60): pad system start. Work in progress. */
#include "types.h"

typedef struct PADPAIR { s16 a, b; } PADPAIR;
typedef unsigned __int128 u128;
typedef struct PB32 { u128 a, b; } PB32;
typedef struct TARPAD {
    u8 x0[4];
    u8 x4, x5;
    u8 x6[0x38 - 6];
    u8 x38, x39;
} TARPAD;

typedef struct PS2SLOT {
    u8 x0, x1, x2, x3, x4;
    u8 x5;
    u8 x6, x7;
    void *buf;
    u8 xC[0x18 - 0xC];
} PS2SLOT;

typedef struct PS2CFG {
    s16 x0, lx, ly, x6;
} PS2CFG;

extern u8 ps2pad_clear[];
extern u8 ps2pad_backup[];
extern u8 pad_dma_buf[];
extern TARPAD tarpad_root[2];
extern PS2SLOT ps2slot[2];
extern PS2CFG ps2pad_config[2];
extern u8 flPadFixedAnalogSelectSwitch[2];
extern u8 flPadFASS[2];
extern s32 MtapPort;
extern s32 MtapSlotMax;
extern long ps2PadShotConf_Basic;

int PADDeviceInit(void);
void PADPortOpen(int, int, void *);
void ps2PADWorkClear(void);

int tarPADInit(void) {
    PADPAIR *s;
    u8 *p16;
    PS2CFG sp78;
    int i;
    PB32 sp50;
    PS2SLOT *sl;
    PADPAIR *d;
    u8 *p17;

    if (PADDeviceInit() == 0) {
        return 0;
    }
    ps2PADWorkClear();
    ps2pad_clear[0] = 0xFF;
    *(u16 *)(ps2pad_clear + 2) = 0xFFFF;
    sp50 = *(PB32 *)ps2pad_clear;
    *(PB32 *)ps2pad_backup = sp50;
    ps2slot[0].buf = pad_dma_buf;
    ps2slot[0].x0 = 0;
    ps2slot[0].x1 = 0;
    ps2slot[0].x2 = 0;
    ps2slot[0].x3 = 0;
    ps2slot[0].x5 = 0;
    s = (PADPAIR *)&sp50;
    d = (PADPAIR *)(ps2pad_backup + 0x20);
    i = 8;
    do {
        i--;
        *d++ = *s++;
    } while (i > 0);
    ps2slot[1].x0 = 0;
    ps2slot[1].x1 = 0;
    p17 = &ps2slot[1].x2;
    p16 = &ps2slot[1].x3;
    *p17 = 0;
    *p16 = 0;
    ps2slot[1].buf = pad_dma_buf + 0x100;
    ps2slot[1].x5 = 0;
    if (MtapPort == -1) {
        PADPortOpen(0, 0, &ps2slot[0]);
        PADPortOpen(1, 0, &ps2slot[1]);
    } else {
        sl = ps2slot;
        for (i = 0; i < MtapSlotMax; i++) {
            PADPortOpen(MtapPort, i, sl);
            sl++;
        }
        if (MtapSlotMax < 2) {
            PADPortOpen((MtapPort + 1) & 1, 0, &ps2slot[i]);
        }
    }
    *(long *)&sp78 = ps2PadShotConf_Basic;
    *(long *)ps2pad_config = *(long *)&sp78;
    flPadFASS[0] = 0;
    flPadFixedAnalogSelectSwitch[0] = 0;
    tarpad_root[0].x4 = ps2slot[0].x2;
    tarpad_root[0].x5 = ps2slot[0].x3;
    ps2pad_config[1] = sp78;
    tarpad_root[1].x38 = *p17;
    tarpad_root[1].x39 = *p16;
    flPadFASS[1] = 0;
    flPadFixedAnalogSelectSwitch[1] = 0;
    return 1;
}
