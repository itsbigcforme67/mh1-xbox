/* NEAR-MATCH (not linked): flPS2InitRenderBuff, 672 of 764 instructions differ although the structure is right (same switches, same store order);
 * the original re-materialises constants (a2 = 2 before sceGsResetGraph) instead of reusing the compare constant, and schedules differently. */
/* fl library render buffer set-up (SLPM_654.95 0x0018C310-0x0018CF00): flPS2InitRenderBuff(fmt, nvram, mode, field, interlace) picks the screen size,
 * GS frame / z buffer layout from the video mode, fills flFrameBuf, builds the double buffer GIF packets (flPs2Db) and the draw start packet. */
#include "types.h"

typedef long s64;
typedef unsigned long u64;

extern u8 flPs2State[];
#define PS2S(T, o) (*(T *)(flPs2State + (o)))
extern u8 flFrameBuf[];
#define FB(o) (*(int *)(flFrameBuf + (o)))
extern u8 flPs2Db[];
#define DB(o) (*(s64 *)(flPs2Db + (o)))
extern u8 flPs2DrawStart[];
#define DS(o) (*(s64 *)(flPs2DrawStart + (o)))
extern int flWidth;
extern int flHeight;
extern u8 flPS2VSyncCallback[];

void flPS2DmaAddEndTag();
int flPS2GetStaticVramArea();
void flPS2InitRenderState();
void sceGsResetGraph();
void sceGsSyncVCallback();

