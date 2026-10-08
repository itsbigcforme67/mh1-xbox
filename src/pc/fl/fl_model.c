/*
 * fl_model.c - AMO model -> clays, CPU skinning and VU1-style lighting.
 *
 * PS2 path (graphics.md 1, 3, 4): ConvertModelMeshAMO_* builds an MLCLAY
 * per part, flCreateClayHandle/flPS2ConvClayData turn it into a VIF1 DMA
 * chain, and the VU1 program transforms, skins and lights each vertex.
 * Here the strips become a triangle list grouped by material (one
 * gfx_batch per texture) and skinning/lighting run on the CPU.
 */
#include "fl.h"
#include "../rt/rt_prof.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* part 0's first material's vertices (player_trans' hair colour) */
static uint8_t *first_material_mask(const amo_part *p)
{
    uint8_t *mask;
    int s, k, mat0;
    if (p->nstrip <= 0)
        return NULL;
    mask = calloc((size_t)p->nvert + 1, 1);
    mat0 = p->strip[0].material;
    for (s = 0; s < p->nstrip; s++)
        if (p->strip[s].material == mat0)
            for (k = 0; k < p->strip[s].count; k++)
                if (p->index[p->strip[s].first + k] < p->nvert)
                    mask[p->index[p->strip[s].first + k]] = 1;
    return mask;
}

/* RT_SKIN_CHECK=1: the vertex program's C model against the CPU skinning */
static double chk_pos, chk_col;
static long chk_n, chk_parts, chk_v0, chk_v1, chk_batches, chk_mb0;
static void chk_report(void)
{
    fprintf(stderr, "skin check: %ld vertices, worst position error %.6f (relative), worst colour %.2f\n",
            chk_n, chk_pos, chk_col);
    fprintf(stderr, "skin check: %ld parts built: %ld vertices -> %ld in the GPU batches, %ld batches (%ld material batches)\n",
            chk_parts, chk_v0, chk_v1, chk_batches, chk_mb0);
}

