/* eft06 - SLPM_654.95 0x00102BD0-0x00103A38 (first matching run; eft06_m
 * is still asm, the rest is in eft06b.c). Whole
 * file 0x00102BD0-0x00105B10. Hit sparks on players and
 * monsters with ten types (arg): each spawns up to eft06_num[arg] sprites
 * (0x38-byte pieces in ew->work) placed on a joint of the owner (ew->stg),
 * with per-type start values (eft06_i) and keyframe animation (eft06_m,
 * still asm; eft06_t draws). Type 7 uses the second prim pool. Spawned by
 * Eft06_set (on a joint), Eft06_set2 (at a point) and Eft06_set_hit (on a
 * hit monster, sized by enemy_shadow/mahi tables of game.bin). Names of the
 * types are not known. */
#include "eft.h"
#include "em.h"
#include "prim.h"
#include "fl.h"

/* One sprite (0x38 bytes) of the work area. */
typedef struct EFT06_PIECE {
    s16 prim_no;        /* 0x00 */
    s16 no;             /* 0x02 sprite kind, 0xFF = unused */
    f32 scale[3];       /* 0x04 */
    f32 pos[3];         /* 0x10 */
    u32 col;            /* 0x1C */
    u16 rot[3];         /* 0x20 */
    s16 drot;           /* 0x26 */
    f32 size;           /* 0x28 */
    PRIM *prim;         /* 0x2C */
    s16 lag;            /* 0x30 frame counter, starts at or below 0 */
    u16 x32;            /* 0x32 */
    u16 x34;            /* 0x34 */
    u16 x36;            /* 0x36 */
} EFT06_PIECE;

extern s16 eft06_num[10];
extern s16 eft06_type3_lag_tbl[6];
extern s16 eft06_type4_lag_tbl[3];
extern f32 eft06_em_scale[35];
extern f32 D_63BD60[];      /* game.bin enemy_mahi_size */

u32 ran_suu(int);
u8 Pl_stg_ck(void *);
int Pl_master_ck(void *);
s16 get_prim2(void);
PRIM *get_prim_ptr2(s16);
void release_prim(s16);
void release_prim2(s16);
FLMAT *get_joint_wmat(void *, int);
void get_joint_pos(void *, int, f32 *);
void flmatCopy(FLMAT *, FLMAT *);
void flvecCopy(f32 *, f32 *);
void flvecApplyMat33_2(f32 *, FLMAT *);
void se_req2(int, int, int, f32 *, int, int);
void Pl_se_req2(void *, int, int, f32 *, int, int);

void eft06_move(EFTW *ew);
void eft06_i(EFTW *ew);
void eft06_m(EFTW *ew);
void eft06_d(EFTW *ew);
void eft06_e(EFTW *ew);
void eft06_t(PRIM *pr);
void eft06_se_req(EFTW *ew, f32 *pos);
void Eft06_set(void *chr, s16 arg, int x05, int joint, f32 scale);

void eft06_move(EFTW *ew) {
    switch (ew->mode) {
    case 0:
        eft06_i(ew);
        break;
    case 1:
        eft06_m(ew);
        break;
    case 2:
        eft06_d(ew);
        break;
    case 3:
        eft06_e(ew);
        break;
    }
}

