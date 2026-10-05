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
} fl_part;

typedef struct {
    amo_model amo;
    ahi_skel skel;          /* bind skeleton of this file (may be empty) */
    int npart;
    fl_part *part;
    int ntex;
    gfx_texture **tex;
    flmat *invbind;         /* per bone: inverse(bind world) */
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

typedef struct {
    ahi_skel skel;
    aan_motion mot[FL_MAX_GROUPS];   /* per AHI group; nbone 0 = none */
    int has_mot[FL_MAX_GROUPS];
    float frame;
    float end;                       /* longest motion end frame */
    float (*chan)[9];                /* per bone: current channels */
    flmat *world;                    /* per bone: world matrices */
} fl_skel;

int  fl_skel_create(fl_skel *s, fmt_blob ahi, int be);
void fl_skel_release(fl_skel *s);
/* Motion id as in frame_init (0x125920): bank = (id % 1000) / 100,
 * slot = id % 100, from the given *_tbl.bin. Returns 0 if found. */
int  fl_skel_set_motion(fl_skel *s, int group, fmt_blob tbl, int id, int be);
/* Evaluate at frame t (loops on end) and compute world matrices. */
void fl_skel_update(fl_skel *s, float t);

#endif
