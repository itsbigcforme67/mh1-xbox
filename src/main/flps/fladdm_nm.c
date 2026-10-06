/* fl PS2 shader matrix packets, SLPM_654.95 (near-match C, not built into the link).
 * flPS2matMul .. PS2SHADER_FLMATRIX_COPY are hand-written VU0 macro assembler (kept as original bytes via
 * config/c_rawfuncs.txt, `static asm` so the compiler knows they leave v0/a1 alone and each
 * `p = helper(p + off, ...)` call keeps the packet pointer in v0).
 * flPS2AddMatrix_NNNN build one DMA packet each in the system temp buffer (flPS2GetSystemTmpBuff): header,
 * ambient colour, matrices (matMul2/matMul), lights, then the closing tag `id | 0x15000000`. */
#include "types.h"
typedef long long s64;

extern u8 flMATRIX[];
extern u8 flLIGHT[];
extern f32 flPS2Ambient[4];
extern u8 flPS2VIEWPROJ[];
extern u8 flPS2CLIPPROJ[];

typedef struct { u32 w[16]; } M64;
extern M64 flPS2VIEWPORT;

typedef struct AMBSRC { u8 _pad[0x24]; f32 a; } AMBSRC;
typedef struct AMDL { u8 _pad[0x4C]; AMBSRC *mat; u8 _pad2[4]; u32 flags; } AMDL;

extern f32 flFogEnd;
extern f32 flFogStart;
extern f32 flPS2FadeColor[4];

u32 *flPS2GetSystemTmpBuff(int size, int align);
void flmatInvert();
void flmatMul(void *dst, void *a, void *b);
void flvecNormalize(void *v);
void flvecApplyMat33(void *d, void *s, void *m);
void flmatTranspose(void *m);
void flmatMul33(void *d, void *a, void *b);
void flmatNormalize33(void *m);

/* packet field accessors (byte offsets from the packet base p) and the recurring packet pieces */
#define W(o) (*(u32 *)((u8 *)p + (o)))
#define F(o) (*(f32 *)((u8 *)p + (o)))
#define LF(o) (*(f32 *)(flLIGHT + (o)))
#define PB(o) (p + ((o) >> 2))
#define AMB (m->mat->a)
#define HEAD(sz, cnt, w7) \
    p = flPS2GetSystemTmpBuff(sz, 0x10); \
    W(0) = cnt; \
    W(4) = n; \
    *(s64 *)((u8 *)p + 8) = 0; \
    W(0x10) = 0x13000000; \
    W(0x14) = 0; \
    W(0x18) = 0x01000404; \
    W(0x1C) = w7; \
    F(0x20) = flPS2Ambient[0] * AMB; \
    F(0x24) = flPS2Ambient[1] * AMB; \
    F(0x28) = flPS2Ambient[2] * AMB; \
    F(0x2C) = flPS2Ambient[3];
#define FADE \
    F(0x30) = flPS2FadeColor[0]; \
    F(0x34) = flPS2FadeColor[1]; \
    F(0x38) = flPS2FadeColor[2]; \
    F(0x3C) = flPS2FadeColor[3];
#define FOG(o) \
    F(o) = flFogEnd; \
    F((o) + 4) = 1.0f / (flFogEnd - flFogStart); \
    W((o) + 8) = 0; \
    W((o) + 12) = 0;
#define TAIL(o) \
    W(o) = id | 0x15000000; \
    W((o) + 4) = 0; \
    W((o) + 8) = 0; \
    W((o) + 12) = 0;
#define VN(i, l) v[i] = -LF(l); v[(i) + 1] = -LF((l) + 4); v[(i) + 2] = -LF((l) + 8);
#define VP3(i, l) v[i] = LF(l); v[(i) + 1] = LF((l) + 4); v[(i) + 2] = LF((l) + 8);
#define LIGHTV \
    v[0] = -LF(0x34); \
    v[1] = -LF(0x38); \
    v[2] = -LF(0x3C); \
    v[4] = -LF(0x9C); \
    v[5] = -LF(0xA0); \
    v[6] = -LF(0xA4); \
    v[8] = -LF(0x104); \
    v[9] = -LF(0x108); \
    v[10] = -LF(0x10C);

static asm u32 *flPS2matMul(void *dst, void *a, void *b)
{
#include "flPS2matMul.inc"
}

