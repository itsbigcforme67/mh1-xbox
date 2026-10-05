/* gm02 - model work setup 0x00122E70-0x00122F38: model_work_set. Whole file in getm_nm.c. */
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
