/* fl PS2 shader matrix packets, SLPM_654.95 0x0017BE20-0x00187C50.
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

/* packet field accessors (byte offsets from the packet base p) and the recurring packet pieces */
#define W(o) (*(u32 *)((u8 *)p + (o)))
#define F(o) (*(f32 *)((u8 *)p + (o)))
#define LF(o) (*(f32 *)(flLIGHT + (o)))
#define PB(o) ((u8 *)p + (o))
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

void flPS2AddMatrix_0000(AMDL *m, u32 id, u32 n) {
    u32 *p;
    f32 tmp[16];
    f32 v[12];
    p = flPS2GetSystemTmpBuff(0x130, 0x10);
    p[0] = 0x20000012;
    p[1] = n;
    *(s64 *)(p + 2) = 0;
    p[4] = 0x13000000;
    p[5] = 0;
    p[6] = 0x01000404;
    p[7] = 0x6C100000;
    ((f32 *)p)[8] = flPS2Ambient[0] * m->mat->a;
    ((f32 *)p)[9] = flPS2Ambient[1] * m->mat->a;
    ((f32 *)p)[10] = flPS2Ambient[2] * m->mat->a;
    ((f32 *)p)[11] = flPS2Ambient[3];
    p = flPS2matMul2(p + 12, tmp, flMATRIX, flMATRIX + 0x840, flPS2VIEWPROJ);
    p = flPS2matMul(p + 28, tmp, flPS2CLIPPROJ);
    v[0] = -*(f32 *)(flLIGHT + 0x34);
    v[1] = -*(f32 *)(flLIGHT + 0x38);
    v[2] = -*(f32 *)(flLIGHT + 0x3C);
    v[4] = -*(f32 *)(flLIGHT + 0x9C);
    v[5] = -*(f32 *)(flLIGHT + 0xA0);
    v[6] = -*(f32 *)(flLIGHT + 0xA4);
    v[8] = -*(f32 *)(flLIGHT + 0x104);
    v[9] = -*(f32 *)(flLIGHT + 0x108);
    v[10] = -*(f32 *)(flLIGHT + 0x10C);
    p = PS2SHADER_ADD_LIGHTVECD3(p + 44, flMATRIX, v);
    p = PS2SHADER_ADD_LIGHTCOL3(m->mat->a, p + 56);
    p[72] = id | 0x15000000;
    p[73] = 0;
    p[74] = 0;
    p[75] = 0;
}

void flPS2AddMatrix_0005(AMDL *m, u32 id, u32 n) {
    u32 *p;
    f32 tmp[16];
    p = flPS2GetSystemTmpBuff(0xC0, 0x10);
    p[0] = 0x2000000B;
    p[1] = n;
    *(s64 *)(p + 2) = 0;
    p[4] = 0x13000000;
    p[5] = 0;
    p[6] = 0x01000404;
    p[7] = 0x6C090000;
    ((f32 *)p)[8] = flPS2Ambient[0] * m->mat->a;
    ((f32 *)p)[9] = flPS2Ambient[1] * m->mat->a;
    ((f32 *)p)[10] = flPS2Ambient[2] * m->mat->a;
    ((f32 *)p)[11] = flPS2Ambient[3];
    p = flPS2matMul2(p + 12, tmp, flMATRIX, flMATRIX + 0x840, flPS2VIEWPROJ);
    p = flPS2matMul(p + 28, tmp, flPS2CLIPPROJ);
    p[44] = id | 0x15000000;
    p[45] = 0;
    p[46] = 0;
    p[47] = 0;
}
void flPS2AddMatrix_0006(AMDL *m, u32 id, u32 n) {
    u32 *p;
    f32 tmp[16];
    f32 v[12];
    HEAD(0x140, 0x20000013, 0x6c110000)
    p = flPS2matMul2(p + 12, tmp, flMATRIX, flMATRIX + 0x840, flPS2VIEWPROJ);
    p = flPS2matMul(p + 28, tmp, flPS2CLIPPROJ);
    LIGHTV
    p = PS2SHADER_ADD_LIGHTVECD3(p + 44, flMATRIX, v);
    p = PS2SHADER_ADD_LIGHTCOL3(AMB, p + 56);
    FOG(0x120)
    TAIL(0x130)
}