static void build_part(fl_model *m, int pi)
{
    amo_part *p = &m->amo.part[pi];
    fl_part *fp = &m->part[pi];
    int nmat = m->amo.nmat + 1, s, k, ntri = 0, mi;
    int *cnt = calloc(nmat, sizeof(int));
    uint16_t *idx;
    gfx_batch *batch = calloc(nmat, sizeof(gfx_batch));
    int nbatch = 0, pos = 0;
    uint8_t *col = malloc(4 * (size_t)(p->nvert + 1));
    gfx_clay_desc d;

    /* count triangles per material (slot nmat-1 = no material) */
    for (s = 0; s < p->nstrip; s++) {
        int mat = p->strip[s].material;
        if (mat < 0 || mat >= m->amo.nmat)
            mat = nmat - 1;
        if (p->strip[s].count >= 3)
            cnt[mat] += p->strip[s].count - 2;
    }
    for (k = 0; k < nmat; k++)
        ntri += cnt[k];
    idx = malloc(sizeof(uint16_t) * 3 * (ntri + 1));

    /* strips -> triangles, alternating winding, degenerates dropped */
    for (mi = 0; mi < nmat; mi++) {
        int start = pos;
        if (!cnt[mi])
            continue;
        for (s = 0; s < p->nstrip; s++) {
            const amo_strip *st = &p->strip[s];
            int mat = st->material;
            if (mat < 0 || mat >= m->amo.nmat)
                mat = nmat - 1;
            if (mat != mi)
                continue;
            for (k = 0; k + 2 < st->count; k++) {
                uint16_t a = p->index[st->first + k], b = p->index[st->first + k + 1];
                uint16_t c = p->index[st->first + k + 2];
                if (k & 1) {
                    uint16_t t = a;
                    a = b;
                    b = t;
                }
                if (a == b || b == c || a == c || a >= p->nvert || b >= p->nvert || c >= p->nvert)
                    continue;
                idx[pos++] = a;
                idx[pos++] = b;
                idx[pos++] = c;
            }
        }
        batch[nbatch].first = start;
        batch[nbatch].count = pos - start;
        batch[nbatch].tex = NULL;
        if (mi < m->amo.nmat) {
            int apx = m->amo.mat[mi].apx;
            if (m->ntex == 1 && m->amo.mat[mi].has_tex)
                apx = 0;                        /* bare APX: one texture for all */
            if (apx >= 0 && apx < m->ntex)
                batch[nbatch].tex = m->tex[apx];
        }
        nbatch++;
    }

    for (k = 0; k < p->nvert; k++) {
        int c;
        for (c = 0; c < 4; c++) {
            float v = p->col ? p->col[4 * k + c] : 255.0f;
            col[4 * k + c] = (uint8_t)(v < 0 ? 0 : v > 255 ? 255 : v);
        }
    }

    fp->skinned = p->infl_n != NULL;
    fp->is_sky = p->has_attr && p->attr[4] != 0;   /* attr +0x10 (stage.md 2) */
    memset(&d, 0, sizeof d);
    d.nvert = p->nvert;
    d.pos = p->pos;
    d.st = p->st;
    d.col = col;
    d.nindex = pos;
    d.index = idx;
    d.nbatch = nbatch;
    d.batch = batch;
    d.dynamic = fp->skinned || fp->lit;
    d.noscroll = p->has_attr && p->attr[7] == 0;     /* attr +0x1C: the UV-scroll type (aa_uvscroll) */
    fp->clay = gfx_create_clay(&d);
    if (d.dynamic) {
        static int check = -1;
        gfx_skin_desc sd;
        uint8_t *tmask = pi == 0 ? first_material_mask(p) : NULL;
        memset(&sd, 0, sizeof sd);
        sd.nvert = p->nvert;
        sd.nrm = p->nrm;
        sd.infl_n = p->infl_n;
        sd.infl_bone = p->infl_bone;
        sd.infl_w = p->infl_w;
        sd.nbone = m->skel.nbone;
        sd.skinned = fp->skinned && m->skel.nbone > 0;
        sd.tint_mask = tmask;
        if (check < 0) {
            check = getenv("RT_SKIN_CHECK") != NULL;
            if (check)
                atexit(chk_report);
        }
        if (gfx_skin_capable() && gfx_clay_set_skin(fp->clay, &sd) == 0) {
            fp->gpu = 1;
        } else if (check) {
            fp->check = calloc(1, sizeof *fp->check);
            if (fp->check && gfx_skin_build(fp->check, &d, &sd) != 0) {
                free(fp->check);
                fp->check = NULL;
            } else if (fp->check) {
                chk_parts++;
                chk_v0 += d.nvert;
                chk_v1 += fp->check->nv;
                chk_batches += fp->check->nbatch;
                chk_mb0 += d.nbatch;
            }
        }
        free(tmask);
    }
    if (d.dynamic && !fp->gpu) {
        fp->skinpos = malloc(sizeof(float) * 3 * (p->nvert + 1));
        fp->skincol = malloc(4 * (size_t)(p->nvert + 1));
    }
    free(col);
    free(idx);
    free(batch);
    free(cnt);
}

static void bone_world(const ahi_skel *s, const float (*chan)[9], flmat *out)
{
    int i, done = 0, guard = 0;
    char *ok = calloc(s->nbone + 1, 1);
    while (done < s->nbone && guard++ < s->nbone + 2) {
        for (i = 0; i < s->nbone; i++) {
            int par = s->bone[i].parent;
            flmat loc;
            if (ok[i] || (par >= 0 && par < s->nbone && !ok[par]))
                continue;
            flmat_srt(loc, chan[i], chan[i] + 3, chan[i] + 6);
            if (par >= 0 && par < s->nbone)
                flmat_mul(out[i], loc, out[par]);
            else
                memcpy(out[i], loc, sizeof(flmat));
            ok[i] = 1;
            done++;
        }
    }
    free(ok);
}

static void bind_channels(const ahi_skel *s, float (*chan)[9])
{
    int i;
    for (i = 0; i < s->nbone; i++) {
        memcpy(chan[i], s->bone[i].s, 3 * sizeof(float));
        memcpy(chan[i] + 3, s->bone[i].r, 3 * sizeof(float));
        memcpy(chan[i] + 6, s->bone[i].t, 3 * sizeof(float));
    }
}

