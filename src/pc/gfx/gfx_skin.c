/*
 * gfx_skin.c - data for skinning on the GPU (gfx.h "GPU skinning"):
 * gfx_skin_build regroups a clay's triangles into batches of at most
 * GFX_SKIN_PALETTE bones (the vertex program's constant space) and copies
 * their vertices with palette-local bone slots; gfx_skin_eval is a C model
 * of the vertex program (src/pc/xbox/shaders/skin.vs.cg), which the PC
 * build checks against fl_model_pose's CPU skinning (RT_SKIN_CHECK=1).
 */
#include "gfx.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

void gfx_skin_free(gfx_skin_mesh *m)
{
    free(m->pos); free(m->nrm); free(m->st); free(m->w); free(m->slot);
    free(m->col); free(m->flag); free(m->src); free(m->index); free(m->batch);
    memset(m, 0, sizeof *m);
}

static int vert_bones(const gfx_skin_desc *s, int v, int out[GFX_SKIN_INFL])
{
    int j, n = 0;
    if (!s->skinned || !s->infl_n || !s->infl_n[v])
        return 0;
    for (j = 0; j < s->infl_n[v] && j < GFX_SKIN_INFL; j++) {
        int b = s->infl_bone[v * GFX_SKIN_INFL + j];
        if (b >= 0 && b < s->nbone)
            out[n++] = b;
    }
    return n;
}

int gfx_skin_build(gfx_skin_mesh *m, const gfx_clay_desc *d, const gfx_skin_desc *s)
{
    int *map = malloc(sizeof(int) * (size_t)(d->nvert + 1)), *touched = malloc(sizeof(int) * (size_t)(d->nvert + 1));
    int nt = 0, cap_b = 16, b, i, k, cap_v = d->nvert + 64;
    memset(m, 0, sizeof *m);
    if (!map || !touched)
        goto fail;
    for (i = 0; i < d->nvert; i++)
        map[i] = -1;
    m->index = malloc(sizeof(uint16_t) * (size_t)(d->nindex + 1));
    m->batch = malloc(sizeof(gfx_skin_batch) * (size_t)cap_b);
#define GROW(p, n) p = realloc(p, sizeof(*p) * (size_t)(n))
    m->pos = NULL; m->nrm = NULL; m->st = NULL; m->w = NULL; m->slot = NULL; m->col = NULL; m->flag = NULL; m->src = NULL;
    GROW(m->pos, 3 * cap_v); GROW(m->nrm, 3 * cap_v); GROW(m->st, 2 * cap_v); GROW(m->w, 4 * cap_v);
    GROW(m->slot, 4 * cap_v); GROW(m->col, 4 * cap_v); GROW(m->flag, 2 * cap_v); GROW(m->src, cap_v);
    if (!m->index || !m->batch || !m->pos || !m->nrm || !m->st || !m->w || !m->slot || !m->col || !m->flag || !m->src)
        goto fail;
    for (b = 0; b < d->nbatch; b++) {
        int t, end = d->batch[b].first + d->batch[b].count;
        gfx_skin_batch *cur = NULL;
        for (t = d->batch[b].first; t + 2 < end; t += 3) {
            int need[3 * GFX_SKIN_INFL], nn = 0, c, add = 0;
            for (c = 0; c < 3; c++) {
                int bb[GFX_SKIN_INFL], n = vert_bones(s, d->index[t + c], bb), j, q;
                for (j = 0; j < n; j++) {
                    for (q = 0; q < nn && need[q] != bb[j]; q++)
                        ;
                    if (q == nn)
                        need[nn++] = bb[j];
                }
            }
            if (cur)
                for (c = 0; c < nn; c++) {
                    int q;
                    for (q = 0; q < cur->nbone && cur->bone[q] != need[c]; q++)
                        ;
                    add += q == cur->nbone;
                }
            if (!cur || cur->nbone + add > GFX_SKIN_PALETTE) {     /* a new batch */
                for (i = 0; i < nt; i++)
                    map[touched[i]] = -1;
                nt = 0;
                if (m->nbatch == cap_b)
                    GROW(m->batch, cap_b *= 2);
                cur = &m->batch[m->nbatch++];
                memset(cur, 0, sizeof *cur);
                cur->first = m->nindex;
                cur->vfirst = m->nv;
                cur->tex = d->batch[b].tex;
            }
            for (c = 0; c < nn; c++) {
                int q;
                for (q = 0; q < cur->nbone && cur->bone[q] != need[c]; q++)
                    ;
                if (q == cur->nbone)
                    cur->bone[cur->nbone++] = (int16_t)need[c];
            }
            for (c = 0; c < 3; c++) {
                int v = d->index[t + c];
                if (map[v] < 0) {
                    int nv = m->nv, j, bb[GFX_SKIN_INFL], rigid = !(s->skinned && s->infl_n && s->infl_n[v]);
                    if (nv >= 65535)
                        goto fail;
                    if (nv == cap_v) {
                        cap_v *= 2;
                        GROW(m->pos, 3 * cap_v); GROW(m->nrm, 3 * cap_v); GROW(m->st, 2 * cap_v); GROW(m->w, 4 * cap_v);
                        GROW(m->slot, 4 * cap_v); GROW(m->col, 4 * cap_v); GROW(m->flag, 2 * cap_v); GROW(m->src, cap_v);
                        if (!m->pos || !m->nrm || !m->st || !m->w || !m->slot || !m->col || !m->flag || !m->src)
                            goto fail;
                    }
                    memcpy(m->pos + 3 * nv, d->pos + 3 * v, 12);
                    if (s->nrm)
                        memcpy(m->nrm + 3 * nv, s->nrm + 3 * v, 12);
                    else {                          /* as fl_model_pose: (0,1,0) rigid, nothing skinned */
                        m->nrm[3 * nv] = m->nrm[3 * nv + 2] = 0;
                        m->nrm[3 * nv + 1] = rigid ? 1.0f : 0.0f;
                    }
                    if (d->st)
                        memcpy(m->st + 2 * nv, d->st + 2 * v, 8);
                    else
                        m->st[2 * nv] = m->st[2 * nv + 1] = 0;
                    if (d->col)
                        memcpy(m->col + 4 * nv, d->col + 4 * v, 4);
                    else
                        memset(m->col + 4 * nv, 255, 4);
                    for (j = 0; j < 4; j++) {
                        m->w[4 * nv + j] = 0;
                        m->slot[4 * nv + j] = 0;
                    }
                    if (!rigid)
                        for (j = 0; j < s->infl_n[v] && j < GFX_SKIN_INFL; j++) {
                            int bn = s->infl_bone[v * GFX_SKIN_INFL + j], q;
                            if (bn < 0 || bn >= s->nbone)
                                continue;       /* fl_model_pose skips these */
                            for (q = 0; q < cur->nbone && cur->bone[q] != bn; q++)
                                ;
                            m->w[4 * nv + j] = s->infl_w[v * GFX_SKIN_INFL + j];
                            m->slot[4 * nv + j] = (uint8_t)q;
                        }
                    (void)bb;
                    m->flag[2 * nv] = rigid ? 1.0f : 0.0f;
                    m->flag[2 * nv + 1] = s->tint_mask && s->tint_mask[v] ? 1.0f : 0.0f;
                    m->src[nv] = v;
                    map[v] = nv - cur->vfirst;
                    touched[nt++] = v;
                    m->nv++;
                    cur->nv++;
                }
                m->index[m->nindex++] = (uint16_t)map[v];  /* local to the batch's vertices */
            }
            cur->count = m->nindex - cur->first;
        }
    }
    (void)k;
    free(map);
    free(touched);
    return 0;
fail:
    free(map);
    free(touched);
    gfx_skin_free(m);
    return -1;
#undef GROW
}