void flPS2AddMatrix_0007(AMDL *m, u32 id, u32 n) {
    u32 *p;
    f32 tmp[16];
    HEAD(0xd0, 0x2000000c, 0x6c0a0000)
    p = flPS2matMul2(p + 12, tmp, flMATRIX, flMATRIX + 0x840, flPS2VIEWPROJ);
    p = flPS2matMul(p + 28, tmp, flPS2CLIPPROJ);
    FOG(0xb0)
    TAIL(0xc0)
}

void flPS2AddMatrix_0008(AMDL *m, u32 id, u32 n) {
    u32 *p;
    f32 tmp[16];
    f32 v[12];
    HEAD(0x160, 0x20000015, 0x6c130000)
    p = flPS2matMul2(p + 12, tmp, flMATRIX, flMATRIX + 0x840, flPS2VIEWPROJ);
    p = flPS2matMul(p + 28, tmp, flPS2CLIPPROJ);
    LIGHTV
    p = PS2SHADER_ADD_LIGHTVECD3(p + 44, flMATRIX, v);
    p = PS2SHADER_ADD_LIGHTCOL3(AMB, p + 56);
    p = PS2SHADER_ADD_UVSCROLL(p + 72);
    TAIL(0x150)
}

void flPS2AddMatrix_0009(AMDL *m, u32 id, u32 n) {
    u32 *p;
    f32 tmp[16];
    HEAD(0xf0, 0x2000000e, 0x6c0c0000)
    p = flPS2matMul2(p + 12, tmp, flMATRIX, flMATRIX + 0x840, flPS2VIEWPROJ);
    p = flPS2matMul(p + 28, tmp, flPS2CLIPPROJ);
    p = PS2SHADER_ADD_UVSCROLL(p + 44);
    TAIL(0xe0)
}

void flPS2AddMatrix_000A(AMDL *m, u32 id, u32 n) {
    u32 *p;
    f32 tmp[16];
    f32 v[12];
    HEAD(0x170, 0x20000016, 0x6c140000)
    p = flPS2matMul2(p + 12, tmp, flMATRIX, flMATRIX + 0x840, flPS2VIEWPROJ);
    p = flPS2matMul(p + 28, tmp, flPS2CLIPPROJ);
    LIGHTV
    p = PS2SHADER_ADD_LIGHTVECD3(p + 44, flMATRIX, v);
    p = PS2SHADER_ADD_LIGHTCOL3(AMB, p + 56);
    p = PS2SHADER_ADD_UVSCROLL(p + 72);
    FOG(0x150)
    TAIL(0x160)
}

void flPS2AddMatrix_000B(AMDL *m, u32 id, u32 n) {
    u32 *p;
    f32 tmp[16];
    HEAD(0x100, 0x2000000f, 0x6c0d0000)
    p = flPS2matMul2(p + 12, tmp, flMATRIX, flMATRIX + 0x840, flPS2VIEWPROJ);
    p = flPS2matMul(p + 28, tmp, flPS2CLIPPROJ);
    p = PS2SHADER_ADD_UVSCROLL(p + 44);
    FOG(0xe0)
    TAIL(0xf0)
}

void flPS2AddMatrix_000E(AMDL *m, u32 id, u32 n) {
    u32 *p;
    f32 tmp[16];
    f32 v[12];
    HEAD(0x140, 0x20000013, 0x6c110000)
    FADE
    p = flPS2matMul2(p + 16, tmp, flMATRIX, flMATRIX + 0x840, flPS2VIEWPROJ);
    p = flPS2matMul(p + 32, tmp, flPS2CLIPPROJ);
    LIGHTV
    p = PS2SHADER_ADD_LIGHTVECD3(p + 48, flMATRIX, v);
    p = PS2SHADER_ADD_LIGHTCOL3(AMB, p + 60);
    TAIL(0x130)
}

void flPS2AddMatrix_000F(AMDL *m, u32 id, u32 n) {
    u32 *p;
    f32 tmp[16];
    HEAD(0xd0, 0x2000000c, 0x6c0a0000)
    FADE
    p = flPS2matMul2(p + 16, tmp, flMATRIX, flMATRIX + 0x840, flPS2VIEWPROJ);
    p = flPS2matMul(p + 32, tmp, flPS2CLIPPROJ);
    TAIL(0xc0)
}