static asm u32 *flPS2matMul2(void *dst, void *tmp, void *a, void *b, void *c)
{
#include "flPS2matMul2.inc"
}

static asm u32 *flPS2matNormalize33(void *dst, void *a)
{
#include "flPS2matNormalize33.inc"
}

static asm u32 *flPS2matMulNormalize33(void *dst, void *a, void *b, void *c)
{
#include "flPS2matMulNormalize33.inc"
}

static asm u32 *PS2SHADER_ADD_LIGHTVECD3(void *dst, void *m, void *v)
{
#include "PS2SHADER_ADD_LIGHTVECD3.inc"
}

static asm u32 *PS2SHADER_ADD_LIGHTVECP1(void *dst, void *a, void *b, void *c)
{
#include "PS2SHADER_ADD_LIGHTVECP1.inc"
}

static asm u32 *PS2SHADER_ADD_LIGHTVECP3(void *dst, void *a, void *b, void *c)
{
#include "PS2SHADER_ADD_LIGHTVECP3.inc"
}

static asm u32 *PS2SHADER_ADD_LIGHTVECD1P2(void *dst, void *a, void *b, void *c)
{
#include "PS2SHADER_ADD_LIGHTVECD1P2.inc"
}

static asm u32 *PS2SHADER_ADD_LIGHTVECD2P1(void *dst, void *a, void *b, void *c)
{
#include "PS2SHADER_ADD_LIGHTVECD2P1.inc"
}

static asm u32 *PS2SHADER_ADD_LIGHTVECD1P2_2(void *dst, void *a, void *b, void *c)
{
#include "PS2SHADER_ADD_LIGHTVECD1P2_2.inc"
}

static asm u32 *PS2SHADER_ADD_LIGHTVECD2P1_2(void *dst, void *a, void *b, void *c)
{
#include "PS2SHADER_ADD_LIGHTVECD2P1_2.inc"
}

static asm u32 *flPS2SHADER_ADD_SVEC1(void *dst, void *a, void *b)
{
#include "flPS2SHADER_ADD_SVEC1.inc"
}

static asm u32 *flPS2SHADER_ADD_SVEC2(void *dst, void *a, void *b)
{
#include "flPS2SHADER_ADD_SVEC2.inc"
}

static asm u32 *PS2SHADER_ADD_LIGHTCOL3(f32 s, void *dst)
{
#include "PS2SHADER_ADD_LIGHTCOL3.inc"
}

static asm u32 *PS2SHADER_ADD_LIGHTCOL3_2(f32 s, void *dst)
{
#include "PS2SHADER_ADD_LIGHTCOL3_2.inc"
}

static asm u32 *PS2SHADER_ADD_LIGHTCOL3_3(f32 s, void *dst)
{
#include "PS2SHADER_ADD_LIGHTCOL3_3.inc"
}

static asm u32 *PS2SHADER_ADD_LIGHTCOL1(f32 s, void *dst, void *a)
{
#include "PS2SHADER_ADD_LIGHTCOL1.inc"
}

static asm u32 *PS2SHADER_ADD_UVSCROLL(void *dst)
{
#include "PS2SHADER_ADD_UVSCROLL.inc"
}

static asm u32 *PS2SHADER_FLMATRIX_COPY(void *dst, u32 mask)
{
#include "PS2SHADER_FLMATRIX_COPY.inc"
}



/* flPS2AddMatrix_0002 / _0003: complete but the lights-loop registers differ (31 of ~190 instructions: the original puts the loop counter in s0 and the
 * light pointers in s4/s3/s2, this compiles to s4/s3/s2/s1). Both use the same loop; flPS2AddMatrix_000D (toon shader) is not written yet. */