void eft06_i(EFTW *ew) {
    EFT06_PIECE *w = ew->work;
    EMW *own = ew->owner;
    EFT06_PIECE *p;
    s16 n;
    s16 i;

    ew->mode++;
    ew->be_flag = 1;
    ew->work14 = 0;
    ew->timer = 0;
    ew->x07 = 0;
    n = eft06_num[ew->arg];
    switch (ew->arg) {
    case 0:
    case 6:
        eft06_se_req(ew, ew->pos);
        break;
    case 4:
        ew->u0A.joint = ran_suu(1);
        ew->scale -= (0.4f / 1000.0f) * ((u16)ran_suu(1) & 0x3FF);
        break;
    case 7:
        ew->u0A.joint = 2;
        p = w;
        for (i = 0; i < n; i++, p++) {
            p->lag = (i * 240) / 2;
            p->x32 = (i * 60) / 2;
            p->size = ew->scale;
            flvecCopy(p->pos, ew->pos);
            p->col = -1;
            p->rot[1] = ran_suu(1);
            p->no = 0;
            p->prim_no = get_prim2();
            if (p->prim_no != -1) {
                p->prim = get_prim_ptr2(p->prim_no);
                p->prim->owner = ew;
                p->prim->no = i;
                p->prim->trans = eft06_t;
            } else {
                p->prim = 0;
            }
        }
        return;
    case 8:
        ew->u0A.joint = ran_suu(1);
        ew->scale -= (0.4f / 1000.0f) * ((u16)ran_suu(1) & 0x3FF);
        ew->pos[0] = 0.0f;
        ew->pos[1] = 20.0f;
        ew->pos[2] = 0.0f;
        break;
    default:
        ew->u0A.joint = own->ang[1];
        break;
    }
    for (i = 0; i < n; i++) {
        w[i].prim_no = get_prim();
        if (w[i].prim_no != -1) {
            w[i].lag = 0;
            w[i].size = ew->scale;
            flvecCopy(w[i].pos, ew->pos);
            w[i].col = -1;
            switch (ew->arg) {
            case 0:
                w[i].rot[0] = 0;
                w[i].rot[1] = 0;
                if (i == 2) {
                    w[i].x34 = 0;
                    w[i].rot[2] = 0;
                    w[i].drot = 0;
                    w[i].no = 1;
                } else {
                    if (i == 0 || i == 1) {
                        w[i].no = 0;
                    } else {
                        w[i].no = 2;
                    }
                    w[i].x34 = 1;
                    w[i].rot[2] = 0;
                    w[i].drot = 0;
                }
                break;
            case 1:
                w[i].rot[0] = ran_suu(1);
                w[i].rot[1] = ran_suu(1);
                w[i].rot[2] = 0;
                w[i].drot = -0x13B0;
                w[i].x34 = 1;
                w[i].no = i;
                w[i].lag = -3 * i;
                w[i].size += 0.1f * i;
                break;
            case 2:
                w[i].no = i;
                w[i].x34 = 0;
                w[i].lag = -2 * i;
                break;
            case 3:
                w[i].size = ew->scale * (1.0f + (0.2f / 1000.0f) * ((u16)ran_suu(1) & 0x3FF));
                w[i].no = i & 1;
                w[i].x34 = 1;
                w[i].lag = eft06_type3_lag_tbl[i];
                w[i].rot[0] = 0;
                w[i].rot[1] = 0;
                if (w[i].no == 0) {
                    w[i].rot[2] = ran_suu(1);
                    w[i].pos[0] = ((u16)ran_suu(1) & 0x1F) - 0x10;
                    w[i].pos[1] = ((u16)ran_suu(1) & 0x1F) - 0x10;
                    w[i].pos[2] = ((u16)ran_suu(1) & 0x1F) - 0x10;
                } else {
                    w[i].rot[2] = 0;
                    w[i].no += (s16)((u16)ran_suu(1) % 3);
                }
                w[i].drot = 0.5f + 65536.0f * (0.001f * ((u16)ran_suu(1) & 0x3FF)) / 360.0f;
                break;
            case 4:
            case 8:
                w[i].no = i;
                w[i].x34 = 1;
                w[i].lag = eft06_type4_lag_tbl[i];
                w[i].rot[0] = 0;
                w[i].rot[1] = 0;
                if (w[i].no == 0) {
                    w[i].rot[2] = 0;
                } else if (w[i].no == 1) {
                    w[i].rot[2] = ran_suu(1);
                } else {
                    w[i].rot[2] = ran_suu(1);
                    w[i].no += (s16)((u16)ran_suu(1) % 3);
                }
                w[i].drot = 0;
                break;
            case 5:
            case 9:
                w[i].no = i;
                w[i].scale[0] = 1.0f;
                w[i].scale[1] = 1.0f;
                w[i].scale[2] = 1.0f;
                w[i].x34 = 0;
                w[i].lag = 0;
                w[i].rot[0] = ran_suu(1);
                w[i].rot[1] = ran_suu(1);
                w[i].rot[2] = ran_suu(1);
                w[i].drot = 0;
                break;
            case 6:
                switch (i) {
                case 0:
                case 1:
                case 2:
                case 3:
                    w[i].no = 0;
                    w[i].lag = -i * 5;
                    break;
                case 4:
                    w[i].no = 1;
                    w[i].lag = 0;
                    w[i].col = 0xCCFFBF5F;
                    break;
                case 5:
                    w[i].no = 2;
                    w[i].lag = -2;
                    break;
                }
                w[i].rot[2] = 0;
                w[i].drot = 0;
                w[i].x34 = 1;
                w[i].x36 = 0;
                break;
            }
            w[i].prim = get_prim_ptr(w[i].prim_no);
            w[i].prim->owner = ew;
            w[i].prim->no = i;
            w[i].prim->trans = eft06_t;
        } else {
            w[i].prim = 0;
        }
    }
}