void flPS2AddMatrix_0010(AMDL *m, u32 id, u32 n) {
    u32 *p;
    f32 tmp[16];
    f32 v[12];
    HEAD(0x150, 0x20000014, 0x6c120000)
    FADE
    p = flPS2matMul2(p + 16, tmp, flMATRIX, flMATRIX + 0x840, flPS2VIEWPROJ);
    p = flPS2matMul(p + 32, tmp, flPS2CLIPPROJ);
    LIGHTV
    p = PS2SHADER_ADD_LIGHTVECD3(p + 48, flMATRIX, v);
    p = PS2SHADER_ADD_LIGHTCOL3(AMB, p + 60);
    FOG(0x130)
    TAIL(0x140)
}

void flPS2AddMatrix_0011(AMDL *m, u32 id, u32 n) {
    u32 *p;
    f32 tmp[16];
    HEAD(0xe0, 0x2000000d, 0x6c0b0000)
    FADE
    p = flPS2matMul2(p + 16, tmp, flMATRIX, flMATRIX + 0x840, flPS2VIEWPROJ);
    p = flPS2matMul(p + 32, tmp, flPS2CLIPPROJ);
    FOG(0xc0)
    TAIL(0xd0)
}

void flPS2AddMatrix_0012(AMDL *m, u32 id, u32 n) {
    u32 *p;
    f32 tmp[16];
    f32 v[12];
    HEAD(0x170, 0x20000016, 0x6c140000)
    FADE
    p = flPS2matMul2(p + 16, tmp, flMATRIX, flMATRIX + 0x840, flPS2VIEWPROJ);
    p = flPS2matMul(p + 32, tmp, flPS2CLIPPROJ);
    LIGHTV
    p = PS2SHADER_ADD_LIGHTVECD3(p + 48, flMATRIX, v);
    p = PS2SHADER_ADD_LIGHTCOL3(AMB, p + 60);
    p = PS2SHADER_ADD_UVSCROLL(p + 76);
    TAIL(0x160)
}

void flPS2AddMatrix_0013(AMDL *m, u32 id, u32 n) {
    u32 *p;
    f32 tmp[16];
    HEAD(0x100, 0x2000000f, 0x6c0d0000)
    FADE
    p = flPS2matMul2(p + 16, tmp, flMATRIX, flMATRIX + 0x840, flPS2VIEWPROJ);
    p = flPS2matMul(p + 32, tmp, flPS2CLIPPROJ);
    p = PS2SHADER_ADD_UVSCROLL(p + 48);
    TAIL(0xf0)
}

void flPS2AddMatrix_0014(AMDL *m, u32 id, u32 n) {
    u32 *p;
    f32 tmp[16];
    f32 v[12];
    HEAD(0x180, 0x20000017, 0x6c150000)
    FADE
    p = flPS2matMul2(p + 16, tmp, flMATRIX, flMATRIX + 0x840, flPS2VIEWPROJ);
    p = flPS2matMul(p + 32, tmp, flPS2CLIPPROJ);
    LIGHTV
    p = PS2SHADER_ADD_LIGHTVECD3(p + 48, flMATRIX, v);
    p = PS2SHADER_ADD_LIGHTCOL3(AMB, p + 60);
    p = PS2SHADER_ADD_UVSCROLL(p + 76);
    FOG(0x160)
    TAIL(0x170)
}

void flPS2AddMatrix_0015(AMDL *m, u32 id, u32 n) {
    u32 *p;
    f32 tmp[16];
    HEAD(0x110, 0x20000010, 0x6c0e0000)
    FADE
    p = flPS2matMul2(p + 16, tmp, flMATRIX, flMATRIX + 0x840, flPS2VIEWPROJ);
    p = flPS2matMul(p + 32, tmp, flPS2CLIPPROJ);
    p = PS2SHADER_ADD_UVSCROLL(p + 48);
    FOG(0xf0)
    TAIL(0x100)
}

void flPS2AddMatrix_001B(AMDL *m, u32 id, u32 n) {
    u32 *p;
    HEAD(0x8d0, 0x2000008c, 0x6c8a0000)
    FADE
    p = flPS2matMul(p + 16, flMATRIX + 0x840, flPS2VIEWPROJ);
    p = flPS2matMul(p + 32, flMATRIX + 0x840, flPS2CLIPPROJ);
    p = PS2SHADER_FLMATRIX_COPY(p + 48, *(u32 *)((u8 *)m + 0x54));
    TAIL(0x8c0)
}

