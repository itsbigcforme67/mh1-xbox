/* weapon3 - SLPM_654.95 0x00164F70-0x1692C0 (f_weapon, part 4): player /
 * weapon / monster model display (item models, weapon joint draw, skeleton
 * matrices). Names guessed from the code. */
#include "types.h"
#include "pl.h"
#include "fl.h"
#include "clay.h"
#include "plf.h"
#include "game.h"
#include "trans_pl.h"

extern u32 mem_tex[];
void Pl_horm_adj(PLW *, s16);

int get_tex_num(int n) {
    int i = 0;
    int c = 0;
    u32 *p = &mem_tex[n];
    for (; i < 0x1E; i++) {
        if (p[i] != 0) c++;
    }
    return c;
}

typedef struct MATSET { s32 x0; s32 num; s32 idx[1]; } MATSET;

void Material_set_sub(u8 *base, MATSET *m) {
    int i;
    for (i = 0; i < m->num; i++) {
        flSetRenderState((i + 0x3A) & 0xFF, (int)(base + m->idx[i] * 0x4C));
    }
}

void plplAdd2(u32 *n, u32 *p) {
    u32 v, w;
top:
    v = *p;
    if (v != 0 && (v & 1)) {
        w = v & ~1;
        if (!(((f32 *)w)[1] <= ((f32 *)n)[1])) {
            p = (u32 *)w;
            goto top;
        }
    }
    *p = (u32)n | 1;
    *n = v;
}

void flmatMul(FLMAT *, f32 *, FLMAT *);
void flmatCopy(void *, void *);
void flCalcTrans(void *, FLMAT *);
void flCalcTransSI(void *, FLMAT *);
void flSetSkinTrans(void *);
void cpAng2Rad_all(void *, f32 *);

/* 0x190-byte skeleton node: matrix at +0x40, translation row at +0x70 */
typedef struct PLNODE {
    u8 _pad00[0x40];
    u8 m40[0x30];       /* 0x40 matrix (rotation part) */
    f32 pos[3];         /* 0x70 (overlaps the matrix translation row) */
    u8 _pad7C[0xC4 - 0x7C];
    s16 jnt;            /* 0xC4 */
    u8 _padC6[0x190 - 0xC6];
} PLNODE;

void player_mat_calc(PLW *pl, PLNODE *n, FLMAT *base) {
    FLMAT sc;
    FLMAT res;
    int i;
    int j;
    PLNODE *np = n;
    PLNODE *q = n;

    for (i = 0; i < 0x15; i++) {
        Pl_horm_adj(pl, np->jnt);
        flmatMakeScale(&sc, *(f32 *)((u8 *)pl->part[i] + 0x98), *(f32 *)((u8 *)pl->part[i] + 0x9C), *(f32 *)((u8 *)pl->part[i] + 0xA0));
        flmatMul(&res, (f32 *)np->m40, &sc);
        np->pos[0] = ((f32 *)res)[12];
        np->pos[1] = ((f32 *)res)[13];
        np->pos[2] = ((f32 *)res)[14];
        np++;
    }
    flCalcTransSI(n, base);
    flSetSkinTrans(n);
    for (j = 0; j < 0x15; j++) {
        flmatCopy((u8 *)pl->part[q->jnt] + 0x40, q);
        q++;
    }
}

void player_modify(PLW *pl) {
    f32 ang[3];
    FLMAT m;
    u8 *mdl;

    if (pl->x01 != 0) {
        mdl = *(u8 **)((u8 *)pl + 0x50C);
        cpAng2Rad_all(&pl->ang, ang);
        flmatMakeScale(&m, pl->scl[0], pl->scl[1], pl->scl[2]);
        flmatRotXYZ33(&m, ang[0], ang[1], ang[2]);
        flmatSetTrans(&m, pl->pos[0], pl->pos[1], pl->pos[2]);
        flmatCopy(pl->rot, &m);
        flCalcTrans(*(void **)(mdl + 0x44), &m);
        player_mat_calc(pl, *(PLNODE **)(mdl + 0x44), &m);
    }
}

void player_mk(void) {
    PLW *p = player_work;
    int i;
    for (i = 0; i < 8; i++) {
        if (p->be_flag != 0) player_modify(p);
        p++;
    }
}

#define PF(p, T, o) (*(T *)((u8 *)(p) + (o)))
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
int PachingerCamChk(PLW *);

