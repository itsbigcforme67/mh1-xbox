/* fl library: GS render state packets (SLPM_654.95 0x00178C90-0x001793D4): flPS2SendRenderState_ALPHA builds the ALPHA register value (blend equation)
 * from the render state bits: 0xC00 selects the table, 0xF the source/destination colour pairing and 0xF0 the alpha source, bit 0x1000000 keeps the
 * table value (otherwise the plain 0xA8 | FIX 0x80 value is used); unsupported combinations return 0. Queues a packet writing ALPHA_1, _2 or both (mode 0, 1, 2). */
#include "types.h"

typedef long u64;
typedef unsigned __int128 u128;

extern u8 flPs2VIF1Control[];
u64 *flPS2GetSystemTmpBuff();
int flPS2DmaAddQueue2();

int flPS2SendRenderState_ALPHA(int rs, int mode) {
    u64 v;
    int bm;
    u128 *p;
    u128 *top;
    int n;

    if (mode != 2) {
        p = (u128 *)flPS2GetSystemTmpBuff(0x30, 0x10);
        n = 2;
    } else {
        p = (u128 *)flPS2GetSystemTmpBuff(0x40, 0x10);
        n = 3;
    }
    top = p;
    *(u64 *)p = (u64)(u32)((n + 0x70000000) | 0x80000000);
    ((u32 *)p)[2] = 0x13000000;
    ((u32 *)p)[3] = n | 0x50000000;
    p++;
    *(u64 *)p = (u64)(u32)(n - 1) | (0x8000 | ((u64)0x10000000 << 32));
    ((u64 *)p)[1] = 0xE;
    p++;
    bm = rs & 0xC00;
    if (bm == 0) {
        switch (rs & 0xF) {
        case 1:
            switch (rs & 0xF0) {
            case 0x10:
                v = 0x68 | ((u64)0x80 << 32);
                break;
            case 0x0:
                v = 0xA8 | ((u64)0x80 << 32);
                break;
            case 0x20:
                v = 0x9;
                break;
            case 0x30:
                v = 0x6;
                break;
            case 0x40:
                v = 0x19;
                break;
            case 0x50:
                v = 0x16;
                break;
            default:
                return 0;
            }
            break;
        case 0:
            switch (rs & 0xF0) {
            case 0x10:
                v = 0x6A | ((u64)0x80 << 32);
                break;
            case 0x0:
                v = 0xAA | ((u64)0x80 << 32);
                break;
            case 0x20:
                v = 0x89;
                break;
            case 0x30:
                v = 0x46;
                break;
            case 0x40:
                v = 0x99;
                break;
            case 0x50:
                v = 0x56;
                break;
            default:
                return 0;
            }
            break;
        case 2:
            switch (rs & 0xF0) {
            case 16:
                v = 0x48;
                break;
            case 0:
                v = 0x88;
                break;
            case 48:
                v = 0x44;
                break;
            default:
                return 0;
            }
            break;
        case 3:
            switch (rs & 0xF0) {
            case 0:
                v = 0x2;
                break;
            case 32:
                v = 0x1;
                break;
            default:
                return 0;
            }
            break;
        case 4:
            switch (rs & 0xF0) {
            case 0x10:
                v = 0x58;
                break;
            case 0x0:
                v = 0x98;
                break;
            case 0x50:
                v = 0x54;
                break;
            default:
                return 0;
            }
            break;
        case 5:
            switch (rs & 0xF0) {
            case 0x0:
                v = 0x12;
                break;
            case 0x40:
                v = 0x11;
                break;
            default:
                return 0;
            }
            break;
        default:
            return 0;
        }
    } else if (bm == 0x400) {
        switch (rs & 0xF) {
        case 1:
            switch (rs & 0xF0) {
            case 16:
                v = 0xA4 | ((u64)0x80 << 32);
                break;
            case 0:
                v = 0x2A | ((u64)0x80 << 32);
                break;
            default:
                return 0;
            }
            break;
        case 0:
            switch (rs & 0xF0) {
            case 0x10:
                v = 0xA6 | ((u64)0x80 << 32);
                break;
            case 0x0:
                v = 0xAA | ((u64)0x80 << 32);
                break;
            case 0x20:
                v = 0x86;
                break;
            case 0x40:
                v = 0x96;
                break;
            default:
                return 0;
            }
            break;
        case 2:
            switch (rs & 0xF0) {
            case 0:
                v = 0x88;
                break;
            case 32:
                v = 0x84;
                break;
            default:
                return 0;
            }
            break;
        case 3:
            switch (rs & 0xF0) {
            case 0:
                v = 2;
                break;
            default:
                return 0;
            }
            break;
        case 4:
            switch (rs & 0xF0) {
            case 0x0:
                v = 0x98;
                break;
            case 0x40:
                v = 0x94;
                break;
            default:
                return 0;
            }
            break;
        case 5:
            switch (rs & 0xF0) {
            case 0:
                v = 0x56;
                break;
            default:
                return 0;
            }
            break;
        default:
            return 0;
        }
    } else if (bm == 0x800) {
        switch (rs & 0xF) {
        case 1:
            switch (rs & 0xF0) {
            case 16:
                v = 0xA1 | ((u64)0x80 << 32);
                break;
            case 0:
                v = 0xA2 | ((u64)0x80 << 32);
                break;
            default:
                return 0;
            }
            break;
        case 0:
            switch (rs & 0xF0) {
            case 0x10:
                v = 0xA9 | ((u64)0x80 << 32);
                break;
            case 0x0:
                v = 0xAA | ((u64)0x80 << 32);
                break;
            case 0x20:
                v = 0x89;
                break;
            case 0x30:
                v = 0x46;
                break;
            case 0x40:
                v = 0x99;
                break;
            case 0x50:
                v = 0x56;
                break;
            default:
                return 0;
            }
            break;
        case 2:
            switch (rs & 0xF0) {
            case 0:
                v = 0x82;
                break;
            case 32:
                v = 0x81;
                break;
            default:
                return 0;
            }
            break;
        case 4:
            switch (rs & 0xF0) {
            case 0x0:
                v = 0x92;
                break;
            case 0x40:
                v = 0x91;
                break;
            default:
                return 0;
            }
            break;
        default:
            return 0;
        }
    }
    if (!(rs & 0x01000000)) {
        v = 0xA8 | ((u64)0x80 << 32);
    }
    switch (mode) {
    case 0:
        *(u64 *)p = v;
        ((u64 *)p)[1] = 0x42;
        break;
    case 1:
        *(u64 *)p = v;
        ((u64 *)p)[1] = 0x43;
        break;
    case 2:
        *(u64 *)p = v;
        ((u64 *)p)[1] = 0x42;
        p++;
        *(u64 *)p = v;
        ((u64 *)p)[1] = 0x43;
        break;
    }
    flPS2DmaAddQueue2(0, (unsigned long)top & 0xFFFFFFFUL, top, flPs2VIF1Control);
    return 1;
}