void gfx_skin_eval(const gfx_skin_mesh *m, int b, int v, const float (*skin)[16], const gfx_light *L,
                   float pos[3], float rgba[4])
{
    static const float id[16] = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
    const gfx_skin_batch *bt = &m->batch[b];
    int g = bt->vfirst + v, k, j, l;
    float rows[3][4], n[3], lc[3] = { 1, 1, 1 }, len2;
    const float *p = m->pos + 3 * g, *nr = m->nrm + 3 * g;
    for (j = 0; j < 3; j++)
        for (k = 0; k < 4; k++)
            rows[j][k] = m->flag[2 * g] * (j == k ? 1.0f : 0.0f);
    for (k = 0; k < 4; k++) {
        const float *M = skin ? skin[bt->bone[m->slot[4 * g + k]]] : id;
        float w = m->w[4 * g + k];
        for (j = 0; j < 3; j++) {           /* row j = column j of the row-vector matrix */
            rows[j][0] += M[0 * 4 + j] * w;
            rows[j][1] += M[1 * 4 + j] * w;
            rows[j][2] += M[2 * 4 + j] * w;
            rows[j][3] += M[3 * 4 + j] * w;
        }
    }
    for (j = 0; j < 3; j++) {
        pos[j] = p[0] * rows[j][0] + p[1] * rows[j][1] + p[2] * rows[j][2] + rows[j][3];
        n[j] = nr[0] * rows[j][0] + nr[1] * rows[j][1] + nr[2] * rows[j][2];
    }
    if (L && L->lit) {
        len2 = n[0] * n[0] + n[1] * n[1] + n[2] * n[2];
        if (len2 > 0) {
            float r = 1.0f / sqrtf(len2);
            n[0] *= r; n[1] *= r; n[2] *= r;
        }
        memcpy(lc, L->ambient, sizeof lc);
        for (l = 0; l < 3; l++) {
            float dd = -(n[0] * L->dir[l][0] + n[1] * L->dir[l][1] + n[2] * L->dir[l][2]);
            if (dd > 0)
                for (j = 0; j < 3; j++)
                    lc[j] += dd * L->col[l][j];
        }
    }
    for (j = 0; j < 3; j++) {
        if (lc[j] > 1)
            lc[j] = 1;
        if (L && L->tint && m->flag[2 * g + 1] > 0.5f)
            lc[j] *= L->tint_rgb[j];
        rgba[j] = m->col[4 * g + j] * lc[j];
    }
    rgba[3] = m->col[4 * g + 3];
}