void player_trans(PLX *pl) {
    FLMAT m;
    FLMAT m2;
    PLMDL *mdl = pl->mdl;
    CLAY *c;
    CLAY *c2;
    PLMDL *am;
    u8 *mat;
    u8 *amat;
    f32 alpha;
    int i, k, jj, t;
    s16 n;
    u8 aid;

    if (Pl_master_ck((PLW *)pl) == 1 && PachingerCamChk((PLW *)pl) != 0) return;
    pl_light_change((PLW *)pl, 1);
    Pl_light_set((PLW *)pl);
    SetFilterMode(1);
    if (pl->x739 != 0 && softdip_ck(0x7B) == 0) {
        flSetRenderState(0x60, 0);
        alpha = 0.2f;
    } else {
        flSetRenderState(0x60, 0xC0);
        alpha = pl->alpha;
    }
    reload_tex(2, pl->id * 2 + 0xA);
    c = mdl->clay;
    flmatCopy((f32 *)m, (f32 *)pl->rot);
    flSetSkinTrans(mdl->skin);
    mat = pl->mdl->mat;
    for (i = 0; i < mdl->num; i++) {
        if (pl->vis[i] == 1) {
            if (c->handle != -1) {
                for (k = 0; k < c->mat_num; k++) {
                    u8 *mm = mat + c->mat_no[k] * 0x4C;
                    *(f32 *)(mm + 0x10) = alpha;
                    flSetRenderState((k + 0x3A) & 0xFF, (u32)mm);
                }
            }
            clay_attr_set(c->attr);
            flExecuteClay(c->handle, 0);
        }
        c++;
    }
    clay_attr_reset();
    SetFilterMode(1);
    if (pl->wpn_on != 0) {
        weapon_trans(alpha, pl);
    }
    reload_tex(0xC, pl->id * 0xC + 0x3A);
    for (t = 1; t < 6; t++) {
        am = pl->amdl[t];
        aid = pl->armor[6 + t];
        if (am != 0) {
            c2 = am->clay;
            n = am->num;
            flCalcTransSI(am->skin, (FLMAT *)((u8 *)pl->part[armor_pos[t * 2]] + 0x40));
            switch (t) {
            case 5:
            case 3:
                flmatInit(&m2);
                SetPartsTrans2(am->skin, (PLW *)pl, t, aid - 1);
                break;
            case 2:
                if (PF(am->skin, s16, 0xC2) >= 5) {
                    flCalcTransSI(am->skin + 0x640, (FLMAT *)(pl->part[0x14] + 0x40));
                }
                flmatInit(&m2);
                SetPartsTrans2(am->skin, (PLW *)pl, t, aid - 1);
                break;
            default:
                SetPartsTrans(am->skin, (PLW *)pl, t, aid - 1);
                break;
            }
            amat = pl->amdl[t]->mat;
            for (jj = 0; jj < n; jj++) {
                if (c2->handle != -1) {
                    for (k = 0; k < c2->mat_num; k++) {
                        u8 *mm = amat + c2->mat_no[k] * 0x4C;
                        *(f32 *)(mm + 0x10) = alpha;
                        if (t == 2 && jj == 0 && k == 0) {
                            *(f32 *)(mm + 4) = (f32)((pl->col5FC >> 16) & 0xFF) / 255.0f;
                            *(f32 *)(mm + 8) = (f32)((pl->col5FC >> 8) & 0xFF) / 255.0f;
                            *(f32 *)(mm + 0xC) = (f32)(pl->col5FC & 0xFF) / 255.0f;
                            flSetRenderState((k + 0x3A) & 0xFF, (u32)mm);
                        } else {
                            flSetRenderState((k + 0x3A) & 0xFF, (u32)mm);
                        }
                    }
                }
                clay_attr_set(c2->attr);
                flExecuteClay(c2->handle, 0);
                c2++;
            }
            clay_attr_reset();
        }
    }
    pl_item_trans(pl);
    if (pl->x739 != 0) {
        flSetRenderState(0x6C, 1);
    }
    flSetRenderState(0x60, 0);
    light_change_normal(1);
    light_set(1);
    if (pl->pch_on == 0 && (pl->sw_now & 8)) {
        if (act_ck((PLW *)pl, 0, 0x36) != 0) {
            sight_disp_ballista((PLW *)pl);
            return;
        }
        if (pl->kind != 5 && pl->kind != 1) return;
        if (Pl_master_ck((PLW *)pl) == 1 && pl->x763 == 0 && pl->flag12 != 0 && pl->work1C != 0 && pl->flag14 != 2) {
            sight_disp2((PLW *)pl);
        }
    }
}

typedef struct SET_MDLW {
    u8 flag;            /* 0x00 */
    u8 _pad01[0x2F];
    CLAY *clay;         /* 0x30 */
} SET_MDLW;
extern SET_MDLW *set_mdlw;
FLMAT *get_joint_wmat(PLW *, int);
s32 frame_check3(f32, f32, PLW *, int);
void flmatGetTrans(f32 *, FLMAT *);
void flvecApplyMat33_2(f32 *, FLMAT *);
void flmatMul33_2(FLMAT *, FLMAT *);
void flmatMakeTrans(FLMAT *, f32, f32, f32);
void flmatCopy(void *, void *);

void lb_pl_item_trans(PLX *pl) {
    SET_MDLW *mw = set_mdlw;
    f32 p[3];
    f32 v[3];
    FLMAT jm;
    FLMAT m;
    CLAY *c;
    f32 *vy = &v[1];
    f32 *vz = &v[2];
    f32 *py = &p[1];
    f32 *pz = &p[2];

    if (mw == 0 || mw->flag == 0) return;
    if (game_w.stage == 0x4D && pl->st == 1 && pl->char0 != 0x260) {
        v[0] = 0.0f;
        *vy = 0.0f;
        *vz = 0.0f;
        c = mw->clay + 2;
    } else if (pl->char0 == 0x287) {
        if (game_w.stage != 0x57) return;
        if (frame_check3(232.0f, 578.0f, (PLW *)pl, 0) == 0) return;
        c = mw->clay;
        v[0] = 2.08f;
        *vy = 3.02f;
        *vz = -2.0f;
    } else {
        return;
    }
    flmatCopy(jm, get_joint_wmat((PLW *)pl, 0x12));
    flmatGetTrans(p, &jm);
    flvecApplyMat33_2(v, &jm);
    p[0] += v[0];
    *py += *vy;
    *pz += *vz;
    flmatInit(&m);
    flmatRotXYZ33(&m, 0.0f, 0.0f, 0.0f);
    flmatMul33_2(&m, &jm);
    flmatSetTrans(&m, p[0], *py, *pz);
    if (c != 0 && c->handle != -1) {
        flSetRenderState(0x1A, (u32)m);
        clay_attr_set(c->attr);
        flExecuteClay(c->handle, 0);
    }
    clay_attr_reset();
}