void *flPS2AddMatrix_0002(AMDL *m, u32 id, u32 n) {
    u32 *p;
    f32 vec[3];
    int i, j, k;
    u8 *mp, *sp2, *dp, *s1, *s0, *lp, *cp, *q;
    HEAD(0xF90, 0x200000F8, 0x6CF60000)
    *(M64 *)PB(0x30) = *(M64 *)flPS2VIEWPROJ;
    *(M64 *)PB(0x70) = *(M64 *)flPS2CLIPPROJ;
    i = 0;
    mp = flMATRIX;
    sp2 = (u8 *)p;
    dp = (u8 *)p;
    for (; i < 32; i++) {
        if (m->flags & (1 << i)) {
            flPS2matMul(sp2 + 0x180, mp, flMATRIX + 0x840);
            j = 0;
            s1 = sp2;
            s0 = dp;
            do {
                *(f32 *)(s0 + 0x980) = *(f32 *)(s1 + 0x180);
                *(f32 *)(s0 + 0x984) = *(f32 *)(s1 + 0x184);
                *(f32 *)(s0 + 0x988) = *(f32 *)(s1 + 0x188);
                flvecNormalize(s0 + 0x980);
                j++;
                s1 += 0x10;
                s0 += 0x10;
            } while (j < 3);
        }
        mp += 0x40;
        sp2 += 0x40;
        dp += 0x30;
    }
    k = 0;
    lp = flLIGHT;
    cp = (u8 *)p;
    q = (u8 *)p;
    do {
        vec[0] = *(f32 *)(lp + 0x34);
        vec[1] = *(f32 *)(lp + 0x38);
        vec[2] = *(f32 *)(lp + 0x3C);
        flvecApplyMat33(vec, vec, flMATRIX + 0x840);
        k++;
        *(f32 *)(cp + 0xB0) = -vec[0];
        *(f32 *)(cp + 0xC0) = -vec[1];
        *(f32 *)(cp + 0xD0) = -vec[2];
        cp += 4;
        *(f32 *)(q + 0x120) = vec[0];
        *(f32 *)(q + 0x124) = vec[1];
        *(f32 *)(q + 0x128) = vec[2];
        *(f32 *)(q + 0x150) = *(f32 *)(lp + 0x14);
        *(f32 *)(q + 0x154) = *(f32 *)(lp + 0x18);
        *(f32 *)(q + 0x158) = *(f32 *)(lp + 0x1C);
        *(f32 *)(q + 0x15C) = *(f32 *)(lp + 0x20);
        lp += 0x68;
        q += 0x10;
    } while (k < 3);
    PS2SHADER_ADD_LIGHTCOL3(AMB, PB(0xE0));
    TAIL(0xF80)
    return p;
}


void *flPS2AddMatrix_0003(AMDL *m, u32 id, u32 n) {
    u32 *p;
    f32 vec[3];
    int j, k;
    u8 *s0, *lp, *cp, *q;
    HEAD(0x200, 0x2000001F, 0x6C1D0000)
    flPS2matMul(PB(0x30), flMATRIX, flMATRIX + 0x840);
    j = 0;
    s0 = (u8 *)p;
    do {
        *(f32 *)(s0 + 0xF0) = *(f32 *)(s0 + 0x30);
        *(f32 *)(s0 + 0xF4) = *(f32 *)(s0 + 0x34);
        *(f32 *)(s0 + 0xF8) = *(f32 *)(s0 + 0x38);
        flvecNormalize(s0 + 0xF0);
        j++;
        s0 += 0x10;
    } while (j < 3);
    flPS2matMul(PB(0x70), PB(0x30), flPS2VIEWPROJ);
    flPS2matMul(PB(0xB0), PB(0x30), flPS2CLIPPROJ);
    k = 0;
    lp = flLIGHT;
    cp = (u8 *)p;
    q = (u8 *)p;
    do {
        vec[0] = *(f32 *)(lp + 0x34);
        vec[1] = *(f32 *)(lp + 0x38);
        vec[2] = *(f32 *)(lp + 0x3C);
        flvecApplyMat33(vec, vec, flMATRIX + 0x840);
        k++;
        *(f32 *)(cp + 0x120) = -vec[0];
        *(f32 *)(cp + 0x130) = -vec[1];
        *(f32 *)(cp + 0x140) = -vec[2];
        cp += 4;
        *(f32 *)(q + 0x190) = vec[0];
        *(f32 *)(q + 0x194) = vec[1];
        *(f32 *)(q + 0x198) = vec[2];
        *(f32 *)(q + 0x1C0) = *(f32 *)(lp + 0x14);
        *(f32 *)(q + 0x1C4) = *(f32 *)(lp + 0x18);
        *(f32 *)(q + 0x1C8) = *(f32 *)(lp + 0x1C);
        *(f32 *)(q + 0x1CC) = *(f32 *)(lp + 0x20);
        lp += 0x68;
        q += 0x10;
    } while (k < 3);
    PS2SHADER_ADD_LIGHTCOL3(AMB, PB(0x150));
    TAIL(0x1F0)
    return p;
}

