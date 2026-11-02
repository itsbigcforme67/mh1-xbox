/* weapon_em - SLPM_654.95 0x00168B10-0x00168EA0 (f_weapon): enemy_trans, the monster model draw callback (light,
 * world matrix, per-clay alpha list em_alpha_clay, material per kind, skin matrices). */
#include "types.h"
#include "pl.h"
#include "fl.h"
#include "clay.h"
#include "plf.h"
#include "game.h"
#include "trans_pl.h"

void flmatMul(FLMAT *, f32 *, FLMAT *);
void flmatCopy(void *, void *);
void flCalcTrans(void *, FLMAT *);
void flCalcTransSI(void *, FLMAT *);
void flSetSkinTrans(void *);
void cpAng2Rad_all(void *, f32 *);
#define PF(p, T, o) (*(T *)((u8 *)(p) + MH2_OFS(p, o)))
extern u16 armor_pos[];
void SetPartsTrans(void *, PLW *, s16, int);
void SetPartsTrans2(void *, PLW *, s16, int);
void weapon_trans(f32, PLX *);
void pl_item_trans(PLX *);
void sight_disp2(PLW *);
void sight_disp_ballista(PLW *);
void SetFilterMode(int);
void reload_tex(int, int);
void pl_light_change(PLW *, int);
void Pl_light_set(PLW *);
void light_change_normal(int);
void light_set(int);
s16 act_ck(PLW *, int, int);
s32 frame_check3(f32, f32, PLW *, int);
void flmatGetTrans(f32 *, FLMAT *);
void flvecApplyMat33_2(f32 *, FLMAT *);
void flmatMul33_2(FLMAT *, FLMAT *);
void flmatMakeTrans(FLMAT *, f32, f32, f32);
void flmatCopy(void *, void *);
typedef struct TRANSEM {
    u8 _pad00[0x18];
    PLX *em;            /* 0x18 */
} TRANSEM;
extern s16 *em_alpha_clay[];
extern u8 mat_list[];
extern u8 mat_palett[];
void cpAng2Rad_all(void *, f32 *);
void cpRotMatrixYXZ2(void *, FLMAT *);
void em_trans_sub(PLX *, int);
void em_material_sub(PLX *, int, CLAY *);
void func_5ACA60(PLX *, int, CLAY *);
void func_5FCBB0(PLX *, int, CLAY *);
void flSetMatrixList(void *, void *);
void flSetSkinTransMatrixList(void *, void *);

void enemy_trans(TRANSEM *tp) {
    PLX *em = tp->em;
    PLMDL *mdl = em->mdl;
    FLMAT mb;
    FLMAT mc;
    FLMAT ma;
    f32 ang[3];
    int n;
    int fgo;
    int i;
    CLAY *cb;
    s16 *lp;
    int k;
    s16 v;

    if (em->be_flag == 0 || em->x01 == 0) return;
    pl_light_change((PLW *)em, 1);
    Pl_light_set((PLW *)em);
    cpAng2Rad_all(em->ang, ang);
    flmatMakeScale(&ma, em->scl[0], em->scl[1], em->scl[2]);
    cpRotMatrixYXZ2(em->ang, &mb);
    flmatSetTrans(&mb, em->pos[0], em->pos[1], em->pos[2]);
    flmatMul33_2(&mb, &ma);
    flmatCopy(em->rot, &mb);
    SetFilterMode(1);
    v = *(lp = em_alpha_clay[k = em->kind]);
    if (k == 3) {
        n = 1;
        cb = mdl->clay + em->x11;
    } else {
        cb = mdl->clay;
        n = mdl->num;
    }
    fgo = PF(mdl->skin, s16, 0xC2) >= 0x21;
    if (fgo == 0) {
        flSetSkinTrans(mdl->skin);
    } else {
        flSetSkinTransMatrixList(mat_palett, mdl->skin);
    }
    flmatInit(&mc);
    flSetRenderState(0x19, (u32)mc);
    reload_tex(0xA, em->x34F * 0x14 + 0x9A);
    for (i = 0; i < n; i++) {
        if (em->vis[i] == 0) goto next;
        while (v != -1) {
            if (v >= i) break;
            lp++;
            v = *lp;
        }
        if (v == i || em->alpha != 1.0f) {
            flSetRenderState(0x60, 0);
            lp++;
            v = *lp;
        } else {
            flSetRenderState(0x60, 0xC0);
        }
        flSetRenderState(0x67, -1);
        em_trans_sub(em, i);
        if (cb[i].handle != -1) {
            switch (em->kind) {
            case 9:
            case 18:
            case 23:
                func_5ACA60(em, i, cb);
                break;
            case 20:
                func_5FCBB0(em, i, cb);
                break;
            default:
                em_material_sub(em, i, cb);
                break;
            }
            clay_attr_set(cb[i].attr);
            if (fgo == 1) {
                flSetMatrixList(mat_list + em->x34F * 0x420 + i * 0x42, mat_palett);
            }
            flExecuteClay(cb[i].handle, 0);
        }
    next:;
    }
    clay_attr_reset();
    flSetRenderState(0x60, 0);
    light_change_normal(1);
    light_set(1);
}