void flPS2InitRenderBuff(int fmt, int nv, int mode, int field, int pal) {
    int w;
    int h;
    int pw;
    int ph;
    u32 q;
    int c;
    int t;

    PS2S(int, 0) = field;
    PS2S(int, 4) = mode;
    PS2S(int, 0x18) = fmt;
    PS2S(int, 0x30) = nv;
    PS2S(int, 0x3BC) = 0;
    PS2S(int, 0x3C0) = 0;
    switch (mode) {
    case 0:
        if (pal != 1) {
            w = 0x280;
            PS2S(int, 0x14) = 3;
        } else {
            w = 0x200;
            PS2S(int, 0x14) = 4;
        }
        if (field != 2) {
            if (field != 1) {
                sceGsResetGraph(0, 1, 2, 1);
                PS2S(int, 0x3B4) = 0x27C;
                h = 0xE0;
                ph = 0x1C0;
                PS2S(int, 0x3B8) = 0x32;
            } else {
                sceGsResetGraph(0, 1, 3, 1);
                PS2S(int, 0x3B4) = 0x290;
                h = 0xE0;
                ph = 0x1C0;
                PS2S(int, 0x3B8) = 0x62;
            }
        } else {
            sceGsResetGraph(0, 1, 3, 1);
            PS2S(int, 0x3B4) = 0x290;
            h = 0x100;
            ph = 0x200;
            PS2S(int, 0x3B8) = 0x48;
        }
        break;
    case 1:
        if (pal != 1) {
            w = 0x280;
            PS2S(int, 0x14) = 3;
        } else {
            w = 0x200;
            PS2S(int, 0x14) = 4;
        }
        if (field != 2) {
            if (field != 1) {
                sceGsResetGraph(0, 1, 2, 0);
                PS2S(int, 0x3B4) = 0x27C;
                h = 0x1C0;
                ph = 0x1C0;
                PS2S(int, 0x3B8) = 0x32;
            } else {
                sceGsResetGraph(0, 1, 3, 0);
                PS2S(int, 0x3B4) = 0x290;
                h = 0x1C0;
                ph = 0x1C0;
                PS2S(int, 0x3B8) = 0x62;
            }
        } else {
            sceGsResetGraph(0, 1, 3, 0);
            PS2S(int, 0x3B4) = 0x290;
            h = 0x200;
            ph = 0x200;
            PS2S(int, 0x3B8) = 0x48;
        }
        break;
    case 2:
        if (pal != 1) {
            w = 0x280;
            PS2S(int, 0x14) = 1;
        } else {
            PS2S(int, 0x14) = 2;
            w = 0x200;
        }
        sceGsResetGraph(0, 0, 0x50, 0);
        PS2S(int, 0x3B4) = 0x138;
        h = 0x1C0;
        ph = 0x1C0;
        PS2S(int, 0x3B8) = 0x23;
        break;
    }
    PS2S(int, 0x10) = ph;
    flWidth = w;
    PS2S(int, 0xC) = w;
    flHeight = h;
    switch (PS2S(int, 0x18)) {
    case 2:
        PS2S(u32, 0x1C) = 2;
        t = 0x40;
        PS2S(u32, 0x20) = 0x40;
        break;
    case 3:
        PS2S(u32, 0x1C) = 1;
        PS2S(u32, 0x20) = 0x40;
        t = 0x20;
        break;
    case 4:
    default:
        PS2S(u32, 0x1C) = 0;
        t = 0x20;
        PS2S(u32, 0x20) = 0x40;
        break;
    }
    PS2S(u32, 0x24) = t;
    q = fmt * (PS2S(u32, 0x24) * (((u32)(h + PS2S(u32, 0x24) - 1) / PS2S(u32, 0x24)) * (PS2S(u32, 0x20) * ((u32)(w + PS2S(u32, 0x20) - 1) / PS2S(u32, 0x20)))));
    PS2S(u32, 0x28) = flPS2GetStaticVramArea(q, PS2S(u32, 0x20)) & 0xFFFF;
    PS2S(u32, 0x2C) = flPS2GetStaticVramArea(q) & 0xFFFF;
    switch (PS2S(int, 0x30)) {
    case 2:
        if (PS2S(int, 0x18) == 2) {
            PS2S(u32, 0x34) = 0x32;
        } else {
            PS2S(u32, 0x34) = 0x3A;
        }
        PS2S(u32, 0x38) = 0x40;
        PS2S(u32, 0x3C) = 0x40;
        PS2S(int, 0x44) = 0x477FFF00;
        break;
    case 3:
        PS2S(u32, 0x34) = 0x31;
        PS2S(u32, 0x38) = 0x40;
        PS2S(u32, 0x3C) = 0x20;
        PS2S(int, 0x44) = 0x4B7FFF28;
        break;
    case 4:
    default:
        if (PS2S(int, 0x18) == 2) {
            PS2S(u32, 0x1C) = 0xA;
        }
        PS2S(u32, 0x34) = 0x30;
        PS2S(u32, 0x38) = 0x40;
        PS2S(u32, 0x3C) = 0x20;
        PS2S(int, 0x44) = 0x4B7FFF28;
        break;
    }
    q = flPS2GetStaticVramArea(nv * (PS2S(u32, 0x38) * ((u32)(w + PS2S(u32, 0x38) - 1) / PS2S(u32, 0x38)) * (PS2S(u32, 0x3C) * ((u32)(h + PS2S(u32, 0x3C) - 1) / PS2S(u32, 0x3C)))), PS2S(u32, 0x3C)) & 0xFFFF;
    PS2S(int, 0x3AC) = 0;
    PS2S(u32, 0x40) = q;
    PS2S(int, 0x3B0) = 0;
    FB(4) = w;
    PS2S(int, 0x3A4) = 0x800 - (w >> 1);
    FB(8) = h;
    PS2S(int, 0x3A8) = 0x800 - (h >> 1);
    FB(0x10) = 0;
    FB(0) = 0;
    c = PS2S(int, 0x18);
    FB(0x14) = c;
    FB(0xC) = w * c;
    switch (c) {
    case 2:
        FB(0x1C) = 0;
        FB(0x34) = 0xA;
        FB(0x18) = 5;
        FB(0x20) = 0x1F;
        FB(0x40) = 0xF;
        FB(0x28) = 5;
        FB(0x30) = 5;
        FB(0x24) = 5;
        FB(0x38) = 0x1F;
        FB(0x2C) = 0x1F;
        FB(0x3C) = 1;
        FB(0x44) = 1;
        c = 0x1F;
        break;
    case 3:
        FB(0x3C) = 0;
        FB(0x40) = 0;
        FB(0x18) = 8;
        FB(0x34) = 0x10;
        FB(0x20) = 0xFF;
        FB(0x24) = 8;
        FB(0x28) = 8;
        FB(0x30) = 8;
        FB(0x2C) = 0xFF;
        FB(0x38) = 0xFF;
        FB(0x44) = 0;
        FB(0x1C) = 0;
        break;
    case 4:
        FB(0x1C) = 0;
        c = 8;
        FB(0x34) = 0x10;
        FB(0x18) = 8;
        FB(0x40) = 0x18;
        FB(0x20) = 0xFF;
        FB(0x24) = 8;
        FB(0x28) = 8;
        FB(0x2C) = 0xFF;
        FB(0x30) = 8;
        FB(0x3C) = 8;
        FB(0x38) = 0xFF;
        FB(0x44) = 0xFF;
        break;
    }
    flPS2InitRenderState(c);
    PS2S(int, 0x50) = 0;
    PS2S(int, 0x4C) = 1;
    flPS2DmaAddEndTag(flPs2Db, 9, 0, 0);
    flPS2DmaAddEndTag(flPs2Db + 0xA0, 9, 0, 0);
    DB(0x18) = 0xE;
    DB(0x10) = 0x8008 | ((s64)0x10000000 << 32);
    DB(0xB0) = DB(0x10);
    DB(0xB8) = 0xE;
    pw = flWidth >> 6;
    DB(0x28) = 0x4C;
    DB(0x20) = ((s64)PS2S(u32, 0x1C) << 0x18) | ((u64)(PS2S(u32, 0x2C) >> 5) | ((s64)pw << 0x10)) | ((s64)1 << 0x20);
    DB(0x38) = 0x4D;
    DB(0x30) = DB(0x20);
    DB(0xC0) = ((s64)PS2S(u32, 0x1C) << 0x18) | ((u64)(PS2S(u32, 0x28) >> 5) | ((s64)pw << 0x10)) | ((s64)1 << 0x20);
    DB(0xD0) = DB(0xC0);
    DB(0xC8) = 0x4C;
    DB(0xD8) = 0x4D;
    DB(0x48) = 0x4E;
    DB(0x58) = 0x4F;
    DB(0x40) = (u64)(PS2S(u32, 0x40) >> 5) | ((s64)PS2S(u32, 0x34) << 0x18);
    DB(0x50) = DB(0x40);
    DB(0xE0) = DB(0x40);
    DB(0xF0) = DB(0x40);
    DB(0xE8) = 0x4E;
    DB(0xF8) = 0x4F;
    DB(0x68) = 0x18;
    DB(0x78) = 0x19;
    DB(0x60) = (PS2S(int, 0x3A4) * 0x10) | ((s64)(PS2S(int, 0x3A8) * 0x10) << 0x20);
    DB(0x70) = DB(0x60);
    DB(0x100) = DB(0x60);
    DB(0x110) = DB(0x60);
    DB(0x108) = 0x18;
    DB(0x118) = 0x19;
    if (PS2S(int, 0x18) == 2) {
        DB(0x80) = 1;
        DB(0x120) = 1;
    } else {
        DB(0x80) = 0;
        DB(0x120) = 0;
    }
    DB(0x88) = 0x45;
    DB(0x128) = 0x45;
    DB(0x90) = 1;
    DB(0x130) = 1;
    DB(0x98) = 0x46;
    DB(0x138) = 0x46;
    flPS2DmaAddEndTag(flPs2DrawStart, 0x13, 1, 0);
    DS(0x20) = 0;
    *(int *)(flPs2DrawStart + 8) = 0x13000000;
    DS(0x30) = 0;
    *(int *)(flPs2DrawStart + 0xC) = 0x51000013;
    DS(0x58) = 0;
    DS(0x60) = 0;
    DS(0x70) = 0;
    DS(0x10) = 0x8012 | ((s64)0x10000000 << 32);
    DS(0x18) = 0xE;
    DS(0x28) = 0x4A;
    DS(0xF8) = 0x4A;
    DS(0x38) = 0x40;
    DS(0x40) = 0x30003;
    DS(0x98) = 0x40;
    DS(0x50) = 6;
    DS(0x48) = 0x47;
    DS(0xB8) = 0x47;
    DS(0xA8) = 0x41;
    DS(0xC8) = 0x48;
    DS(0xD8) = 0x4E;
    DS(0xE8) = 0x4F;
    DS(0x108) = 0x4B;
    DS(0x118) = 0x49;
    DS(0x128) = 0x1A;
    DS(0x68) = 1;
    DS(0x120) = 1;
    DS(0x78) = 5;
    DS(0x88) = 5;
    DS(0x80) = 0;
    DS(0x130) = 0x71603524 | ((s64)0x60712435 << 0x20);
    DS(0x138) = 0x44;
    DS(0x90) = 0;
    DS(0xA0) = 0;
    DS(0xB0) = 0;
    DS(0xC0) = 0;
    DS(0xD0) = 0;
    DS(0xE0) = 0;
    DS(0xF0) = 0;
    DS(0x100) = 0;
    DS(0x110) = 0;
    sceGsSyncVCallback(flPS2VSyncCallback, 0x47);
}