void Lb_player_trans(PLX *pl) {
    FLMAT m;
    FLMAT m2;
    FLMAT m3;
    PLMDL *mdl = pl->mdl;
    CLAY *c;
    CLAY *c2;
    PLMDL *am;
    u8 *mat;
    u8 *amat;
    f32 alpha;
    f32 uv;
    int i, k, jj, t;
    s16 n;
    u8 aid;

    pl_light_change((PLW *)pl, 1);
    Pl_light_set((PLW *)pl);
    SetFilterMode(1);
    if (pl->x739 != 0 && softdip_ck(0x7B) == 0) {
        flSetRenderState(0x60, 0);
        alpha = 0.2f;
    } else {
        flSetRenderState(0x60, 0xC0);
        alpha = pl->alpha;
    }
    reload_tex(2, pl->id * 2 + 0xA);
    c = mdl->clay;
    flmatCopy((f32 *)m, (f32 *)pl->rot);
    flSetSkinTrans(mdl->skin);
    mat = pl->mdl->mat;
    for (i = 0; i < mdl->num; i++) {
        if (pl->vis[i] == 1) {
            if (c->handle != -1) {
                for (k = 0; k < c->mat_num; k++) {
                    u8 *mm = mat + c->mat_no[k] * 0x4C;
                    *(f32 *)(mm + 0x10) = alpha;
                    flSetRenderState((k + 0x3A) & 0xFF, (u32)mm);
                }
            }
            clay_attr_set(c->attr);
            flExecuteClay(c->handle, 0);
        }
        c++;
    }
    clay_attr_reset();
    SetFilterMode(1);
    reload_tex(0xC, pl->id * 0xC + 0x3A);
    for (t = 1; t < 6; t++) {
        am = pl->amdl[t];
        aid = pl->armor[6 + t];
        if (am != 0) {
            c2 = am->clay;
            n = am->num;
            flCalcTransSI(am->skin, (FLMAT *)((u8 *)pl->part[armor_pos[t * 2]] + 0x40));
            switch (t) {
            case 3:
            case 5:
                flmatInit(&m2);
                SetPartsTrans2(am->skin, (PLW *)pl, t, aid - 1);
                break;
            case 2:
                if (PF(am->skin, s16, 0xC2) >= 5) {
                    flCalcTransSI(am->skin + 0x640, (FLMAT *)(pl->part[0x14] + 0x40));
                }
                flmatInit(&m2);
                SetPartsTrans2(am->skin, (PLW *)pl, t, aid - 1);
                break;
            default:
                SetPartsTrans(am->skin, (PLW *)pl, t, aid - 1);
                break;
            }
            amat = pl->amdl[t]->mat;
            for (jj = 0; jj < n; jj++) {
                if (c2->handle != -1) {
                    for (k = 0; k < c2->mat_num; k++) {
                        u8 *mm = amat + c2->mat_no[k] * 0x4C;
                        *(f32 *)(mm + 0x10) = alpha;
                        if (t == 2 && jj == 0 && k == 0) {
                            *(f32 *)(mm + 4) = (f32)((pl->col5FC >> 16) & 0xFF) / 255.0f;
                            *(f32 *)(mm + 8) = (f32)((pl->col5FC >> 8) & 0xFF) / 255.0f;
                            *(f32 *)(mm + 0xC) = (f32)(pl->col5FC & 0xFF) / 255.0f;
                            flSetRenderState((k + 0x3A) & 0xFF, (u32)mm);
                        } else {
                            if (t == 1 && pl->x11 == 1) {
                                switch (pl->work39C % 0x78) {
                                case 0:
                                case 1:
                                case 7:
                                case 8:
                                case 9:
                                    uv = 0.166f;
                                    break;
                                case 2:
                                case 3:
                                case 4:
                                case 5:
                                case 6:
                                    uv = 0.332f;
                                    break;
                                default:
                                    uv = 0.0f;
                                    break;
                                }
                                flmatMakeTrans(&m3, uv, 0.0f, 0.0f);
                                flSetRenderState(0x19, (u32)m3);
                            }
                            flSetRenderState((k + 0x3A) & 0xFF, (u32)mm);
                        }
                    }
                }
                clay_attr_set(c2->attr);
                flExecuteClay(c2->handle, 0);
                c2++;
            }
            clay_attr_reset();
        }
    }
    lb_pl_item_trans(pl);
    if (pl->x739 != 0) {
        flSetRenderState(0x6C, 1);
    }
    flSetRenderState(0x60, 0);
    pl_light_change((PLW *)pl, 1);
    light_set(1);
}