int fl_model_create(fl_model *m, fmt_blob amo, fmt_blob ahi, fmt_blob tex, int lit, int be)
{
    int i;
    memset(m, 0, sizeof *m);
    if (fmt_amo_load(&m->amo, amo, be) != 0)
        return -1;
    if (ahi.p && fmt_ahi_load(&m->skel, ahi, be) == 0) {
        float (*chan)[9] = calloc(m->skel.nbone, sizeof *chan);
        flmat *w = calloc(m->skel.nbone, sizeof(flmat));
        m->invbind = calloc(m->skel.nbone, sizeof(flmat));
        bind_channels(&m->skel, chan);
        bone_world(&m->skel, (const float (*)[9])chan, w);
        for (i = 0; i < m->skel.nbone; i++)
            flmat_invert_affine(m->invbind[i], w[i]);
        free(chan);
        free(w);
    }
    if (tex.p) {
        if (fmt_apx_is_bare(tex, be)) {
            m->ntex = 1;
            m->tex = calloc(1, sizeof(gfx_texture *));
            {
                apx_image img;
                if (fmt_apx_decode(&img, tex, be) == 0) {
                    gfx_tex_src_hint = img.src_bytes;
                    m->tex[0] = gfx_create_texture(img.w, img.h, img.rgba);
                    free(img.rgba);
                }
            }
        } else {
            m->ntex = fmt_link_count(tex, be);
            m->tex = calloc(m->ntex + 1, sizeof(gfx_texture *));
            for (i = 0; i < m->ntex; i++) {
                apx_image img;
                if (fmt_apx_decode(&img, fmt_link_entry(tex, i, be), be) == 0) {
                    gfx_tex_src_hint = img.src_bytes;
                    m->tex[i] = gfx_create_texture(img.w, img.h, img.rgba);
                    free(img.rgba);
                }
            }
        }
    }
    if (lit && getenv("RT_LIGHT_TRACE") && atoi(getenv("RT_LIGHT_TRACE")) >= 4) {
        for (i = 0; i < m->amo.npart; i++)
            fprintf(stderr, "lit model part %d: shader family %d, specular %d, lighting type %d\n", i, m->amo.part[i].attr[1], m->amo.part[i].attr[2],
                    m->amo.part[i].attr[5]);
    }
    if (lit && getenv("RT_LIGHT_TRACE") && atoi(getenv("RT_LIGHT_TRACE")) >= 3) {
        for (i = 0; i < m->amo.nmat; i++) {
            const amo_material *q = &m->amo.mat[i];
            fprintf(stderr, "material %d: B(diffuse) %.2f %.2f %.2f %.2f  A(ambient) %.2f %.2f %.2f %.2f\n", i, q->col_b[0], q->col_b[1], q->col_b[2],
                    q->col_b[3], q->col_a[0], q->col_a[1], q->col_a[2], q->col_a[3]);
        }
    }
    m->npart = m->amo.npart;
    m->part = calloc(m->npart + 1, sizeof(fl_part));
    for (i = 0; i < m->npart; i++) {
        m->part[i].lit = lit;
        build_part(m, i);
    }
    return 0;
}

void fl_model_release(fl_model *m)
{
    int i;
    for (i = 0; i < m->npart; i++) {
        if (m->part[i].check) {
            gfx_skin_free(m->part[i].check);
            free(m->part[i].check);
        }
        gfx_release_clay(m->part[i].clay);
        free(m->part[i].skinpos);
        free(m->part[i].skincol);
    }
    for (i = 0; i < m->ntex; i++)
        gfx_release_texture(m->tex[i]);
    free(m->tex);
    free(m->part);
    free(m->invbind);
    free(m->tint_mask);
    m->tint_mask = NULL;
    fmt_ahi_free(&m->skel);
    fmt_amo_free(&m->amo);
    memset(m, 0, sizeof *m);
}

