/* spriteput_nm - SLPM_654.95 0x0015A6D0-0x0015AED0 (f_calcpoint, after
 * CalcPoint): SpritePut, queues one sprite request into sprite_area
 * (0x50 bytes per entry, at most 0x100, spr_list_no) for trans_sprite
 * (sprite/trans2.c), which draws entry kind s[0] & 3 with flps0D00 (flat,
 * screen), flps1300 (flat, 3D), flps0F00 (textured, screen) or flps1400 /
 * flps1600 (textured, 3D; 1600 = two rotated triangles).
 *
 * Written from the asm (asm/main/text/SpritePut.s) for the PC port; not in
 * c_files.txt, not compared with check.py. Field names are descriptive
 * guesses; offsets are from the asm.
 *
 * Request (SPRQ):
 *   +0x00 texture number (mem_tex index)    +0x04 x, +0x08 y, +0x0C z
 *   +0x10 w, +0x14 h, +0x18 x scale, +0x1C y scale
 *   +0x20 u0, +0x24 v0, +0x28 u1, +0x2C v1   +0x30 alpha 0..1
 *   +0x34 kind: bits 0-1 prim kind (bit 0 = 3D: sorted, y up), bit 2
 *         rotated (CalcPoint by +0x40..+0x48 degrees)
 *   +0x38 flags: bits 0-1 x anchor (0/1 left, 2 centre, 3 right), bits 2-3
 *         y anchor, 0x10 flip x, 0x20 flip y, 0x1000 filter, 0x2000 rotate
 *         in view space (sets kind bit 2, angles 0), 0x4000 additive
 *   +0x3C colour 0x..RRGGBB (alpha from +0x30)
 * Entry (sprite_area): +0 kind | 8 filter, +1 additive, +2 u16 texture,
 *   +4 f32 z (sort key), +8.. the prim's packet. */
#include "types.h"

typedef struct SPRQ {
    s32 tex;
    f32 x, y, z;
    f32 w, h, sx, sy;
    f32 u0, v0, u1, v1;
    f32 alpha;
    s32 kind;
    u32 flags;
    u32 col;
    f32 ang[3];
} SPRQ;

extern u8 *sprite_area;
extern s16 spr_list_no;

f32 flFloor(f32);
void CalcPoint(f32 *pts, f32 *center, int n, void *d);
void *memcpy(void *, const void *, unsigned int);

#define SF(o) (*(f32 *)(s + (o)))
#define SU(o) (*(u32 *)(s + (o)))