/* ---- pl_item_trans_sub: draw one item model with per-type colouring ---- */
extern u8 bone_col[][3];
extern u8 meat_col[][3];

void pl_item_trans_sub(PLX *pl, CLAY *c, FLMAT *m, u8 *mat, u8 type) {
    int k;
    s16 idx;
    int g;
    u8 *mm;

    if (c == 0 || c->handle == -1) return;
    if (type != 5) {
        switch (type) {
        case 1:
            idx = pl->x8D0;
            break;
        case 2:
            switch (pl->x88A) {
            case 19:
                idx = 1;
                break;
            case 20:
                idx = 2;
                break;
            case 21:
                idx = 3;
                break;
            default:
                return;
            }
            break;
        case 3:
            g = 0x3F;
            break;
        case 4:
            g = 0xFF;
            break;
        default:
            return;
        }
    } else {
        g = 0x9F;
    }
    flSetRenderState(0x1A, (u32)m);
    for (k = 0; k < c->mat_num; k++) {
        mm = mat + c->mat_no[k] * 0x4C;
        *(f32 *)(mm + 0x10) = pl->alpha;
        switch (type) {
        case 2:
        case 1:
            switch (k) {
            case 0:
                *(f32 *)(mm + 4) = (f32)meat_col[idx][0] / 255.0f;
                *(f32 *)(mm + 8) = (f32)meat_col[idx][1] / 255.0f;
                *(f32 *)(mm + 0xC) = (f32)meat_col[idx][2] / 255.0f;
                break;
            case 1:
                *(f32 *)(mm + 4) = (f32)bone_col[idx][0] / 255.0f;
                *(f32 *)(mm + 8) = (f32)bone_col[idx][1] / 255.0f;
                *(f32 *)(mm + 0xC) = (f32)bone_col[idx][2] / 255.0f;
                break;
            }
            break;
        case 5:
        case 4:
        case 3:
            if (k == 0) {
                *(f32 *)(mm + 4) = (f32)g / 255.0f;
                *(f32 *)(mm + 8) = (f32)g / 255.0f;
                *(f32 *)(mm + 0xC) = (f32)g / 255.0f;
            }
            break;
        }
        flSetRenderState((k + 0x3A) & 0xFF, (u32)mm);
    }
    clay_attr_set(c->attr);
    flExecuteClay(c->handle, 0);
}

/* ---- weapon_trans: weapon model placed on the player's hand ---- */
typedef struct WDISP {
    f32 p[3];           /* 0x00 offset from the joint */
    f32 r[3];           /* 0x0C rotation (radians) */
} WDISP;
extern WDISP weapon_disp_tbl_r[];
extern WDISP weapon_disp_tbl_l[];
extern WDISP weapon_disp_tbl_b[];
extern WDISP weapon_disp_tbl_ex[];
extern f32 we00_0413_xz03[], we00_1002_xz03[], we00_1003_xz03[], we00_1017_xz03[], we00_1410_xz03[];
extern f32 we00_1002_z[], we00_1003_z[], we00_1017_z[], we00_1410_z[], we00_413_z[];
extern f32 we02_1404_x[], we02_1406_x[];
extern f32 we06_1002_z[], we06_1003_z[], we06_1009_z[], we06_1010_z[], we06_1018_z[], we06_413_z[];
extern f32 we11_1002_r[], we11_1002_t[], we11_1003_r[], we11_1003_t[], we11_1009_r[], we11_1009_t[];
extern f32 we11_1010_r[], we11_1010_t[], we11_1401_y[], we11_1401_z[], we11_1404_y[], we11_1404_z[];
extern f32 we11_1405_y[], we11_1405_y2[], we11_1405_z[], we11_1405_z2[];
extern f32 we13_1002_rx[], we13_1002_y[], we13_1002_z[], we13_1003_rx[], we13_1003_y[], we13_1003_z[];
extern f32 we13_1401_y[], we13_1401_z[], we13_1404_y[], we13_1404_z[], we13_1405_y[], we13_1405_y2[];
extern f32 we13_1405_z[], we13_1405_z2[], we13_1408_y[], we13_1408_z[];

f32 weapon_dat_make(PLW *, f32 *);
void weapon_dat_make2(PLW *, f32 *, f32 *);
void weapon_dat_make3(PLW *, f32 *, f32 *, f32 *);
s16 weapon_joint_calc(PLW *);
void SetVector(f32 *, f32, f32, f32);
void flmatAddTrans2(FLMAT *, f32 *);
void flmatSetXYZ33(FLMAT *, f32, f32, f32);
void flvecApplyMat33(f32 *, f32 *, f32 *);
int Pl_barrel_ck(PLW *);
int Pl_silencer_ck(PLW *);

#define DEG2RAD(d) (2.0f * (3.1415927f * ((d) / 360.0f)))

