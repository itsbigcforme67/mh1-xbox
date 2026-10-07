/* NEAR-MATCH (not linked): flSetRenderState, 594 of 895 instructions differ. The structure is right (one flat switch, ladder identical, zero-check first in the
 * inner switches: `if (val == 0) break;` before them); what is off is the register choice of temporaries (state word in a0 / value in v1 in the original). */
/* fl library render state setter (SLPM_654.95 0x00177720-0x001784F4): flSetRenderState(id, value, ...) stores one render state value in the
 * fl state words (flSystemRenderState / flSystemRenderOperation bit fields, the matrix / material / light register files, colours as 0..1 floats)
 * and sends GS register packets for the states that need one (ALPHA, FOGCOL, TEST, TEX1, ZBUF). Ids 0x1A-0x39 are matrix registers,
 * 0x3A-0x59 materials, 0x5A-0x5C lights. Names of the bit fields are guesses. */
#include "types.h"

typedef unsigned long u64;
typedef struct FM { int m[16]; } FM;
typedef struct MAT19 { int w[19]; } MAT19;
typedef struct LGT { int a[4]; int x10; int b[3]; int x20; int c[11]; f32 f50; int d[5]; } LGT;

extern FM flMATRIX[];
extern MAT19 flMATERIAL[];
extern LGT flLIGHT[];
extern f32 flPS2ATTENUATION1[];
extern f32 flPS2Ambient[];
extern f32 flPS2FadeColor[];
extern u8 flPS2VIEWPROJ[];
extern u8 flACRVIEWPROJ[];
extern u8 flACRVIEWPORT[];
extern int flTextureStage[];
extern u32 flAlphaRefValue;
extern u32 flAmbient;
extern u32 flCTH;
extern u32 flFadeColor;
extern u32 flFogColor;
extern f32 flFogEnd;
extern f32 flFogStart;
extern u32 flMipmapK;
extern u32 flMipmapL;
extern u32 flSystemRenderOperation;
extern u32 flSystemRenderState;

void flPS2SendRenderState_ALPHA();
void flPS2SendRenderState_FOGCOL();
void flPS2SendRenderState_TEST();
void flPS2SendRenderState_TEX1();
void flPS2SendRenderState_ZBUF();
void flPS2SendTextureRegister();
void flPS2SetClearColor();
void flReloadTexture();
void flmatMul();
void flmatrMakeViewport();
void flvecNormalize();

