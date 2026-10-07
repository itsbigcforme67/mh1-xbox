/*
 * fl.h - the port's version of Capcom's "fl" model layer, on top of the
 * format readers (src/pc/fmt) and the graphics interface (src/pc/gfx).
 *
 * fl_model   one AMO file turned into clays (one gfx_clay per AMO part),
 *            plus its textures and optional AHI skeleton. Like
 *            plAMOCreateClayFromImage + flCreateClayHandle.
 * fl_skel    an AHI skeleton with per-group motions (AAN) playing on it;
 *            produces bone world matrices (flGetMotionMatrix /
 *            flSetSkinTrans).
 * fl_light   the VU1 lighting model: 3 directional lights + ambient,
 *            clamped (Vu1Code_0001_0002, graphics.md 4.2), evaluated on
 *            the CPU per vertex.
 */
#ifndef MH_FL_H
#define MH_FL_H

#include "../fmt/fmt.h"
#include "../gfx/gfx.h"
#include "flmat.h"

typedef struct {
    float dir[3][3];        /* direction the light travels (normalised) */
    float col[3][3];        /* 0..1 */
    float ambient[3];
} fl_light;

typedef struct {
    gfx_clay *clay;
    int skinned;            /* has weights */
    int lit;                /* apply fl_light (else pre-lit vertex colours only) */
    float *skinpos;         /* work buffers for dynamic parts */
    uint8_t *skincol;
    int is_sky;             /* attribute +0x10 set: background part */
    int skip;               /* set by the caller: not drawn this frame, fl_model_pose leaves it */
    int gpu;                /* skinned and lit by the backend (gfx_clay_pose), not here */
    gfx_skin_mesh *check;   /* RT_SKIN_CHECK=1: the GPU data, compared with the CPU result */
} fl_part;

typedef struct {
    amo_model amo;
    ahi_skel skel;          /* bind skeleton of this file (may be empty) */
    int npart;
    fl_part *part;
    int ntex;
    gfx_texture **tex;
    flmat *invbind;         /* per bone: inverse(bind world) */
    /* material colour override (player_trans 0x167C38: the hair part's
     * first clay's first material gets PLW+0x5FC): has_tint set by the
     * caller; the vertices of part 0's first material are multiplied by
     * tint (0..1) when posed */
    int has_tint;
    float tint[3];
    uint8_t *tint_mask;     /* part 0: 1 = vertex used by its first material (built lazily) */
} fl_model;

/* amo: AMO bytes; ahi: AHI bytes or {NULL,0}; tex: *_tex.bin link file or
 * a bare APX or {NULL,0}. lit: use directional lighting for this model. */
int  fl_model_create(fl_model *m, fmt_blob amo, fmt_blob ahi, fmt_blob tex, int lit, int be);
void fl_model_release(fl_model *m);
/* Skin with bone world matrices (one per bone of m->skel; NULL = bind
 * pose) and light; uploads to the clays. */
void fl_model_pose(fl_model *m, const flmat *bone_world, const fl_light *light);
/* Draw all parts with the current WORLD matrix. sky: 1 = only sky parts,
 * 0 = only the rest, -1 = all. */
void fl_model_draw(fl_model *m, int sky);

/* ------------------------------------------------------------ skeleton */
#define FL_MAX_GROUPS 4

/* One AHI group's pose: motion m at frame t (no looping: the caller keeps
 * t in range, as frame_move does), optionally blended with m2 at t2:
 * channels = wa * m + wb * m2 (flBlendMotionEx; rotations take the short
 * way round). m NULL = bind pose. */
typedef struct {
    const aan_motion *m;
    float t;
    const aan_motion *m2;
    float t2, wa, wb;
} fl_group_pose;

typedef struct {
    ahi_skel skel;
    aan_motion mot[FL_MAX_GROUPS];   /* per AHI group; nbone 0 = none */
    int has_mot[FL_MAX_GROUPS];
    float frame;
    float end;                       /* longest motion end frame */
    int root_lock;                   /* fl_skel_pose_groups: 1 = keep the X/Z
                                        translation of the root motion bone
                                        (AAN bone 1 of group 0) at its bind
                                        value; the game moves the actor by it */
    float (*chan)[9];                /* per bone: current channels */
    flmat *world;                    /* per bone: world matrices */
    /* fl_skel_pose_groups' last inputs: the same pose asked again (the
     * host asks up to three times per tick) is not evaluated again */
    fl_group_pose last[FL_MAX_GROUPS];
    int last_ok, last_lock;
} fl_skel;

int  fl_skel_create(fl_skel *s, fmt_blob ahi, int be);
void fl_skel_release(fl_skel *s);
/* Motion id as in frame_init (0x125920): bank = (id % 1000) / 100,
 * slot = id % 100, from the given *_tbl.bin. Returns 0 if found. */
int  fl_skel_set_motion(fl_skel *s, int group, fmt_blob tbl, int id, int be);
/* Evaluate at frame t (loops on end) and compute world matrices. */
void fl_skel_update(fl_skel *s, float t);

void fl_skel_pose_groups(fl_skel *s, const fl_group_pose g[FL_MAX_GROUPS]);

#endif
