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

#include <stdlib.h>
#include <string.h>

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
    fp->clay = gfx_create_clay(&d);
    if (d.dynamic) {
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
                    m->tex[i] = gfx_create_texture(img.w, img.h, img.rgba);
                    free(img.rgba);
                }
            }
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
        gfx_release_clay(m->part[i].clay);
        free(m->part[i].skinpos);
        free(m->part[i].skincol);
    }
    for (i = 0; i < m->ntex; i++)
        gfx_release_texture(m->tex[i]);
    free(m->tex);
    free(m->part);
    free(m->invbind);
    fmt_ahi_free(&m->skel);
    fmt_amo_free(&m->amo);
    memset(m, 0, sizeof *m);
}

void fl_model_pose(fl_model *m, const flmat *bone_world_mats, const fl_light *L)
{
    int pi, nb = m->skel.nbone;
    flmat *skin = NULL;

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
        if (!fp->skinpos)
            continue;
        for (v = 0; v < p->nvert; v++) {
            float pos[3], n[3] = { 0, 1, 0 }, lc[3], len;
            int c;
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
            for (c = 0; c < 3; c++) {
                float vc = (p->col ? p->col[4 * v + c] : 255.0f) * (lc[c] > 1 ? 1 : lc[c]);
                fp->skincol[4 * v + c] = (uint8_t)(vc > 255 ? 255 : vc < 0 ? 0 : vc);
            }
            fp->skincol[4 * v + 3] = (uint8_t)(p->col ? p->col[4 * v + 3] : 255);
        }
        gfx_update_clay(fp->clay, fp->skinpos, fp->skincol);
    }
    free(skin);
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
}

static void eval_group(const ahi_skel *sk, int g, const aan_motion *m, float t, float (*chan)[9])
{
    int i, k = 0;
    for (i = 0; i < sk->nbone && k < m->nbone; i++) {
        if (sk->bone[i].group != g)
            continue;
        fmt_aan_eval(m, k++, t, chan[i]);
    }
}

void fl_skel_pose_groups(fl_skel *s, const fl_group_pose g[FL_MAX_GROUPS])
{
    int gi, i, c;
    float (*tmp)[9] = NULL;
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
}