int flSetRenderState(int id, u32 val, int arg2) {
    int n;
    int m;
    int li;
    u32 t;
    u32 c;

    n = id & 0xFF;
    if (n >= 0x1A && n < 0x3A) {
        m = n - 0x1A;
        if (val != 0) {
            flMATRIX[m] = *(FM *)val;
        }
        return 1;
    }
    if (n >= 0x3A && n < 0x5A) {
        m = n - 0x3A;
        flMATERIAL[m] = *(MAT19 *)val;
        return 1;
    }
    if (n >= 0x5A && n < 0x5D) {
        li = n - 0x5A;
        flLIGHT[li] = *(LGT *)val;
        flvecNormalize(&flLIGHT[li].c[4]);
        flLIGHT[li].x10 = 0;
        flLIGHT[li].x20 = 0;
        flPS2ATTENUATION1[li] = 1.0f / flLIGHT[li].f50;
        return 1;
    }
    switch (n) {
    case 0:
        flSystemRenderState &= ~0x60;
        flSystemRenderState |= val;
        break;
    case 3:
        break;
    case 1:
        flSystemRenderState &= ~0xF00;
        if (val == 0) {
            break;
        }
        switch (val) {
        case 0x200:
            flSystemRenderState |= 0x200;
            break;
        case 0x100:
            flSystemRenderState |= 0x100;
            break;
        case 0x300:
            flSystemRenderState |= 0x300;
            break;
        case 0x400:
            flSystemRenderState |= 0x400;
            break;
        case 0x500:
            flSystemRenderState |= 0x500;
            break;
        case 0x600:
            flSystemRenderState |= 0x600;
            break;
        case 0x700:
            flSystemRenderState |= 0x700;
            break;
        case 0x800:
            flSystemRenderState |= 0x800;
            break;
        }
        break;
    case 2:
        flSystemRenderState &= ~0x1C;
        if (val == 0) {
            break;
        }
        switch (val) {
        case 4:
        case 8:
        case 12:
        case 16:
        case 20:
        case 24:
            flSystemRenderState |= val;
            break;
        }
        break;
    case 4:
    case 5:
    case 6:
    case 7:
        t = val;
        if (n == 4) {
            flReloadTexture(1, &t, 4, 8);
            flPS2SendTextureRegister(t);
            flCTH = t;
        }
        flTextureStage[n - 4] = t;
        break;
    case 8:
    case 9:
    case 0xA:
    case 0xB:
        break;
    case 0xC:
        flSystemRenderState &= ~0x80;
        if (val == 1) {
            flSystemRenderState |= 0x80;
        }
        break;
    case 0xD:
        if ((flSystemRenderOperation & 0xC00) != val) {
            flSystemRenderOperation &= ~0xC00;
            flSystemRenderOperation |= val;
            flPS2SendRenderState_ALPHA(flSystemRenderOperation, 0, arg2);
        }
        break;
    case 0x5E:
        if ((flSystemRenderOperation & 0xFF) != val) {
            flSystemRenderOperation &= ~0xFF;
            flSystemRenderOperation |= val;
            flPS2SendRenderState_ALPHA(flSystemRenderOperation, 0, arg2);
        }
        break;
    case 0x61:
        if ((flSystemRenderOperation & 0x01000000) != val) {
            flSystemRenderOperation &= 0xFEFFFFFF;
            flSystemRenderOperation |= val;
            flPS2SendRenderState_ALPHA(flSystemRenderOperation, 0, arg2);
        }
        break;
    case 0xE:
        flAmbient = val;
        flPS2Ambient[0] = (f32)((flAmbient >> 16) & 0xFF) / 255.0f;
        flPS2Ambient[1] = (f32)((flAmbient >> 8) & 0xFF) / 255.0f;
        flPS2Ambient[2] = (f32)(flAmbient & 0xFF) / 255.0f;
        c = (flAmbient >> 24) & 0xFF;
        if (c == 0xFF) {
            flPS2Ambient[3] = 128.0f;
        } else if (c != 0) {
            c >>= 1;
            if (c == 0) {
                c = 1;
            }
            flPS2Ambient[3] = (f32)c;
        } else {
            flPS2Ambient[3] = 0.0f;
        }
        break;
    case 0xF:
        if (flFogColor != val) {
            flFogColor = val;
            flPS2SendRenderState_FOGCOL(val, arg2);
        }
        break;
    case 0x10:
        flFogStart = *(f32 *)&val;
        break;
    case 0x11:
        flFogEnd = *(f32 *)&val;
        break;
    case 0x12:
        flSystemRenderState &= 0xFFFE7FFF;
        if (val == 0x8000) {
            flSystemRenderState |= 0x8000;
        }
        break;
    case 0x13:
        break;
    case 0x14:
        flPS2SetClearColor(val, arg2);
        break;
    case 0x15:
        flSystemRenderState &= ~3;
        if (val != 0 && val == 2) {
            flSystemRenderState |= 2;
        }
        break;
    case 0x16:
        *(FM *)(flMATRIX + 0x21) = *(FM *)val;
        break;
    case 0x17:
        *(FM *)(flMATRIX + 0x20) = *(FM *)val;
        flmatMul(flPS2VIEWPROJ, flMATRIX + 0x20, flMATRIX + 0x22);
        flmatMul(flACRVIEWPROJ, flMATRIX + 0x20, flACRVIEWPORT);
        break;
    case 0x18:
        flmatrMakeViewport(*(f32 *)(val + 0x10), 0x22, *(int *)val, *(int *)(val + 4), *(int *)(val + 8));
        break;
    case 0x19:
        *(FM *)(flMATRIX + 0x23) = *(FM *)val;
        break;
    case 0x5D:
        flSystemRenderState = (u64)flSystemRenderState & 0xFFFFFF;
        flSystemRenderState |= val;
        break;
    case 0x5F:
        switch (val) {
        case 0:
            val = 0;
            break;
        case 1:
            val = 0x80000;
            break;
        case 2:
            val = 0x100000;
            break;
        case 3:
            val = 0x180000;
            break;
        case 4:
            val = 0x200000;
            break;
        case 5:
            val = 0x280000;
            break;
        case 6:
            val = 0x300000;
            break;
        case 7:
            val = 0x380000;
            break;
        }
        if ((flSystemRenderOperation & 0x380000) != val) {
            flSystemRenderOperation &= 0xFFC7FFFF;
            flSystemRenderOperation |= val;
            flPS2SendRenderState_TEST(flSystemRenderOperation, 0, arg2);
        }
        break;
    case 0x60:
        if (val != flAlphaRefValue) {
            flAlphaRefValue = val;
            flPS2SendRenderState_TEST(flSystemRenderOperation, 0, arg2);
        }
        break;
    case 0x62:
        flSystemRenderState &= ~0x4000;
        if (val != 0) {
            flSystemRenderState |= 0x4000;
        }
        break;
    case 0x63:
        flSystemRenderOperation &= 0xFFFEFFFF;
        if (val == 0x10000) {
            flSystemRenderOperation |= 0x10000;
        }
        flPS2SendRenderState_TEX1(flSystemRenderOperation, 0, arg2);
        break;
    case 0x64:
        flSystemRenderOperation &= 0xFFF9FFFF;
        if (val != 0 && val == 0x20000) {
            flSystemRenderOperation |= 0x20000;
        }
        break;
    case 0x65:
        break;
    case 0x66:
        flSystemRenderState &= 0xFFFDFFFF;
        if (val == 1) {
            flSystemRenderState |= 0x20000;
        }
        break;
    case 0x67:
        flFadeColor = val;
        flPS2FadeColor[0] = (f32)((val >> 16) & 0xFF) / 255.0f;
        flPS2FadeColor[1] = (f32)((val >> 8) & 0xFF) / 255.0f;
        flPS2FadeColor[2] = (f32)(val & 0xFF) / 255.0f;
        flPS2FadeColor[3] = (f32)((val >> 24) & 0xFF) / 255.0f;
        break;
    case 0x68:
        break;
    case 0x69:
        flMipmapL = val;
        break;
    case 0x6A:
        flMipmapK = val;
        break;
    case 0x6B:
        if ((flSystemRenderOperation & 0xC00000) != val) {
            flSystemRenderOperation &= 0xFF3FFFFF;
            flSystemRenderOperation |= val;
        }
        break;
    case 0x6C:
        t = 0;
        if (val == 1) {
            t = 0x8000;
        }
        val = t;
        if ((flSystemRenderOperation & 0x8000) != val) {
            flSystemRenderOperation &= 0xFFFF7FFF;
            flSystemRenderOperation |= val;
            flPS2SendRenderState_ZBUF(flSystemRenderOperation, 0, arg2);
        }
        break;
    case 0x6D:
        switch (val) {
        case 0:
            val = 0;
            break;
        case 1:
            val = 0x1000;
            break;
        case 2:
        case 4:
        case 5:
        case 6:
            break;
        case 3:
            val = 0x3000;
            break;
        case 7:
            val = 0x7000;
            break;
        }
        if ((flSystemRenderOperation & 0x7000) != val) {
            flSystemRenderOperation &= ~0x7000;
            flSystemRenderOperation |= val;
            flPS2SendRenderState_TEST(flSystemRenderOperation, 0, arg2);
        }
        break;
    }
    return 1;
}