void SpritePut(SPRQ *sp) {
    u8 *s;
    f32 sign;
    f32 x0, y0, x1, y1;     /* first corner (sp+0xE0..), second (sp+0xD0..) */
    f32 ua, ub, va, vb;
    u32 flags, col;
    f32 a;
    f32 p[4][3];
    f32 c[3];
    int n = spr_list_no;

    if (n == 0x100)
        return;
    s = sprite_area + n * 0x50;
    *(u16 *)(s + 2) = (u16)sp->tex;
    SF(4) = sp->z;
    s[0] = sp->kind & 0xF;
    if (sp->flags & 0x1000)
        s[0] |= 8;
    s[1] = (sp->flags & 0x4000) ? 1 : 0;
    sign = (s[0] & 1) ? -1.0f : 1.0f;
    flags = sp->flags;

    switch (flags & 3) {
    case 0:
    case 1:
        if (flags & 0x10) {
            x1 = sp->x;
            x0 = sp->x - sp->w * sp->sx;
        } else {
            x0 = sp->x;
            x1 = sp->x + sp->w * sp->sx;
        }
        break;
    case 2:
        x0 = sp->x - sp->w * sp->sx / 2.0f;
        x1 = x0 + sp->w * sp->sx;
        break;
    case 3:
        if (flags & 0x10) {
            x0 = sp->x;
            x1 = sp->x + sp->w * sp->sx;
        } else {
            x1 = sp->x;
            x0 = sp->x - sp->w * sp->sx;
        }
        break;
    }
    switch ((flags & 0xC) >> 2) {
    case 0:
    case 1:
        if (flags & 0x20) {
            y1 = sp->y;
            y0 = sp->y + sign * (-1.0f * (sp->h * sp->sy));
        } else {
            y0 = sp->y;
            y1 = sp->y + sign * (sp->h * sp->sy);
        }
        break;
    case 2:
        y0 = sp->y + sign * (-1.0f * (sp->h * sp->sy / 2.0f));
        y1 = y0 + sign * (sp->h * sp->sy);
        break;
    case 3:
        if (flags & 0x20) {
            y0 = sp->y;
            y1 = sp->y + sign * (sp->h * sp->sy);
        } else {
            y1 = sp->y;
            y0 = sp->y + sign * (-1.0f * (sp->h * sp->sy));
        }
        break;
    }
    a = 255.0f * sp->alpha;
    col = ((u32)a << 24) | (sp->col & 0xFFFFFF);

    switch (s[0] & 3) {
    case 0:     /* flps0D00: two screen corners {x, y, z, 1}, colour */
        SF(8) = flFloor(x0);
        SF(0xC) = flFloor(y0);
        SF(0x10) = sp->z;
        SF(0x14) = 1.0f;
        SF(0x18) = flFloor(x1);
        SF(0x1C) = flFloor(y1);
        SF(0x20) = sp->z;
        SF(0x24) = 1.0f;
        SU(0x28) = col;
        break;
    case 1:     /* flps1300: two corners {x, y, z}, colour */
        SF(8) = x0;
        SF(0xC) = y0;
        SF(0x10) = sp->z;
        SF(0x14) = x1;
        SF(0x18) = y1;
        SF(0x1C) = sp->z;
        SU(0x20) = col;
        break;
    case 2:     /* flps0F00: as kind 0, then the UVs */
        SF(8) = flFloor(x0);
        SF(0xC) = flFloor(y0);
        SF(0x10) = sp->z;
        SF(0x14) = 1.0f;
        SF(0x18) = flFloor(x1);
        SF(0x1C) = flFloor(y1);
        SF(0x20) = sp->z;
        SF(0x24) = 1.0f;
        SU(0x28) = col;
        SF(0x2C) = (flags & 0x10) ? sp->u1 : sp->u0;
        SF(0x30) = (flags & 0x20) ? sp->v1 : sp->v0;
        SF(0x34) = (flags & 0x10) ? sp->u0 : sp->u1;
        SF(0x38) = (flags & 0x20) ? sp->v0 : sp->v1;
        break;
    case 3:     /* flps1400 / flps1600 (rotated: two triangles) */
        if (flags & 0x10) {
            ub = sp->u0;
            ua = sp->u1;
        } else {
            ua = sp->u0;
            ub = sp->u1;
        }
        if (flags & 0x20) {
            vb = sp->v0;
            va = sp->v1;
        } else {
            va = sp->v0;
            vb = sp->v1;
        }
        if ((flags & 0x2000) && !(sp->kind & 4)) {
            sp->kind |= 4;
            sp->ang[2] = 0.0f;
            sp->ang[1] = 0.0f;
            sp->ang[0] = 0.0f;
        }
        if (sp->kind & 4) {
            c[0] = sp->x;
            c[1] = sp->y;
            c[2] = sp->z;
            p[0][0] = x0; p[0][1] = y0; p[0][2] = sp->z;
            p[1][0] = x1; p[1][1] = y0; p[1][2] = sp->z;
            p[2][0] = x0; p[2][1] = y1; p[2][2] = sp->z;
            p[3][0] = x1; p[3][1] = y1; p[3][2] = sp->z;
            CalcPoint(&p[0][0], c, 4, sp);
            SF(8) = p[0][0]; SF(0xC) = p[0][1]; SF(0x10) = p[0][2];
            SF(0x14) = p[1][0]; SF(0x18) = p[1][1]; SF(0x1C) = p[1][2];
            SF(0x20) = p[2][0]; SF(0x24) = p[2][1]; SF(0x28) = p[2][2];
            SU(0x2C) = col;
            SU(0x30) = col;
            SU(0x34) = col;
            SF(0x38) = ua; SF(0x3C) = va;
            SF(0x40) = ub; SF(0x44) = va;
            SF(0x48) = ua; SF(0x4C) = vb;
            if (spr_list_no != 0xFF) {
                memcpy(s + 0x50, s, 0x50);
                spr_list_no++;
                SF(0x58) = p[1][0]; SF(0x5C) = p[1][1]; SF(0x60) = p[1][2];
                SF(0x64) = p[2][0]; SF(0x68) = p[2][1]; SF(0x6C) = p[2][2];
                SF(0x70) = p[3][0]; SF(0x74) = p[3][1]; SF(0x78) = p[3][2];
                SF(0x88) = ub; SF(0x8C) = va;
                SF(0x90) = ua; SF(0x94) = vb;
                SF(0x98) = ub; SF(0x9C) = vb;
            }
        } else {
            SF(8) = x0;
            SF(0xC) = y0;
            SF(0x10) = sp->z;
            SF(0x14) = x1;
            SF(0x18) = y1;
            SF(0x1C) = sp->z;
            SU(0x20) = col;
            SF(0x24) = ua;
            SF(0x28) = va;
            SF(0x2C) = ub;
            SF(0x30) = vb;
        }
        break;
    }
    spr_list_no++;
}
