/* eft08 - game.bin 0x00543FA0-0x00544DCC. A small puff/burst effect with
 * seven types (arg): each spawns 1-6 sprites that grow and fade along
 * keyframe tables (eft08_data, picked through eft08_index/param), tinted
 * by x07 (white, orange or dust). Placed at a point (Eft08_set) or at a
 * player (Eft08_set2). */
#include "eft.h"
#include "game.h"
#include "pl.h"
#include "prim.h"
#include "clay.h"
#include "fl.h"

typedef struct EFT_MDLW {
    u8 flag;            /* 0x00 */
    u8 _pad01[0x0F];
    void *mat;          /* 0x10 material table */
    u8 _pad14[0x1C];
    CLAY *clay;         /* 0x30 */
} EFT_MDLW;

/* One sprite (0x34 bytes) of the work area. */
typedef struct EFT08_PUFF {
    s16 prim_no;        /* 0x00 */
    s16 no;             /* 0x02 */
    f32 scale[3];       /* 0x04 */
    f32 ofs[3];         /* 0x10 */
    u32 col;            /* 0x1C used when noalpha is set */
    u16 rot;            /* 0x20 */
    s16 drot;           /* 0x22 */
    f32 size;           /* 0x24 */
    PRIM *prim;         /* 0x28 */
    s16 time;           /* 0x2C */
    u16 noalpha;        /* 0x2E */
    f32 alpha;          /* 0x30 */
} EFT08_PUFF;

#define ANG2DEG(a) (360.0f * (f32)(a) / 65536.0f)
#define DEG2RAD(d) (2.0f * (3.1415927f * ((d) / 360.0f)))

extern EFT_MDLW *eft_mdlw[5];
extern FLMAT rview_mat;
extern void *eft08_data[];
extern s16 eft08_num[7];
extern u16 eft08_all_time[7];
extern s16 eft08_param[7];
extern s16 eft08_index[7];

u32 ran_suu(int);
u8 Pl_stg_ck(PLW *);
void release_prim(s16);
EFTW *pull_eft_work2(int);
void get_joint_pos(EMW *, int, f32 *);
void flvecCopy(f32 *, f32 *);
void eft_vec_linear(f32, void *, f32 *);
void eft_alpha_linear(f32, void *, f32 *);
void make_mat_srt(f32 *, f32 *, f32 *, int, FLMAT *);
void eft_trans_sub_col(CLAY *, FLMAT *, u32, u16, void *);

static void eft08_move(EFTW *ew);
static void eft08_i(EFTW *ew);
static void eft08_m(EFTW *ew);
static void eft08_d(EFTW *ew);
static void eft08_e(EFTW *ew);
static void eft08_t(PRIM *pr);
static f32 eft08_ran_suu_sub2(void);
static EFTW *eft08_set_com(int arg);

static void eft08_move(EFTW *ew) {
    switch (ew->mode) {
    case 0:
        eft08_i(ew);
        break;
    case 1:
        eft08_m(ew);
        break;
    case 2:
        eft08_d(ew);
        break;
    case 3:
        eft08_e(ew);
        break;
    }
}