void weapon_trans(f32 alpha, PLX *pl) {
    f32 p[3];
    f32 r[3];
    f32 o[3];
    f32 t[3];
    f32 s[3];
    FLMAT m0;
    FLMAT m1;
    FLMAT m2;
    PLMDL *wm;
    WNODE *nd;
    CLAY *c;
    u8 *mat;
    u8 *mm;
    WDISP *tb;
    int jt;
    int i, k;
    int pi;
    f32 sc = 1.0f;
    f32 f, g, h;

    flSetRenderState(0x67, -1);
    reload_tex(4, pl->id * 4 + 0x1A);
    wm = pl->wmdl;
    jt = weapon_joint_calc((PLW *)pl);
    c = wm->clay;
    nd = (WNODE *)wm->skel;
    if (jt != 2) {
        pi = (jt == 0) ? 0x12 : 0xE;
        flmatCopy(&m0, pl->part[pi] + 0x40);
        if (jt == 0) {
            tb = &weapon_disp_tbl_r[pl->kind];
        } else {
            tb = &weapon_disp_tbl_l[pl->kind];
        }
        if (pl->char0 == 0x19D) tb = &weapon_disp_tbl_ex[pl->kind];
        p[0] = tb->p[0];
        p[1] = tb->p[1];
        p[2] = tb->p[2];
        r[0] = tb->r[0];
        r[1] = tb->r[1];
        r[2] = tb->r[2];
        flvecApplyMat33(o, p, (f32 *)(pl->part[pi] + 0x40));
        flmatAddTrans2(&m0, o);
        flmatInit(&m1);
        if (pl->kind == 3 || pl->kind == 5) {
            flmatGetTrans(t, &nd[2].m);
            flmatMakeScale(&nd[2].m, 1.0f, 1.0f, 1.0f);
            flmatSetTrans(&nd[2].m, t[0], t[1], t[2]);
        }
    } else {
        if (pl->kind == 4) {
            flmatCopy(&m0, pl->part[9] + 0x40);
        } else {
            flmatCopy(&m0, pl->part[10] + 0x40);
        }
        if (pl->kind == 5 && (pl->char0 == 0x3EA || pl->char0 == 0x3EB || pl->char0 == 0x3F1 || pl->char0 == 0x3F2) && pl->x1C4 == 0) {
            switch (pl->char0) {
            case 0x3EA:
                weapon_dat_make2((PLW *)pl, we11_1002_t, p);
                weapon_dat_make2((PLW *)pl, we11_1002_r, r);
                break;
            case 0x3EB:
                weapon_dat_make2((PLW *)pl, we11_1003_t, p);
                weapon_dat_make2((PLW *)pl, we11_1003_r, r);
                break;
            case 0x3F1:
                weapon_dat_make2((PLW *)pl, we11_1009_t, p);
                weapon_dat_make2((PLW *)pl, we11_1009_r, r);
                break;
            case 0x3F2:
                weapon_dat_make2((PLW *)pl, we11_1010_t, p);
                weapon_dat_make2((PLW *)pl, we11_1010_r, r);
                break;
            }
            r[0] = DEG2RAD(r[0]);
            r[1] = DEG2RAD(r[1]);
            r[2] = DEG2RAD(r[2]);
        } else {
            tb = &weapon_disp_tbl_b[pl->kind];
            p[0] = tb->p[0];
            p[1] = tb->p[1];
            p[2] = tb->p[2];
            r[0] = tb->r[0];
            r[1] = tb->r[1];
            r[2] = tb->r[2];
        }
        flvecApplyMat33(o, p, (f32 *)(pl->part[10] + 0x40));
        flmatAddTrans2(&m0, o);
        switch (pl->kind) {
        case 1:
            flmatGetTrans(t, &nd[3].m);
            flmatMakeScale(&nd[3].m, 1.0f, 0.0f, 3.0f);
            flmatSetTrans(&nd[3].m, t[0], t[1], t[2]);
            sc = 0.8f;
            break;
        case 5:
            flmatGetTrans(t, &nd[3].m);
            flmatMakeScale(&nd[3].m, 1.0f, 0.0f, 3.0f);
            flmatSetTrans(&nd[3].m, t[0], t[1], t[2]);
            flmatGetTrans(t, &nd[2].m);
            flmatMakeScale(&nd[2].m, 1.0f, 1.0f, 0.5f);
            flmatSetTrans(&nd[2].m, t[0], t[1], t[2]);
            sc = 1.0f;
            break;
        default:
            sc = 0.8f;
            break;
        case 4:
            sc = 1.0f;
            break;
        case 3:
            sc = 0.9f;
            flmatGetTrans(t, &nd[2].m);
            flmatMakeScale(&nd[2].m, 1.0f, 1.0f, 0.45f);
            flmatSetTrans(&nd[2].m, t[0], t[1], t[2]);
            break;
        }
    }
    flmatInit(&nd[0].m);
    flmatSetXYZ33(&nd[0].m, r[0], r[1], r[2]);
    flmatMakeScale(&m2, sc, sc, sc);
    flmatMul33_2(&nd[0].m, &m2);
    switch (pl->kind) {
    case 2:
        flmatGetTrans(t, &nd[3].m);
        f = 1.0f;
        if (pl->x87C > 0 && pl->x87C / 30 >= 2) f = 1.5f;
        switch (pl->char0) {
        case 0x57C:
            if (pl->x1C4 == 0) f = weapon_dat_make((PLW *)pl, we02_1404_x);
            else f = 1.5f;
            break;
        case 0x57E:
            if (pl->x1C4 == 0) f = weapon_dat_make((PLW *)pl, we02_1406_x);
            else f = 1.5f;
            break;
        }
        flmatMakeScale(&nd[3].m, f, f, f);
        flmatSetTrans(&nd[3].m, t[0], t[1], t[2]);
        break;
    case 0:
        flmatGetTrans(t, &nd[2].m);
        flmatGetTrans(p, &nd[3].m);
        if (pl->flag12 != 0 && jt != 2) {
            SetVector(s, 2.6f, 1.0f, 2.5f);
        } else {
            SetVector(s, 1.0f, 1.0f, 1.0f);
        }
        switch (pl->char0) {
        case 0x19D:
            if (pl->x1C4 == 0) {
                f = weapon_dat_make((PLW *)pl, we00_413_z);
                weapon_dat_make3((PLW *)pl, we00_0413_xz03, &s[0], &s[2]);
            } else {
                f = 0.73f;
            }
            break;
        case 0x582:
        case 0x3F8:
        case 0x3EA:
            if (pl->x1C4 == 0) {
                if (pl->char0 != 0x582) {
                    f = weapon_dat_make((PLW *)pl, we00_1002_z);
                    weapon_dat_make3((PLW *)pl, we00_1002_xz03, &s[0], &s[2]);
                } else {
                    f = weapon_dat_make((PLW *)pl, we00_1410_z);
                    weapon_dat_make3((PLW *)pl, we00_1410_xz03, &s[0], &s[2]);
                }
            } else {
                f = 0.73f;
                SetVector(s, 1.0f, 1.0f, 1.0f);
            }
            break;
        case 0x3F9:
        case 0x3EB:
            if (pl->x1C4 == 0) {
                if (pl->char0 == 0x3EB) {
                    f = weapon_dat_make((PLW *)pl, we00_1003_z);
                    weapon_dat_make3((PLW *)pl, we00_1003_xz03, &s[0], &s[2]);
                } else {
                    f = weapon_dat_make((PLW *)pl, we00_1017_z);
                    weapon_dat_make3((PLW *)pl, we00_1017_xz03, &s[0], &s[2]);
                }
            } else {
                f = 1.0f;
                SetVector(s, 2.6f, 1.0f, 2.5f);
            }
            break;
        default:
            if (pl->flag12 != 0 && jt != 2) f = 1.0f;
            else f = 0.73f;
            break;
        }
        flmatMakeScale(&nd[2].m, 1.0f, 1.0f, f);
        flmatSetTrans(&nd[2].m, t[0], t[1], t[2]);
        flmatMakeScale(&nd[3].m, s[0], s[1], s[2]);
        flmatSetTrans(&nd[3].m, p[0], p[1], p[2]);
        break;
    case 3:
        flmatGetTrans(t, &nd[2].m);
        switch (pl->char0) {
        case 0x19D:
            if (pl->x1C4 == 0) f = weapon_dat_make((PLW *)pl, we06_413_z);
            else f = 0.45f;
            break;
        case 0x3F2:
        case 0x3EA:
            if (pl->x1C4 == 0) {
                if (pl->char0 != 0x3F2) f = weapon_dat_make((PLW *)pl, we06_1002_z);
                else f = weapon_dat_make((PLW *)pl, we06_1010_z);
            } else {
                f = 0.45f;
            }
            break;
        case 0x3F1:
        case 0x3EB:
            if (pl->x1C4 == 0) {
                if (pl->char0 == 0x3EB) f = weapon_dat_make((PLW *)pl, we06_1003_z);
                else f = weapon_dat_make((PLW *)pl, we06_1009_z);
            } else {
                f = 1.0f;
            }
            break;
        case 0x3FA:
            if (pl->x1C4 == 0) f = weapon_dat_make((PLW *)pl, we06_1018_z);
            else f = 0.45f;
            break;
        default:
            if (pl->flag12 != 0 && jt != 2) f = 1.0f;
            else f = 0.45f;
            break;
        }
        flmatMakeScale(&nd[2].m, 1.0f, 1.0f, f);
        flmatSetTrans(&nd[2].m, t[0], t[1], t[2]);
        break;
    case 5:
    case 1:
        if (pl->kind == 5) {
            flmatGetTrans(p, &nd[3].m);
        } else {
            flmatGetTrans(p, &nd[3].m);
            flmatSetXYZ33(&nd[2].m, 3.1415927f, 0.0f, 0.0f);
        }
        h = 1.0f;
        g = 1.0f;
        if (pl->x1C4 == 0) {
            switch (pl->char0) {
            case 0x579:
                if (pl->kind == 5) {
                    g = weapon_dat_make((PLW *)pl, we11_1401_z);
                    weapon_dat_make((PLW *)pl, we11_1401_y);
                } else {
                    g = weapon_dat_make((PLW *)pl, we13_1401_z);
                    h = weapon_dat_make((PLW *)pl, we13_1401_y);
                }
                break;
            case 0x57D:
                if (pl->work1C == 0) {
                    if (pl->kind == 5) {
                        g = weapon_dat_make((PLW *)pl, we11_1405_z);
                        weapon_dat_make((PLW *)pl, we11_1405_y);
                    } else {
                        g = weapon_dat_make((PLW *)pl, we13_1405_z);
                        h = weapon_dat_make((PLW *)pl, we13_1405_y);
                    }
                } else if (pl->kind == 5) {
                    g = weapon_dat_make((PLW *)pl, we11_1405_z2);
                    weapon_dat_make((PLW *)pl, we11_1405_y2);
                } else {
                    g = weapon_dat_make((PLW *)pl, we13_1405_z2);
                    h = weapon_dat_make((PLW *)pl, we13_1405_y2);
                }
                break;
            case 0x580:
                g = weapon_dat_make((PLW *)pl, we13_1408_z);
                if (pl->kind == 5) weapon_dat_make((PLW *)pl, we13_1408_y);
                else h = weapon_dat_make((PLW *)pl, we13_1408_y);
                break;
            case 0x57C:
                if (pl->kind == 5) {
                    g = weapon_dat_make((PLW *)pl, we11_1404_z);
                    weapon_dat_make((PLW *)pl, we11_1404_y);
                } else {
                    g = weapon_dat_make((PLW *)pl, we13_1404_z);
                    h = weapon_dat_make((PLW *)pl, we13_1404_y);
                }
                break;
            case 0x3EA:
                if (pl->kind == 1) {
                    flmatSetXYZ33(&nd[2].m, DEG2RAD(weapon_dat_make((PLW *)pl, we13_1002_rx)), 0.0f, 0.0f);
                    g = weapon_dat_make((PLW *)pl, we13_1002_z);
                    h = weapon_dat_make((PLW *)pl, we13_1002_y);
                }
                break;
            case 0x3EB:
                if (pl->kind == 1) {
                    flmatSetXYZ33(&nd[2].m, DEG2RAD(weapon_dat_make((PLW *)pl, we13_1003_rx)), 0.0f, 0.0f);
                    h = weapon_dat_make((PLW *)pl, we13_1003_y);
                }
                break;
            case 0x3F1:
                if (pl->kind == 1) {
                    flmatSetXYZ33(&nd[2].m, DEG2RAD(weapon_dat_make((PLW *)pl, we13_1003_rx)), 0.0f, 0.0f);
                    g = weapon_dat_make((PLW *)pl, we13_1003_z);
                    h = weapon_dat_make((PLW *)pl, we13_1003_y);
                }
                break;
            default:
                if (pl->flag12 != 0 && jt != 2) {
                    if (pl->work1C != 0) {
                        if (pl->kind == 5) {
                            g = 2.0f;
                        } else {
                            h = 0.8f;
                            g = 3.0f;
                        }
                    }
                } else {
                    flmatSetXYZ33(&nd[1].m, 0.0f, 0.0f, 0.0f);
                    if (pl->kind == 1) flmatSetXYZ33(&nd[2].m, 0.0f, 0.0f, 0.0f);
                    flmatSetXYZ33(&nd[3].m, 0.0f, 0.0f, 0.0f);
                    if (pl->kind == 5) {
                        g = 2.2f;
                    } else {
                        h = -0.5f;
                        g = 3.0f;
                    }
                }
                break;
            }
        } else {
            if (pl->work1C != 0 ? pl->char0 != 0x57C : 0) {
                goto blk199;
            }
            if (pl->char0 == 0x579) {
            blk199:
                if (pl->kind == 5) {
                    g = 2.0f;
                } else {
                    h = 0.8f;
                    g = 3.0f;
                }
            }
            if (pl->char0 == 0x3EB || pl->char0 == 0x3F1 || (pl->flag12 != 0 && jt != 2)) {
                if (pl->char0 == 0x3EA) goto blk208;
            } else {
            blk208:
                flmatSetXYZ33(&nd[1].m, 0.0f, 0.0f, 0.0f);
                if (pl->kind == 1) flmatSetXYZ33(&nd[2].m, 0.0f, 0.0f, 0.0f);
                flmatSetXYZ33(&nd[3].m, 0.0f, 0.0f, 0.0f);
                if (pl->kind == 5) {
                    g = 2.2f;
                } else {
                    h = -0.5f;
                    g = 3.0f;
                }
            }
        }
        flmatMakeScale(&nd[3].m, h, sc, g);
        flmatSetTrans(&nd[3].m, p[0], p[1], p[2]);
        break;
    }
    flCalcTransSI(wm->skel, (FLMAT *)&m0);
    flSetSkinTrans(wm->skel);
    mat = pl->wmdl->mat;
    for (i = 0; i < wm->num; i++) {
        if (i != 0 && (pl->kind == 3 || pl->kind == 4)) {
            WNODE *n2 = (WNODE *)wm->skel2;
            flmatCopy(&m0, get_joint_wmat((PLW *)pl, 0x11));
            if (pl->kind == 4) {
                flmatSetXYZ33(&n2->m, -0.453785628f, -0.0523598827f, 0.139626354f);
                p[0] = -21.0f;
                p[1] = 2.2f;
                p[2] = 5.0f;
            } else {
                flmatSetXYZ33(&n2->m, 0.366519153f, -3.00196648f, -0.0436332338f);
                p[0] = -18.0f;
                p[1] = 2.0f;
                p[2] = -14.0f;
            }
            flvecApplyMat33(o, p, (f32 *)&m0);
            flmatAddTrans2(&m0, o);
            flCalcTransSI(wm->skel2, (FLMAT *)&m0);
            flSetSkinTrans(wm->skel2);
        }
        if (c->handle != -1) {
            for (k = 0; k < c->mat_num; k++) {
                mm = mat + c->mat_no[k] * 0x4C;
                *(f32 *)(mm + 0x10) = alpha;
                if (pl->kind == 1 || pl->kind == 5) {
                    if (k == 1 && Pl_barrel_ck((PLW *)pl) == 0) *(f32 *)(mm + 0x10) = 0.0f;
                    if (k == 2 && Pl_silencer_ck((PLW *)pl) == 0) *(f32 *)(mm + 0x10) = 0.0f;
                }
                flSetRenderState((k + 0x3A) & 0xFF, (u32)mm);
            }
        }
        flExecuteClay(c->handle, 0);
        c++;
    }
}