void flPS2AddMatrix_001E(AMDL *m, u32 id, u32 n) {
    u32 *p;
    HEAD(0xf40, 0x200000f3, 0x6cf10000)
    p = flPS2matMul(p + 12, flMATRIX + 0x840, flPS2VIEWPROJ);
    p = flPS2matMul(p + 28, flMATRIX + 0x840, flPS2CLIPPROJ);
    F(0xb0) = -LF(0x34);
    F(0xb4) = -LF(0x38);
    F(0xb8) = -LF(0x3c);
    F(0xc0) = -LF(0x9c);
    F(0xc4) = -LF(0xa0);
    F(0xc8) = -LF(0xa4);
    F(0xd0) = -LF(0x104);
    F(0xd4) = -LF(0x108);
    F(0xd8) = -LF(0x10c);
    p = PS2SHADER_FLMATRIX_COPY(p + 76, *(u32 *)((u8 *)m + 0x54));
    p = PS2SHADER_ADD_LIGHTCOL3(AMB, p + 56);
    FOG(0x120)
    TAIL(0xf30)
}

void flPS2AddMatrix_0020(AMDL *m, u32 id, u32 n) {
    u32 *p;
    HEAD(0x8e0, 0x2000008d, 0x6c8b0000)
    FADE
    p = flPS2matMul(p + 20, flMATRIX + 0x840, flPS2VIEWPROJ);
    p = flPS2matMul(p + 36, flMATRIX + 0x840, flPS2CLIPPROJ);
    p = PS2SHADER_FLMATRIX_COPY(p + 52, *(u32 *)((u8 *)m + 0x54));
    FOG(0x40)
    TAIL(0x8d0)
}

void flPS2AddMatrix_0021(AMDL *m, u32 id, u32 n) {
    u32 *p;
    HEAD(0xf60, 0x200000f5, 0x6cf30000)
    p = flPS2matMul(p + 12, flMATRIX + 0x840, flPS2VIEWPROJ);
    p = flPS2matMul(p + 28, flMATRIX + 0x840, flPS2CLIPPROJ);
    p = PS2SHADER_ADD_UVSCROLL(p + 72);
    F(0xb0) = -LF(0x34);
    F(0xb4) = -LF(0x38);
    F(0xb8) = -LF(0x3c);
    F(0xc0) = -LF(0x9c);
    F(0xc4) = -LF(0xa0);
    F(0xc8) = -LF(0xa4);
    F(0xd0) = -LF(0x104);
    F(0xd4) = -LF(0x108);
    F(0xd8) = -LF(0x10c);
    p = PS2SHADER_FLMATRIX_COPY(p + 84, *(u32 *)((u8 *)m + 0x54));
    p = PS2SHADER_ADD_LIGHTCOL3(AMB, p + 56);
    TAIL(0xf50)
}

void flPS2AddMatrix_004E(AMDL *m, u32 id, u32 n) {
    u32 *p;
    HEAD(0xf70, 0x200000f6, 0x6cf40000)
    p = flPS2matMul(p + 12, flMATRIX + 0x840, flPS2VIEWPROJ);
    p = flPS2matMul(p + 28, flMATRIX + 0x840, flPS2CLIPPROJ);
    F(0xb0) = -LF(0x34);
    F(0xb4) = -LF(0x38);
    F(0xb8) = -LF(0x3c);
    F(0xc0) = -LF(0x9c);
    F(0xc4) = -LF(0xa0);
    F(0xc8) = -LF(0xa4);
    F(0xd0) = -LF(0x104);
    F(0xd4) = -LF(0x108);
    F(0xd8) = -LF(0x10c);
    p = PS2SHADER_FLMATRIX_COPY(p + 88, *(u32 *)((u8 *)m + 0x54));
    p = PS2SHADER_ADD_LIGHTCOL3(AMB, p + 56);
    FOG(0x120)
    p = PS2SHADER_ADD_UVSCROLL(p + 76);
    TAIL(0xf60)
}

void flPS2AddMatrix_0016(AMDL *m, u32 id, u32 n) {
    u32 *p;
    f32 tmp[16];
    HEAD(0x120, 0x20000011, 0x6c0f0000)
    FADE
    p = flPS2matMul2(p + 16, tmp, flMATRIX, flMATRIX + 0x840, flPS2VIEWPROJ);
    p = flPS2matMul(p + 32, tmp, flPS2CLIPPROJ);
    *(M64 *)((u8 *)p + 0xd0) = flPS2VIEWPORT;
    FOG(0xc0)
    TAIL(0x110)
}

