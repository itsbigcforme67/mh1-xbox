/*
 * rt_light.c - the game's stage lights for the host's CPU lighting (docs/pc.md "Lighting").
 *
 * The PS2 keeps two sets of three lights in light_work (set 0 for the stage model, set 1 for hunters, monsters, NPCs, effects).
 * Every actor that draws itself calls pl_light_change (its colour rows: the stage rows, the near-monster rows on stages 12/13/14/28/30,
 * the actor's own ground table for light 0 and 1) and Pl_light_set (eases the colour toward the colour it had last time, hands the
 * three blocks to flSetRenderState(0x5A..0x5C)), and light_change_normal puts the stage rows back afterwards. The host draws the models
 * itself, so this file runs the same game C per actor right before the host poses it and reads the three blocks back
 * (rt_fl.c keeps what flSetRenderState(0x5A + i) got in rt_light_blk).
 * Only used with RT_LIGHT_GAME=1 (viewer.c); the default is still the fixed light.
 */
#include "rt.h"
#include "types.h"
#include "game.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern GAME_W game_w;
extern unsigned char light_work[];
extern unsigned char rt_light_blk[3][0x68];
extern u8 *ground_tbl_add[];
extern u8 *diffuse_tbl_add[];
extern u8 *shadow_tbl_add[];
extern u8 *light_tbl_add[];

void light_change_normal(int n);
void pl_light_change(void *actor, int n);
void Pl_light_set(void *actor);

/* GetPlayerDiffuseData (groundmat_nm.c; the PC stands GetPlayerMaterialData in for a no-op): the ground area the hunter stands on may
 * pick a 32-byte pair of colour rows (the actor's own light rows, +0x710 of the work) by distance to a sphere of the stage. */
static void player_diffuse(u8 *pl)
{
    u8 **out = (u8 **)(pl + 0x710);
    int stg = game_w.stage;
    u8 *gt = ground_tbl_add[stg];
    u8 *diff = diffuse_tbl_add[stg];
    u8 *rec, *area = NULL;
    float d, pos[3];
    int shadow, light;

    *out = 0;
    if (!gt || !diff)
        return;
    rec = gt + pl[0x70C] * 16;
    shadow = rec[2];
    light = rec[3];
    if (shadow && shadow_tbl_add[stg])
        area = shadow_tbl_add[stg] + shadow * 24;
    else if (light && light_tbl_add[stg])
        area = light_tbl_add[stg] + light * 24;
    if (!area)
        return;
    memcpy(pos, pl + 0xAC, sizeof pos);
    {
        float a[3];
        memcpy(a, area, sizeof a);
        d = sqrtf((a[0] - pos[0]) * (a[0] - pos[0]) + (a[1] - pos[1]) * (a[1] - pos[1]) + (a[2] - pos[2]) * (a[2] - pos[2]));
    }
    {
        float r_in, r_out;
        unsigned short set_in, set_out;
        memcpy(&r_in, area + 0xC, 4);
        memcpy(&r_out, area + 0x10, 4);
        memcpy(&set_in, area + 0x14, 2);
        memcpy(&set_out, area + 0x16, 2);
        if (d <= r_out)
            *out = diff + (d <= r_in ? set_in : set_out) * 32;
    }
}

/* the three blocks (0x68 bytes each) -> direction the light travels, colour, ambient (sum of the +0x24 rows) */
static int blocks_out(const unsigned char *b, float dir[3][3], float col[3][3], float amb[3])
{
    int i, k, any = 0;
    amb[0] = amb[1] = amb[2] = 0;
    for (i = 0; i < 3; i++, b += 0x68) {
        float d[3], len;
        memcpy(d, b + 0x34, sizeof d);
        len = sqrtf(d[0] * d[0] + d[1] * d[1] + d[2] * d[2]);
        for (k = 0; k < 3; k++) {
            float c, a;
            memcpy(&c, b + 4 + 4 * k, 4);
            memcpy(&a, b + 0x24 + 4 * k, 4);
            dir[i][k] = len > 1e-6f ? d[k] / len : 0.0f;
            col[i][k] = len > 1e-6f ? c : 0.0f;         /* an unused light has no direction row */
            amb[k] += a;
            any |= c != 0.0f || a != 0.0f;
        }
    }
    if (any && getenv("RT_LIGHT_VU")) {
        /* the literal Vu1Code_0001 ambient (docs/pc.md "Lighting, material factors"): PS2SHADER_ADD_LIGHTCOL3 makes mem15 =
         * min(sum of the c rows * AMB, AMB) with AMB = the model's mat+0x24 (0.5 in every model checked), the VU multiplies it by the
         * material's ambient qword (0.5 everywhere) and adds it to a colour whose 1.0 is 128: practically zero. The diffuse
         * qword (material colour B) is 1.0 everywhere, so the light colours need no factor. A guess whether the actors use this family. */
        for (k = 0; k < 3; k++) {
            float m = amb[k] * 0.5f;
            amb[k] = (m > 0.5f ? 0.5f : m) * 0.5f / 128.0f;
        }
    }
    return any;
}

/* actor == NULL: set 1 as the stage rows leave it (monsters, set objects: nothing in the game gives them their own rows).
 * actor != NULL: that hunter (hunter != 0: its ground table first) or NPC work area through pl_light_change + Pl_light_set.
 * Returns 0 while light_work is empty. */
int rt_light_get(void *actor, int hunter, float dir[3][3], float col[3][3], float amb[3])
{
    int r;
    if (getenv("RT_LIGHT_TRACE") && light_work[0x151]) { float a; memcpy(&a, light_work + 0x158 + 0x24, 4); fprintf(stderr, "flash state %d,%d,%d amb0 %.2f\n", light_work[0x150], light_work[0x151], light_work[0x152], a); }
    light_change_normal(1);         /* what the end of every actor's draw does */
    if (!actor)
        return blocks_out(light_work + 0x158, dir, col, amb);
    if (hunter)
        player_diffuse(actor);
    pl_light_change(actor, 1);
    Pl_light_set(actor);
    r = blocks_out(&rt_light_blk[0][0], dir, col, amb);
    light_change_normal(1);
    return r;
}

/* test aid RT_LIGHT_FLASH=N: start the game's flash_move effect on both light sets every N ticks (nothing in the game does:
 * light_work[0x11] / [0x151] is never written; docs/pc.md) */
void rt_light_tick(void)
{
    static int env = -1, t;
    if (env < 0)
        env = getenv("RT_LIGHT_FLASH") ? atoi(getenv("RT_LIGHT_FLASH")) : 0;
    if (env > 0 && ++t % env == 0) {
        int s;
        for (s = 0; s < 2; s++) {
            light_work[s * 0x140 + 0x10] = 0;       /* flash_move state 0 = start */
            light_work[s * 0x140 + 0x11] = 1;       /* light_move runs flash_move */
        }
        if (getenv("RT_LIGHT_TRACE")) fprintf(stderr, "light flash started (tick %d)\n", t);
    }
}