void fl_model_pose(fl_model *m, const flmat *bone_world_mats, const fl_light *L)
{
    int pi, nb = m->skel.nbone;
    flmat *skin = NULL;

    rt_prof_begin(RTP_SKIN);
    if (bone_world_mats && nb) {
        int b;
        skin = malloc(sizeof(flmat) * nb);
        for (b = 0; b < nb; b++)
            flmat_mul(skin[b], m->invbind[b], bone_world_mats[b]);
    }
    for (pi = 0; pi < m->npart; pi++) {
        amo_part *p = &m->amo.part[pi];
        fl_part *fp = &m->part[pi];
        int v;
        gfx_light gl;
        if (fp->skip || (!fp->skinpos && !fp->gpu))
            continue;
        memset(&gl, 0, sizeof gl);
        gl.lit = fp->lit && L;
        if (gl.lit) {
            memcpy(gl.dir, L->dir, sizeof gl.dir);
            memcpy(gl.col, L->col, sizeof gl.col);
            memcpy(gl.ambient, L->ambient, sizeof gl.ambient);
        }
        gl.tint = m->has_tint && pi == 0;
        memcpy(gl.tint_rgb, m->tint, sizeof gl.tint_rgb);
        if (fp->gpu) {
            rt_prof_count(RTPC_SKIN_VERTS, p->nvert);
            rt_prof_begin(RTP_GFX);
            gfx_clay_pose(fp->clay, (const float (*)[16])skin, &gl);
            rt_prof_end(RTP_GFX);
            continue;
        }
        if (m->has_tint && pi == 0 && !m->tint_mask && p->nstrip > 0) {   /* part 0's first material */
            int s, k, mat0 = p->strip[0].material;
            m->tint_mask = calloc((size_t)p->nvert + 1, 1);
            for (s = 0; s < p->nstrip; s++)
                if (p->strip[s].material == mat0)
                    for (k = 0; k < p->strip[s].count; k++)
                        if (p->index[p->strip[s].first + k] < p->nvert)
                            m->tint_mask[p->index[p->strip[s].first + k]] = 1;
        }
        for (v = 0; v < p->nvert; v++) {
            float pos[3], n[3] = { 0, 1, 0 }, lc[3], len;
            int c, tint = m->has_tint && pi == 0 && m->tint_mask && m->tint_mask[v];
            if (skin && fp->skinned && p->infl_n[v]) {
                int j;
                pos[0] = pos[1] = pos[2] = 0;
                n[0] = n[1] = n[2] = 0;
                for (j = 0; j < p->infl_n[v]; j++) {
                    int b = p->infl_bone[v * AMO_MAX_INFL + j];
                    float w = p->infl_w[v * AMO_MAX_INFL + j], tp[3], tn[3];
                    if (b < 0 || b >= nb)
                        continue;
                    flmat_apply(tp, p->pos + 3 * v, skin[b]);
                    pos[0] += tp[0] * w; pos[1] += tp[1] * w; pos[2] += tp[2] * w;
                    if (p->nrm) {
                        flmat_apply33(tn, p->nrm + 3 * v, skin[b]);
                        n[0] += tn[0] * w; n[1] += tn[1] * w; n[2] += tn[2] * w;
                    }
                }
            } else {
                memcpy(pos, p->pos + 3 * v, sizeof pos);
                if (p->nrm)
                    memcpy(n, p->nrm + 3 * v, sizeof n);
            }
            memcpy(fp->skinpos + 3 * v, pos, sizeof pos);

            /* VU1 lighting: ambient + sum max(0, n.-dir) * colour, clamped */
            lc[0] = lc[1] = lc[2] = 1;
            if (fp->lit && L) {
                int l;
                len = sqrtf(n[0] * n[0] + n[1] * n[1] + n[2] * n[2]);
                if (len > 0) {
                    n[0] /= len; n[1] /= len; n[2] /= len;
                }
                memcpy(lc, L->ambient, sizeof lc);
                for (l = 0; l < 3; l++) {
                    float d = -(n[0] * L->dir[l][0] + n[1] * L->dir[l][1] + n[2] * L->dir[l][2]);
                    if (d > 0) {
                        lc[0] += d * L->col[l][0];
                        lc[1] += d * L->col[l][1];
                        lc[2] += d * L->col[l][2];
                    }
                }
            }
            if (tint)
                for (c = 0; c < 3; c++)
                    lc[c] = (lc[c] > 1 ? 1 : lc[c]) * m->tint[c];
            for (c = 0; c < 3; c++) {
                float vc = (p->col ? p->col[4 * v + c] : 255.0f) * (lc[c] > 1 ? 1 : lc[c]);
                fp->skincol[4 * v + c] = (uint8_t)(vc > 255 ? 255 : vc < 0 ? 0 : vc);
            }
            fp->skincol[4 * v + 3] = (uint8_t)(p->col ? p->col[4 * v + 3] : 255);
        }
        if (fp->check) {        /* RT_SKIN_CHECK: the GPU path's result for every copy of every vertex */
            const gfx_skin_mesh *cm = fp->check;
            int b, k;
            for (b = 0; b < cm->nbatch; b++)
                for (k = 0; k < cm->batch[b].nv; k++) {
                    float q[3], rgba[4], scale = 1, e = 0;
                    int sv = cm->src[cm->batch[b].vfirst + k], c;
                    gfx_skin_eval(cm, b, k, (const float (*)[16])(skin && fp->skinned ? skin : NULL), &gl, q, rgba);
                    for (c = 0; c < 3; c++) {
                        float a = fp->skinpos[3 * sv + c];
                        scale = fabsf(a) > scale ? fabsf(a) : scale;
                        e = fabsf(q[c] - a) > e ? fabsf(q[c] - a) : e;
                    }
                    if (e / scale > chk_pos)
                        chk_pos = e / scale;
                    for (c = 0; c < 4; c++) {
                        float vc = rgba[c] > 255 ? 255 : rgba[c] < 0 ? 0 : rgba[c];
                        double dc = fabs(vc - fp->skincol[4 * sv + c]);
                        if (dc > chk_col)
                            chk_col = dc;
                    }
                    chk_n++;
                }
        }
        rt_prof_count(RTPC_SKIN_VERTS, p->nvert);
        rt_prof_begin(RTP_GFX);
        gfx_update_clay(fp->clay, fp->skinpos, fp->skincol);
        rt_prof_end(RTP_GFX);
    }
    free(skin);
    rt_prof_end(RTP_SKIN);
}

