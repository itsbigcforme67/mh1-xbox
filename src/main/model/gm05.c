/* gm05 - model work setup 0x00123460-0x001234F8: model_work_free2, all_model_free, all_initmotion_free. Whole file in getm_nm.c. */
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