/* ---- Ed_player_trans: player model in the character editor ---- */
extern struct { u8 _pad00[4]; u8 x4; u8 x5; u8 x6; u8 x7; u32 col; } edit_w;
extern PLMDL *edit_mdlw[2];
extern u8 skin_col_tbl_m[];
extern u8 skin_col_tbl_f[];

void Ed_player_trans(PLX *pl) {
    FLMAT m;
    PLMDL *mdl = pl->mdl;
    PLMDL *em;
    CLAY *c;
    u8 *mat;
    u8 *mm;
    int k;

    pl_light_change((PLW *)pl, 1);
    Pl_light_set((PLW *)pl);
    flSetRenderState(0x60, 0xC0);
    reload_tex(0x32, edit_w.x4 * 0x32 + 0xA);
    flmatCopy(m, pl->rot);
    flSetSkinTrans(mdl->skin);
    SetFilterMode(1);
    if (edit_w.x4 == 0) {
        em = edit_mdlw[0];
        mat = em->mat;
        k = (3 - skin_col_tbl_m[edit_w.x5]) & 0xFF;
    } else {
        em = edit_mdlw[1];
        mat = em->mat;
        k = (3 - skin_col_tbl_f[edit_w.x5]) & 0xFF;
    }
    c = em->clay + k;
    if (c->handle != -1) {
        for (k = 0; k < c->mat_num; k++) {
            flSetRenderState((k + 0x3A) & 0xFF, (u32)(mat + c->mat_no[k] * 0x4C));
        }
    }
    clay_attr_set(c->attr);
    flExecuteClay(c->handle, 0);
    c = em->clay + edit_w.x5 + 4;
    if (c->handle != -1) {
        for (k = 0; k < c->mat_num; k++) {
            flSetRenderState((k + 0x3A) & 0xFF, (u32)(mat + c->mat_no[k] * 0x4C));
        }
    }
    clay_attr_set(c->attr);
    flExecuteClay(c->handle, 0);
    c = em->clay + edit_w.x7 + 0x1C;
    if (c->handle != -1) {
        for (k = 0; k < c->mat_num; k++) {
            mm = mat + c->mat_no[k] * 0x4C;
            if (k == 0) {
                *(f32 *)(mm + 4) = (f32)((edit_w.col >> 16) & 0xFF) / 255.0f;
                *(f32 *)(mm + 8) = (f32)((edit_w.col >> 8) & 0xFF) / 255.0f;
                *(f32 *)(mm + 0xC) = (f32)(edit_w.col & 0xFF) / 255.0f;
                flSetRenderState((k + 0x3A) & 0xFF, (u32)mm);
            }
            flSetRenderState((k + 0x3A) & 0xFF, (u32)mm);
        }
    }
    clay_attr_set(c->attr);
    flExecuteClay(c->handle, 0);
    clay_attr_reset();
    light_change_normal(1);
    light_set(1);
}

