/*
 * amo.c - AMO model reader (docs/formats/graphics.md 2.3).
 *
 * Every chunk is {u32 type, u32 count, u32 size incl. 12-byte header}
 * followed by its payload or children (GetSubDataAMO in
 * f_convertmodelmeshamo.s). The root's size can be short (cube.amo), so
 * the root's children are walked to the end of the file.
 */
#include "fmt.h"

#include <stdlib.h>
#include <string.h>

typedef struct { const uint8_t *p; uint32_t type, count, size; } chunk;

static int chunk_at(fmt_blob f, size_t off, size_t end, chunk *c, int be)
{
    if (off + 12 > end || end > f.n)
        return 0;
    c->p = f.p + off;
    c->type = fmt_u32(c->p, be);
    c->count = fmt_u32(c->p + 4, be);
    c->size = fmt_u32(c->p + 8, be);
    return c->size >= 12 && off + c->size <= end;
}

/* Find the first child of type typ inside [off+12, end). */
static int child(fmt_blob f, const chunk *parent, size_t pend, uint32_t typ, chunk *out, int be)
{
    size_t o = (size_t)(parent->p - f.p) + 12;
    size_t end = pend ? pend : (size_t)(parent->p - f.p) + parent->size;
    chunk c;
    while (chunk_at(f, o, end, &c, be)) {
        if (c.type == typ) {
            *out = c;
            return 1;
        }
        o += c.size;
    }
    return 0;
}

static float *read_floats(const uint8_t *p, int n, int be)
{
    float *v = malloc(sizeof(float) * (n ? n : 1));
    int i;
    for (i = 0; i < n; i++)
        v[i] = fmt_f32(p + 4 * i, be);
    return v;
}

static int load_part(amo_part *pt, fmt_blob f, const chunk *mc, int be)
{
    chunk c, il, s;
    int nprim = 0, k;
    uint32_t *primmat = NULL, *matlist = NULL;
    int nprimmat = 0, nmatlist = 0;

    memset(pt, 0, sizeof *pt);
    if (!child(f, mc, 0, AMO_VERTEX, &c, be))
        return -1;
    pt->nvert = (int)c.count;
    pt->pos = read_floats(c.p + 12, pt->nvert * 3, be);
    if (child(f, mc, 0, AMO_NORMAL, &c, be))
        pt->nrm = read_floats(c.p + 12, pt->nvert * 3, be);
    if (child(f, mc, 0, AMO_ST, &c, be))
        pt->st = read_floats(c.p + 12, pt->nvert * 2, be);
    if (child(f, mc, 0, AMO_COLOR, &c, be))
        pt->col = read_floats(c.p + 12, pt->nvert * 4, be);
    if (child(f, mc, 0, AMO_ATTR, &c, be) && c.size >= 12 + 0x48) {
        pt->has_attr = 1;
        for (k = 0; k < 18; k++)
            pt->attr[k] = fmt_s32(c.p + 12 + 4 * k, be);
    }

    /* weights; bone = index into the 0x100000 list if present, else the
     * part's own AHI bone (player parts) */
    if (child(f, mc, 0, AMO_WEIGHT, &c, be)) {
        chunk mx;
        int havemx = child(f, mc, 0, AMO_MATRIX, &mx, be);
        const uint8_t *p = c.p + 12;
        int v;
        pt->ninfl = AMO_MAX_INFL;
        pt->infl_n = calloc(pt->nvert, 1);
        pt->infl_bone = calloc((size_t)pt->nvert * AMO_MAX_INFL, sizeof(int16_t));
        pt->infl_w = calloc((size_t)pt->nvert * AMO_MAX_INFL, sizeof(float));
        for (v = 0; v < pt->nvert && v < (int)c.count; v++) {
            uint32_t n = fmt_u32(p, be), j;
            p += 4;
            for (j = 0; j < n; j++, p += 8) {
                uint32_t b = fmt_u32(p, be);
                float w = fmt_f32(p + 4, be) / 100.0f;      /* stored in percent */
                if (havemx && b < mx.count)
                    b = fmt_u32(mx.p + 12 + 4 * b, be);
                if (j < AMO_MAX_INFL) {
                    pt->infl_bone[v * AMO_MAX_INFL + j] = (int16_t)b;
                    pt->infl_w[v * AMO_MAX_INFL + j] = w;
                }
            }
            pt->infl_n[v] = (uint8_t)(n < AMO_MAX_INFL ? n : AMO_MAX_INFL);
        }
    }

    if (child(f, mc, 0, AMO_MATLIST, &c, be)) {
        nmatlist = (int)c.count;
        matlist = malloc(4 * (nmatlist ? nmatlist : 1));
        for (k = 0; k < nmatlist; k++)
            matlist[k] = fmt_u32(c.p + 12 + 4 * k, be);
    }
    if (child(f, mc, 0, AMO_PRIMMAT, &c, be)) {
        nprimmat = (int)c.count;
        primmat = malloc(4 * (nprimmat ? nprimmat : 1));
        for (k = 0; k < nprimmat; k++)
            primmat[k] = fmt_u32(c.p + 12 + 4 * k, be);
    }

    /* strips: 0x30000 list(s) then 0x40000, in file order */
    if (child(f, mc, 0, AMO_INDEXLISTS, &il, be)) {
        size_t o, end = (size_t)(il.p - f.p) + il.size;
        int pass;
        for (pass = 0; pass < 2; pass++) {
            for (o = (size_t)(il.p - f.p) + 12; chunk_at(f, o, end, &s, be); o += s.size) {
                const uint8_t *p;
                uint32_t j;
                if (s.type != AMO_STRIPS && s.type != AMO_STRIPS1)
                    continue;
                p = s.p + 12;
                for (j = 0; j < s.count; j++) {
                    uint32_t n = fmt_u32(p, be) & 0x7FFFFFFF, q;
                    if (pass == 1) {
                        amo_strip *st = &pt->strip[pt->nstrip++];
                        st->first = pt->nindex;
                        st->count = (int)n;
                        st->material = -1;
                        if (nprim < nprimmat && primmat[nprim] < (uint32_t)nmatlist)
                            st->material = (int)matlist[primmat[nprim]];
                        for (q = 0; q < n; q++)
                            pt->index[pt->nindex++] = (uint16_t)fmt_u32(p + 4 + 4 * q, be);
                    } else {
                        pt->nindex += (int)n;
                        pt->nstrip++;
                    }
                    nprim++;
                    p += 4 + 4 * n;
                }
            }
            if (pass == 0) {
                pt->index = malloc(sizeof(uint16_t) * (pt->nindex ? pt->nindex : 1));
                pt->strip = malloc(sizeof(amo_strip) * (pt->nstrip ? pt->nstrip : 1));
                pt->nindex = pt->nstrip = 0;
                nprim = 0;
            }
        }
    }
    free(primmat);
    free(matlist);
    return 0;
}

