/* Model work setup and release. SLPM_654.95 0x00121B70-0x00123500 (f_get), the parts that
 * are not the heavy mkModel* builders: model_work_init, render attribute decoding, model work
 * setters per kind, release. */
#include "types.h"
#include "mdlw.h"

extern u8 *mdlw_heap_area;
extern int aa_material[2], aa_fog[2], aa_scissor[2], aa_fadecol[2], aa_uvscroll[2], aa_filt[2];
extern int aa_light[9], aa_specular[4], aa_cull[4], aa_alpha_src[10], aa_alpha_ope[3], aa_addr[3];

void *memset(void *, int, unsigned);
void clr_used_material(int, int);
void clr_used_hierarchy(int, int);
void clr_used_clay(int, int);
void clr_used_mdlw(int, int);
void flSetRenderState(int, int);
void SetTrnslMode(int, int);
void SetOpeMode(int);
void SetFilterMode(int);
void flReleaseClayHandle(int);
void flReleaseInitMotionSetHandle(int);
void release_texture(s16, s16);
int load_texlist(int, int, int);
void load_texlist_pl(int *, int, int, int);
int mkMaterial(MDLW *, int);
void mkModel(MDLW *, int, int);
void mkModel3(MDLW *, int, int, int);
void mkModel4(MDLW *, int, int, int);

void model_work_init(void) {
    memset(mdlw_heap_area, 0, 0x4000);
    clr_used_material(0, 0x400);
    clr_used_hierarchy(0, 0x800);
    clr_used_clay(0, 0x180);
    clr_used_mdlw(0, 0x80);
}

/* Decodes a material attribute block (AMO) into render states and a packed word at *out:
 * bit 0 set, 1 fade, 2-5 alpha source, 6-9 alpha destination, 10-11 op, 12 filter, 13-14 address. */
void Attribute_from_amo(int *a, u32 *out) {
    flSetRenderState(0x15, aa_material[a[1]]);
    flSetRenderState(0x12, aa_fog[a[8]]);
    flSetRenderState(1, aa_light[a[5]]);
    flSetRenderState(2, aa_specular[a[2]]);
    flSetRenderState(0xC, aa_scissor[a[4]]);
    flSetRenderState(0x66, aa_fadecol[a[9]]);
    flSetRenderState(0x62, aa_uvscroll[a[7]]);
    flSetRenderState(0, aa_cull[a[3]]);
    if (aa_fadecol[a[9]] == 1) {
        flSetRenderState(0x67, -1);
    }
    switch (a[10]) {
    case 6:
        flSetRenderState(0x5D, 0x6000000);
        break;
    case 5:
        flSetRenderState(0x5D, 0x5000000);
        break;
    case 0:
    default:
        flSetRenderState(0x5D, 0);
        break;
    }
    *out = 1;
    *out |= (a[9] & 1) << 1;
    *out |= (a[11] & 0xF) << 2;
    *out |= (a[12] & 0xF) << 6;
    *out |= (a[13] & 3) << 10;
    *out |= (a[16] & 1) << 12;
    *out |= (a[17] & 3) << 13;
}

void clay_attr_set(u32 attr) {
    if (attr & 1) {
        SetTrnslMode(aa_alpha_src[(attr >> 2) & 0xF], aa_alpha_src[(attr >> 6) & 0xF]);
        SetOpeMode(aa_alpha_ope[(attr >> 10) & 3]);
        SetFilterMode(aa_filt[(attr >> 12) & 1]);
        flSetRenderState(0x64, aa_addr[(attr >> 13) & 3]);
    }
}

void clay_attr_reset(void) {
    SetTrnslMode(4, 5);
    SetOpeMode(0);
    SetFilterMode(1);
}

MDLW *model_work_set(int idx, int mat, int tex, int texfile, int p1, int p2) {
    MDLW *w = get_mdlw_ptr((s16)idx);

    w->flag = 1;
    w->x75 = 0;
    w->x01 = 0;
    w->mat_set = mat;
    w->tex_start = tex;
    w->tex_n = load_texlist(texfile, (s16)tex, 0);
    if (mkMaterial(w, mat) == -1) {
        return 0;
    }
    mkModel(w, p1, p2);
    return w;
}