static void eft08_i(EFTW *ew) {
    EFT08_PUFF *p = ew->work;
    s16 n;
    s16 i;
    f32 y;

    ew->mode++;
    ew->be_flag = 1;
    ew->work14 = 0;
    ew->timer = 0;
    n = eft08_num[ew->arg];
    switch (ew->arg) {
    case 0:
        ew->pos[1] += 20.0f;
        break;
    case 6:
        y = ew->pos[1];
        ew->pos[0] = 0.0f;
        ew->pos[1] = 0.0f;
        ew->pos[2] = 0.0f;
        break;
    }
    for (i = 0; i < n; i++, p++) {
        p->prim_no = get_prim();
        p->no = i;
        p->time = 0;
        p->size = ew->scale;
        p->noalpha = 0;
        p->ofs[0] = 0.0f;
        p->ofs[1] = 0.0f;
        p->ofs[2] = 0.0f;
        switch (ew->arg) {
        case 0:
            p->rot = ran_suu(1);
            p->drot = 0;
            break;
        case 1:
        case 5:
            p->no = (u16)ran_suu(1) & 7;
            p->time = -(i & 1) * 4;
            p->rot = 0;
            p->drot = 0;
            p->ofs[0] = i * (10.0f * ew->scale * eft08_ran_suu_sub2());
            p->ofs[1] = 0.0f;
            p->ofs[2] = i * (10.0f * ew->scale * eft08_ran_suu_sub2());
            break;
        case 2:
            p->no = (u16)ran_suu(1) & 7;
            p->rot = 0;
            p->drot = 0;
            break;
        case 3:
            p->rot = ran_suu(1);
            p->drot = (ran_suu(1) & 0xFF) - 0x80;
            break;
        case 4:
            p->rot = ran_suu(1);
            p->drot = (ran_suu(1) & 0xFF) - 0x80;
            break;
        case 6:
            p->time = -i * 2;
            p->ofs[1] = y;
            p->no = (u16)ran_suu(1) & 7;
            p->rot = 0;
            p->drot = 0;
            break;
        }
        if (p->prim_no != -1) {
            p->prim = get_prim_ptr(p->prim_no);
            p->prim->owner = ew;
            p->prim->no = i;
            p->prim->trans = eft08_t;
        } else {
            p->prim = 0;
        }
    }
}

static void eft08_m(EFTW *ew) {
    f32 v[3];
    EFT08_PUFF *p = ew->work;
    u16 lim;
    s16 i;
    void *d;
    s16 num = eft08_num[ew->arg];
    u16 all = eft08_all_time[ew->arg];
    s16 idx = eft08_index[ew->arg];
    s16 step = eft08_param[ew->arg];

    switch (ew->arg) {
    default:
        lim = all;
        break;
    case 1:
    case 5:
        lim = 20;
        break;
    case 6:
        lim = 10;
        break;
    }
    if (++ew->timer > all) {
        ew->mode++;
        ew->be_flag = 0;
        return;
    }
    for (i = 0; i < num; i++) {
        switch (ew->arg) {
        case 1:
        case 5:
        case 6:
            idx = eft08_index[(u8)ew->arg];
            break;
        }
        if (++p->time <= 0) {
            idx += step;
            p++;
            continue;
        }
        if (p->time == 1) {
            if (ew->arg == 6) {
                if (ew->owner->be_flag != 0) {
                    get_joint_pos(ew->owner, 2, v);
                    p->ofs[0] = v[0];
                    p->ofs[2] = v[2];
                } else {
                    idx += step;
                    p++;
                    continue;
                }
            }
        } else if (p->time > lim) {
            idx += step;
            p++;
            continue;
        }
        d = eft08_data[idx++];
        eft_vec_linear(p->time, d, p->scale);
        if (p->noalpha == 0) {
            d = eft08_data[idx++];
            eft_alpha_linear(p->time, d, &p->alpha);
        }
        p->rot += p->drot;
        switch (ew->arg) {
        case 0:
            if (p->time > 4) {
                p->ofs[1] -= 10.0f / (f32)(lim - 4);
            }
            break;
        }
        if (p->prim != 0) {
            p->prim->pos[0] = ew->pos[0] + p->ofs[0];
            p->prim->pos[1] = ew->pos[1] + p->ofs[1];
            p->prim->pos[2] = ew->pos[2] + p->ofs[2];
            add_prim(ot0, p->prim, 0x40, 0);
        }
        p++;
    }
}

static void eft08_d(EFTW *ew) {
    EFT08_PUFF *p = ew->work;
    s16 n;
    s16 i;

    ew->mode++;
    n = eft08_num[ew->arg];
    for (i = 0; i < n; i++, p++) {
        if (p->prim != 0) {
            release_prim(p->prim_no);
        }
    }
}

static void eft08_e(EFTW *ew) {
    push_eft_work(ew);
}