int fmt_amo_load(amo_model *m, fmt_blob f, int be)
{
    chunk root, models, mats, texs, c;
    size_t o, end;
    int *slot_apx = NULL, nslot = 0, k;

    memset(m, 0, sizeof *m);
    if (f.n < 12)
        return -1;
    root.p = f.p;      /* size field not trusted: children run to f.n */
    root.type = fmt_u32(f.p, be);
    if (root.type != AMO_ROOT)
        return -1;
    if (!child(f, &root, f.n, AMO_MODELS, &models, be))
        return -1;

    end = (size_t)(models.p - f.p) + models.size;
    for (o = (size_t)(models.p - f.p) + 12; chunk_at(f, o, end, &c, be); o += c.size)
        if (c.type == AMO_MODEL)
            m->npart++;
    m->part = calloc(m->npart ? m->npart : 1, sizeof(amo_part));
    m->npart = 0;
    for (o = (size_t)(models.p - f.p) + 12; chunk_at(f, o, end, &c, be); o += c.size)
        if (c.type == AMO_MODEL && load_part(&m->part[m->npart], f, &c, be) == 0)
            m->npart++;

    if (child(f, &root, f.n, AMO_TEXTURES, &texs, be)) {
        end = (size_t)(texs.p - f.p) + texs.size;
        slot_apx = malloc(sizeof(int) * (texs.count + 1));
        for (o = (size_t)(texs.p - f.p) + 12; chunk_at(f, o, end, &c, be) && nslot < (int)texs.count; o += c.size)
            slot_apx[nslot++] = (int)fmt_u32(c.p + 12, be);
    }
    if (child(f, &root, f.n, AMO_MATERIALS, &mats, be)) {
        end = (size_t)(mats.p - f.p) + mats.size;
        m->mat = calloc(mats.count + 1, sizeof(amo_material));
        for (o = (size_t)(mats.p - f.p) + 12; chunk_at(f, o, end, &c, be); o += c.size) {
            amo_material *mt = &m->mat[m->nmat++];
            const uint8_t *p = c.p + 12;
            for (k = 0; k < 4; k++) {
                mt->col_a[k] = fmt_f32(p + 4 * k, be);
                mt->col_b[k] = fmt_f32(p + 0x10 + 4 * k, be);
                mt->col_c[k] = fmt_f32(p + 0x20 + 4 * k, be);
            }
            mt->power = fmt_f32(p + 0x30, be);
            mt->has_tex = (int)fmt_u32(p + 0x34, be);
            mt->tex_slot = c.size >= 12 + 0x104 ? (int)fmt_u32(p + 0x100, be) : -1;
            mt->apx = (mt->has_tex && mt->tex_slot >= 0 && mt->tex_slot < nslot) ? slot_apx[mt->tex_slot] : -1;
        }
    }
    free(slot_apx);
    return 0;
}

void fmt_amo_free(amo_model *m)
{
    int i;
    for (i = 0; i < m->npart; i++) {
        amo_part *p = &m->part[i];
        free(p->pos); free(p->nrm); free(p->st); free(p->col);
        free(p->infl_n); free(p->infl_bone); free(p->infl_w);
        free(p->index); free(p->strip);
    }
    free(m->part);
    free(m->mat);
    memset(m, 0, sizeof *m);
}