void fl_model_draw(fl_model *m, int sky)
{
    int i;
    for (i = 0; i < m->npart; i++)
        if (sky < 0 || sky == m->part[i].is_sky)
            gfx_execute_clay(m->part[i].clay);
}

/* ------------------------------------------------------------ skeleton */

int fl_skel_create(fl_skel *s, fmt_blob ahi, int be)
{
    memset(s, 0, sizeof *s);
    if (fmt_ahi_load(&s->skel, ahi, be) != 0)
        return -1;
    s->chan = calloc(s->skel.nbone, sizeof *s->chan);
    s->world = calloc(s->skel.nbone, sizeof(flmat));
    bind_channels(&s->skel, s->chan);
    bone_world(&s->skel, (const float (*)[9])s->chan, s->world);
    return 0;
}

void fl_skel_release(fl_skel *s)
{
    int g;
    for (g = 0; g < FL_MAX_GROUPS; g++)
        if (s->has_mot[g])
            fmt_aan_free(&s->mot[g]);
    free(s->chan);
    free(s->world);
    fmt_ahi_free(&s->skel);
    memset(s, 0, sizeof *s);
}

int fl_skel_set_motion(fl_skel *s, int group, fmt_blob tbl, int id, int be)
{
    fmt_blob a;
    if (group < 0 || group >= FL_MAX_GROUPS)
        return -1;
    a = fmt_tbl_motion(tbl, (id % 1000) / 100, id % 100, be);
    if (!a.p)
        return -1;
    if (s->has_mot[group])
        fmt_aan_free(&s->mot[group]);
    if (fmt_aan_load(&s->mot[group], a, be) != 0)
        return -1;
    s->has_mot[group] = 1;
    if (s->mot[group].end > s->end)
        s->end = s->mot[group].end;
    return 0;
}

void fl_skel_update(fl_skel *s, float t)
{
    int g;
    rt_prof_begin(RTP_MOTION);
    rt_prof_count(RTPC_SKEL_EVALS, 1);
    s->last_ok = 0;
    bind_channels(&s->skel, s->chan);
    for (g = 0; g < FL_MAX_GROUPS; g++) {
        const aan_motion *m = &s->mot[g];
        float ft = t;
        int i, k = 0;
        if (!s->has_mot[g])
            continue;
        if (m->end > 0 && ft > m->end) {   /* loop like frame_move */
            float ls = m->loop ? m->loop_start : 0;
            float len = m->end - ls;
            ft = len > 0 ? ls + fmodf(ft - ls, len) : m->end;
        }
        /* AAN bone i drives the i-th bone of this group */
        for (i = 0; i < s->skel.nbone && k < m->nbone; i++) {
            if (s->skel.bone[i].group != g)
                continue;
            fmt_aan_eval(m, k++, ft, s->chan[i]);
        }
    }
    s->frame = t;
    bone_world(&s->skel, (const float (*)[9])s->chan, s->world);
    rt_prof_end(RTP_MOTION);
}