void eft06_type2_init_sub(EFTW *ew, EFT06_PIECE *p, s16 k) {
    FLMAT m;

    switch (k) {
    case 0:
        p->no += 4;
        p->size = ew->scale * (1.0f - (0.8f / 15.0f) * p->no);
        p->rot[2] = ran_suu(1);
        flmatCopy(&m, get_joint_wmat(ew->owner, ew->stg));
        p->pos[0] = ew->scale * (((u16)ran_suu(1) & 0x1F) - 0x10);
        p->pos[1] = ew->scale * (((u16)ran_suu(1) & 0x1F) - 0x10);
        p->pos[2] = ew->scale * (((u16)ran_suu(1) & 0x1F) - 0x10);
        flvecApplyMat33_2(p->pos, &m);
        p->pos[0] += m[3][0];
        p->pos[1] += m[3][1];
        p->pos[2] += m[3][2];
        break;
    case 1:
        if (p->no + 4 < 15) {
            p->lag = -2;
        }
        break;
    }
}

void eft06_type3_init_sub(EFTW *ew, EFT06_PIECE *p, s16 k) {
    FLMAT m;

    p->rot[2] = ran_suu(1);
    flmatCopy(&m, get_joint_wmat(ew->owner, ew->stg));
    p->pos[0] += ew->pos[0];
    p->pos[1] += ew->pos[1];
    p->pos[2] += ew->pos[2];
    flvecApplyMat33_2(p->pos, &m);
    p->pos[0] += m[3][0];
    p->pos[1] += m[3][1];
    p->pos[2] += m[3][2];
    if (k < 5) {
        flvecCopy(p[1].pos, p->pos);
    }
    if (k == 0) {
        eft06_se_req(ew, p->pos);
    }
}

void eft06_type4_init_sub(EFTW *ew, EFT06_PIECE *w, s16 k) {
    FLMAT m;

    switch (k) {
    case 0:
        flmatCopy(&m, get_joint_wmat(ew->owner, ew->stg));
        w[k].pos[0] = ew->pos[0] + (((u16)ran_suu(1) & 0x1F) - 0x10);
        w[k].pos[1] = ew->pos[1] + (((u16)ran_suu(1) & 0x1F) - 0x10);
        w[k].pos[2] = ew->pos[2] + (((u16)ran_suu(1) & 0x1F) - 0x10);
        flvecApplyMat33_2(w[k].pos, &m);
        w[k].pos[0] += m[3][0];
        w[k].pos[1] += m[3][1];
        w[k].pos[2] += m[3][2];
        break;
    case 1:
    case 2:
        flvecCopy(w[k].pos, w[0].pos);
        break;
    }
}

void eft06_type6_init_sub(EFTW *ew, EFT06_PIECE *p) {
    p->rot[2] = ran_suu(1);
    p->x36 = ew->x07;
    p->x32 = ew->timer++;
    p->rot[2] = ran_suu(1);
    switch (p->x36) {
    case 0:
    case 1:
        p->size = ew->scale * (0.5f + 0.05f * p->x32);
        if (p->size > 0.8f * ew->scale) {
            p->size = 0.8f * ew->scale;
            p->x32--;
            ew->timer--;
        }
        break;
    case 2:
    default:
        p->size = ew->scale;
        break;
    }
}

void eft06_type9_init_sub(EFTW *ew, EFT06_PIECE *p) {
    p->lag = 0;
    p->rot[0] = ran_suu(1);
    p->rot[1] = ran_suu(1);
    p->rot[2] = ran_suu(1);
}

int pw_die_ck(EMW *em) {
    if (em->be_flag == 0 || em->x04 == 3) {
        return 1;
    }
    return 0;
}

void eft06_continue(EFT06_PIECE *p, s16 *t, s16 n, s16 *c) {
    *t += n;
    if (p->x34 != 0) {
        (*c)++;
    }
}

int eft06_type_ck(EFT06_PIECE *p, s16 n) {
    s16 i;

    for (i = 0; i < n; i++, p++) {
        if (p->no != 0xFF) {
            return 0;
        }
    }
    return 1;
}

