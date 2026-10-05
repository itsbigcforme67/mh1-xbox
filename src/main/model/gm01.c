/* gm01 - model work setup 0x00121B70-0x00121F10: model_work_init, Attribute_from_amo, clay_attr_set, clay_attr_reset. Whole file in getm_nm.c. */
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












void model_work_free2(int idx);






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