static void eval_group(const ahi_skel *sk, int g, const aan_motion *m, float t, float (*chan)[9])
{
    int i, k = 0;
    for (i = 0; i < sk->nbone && k < m->nbone; i++) {
        if (sk->bone[i].group != g)
            continue;
        fmt_aan_eval(m, k, t, chan[i]);
        /* The root-motion bone (second bone of group 0) carries the motion's own displacement; its AHI bind
         * translation is only the model pivot (bone 2's bind is the opposite offset). A motion with no
         * height curve for it (Aptonoth idle / walk / eat, ids 1001, 1004-1006) has no height, not the pivot's
         * 427 units: the body hung 680 units above the ground on stages whose ground is not y = 0. */
        if (g == 0 && k == 1) {
            int c, has = 0, j, n = 0, i2 = -1;
            for (j = 0; j < sk->nbone && i2 < 0; j++)       /* the third bone of group 0 */
                if (sk->bone[j].group == 0 && n++ == 2)
                    i2 = j;
            for (c = 0; c < m->ncurve[k]; c++)
                if (m->curve[k][c].channel == 7)
                    has = 1;
            /* only the pivot pair (bone 3 binds at exactly minus bone 2's offset, as the Aptonoth's does): birds, other monsters and
             * set models whose second bone is an ordinary joint keep their bind height */
            if (!has && i2 >= 0 && sk->bone[i2].t[1] != 0.0f && fabsf(sk->bone[i2].t[1] + sk->bone[i].t[1]) < 0.05f
                && fabsf(sk->bone[i2].t[0] + sk->bone[i].t[0]) < 0.05f && fabsf(sk->bone[i2].t[2] + sk->bone[i].t[2]) < 0.05f)
                chan[i][7] = 0.0f;
        }
        k++;
    }
}

void fl_skel_pose_groups(fl_skel *s, const fl_group_pose g[FL_MAX_GROUPS])
{
    int gi, i, c;
    float (*tmp)[9] = NULL;
    if (s->last_ok && s->last_lock == s->root_lock && !memcmp(s->last, g, sizeof s->last)
        && !getenv("RT_POSE_ALL"))
        return;                             /* this pose is what chan / world hold */
    rt_prof_begin(RTP_MOTION);
    rt_prof_count(RTPC_SKEL_EVALS, 1);
    memcpy(s->last, g, sizeof s->last);
    s->last_ok = 1;
    s->last_lock = s->root_lock;
    bind_channels(&s->skel, s->chan);
    for (gi = 0; gi < FL_MAX_GROUPS; gi++) {
        if (!g[gi].m)
            continue;
        eval_group(&s->skel, gi, g[gi].m, g[gi].t, s->chan);
        if (!g[gi].m2)
            continue;
        if (!tmp)
            tmp = malloc(s->skel.nbone * sizeof *tmp);
        bind_channels(&s->skel, tmp);
        eval_group(&s->skel, gi, g[gi].m2, g[gi].t2, tmp);
        for (i = 0; i < s->skel.nbone; i++) {
            if (s->skel.bone[i].group != gi)
                continue;
            for (c = 0; c < 9; c++) {
                float a = s->chan[i][c], b = tmp[i][c];
                if (c >= 3 && c < 6) {          /* angles: shortest way */
                    while (b - a > 3.14159265f) b -= 6.2831853f;
                    while (b - a < -3.14159265f) b += 6.2831853f;
                }
                s->chan[i][c] = g[gi].wa * a + g[gi].wb * b;
            }
        }
    }
    free(tmp);
    if (s->root_lock) {
        int k = 0;
        for (i = 0; i < s->skel.nbone; i++) {
            if (s->skel.bone[i].group != 0)
                continue;
            if (k++ == 1) {
                s->chan[i][6] = s->skel.bone[i].t[0];
                s->chan[i][8] = s->skel.bone[i].t[2];
                break;
            }
        }
    }
    bone_world(&s->skel, (const float (*)[9])s->chan, s->world);
    rt_prof_end(RTP_MOTION);
}
