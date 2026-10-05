/*
 * rt_fl.c - Capcom "fl" library calls used by decompiled game C, routed to
 * the port's gfx interface (src/pc/gfx/gfx.h).
 *
 * flSetRenderState state numbers (graphics.md section 5): 0x19 texture
 * matrix (UV scroll), 0x1A world matrix, 0x60 alpha reference, 0x67 fade
 * colour, 0x6C z-write. Values that are matrices are passed by the game as
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
}

void flSetRenderState(int state, u32 value)
{
    static unsigned char warned[0x100];
    switch (state) {
    case 0x19:   /* texture matrix */
    case 0x1A:   /* world matrix */
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

/* Not mapped yet: the clay attribute chunk (0xF0000: cull, blend, fog per
 * part, stage.md) and the texture filter mode. */
void clay_attr_set(s32 attr) { (void)attr; }
void clay_attr_reset(void) { }
void SetFilterMode(int mode) { (void)mode; }