/* ---- enemy_trans: monster model draw (display callback, arg = {.., em at +0x18}) ---- */
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
    FLMAT ma;
    FLMAT mb;
    FLMAT mc;
    f32 ang[3];
    s16 *lp;
    s16 v;
    s16 n;
    CLAY *cb;
    CLAY *c;
    int fgo;
    int i;
    int k;
    u8 *mat;
    u8 *mm;

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
    lp = em_alpha_clay[em->kind];
    v = *lp;
    if (em->kind == 3) {
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
    c = cb;
    for (i = 0; i < n; i++) {
        if (em->vis[i] != 0) {
            for (;;) {
                if (v == -1) break;
                if (v < i) {
                    lp++;
                    v = *lp;
                    continue;
                }
                break;
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
            if (c->handle != -1) {
                switch (em->kind) {
                case 23:
                case 18:
                case 9:
                    func_5ACA60(em, i, cb);
                    break;
                case 20:
                    func_5FCBB0(em, i, cb);
                    break;
                default:
                    em_material_sub(em, i, cb);
                    break;
                }
                clay_attr_set(c->attr);
                if (fgo == 1) {
                    flSetMatrixList(mat_list + em->x34F * 0x420 + i * 0x42, mat_palett);
                }
                flExecuteClay(c->handle, 0);
            }
        }
        c++;
    }
    clay_attr_reset();
    flSetRenderState(0x60, 0);
    light_change_normal(1);
    light_set(1);
}
