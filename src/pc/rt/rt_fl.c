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
unsigned char rt_light_blk[3][0x68];       /* flLIGHT[0..2] as flSetRenderState(0x5A + i) last got them */

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
    case 0x67:   /* fade colour 0xAARRGGBB: eft05 builds (r << 16) | (g << 8) | b
                  * from its RGB bytes and eft14 passes 0xFF5F00 for fire orange.
                  * Alpha 0xFF = 1.0 (eft_trans_sub sends 255 * a). */
        gfx_set_render_state(GFX_RS_FADE_COLOR, value);
        break;
    case 0x04: { /* texture (mem_tex handle from flCreateTextureFromApx_mem, rt_2d.c) */
        void rt_2d_set_texture(u32 h);
        rt_2d_set_texture(value);
        break;
    }
    case 0x6D:   /* alpha test method (GS TEST 0x7000 bits): 3 normal, 7 set13 glare [not traced] */
        break;
    case 0x0F:   /* fog colour (GS FOGCOL); fog is only switched on by 0x12, which
                  * nothing calls yet: InitRenderState(0) sets 0x0F-0x11 */
        gfx_set_render_state(GFX_RS_FOG_COLOR, value & 0xFFFFFF);
        break;
    case 0x10:   /* fog start / end: the float's bits */
    case 0x11: {
        static float fog[2];
        memcpy(&fog[state - 0x10], &value, 4);
        gfx_set_render_state(state == 0x10 ? GFX_RS_FOG_START : GFX_RS_FOG_END, (uintptr_t)&fog[state - 0x10]);
        break;
    }
    case 0x5F:   /* Z test mode 0-7 -> RenderOperation (mode << 19) -> GS TEST
                  * (flSetRenderState's table lit_482, 0x35BE50). The game passes
                  * 4 (InitRenderState(0)): the normal depth test. Taken as a
                  * compare-function index whose last value (7) is "always"
                  * [guess]: 7 turns the host depth test off, the rest on. */
        gfx_set_render_state(GFX_RS_ZTEST, (value & 7) != 7);
        break;
    case 0x5A:   /* the three light blocks (0x68 bytes: Pl_light_set / light_set): the host reads them (rt_light.c) */
    case 0x5B:
    case 0x5C:
        if (value)
            memcpy(rt_light_blk[state - 0x5A], (const void *)(uintptr_t)value, 0x68);
        break;
    case 0x01:   /* shader kind / family / ambient: the host lights clays itself */
    case 0x0E:   /* (fl_model's VU1-style lighting) */
    case 0x15:
        break;
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