static void eft08_t(PRIM *pr) {
    f32 sc[3];
    f32 rot[3];
    FLMAT m;
    FLMAT uv;
    EFTW *ew = pr->owner;
    EFT08_PUFF *p = &((EFT08_PUFF *)ew->work)[pr->no];
    EFT_MDLW *mw = eft_mdlw[0];
    u16 flag = 0;
    void *mats;
    CLAY *cl;
    u8 r, g, b;

    if (mw != 0 && mw->flag != 0) {
        mats = mw->mat;
        sc[0] = p->size * p->scale[0];
        sc[1] = p->size * p->scale[1];
        sc[2] = p->size * p->scale[2];
        flSetRenderState(0x6C, 0);
        if (ew->x07 == 0) {
            r = 0xFF;
            g = r;
            b = r;
        } else if (ew->x07 == 1) {
            r = 0xFF;
            g = 0x7F;
            b = 0;
        } else {
            r = 0xD8;
            g = 0xB6;
            b = 0x8A;
        }
        switch (ew->arg) {
        case 0:
            if (ew->x07 == 1) {
                flag |= 2;
            }
            cl = &mw->clay[94];
            rot[2] = DEG2RAD(ANG2DEG(p->rot));
            make_mat_srt(sc, rot, pr->pos, 2, &m);
            flmatMul33_2(&m, &rview_mat);
            flmatMakeTrans(&uv, 0.0f, 0.0f, 0.0f);
            flSetRenderState(0x19, (u32)&uv);
            break;
        case 1:
        case 2:
        case 5:
        case 6:
            if (ew->x07 == 1) {
                flag |= 2;
            }
            cl = &mw->clay[26];
            rot[0] = 0.0f;
            if (p->no & 1) {
                rot[1] = 3.1415927f;
            } else {
                rot[1] = 0.0f;
            }
            rot[2] = DEG2RAD(ANG2DEG(p->rot));
            make_mat_srt(sc, rot, pr->pos, 0xE, &m);
            flmatMul33_2(&m, &rview_mat);
            flmatMakeTrans(&uv, 0.125f * (p->no >> 1), 0.0f, 0.0f);
            flSetRenderState(0x19, (u32)&uv);
            break;
        case 3:
        case 4:
            cl = &mw->clay[93];
            rot[1] = DEG2RAD(ANG2DEG(p->rot));
            make_mat_srt(sc, rot, pr->pos, 4, &m);
            flmatMakeTrans(&uv, 0.75f, 0.75f, 0.0f);
            flSetRenderState(0x19, (u32)&uv);
            break;
        }
        eft_trans_sub_col(cl, &m,
                          p->noalpha == 0 ? ((u8)(255.0f * p->alpha) << 24) | (r << 16) | (g << 8) | b : p->col,
                          flag, mats);
        flSetRenderState(0x6C, 1);
    }
}

static f32 eft08_ran_suu_sub2(void) {
    return 0.001f * (((u16)ran_suu(1) & 0x3FF) - 0x200);
}

static EFTW *eft08_set_com(int arg) {
    return pull_eft_work2(1);
}

void Eft08_set(f32 *pos, int arg, int x07, f32 scale) {
    EFTW *ew = eft08_set_com(arg);

    if (ew != 0) {
        ew->type = 8;
        ew->move = eft08_move;
        ew->owner = 0;
        ew->arg = arg;
        ew->x07 = x07;
        ew->scale = scale;
        flvecCopy(ew->pos, pos);
    }
}

void Eft08_set2(PLW *pl, int arg, int x07, f32 scale, f32 y) {
    EFTW *ew;

    if (Pl_stg_ck(pl) != 0) {
        if ((ew = eft08_set_com(arg)) != 0) {
            ew->type = 8;
            ew->move = eft08_move;
            ew->owner = (EMW *)pl;
            ew->arg = arg;
            ew->x07 = x07;
            ew->scale = scale;
            flvecCopy(ew->pos, pl->pos);
            ew->pos[1] = y;
        }
    }
}