void flPS2AddMatrix_0017(AMDL *m, u32 id, u32 n) {
    u32 *p;
    f32 tmp[16];
    HEAD(0x150, 0x20000014, 0x6c120000)
    FADE
    p = flPS2matMul2(p + 16, tmp, flMATRIX, flMATRIX + 0x840, flPS2VIEWPROJ);
    p = flPS2matMul(p + 32, tmp, flPS2CLIPPROJ);
    p = PS2SHADER_ADD_UVSCROLL(p + 48);
    *(M64 *)((u8 *)p + 0x100) = flPS2VIEWPORT;
    FOG(0xf0)
    TAIL(0x140)
}

void flPS2AddMatrix_0018(AMDL *m, u32 id, u32 n) {
    u32 *p;
    f32 tmp[16];
    f32 v[12];
    HEAD(0x170, 0x20000016, 0x6c140000)
    p = flPS2matMul2(p + 12, tmp, flMATRIX, flMATRIX + 0x840, flPS2VIEWPROJ);
    p = flPS2matMul(p + 28, tmp, flPS2CLIPPROJ);
    LIGHTV
    p = PS2SHADER_ADD_LIGHTVECD3(p + 44, flMATRIX, v);
    p = PS2SHADER_ADD_LIGHTCOL3(AMB, p + 56);
    *(M64 *)((u8 *)p + 0x120) = flPS2VIEWPORT;
    TAIL(0x160)
}

void flPS2AddMatrix_0019(AMDL *m, u32 id, u32 n) {
    u32 *p;
    f32 tmp[16];
    f32 v[12];
    HEAD(0x1c0, 0x2000001b, 0x6c190000)
    FADE
    p = flPS2matMul2(p + 16, tmp, flMATRIX, flMATRIX + 0x840, flPS2VIEWPROJ);
    p = flPS2matMul(p + 32, tmp, flPS2CLIPPROJ);
    LIGHTV
    p = PS2SHADER_ADD_LIGHTVECD3(p + 48, flMATRIX, v);
    p = PS2SHADER_ADD_LIGHTCOL3(AMB, p + 60);
    p = PS2SHADER_ADD_UVSCROLL(p + 76);
    FOG(0x160)
    *(M64 *)((u8 *)p + 0x170) = flPS2VIEWPORT;
    TAIL(0x1b0)
}

void flPS2AddMatrix_001A(AMDL *m, u32 id, u32 n) {
    u32 *p;
    f32 tmp[16];
    HEAD(0x120, 0x20000011, 0x6c0f0000)
    FADE
    p = flPS2matMul2(p + 16, tmp, flMATRIX, flMATRIX + 0x840, flPS2VIEWPROJ);
    p = flPS2matMul(p + 32, tmp, flPS2CLIPPROJ);
    *(M64 *)((u8 *)p + 0xc0) = *(M64 *)(flMATRIX + 0x40);
    FOG(0x100)
    TAIL(0x110)
}

void flPS2AddMatrix_001C(AMDL *m, u32 id, u32 n) {
    u32 *p;
    f32 tmp[16];
    HEAD(0x100, 0x2000000f, 0x6c0d0000)
    p = flPS2matMul2(p + 12, tmp, flMATRIX, flMATRIX + 0x840, flPS2VIEWPROJ);
    p = flPS2matMul(p + 28, tmp, flPS2CLIPPROJ);
    *(M64 *)((u8 *)p + 0xb0) = flPS2VIEWPORT;
    TAIL(0xf0)
}

void flPS2AddMatrix_001D(AMDL *m, u32 id, u32 n) {
    u32 *p;
    f32 tmp[16];
    HEAD(0x110, 0x20000010, 0x6c0e0000)
    FADE
    p = flPS2matMul2(p + 16, tmp, flMATRIX, flMATRIX + 0x840, flPS2VIEWPROJ);
    p = flPS2matMul(p + 32, tmp, flPS2CLIPPROJ);
    *(M64 *)((u8 *)p + 0xc0) = flPS2VIEWPORT;
    TAIL(0x100)
}