MDLW *model_work_set2(int idx, int mat, int tex, int *pl, int skin, int p1, int p2) {
    MDLW *w = get_mdlw_ptr((s16)idx);

    w->flag = 1;
    w->x75 = 0;
    w->x01 = 0;
    w->mat_set = mat;
    w->tex_start = tex;
    if (pl != 0) {
        w->tex_n = pl[0];
        load_texlist_pl(pl, (s16)tex, 0, (u16)skin);
    } else {
        w->tex_n = 0;
    }
    if (mkMaterial(w, mat) == -1) {
        return 0;
    }
    mkModel(w, p1, p2);
    return w;
}

MDLW *st_model_work_set(int idx, int mat, int tex, int texfile, int kind) {
    MDLW *w = get_mdlw_ptr((s16)idx);

    w->flag = 1;
    w->x75 = 0;
    w->x01 = 0;
    w->mat_set = mat;
    w->tex_start = tex;
    w->tex_n = load_texlist(texfile, (s16)tex, 0);
    if (mkMaterial(w, mat) == -1) {
        return 0;
    }
    mkModel(w, kind, 0);
    return w;
}

MDLW *pl_model_work_set(int idx, int mat, int tex, int texfile, int p1, int p2) {
    MDLW *w = get_mdlw_ptr((s16)idx);

    w->flag = 1;
    w->x75 = 0;
    w->x01 = 0;
    w->mat_set = mat;
    w->tex_start = tex;
    w->tex_n = load_texlist(texfile, (s16)tex, 0);
    if (mkMaterial(w, mat) == -1) {
        return 0;
    }
    mkModel3(w, p1, p2, 2);
    return w;
}

MDLW *em_model_work_set(int idx, int mat, int tex, int texfile, int p1, int p2) {
    MDLW *w = get_mdlw_ptr((s16)idx);

    w->flag = 1;
    w->x75 = 0;
    w->x01 = 0;
    w->mat_set = mat;
    w->tex_start = tex;
    w->tex_n = load_texlist(texfile, (s16)tex, 0);
    if (mkMaterial(w, mat) == -1) {
        return 0;
    }
    mkModel4(w, p1, p2, 2);
    return w;
}

/* Releases the clay handles of n CLAY entries (0x8C bytes each). */
void release_model(int *clay, int n) {
    int i;
    int *c;

    for (i = 0, c = clay; i < n; i++) {
        if (c[0] != -1) {
            flReleaseClayHandle(c[0]);
        }
        c = (int *)((u8 *)c + 0x8C);
    }
}

void release_motion(MDLW *w, int *handles) {
    int i;

    for (i = 0; i < w->num; i++) {
        if (*handles != 0) {
            flReleaseInitMotionSetHandle(*handles);
        }
        handles++;
    }
}

void model_work_free2(int idx);

void model_work_free(int idx) {
    MDLW *w = get_mdlw_ptr(idx);

    if (w->x75 != 0) {
        model_work_free2(idx);
    } else if (w->flag != 0) {
        clr_used_material(w->mat_start, w->mat_n);
        clr_used_hierarchy(w->hier_no, w->hier_n);
        clr_used_clay(w->clay_start, w->clay_n);
        release_model((int *)w->clay, w->clay_n);
        if (w->tex_n != 0) {
            release_texture(w->tex_start, w->tex_n);
        }
        release_motion(w, (int *)w->si);
        memset(w, 0, 0x80);
    }
}

void model_work_free2(int idx) {
    MDLW *w = get_mdlw_ptr(idx);

    if (w->flag != 0) {
        clr_used_hierarchy(w->hier_no, w->hier_n);
        memset(w, 0, 0x80);
    }
}

void all_model_free(void) {
    int i;

    for (i = 0; i < 0x80; i++) {
        model_work_free(i);
    }
}

void all_initmotion_free(void) {
    /* empty in the retail build */
}

