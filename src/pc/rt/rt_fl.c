/*
 * rt_fl.c - Capcom "fl" library calls used by decompiled game C, routed to
 * the port's gfx interface (src/pc/gfx/gfx.h).
 *
 * flSetRenderState state numbers (graphics.md section 5): 0x0D blend
 * operation, 0x19 texture matrix (UV scroll), 0x1A world matrix, 0x5E blend
 * factors, 0x60 alpha reference, 0x63 texture filter, 0x64 texture clamp,
 * 0x67 fade colour, 0x6C z-write. Values that are matrices are passed by the game as
 * pointers cast to u32, which only works in the 32-bit build.
 */
#include "rt.h"
#include "types.h"
#include "fl.h"
#include "clay.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

gfx_clay *rt_clay(int handle);

/* Defaults the host draw uses (viewer.c); restored after every prim. */
#define RT_ALPHA_REF 0x40

void rt_fl_reset_states(void)
{
    gfx_set_render_state(GFX_RS_TEXMAT, 0);
    gfx_set_render_state(GFX_RS_ALPHA_REF, RT_ALPHA_REF);
    gfx_set_render_state(GFX_RS_FADE_COLOR, 0xFFFFFFFFu);
    gfx_set_render_state(GFX_RS_ZWRITE, 1);
    gfx_set_render_state(GFX_RS_BLEND, 1);       /* = SetTrnslMode(4, 5) */
    gfx_set_render_state(GFX_RS_BLEND_OP, 0);
    gfx_set_render_state(GFX_RS_FILTER, 0);
    gfx_set_render_state(GFX_RS_TEX_CLAMP, 0);
}

void flSetRenderState(int state, u32 value)
{
    static unsigned char warned[0x100];
    switch (state) {
    case 0x0D:   /* blend operation (RenderOperation 0xC00) */
    case 0x19:   /* texture matrix */
    case 0x1A:   /* world matrix */
    case 0x5E:   /* blend factors (RenderOperation 0xFF) */
    case 0x63:   /* texture filter (RenderOperation 0x10000) */
    case 0x64:   /* texture clamp (RenderOperation 0x60000) */
    case 0x6C:   /* z-write */
        gfx_set_render_state(state, (uintptr_t)value);
        break;
    case 0x60:   /* alpha reference */
        gfx_set_render_state(GFX_RS_ALPHA_REF, value & 0xFF);
        break;
    case 0x67: { /* fade colour: 0xAABBGGRR on the GS? the game builds (a << 24) | 0xFFFFFF */
        u32 a = value >> 24, b = (value >> 16) & 0xFF, g = (value >> 8) & 0xFF, r = value & 0xFF;
        /* GS colours are R in the low byte; gfx wants 0xAARRGGBB. PS2 alpha
         * 0x80 means 1.0, but set14 passes up to 0xFF, so it is taken as-is. */
        gfx_set_render_state(GFX_RS_FADE_COLOR, a << 24 | r << 16 | g << 8 | b);
        break;
    }
    default:
        if (state >= 0 && state < 0x100 && !warned[state]) {
            warned[state] = 1;
            fprintf(stderr, "rt: flSetRenderState 0x%X not mapped yet\n", state);
        }
        break;
    }
}

void flmatMakeTrans(FLMAT *m, f32 x, f32 y, f32 z)
{
    memset(m, 0, sizeof *m);
    (*m)[0][0] = (*m)[1][1] = (*m)[2][2] = (*m)[3][3] = 1.0f;
    (*m)[3][0] = x;
    (*m)[3][1] = y;
    (*m)[3][2] = z;
}

f32 flFloor(f32 x)
{
    return floorf(x);
}

void flExecuteClay(s32 handle, int flag)
{
    gfx_clay *c = rt_clay(handle);
    (void)flag;
    if (c)
        gfx_execute_clay(c);
}

/* ------------------------------------------------------------ clay attributes */
/* Main-program helpers (f_font 0x161470..): blend mode, blend operation and
 * texture filter. The PS2 versions skip the call when the mode is already
 * current (system_w+0x2C..0x30); the runtime resets states after every
 * prim, so it always sends. Tables are read from the ELF (rt_data.c). */
extern u32 src_mode_00300620[12], dst_mode_00300650[10], ope_mode_00300678[4];
extern u32 filter_mode_00387900[2];
extern u32 aa_alpha_src[10], aa_alpha_ope[4], aa_filt[2], aa_addr[4];

void SetTrnslMode(int src, int dst)
{
    flSetRenderState(0x5E, src_mode_00300620[src] | dst_mode_00300650[dst]);
}

void SetOpeMode(int ope)
{
    flSetRenderState(0x0D, ope_mode_00300678[ope]);
}

void SetFilterMode(int mode)
{
    flSetRenderState(0x63, filter_mode_00387900[mode]);
}

/* clay_attr_set (0x121E20): the CLAY+0x88 word packed by Attribute_from_amo. */
void clay_attr_set(s32 attr)
{
    if (!(attr & 1))
        return;
    SetTrnslMode(aa_alpha_src[(attr >> 2) & 0xF], aa_alpha_src[(attr >> 6) & 0xF]);
    SetOpeMode(aa_alpha_ope[(attr >> 10) & 3]);
    SetFilterMode(aa_filt[(attr >> 12) & 1]);
    flSetRenderState(0x64, aa_addr[(attr >> 13) & 3]);
}

/* clay_attr_reset (0x121EE0) */
void clay_attr_reset(void)
{
    SetTrnslMode(4, 5);
    SetOpeMode(0);
    SetFilterMode(1);
}

/* The attribute word half of Attribute_from_amo (0x121BD0): packs the AMO
 * 0xF0000 chunk's words +0x24 (fade bit), +0x2C/+0x30 (blend src/dst),
 * +0x34 (operation), +0x40 (filter), +0x44 (clamp) into CLAY+0x88. */
uint32_t rt_clay_attr_word(const int32_t *a)
{
    if (!a)
        return 0;
    return 1u | (a[0x24 / 4] & 1u) << 1 | (a[0x2C / 4] & 0xFu) << 2 | (a[0x30 / 4] & 0xFu) << 6
         | (a[0x34 / 4] & 3u) << 10 | (a[0x40 / 4] & 1u) << 12 | (a[0x44 / 4] & 3u) << 13;
}

void rt_clay_attr_set(uint32_t attr) { clay_attr_set((s32)attr); }
void rt_clay_attr_reset(void) { clay_attr_reset(); }