void flPS2AddMatrix_001F(AMDL *m, u32 id, u32 n) {
    u32 *p;
    HEAD(0xf70, 0x200000f6, 0x6cf40000)
    p = flPS2matMul(p + 12, flMATRIX + 0x840, flPS2VIEWPROJ);
    p = flPS2matMul(p + 28, flMATRIX + 0x840, flPS2CLIPPROJ);
    F(0xb0) = -LF(0x34);
    F(0xb4) = -LF(0x38);
    F(0xb8) = -LF(0x3c);
    F(0xc0) = -LF(0x9c);
    F(0xc4) = -LF(0xa0);
    F(0xc8) = -LF(0xa4);
    F(0xd0) = -LF(0x104);
    F(0xd4) = -LF(0x108);
    F(0xd8) = -LF(0x10c);
    p = PS2SHADER_FLMATRIX_COPY(p + 88, *(u32 *)((u8 *)m + 0x54));
    p = PS2SHADER_ADD_LIGHTCOL3(AMB, p + 56);
    *(M64 *)((u8 *)p + 0x120) = flPS2VIEWPORT;
    TAIL(0xf60)
}

void flPS2AddMatrix_0022(AMDL *m, u32 id, u32 n) {
    u32 *p;
    HEAD(0x900, 0x2000008f, 0x6c8d0000)
    FADE
    p = PS2SHADER_ADD_UVSCROLL(p + 48);
    p = flPS2matMul(p + 16, flMATRIX + 0x840, flPS2VIEWPROJ);
    p = flPS2matMul(p + 32, flMATRIX + 0x840, flPS2CLIPPROJ);
    p = PS2SHADER_FLMATRIX_COPY(p + 60, *(u32 *)((u8 *)m + 0x54));
    TAIL(0x8f0)
}

void flPS2AddMatrix_0023(AMDL *m, u32 id, u32 n) {
    u32 *p;
    HEAD(0x940, 0x20000093, 0x6c910000)
    FADE
    p = PS2SHADER_ADD_UVSCROLL(p + 48);
    p = flPS2matMul(p + 16, flMATRIX + 0x840, flPS2VIEWPROJ);
    p = flPS2matMul(p + 32, flMATRIX + 0x840, flPS2CLIPPROJ);
    *(M64 *)((u8 *)p + 0xf0) = flPS2VIEWPORT;
    p = PS2SHADER_FLMATRIX_COPY(p + 76, *(u32 *)((u8 *)m + 0x54));
    TAIL(0x930)
}

void flPS2AddMatrix_0024(AMDL *m, u32 id, u32 n) {
    u32 *p;
    HEAD(0x910, 0x20000090, 0x6c8e0000)
    FADE
    p = flPS2matMul(p + 16, flMATRIX + 0x840, flPS2VIEWPROJ);
    p = flPS2matMul(p + 32, flMATRIX + 0x840, flPS2CLIPPROJ);
    *(M64 *)((u8 *)p + 0xc0) = flPS2VIEWPORT;
    p = PS2SHADER_FLMATRIX_COPY(p + 64, *(u32 *)((u8 *)m + 0x54));
    TAIL(0x900)
}

void flPS2AddMatrix_0025(AMDL *m, u32 id, u32 n) {
    u32 *p;
    f32 tmp[16];
    HEAD(0x110, 0x20000010, 0x6c0e0000)
    FOG(0x30)
    p = flPS2matMul2(p + 16, tmp, flMATRIX, flMATRIX + 0x840, flPS2VIEWPROJ);
    p = flPS2matMul(p + 32, tmp, flPS2CLIPPROJ);
    *(M64 *)((u8 *)p + 0xc0) = flPS2VIEWPORT;
    TAIL(0x100)
}

void flPS2AddMatrix_0026(AMDL *m, u32 id, u32 n) {
    u32 *p;
    f32 tmp[16];
    HEAD(0x130, 0x20000012, 0x6c100000)
    p = PS2SHADER_ADD_UVSCROLL(p + 44);
    p = flPS2matMul2(p + 12, tmp, flMATRIX, flMATRIX + 0x840, flPS2VIEWPROJ);
    p = flPS2matMul(p + 28, tmp, flPS2CLIPPROJ);
    *(M64 *)((u8 *)p + 0xe0) = flPS2VIEWPORT;
    TAIL(0x120)
}

void flPS2AddMatrix_0051(AMDL *m, u32 id, u32 n) {
    u32 *p;
    f32 tmp[16];
    HEAD(0x140, 0x20000013, 0x6c110000)
    FADE
    p = flPS2matMul2(p + 16, tmp, flMATRIX, flMATRIX + 0x840, flPS2VIEWPROJ);
    p = flPS2matMul(p + 32, tmp, flPS2CLIPPROJ);
    p = PS2SHADER_ADD_UVSCROLL(p + 48);
    *(M64 *)((u8 *)p + 0xf0) = flPS2VIEWPORT;
    TAIL(0x130)
}

void flPS2AddMatrix_0001(AMDL *m, u32 id, u32 n) {
    u32 *p;
    HEAD(0xf30, 0x200000f2, 0x6cf00000)
    p = flPS2matMul(p + 12, flMATRIX + 0x840, flPS2VIEWPROJ);
    p = flPS2matMul(p + 28, flMATRIX + 0x840, flPS2CLIPPROJ);
    F(0xb0) = -LF(0x34);
    F(0xb4) = -LF(0x38);
    F(0xb8) = -LF(0x3c);
    F(0xc0) = -LF(0x9c);
    F(0xc4) = -LF(0xa0);
    F(0xc8) = -LF(0xa4);
    F(0xd0) = -LF(0x104);
    F(0xd4) = -LF(0x108);
    F(0xd8) = -LF(0x10c);
    p = PS2SHADER_FLMATRIX_COPY(p + 72, *(u32 *)((u8 *)m + 0x54));
    p = PS2SHADER_ADD_LIGHTCOL3(AMB, p + 56);
    TAIL(0xf20)
}

void flPS2AddMatrix_000C(AMDL *m, u32 id, u32 n) {
    u32 *p;
    f32 tmp[16];
    HEAD(0x120, 0x20000011, 0x6C0F0000)
    FADE
    p = flPS2matMul2(PB(0x80), tmp, flMATRIX, flMATRIX + 0x840, flPS2VIEWPROJ);
    p = flPS2matNormalize33(tmp, tmp);
    *(M64 *)PB(0x40) = *(M64 *)tmp;
    p = flPS2matMul(PB(0xC0), tmp, flPS2CLIPPROJ);
    FOG(0x100)
    TAIL(0x110)
}

void flPS2AddMatrix_002C(AMDL *m, u32 id, u32 n) {
    u32 *p;
    f32 v[12];
    int i;
    u8 *mp, *sp2, *dp;
    HEAD(0xF70, 0x200000F6, 0x6CF40000)
    FOG(0x30)
    *(M64 *)PB(0x40) = *(M64 *)flPS2VIEWPROJ;
    *(M64 *)PB(0x80) = *(M64 *)flPS2CLIPPROJ;
    i = 0;
    mp = flMATRIX;
    sp2 = (u8 *)p;
    dp = (u8 *)p;
    for (; i < 32; i++) {
        if (m->flags & (1 << i))
            p = flPS2matMulNormalize33(dp + 0x960, sp2 + 0x160, mp, flMATRIX + 0x840);
        mp += 0x40;
        sp2 += 0x40;
        dp += 0x30;
    }
    v[3] = 1.0f;
    v[7] = 1.0f;
    v[11] = 1.0f;
    v[0] = -LF(0x34);
    v[1] = -LF(0x38);
    v[2] = -LF(0x3C);
    v[4] = LF(0xA8);
    v[5] = LF(0xAC);
    v[6] = LF(0xB0);
    v[8] = LF(0x110);
    v[9] = LF(0x114);
    v[10] = LF(0x118);
    p = PS2SHADER_ADD_LIGHTVECD1P2_2(PB(0xC0), PB(0xF0), flMATRIX + 0x840, v);
    p = PS2SHADER_ADD_LIGHTCOL3_2(AMB, PB(0x100));
    TAIL(0xF60)
}

void *flPS2AddMatrix_0028(AMDL *m, u32 id, u32 n) {
    u32 *p;
    f32 tmp[16];
    f32 v[4];
    HEAD(0x120, 0x20000011, 0x6C0F0000)
    flPS2matMul2(PB(0x30), tmp, flMATRIX, flMATRIX + 0x840, flPS2VIEWPROJ);
    flPS2matMul(PB(0x70), tmp, flPS2CLIPPROJ);
    v[3] = 1.0f;
    VP3(0, 0x40)
    PS2SHADER_ADD_LIGHTVECP1(PB(0xB0), PB(0xE0), flMATRIX, v);
    PS2SHADER_ADD_LIGHTCOL1(AMB, PB(0xF0), PB(0x100));
    TAIL(0x110)
    return p;
}

