/* eft06_nm - NOT BUILT. Near-match C for eft06_m (0x00103A40), the last
 * eft06 function still in asm. Written from an m2c draft checked against
 * the asm (switch defaults verified); believed equivalent, not matched. */
#include "eft.h"
#include "em.h"
#include "game.h"
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


#define ANG2DEG(a) (360.0f * (f32)(a) / 65536.0f)
#define DEG2RAD(d) (2.0f * (3.1415927f * ((d) / 360.0f)))

extern void *eft06_data[19];
extern void *eft06_fade_data[60];
extern s16 eft06_all_time[10];
extern s16 *eft06_time[10];
extern s16 eft06_index[10];
extern s16 eft06_param[10];
extern s16 eft06_fade_index[10];
extern FLMAT rview_mat;

int pw_die_ck(EMW *em);
void eft06_continue(EFT06_PIECE *p, s16 *t, s16 n, s16 *c);
int eft06_type_ck(EFT06_PIECE *p, s16 n);
void eft06_type2_init_sub(EFTW *ew, EFT06_PIECE *p, s16 k);
void eft06_type3_init_sub(EFTW *ew, EFT06_PIECE *p, s16 k);
void eft06_type4_init_sub(EFTW *ew, EFT06_PIECE *w, s16 k);
void eft06_type6_init_sub(EFTW *ew, EFT06_PIECE *p);
void eft06_type9_init_sub(EFTW *ew, EFT06_PIECE *p);
void eft_vec_linear(f32, void *, f32 *);
void eft_rgba_linear(void *, int, u32 *);
void PointToPoint(f32 *, f32 *, f32 *);
f32 flSin(f32);

void eft06_m(EFTW *ew) {
    f32 dv[3];
    f32 jp[3];
    EFT06_PIECE *p = ew->work;
    EFT06_PIECE *w = p;
    s16 *tt0 = eft06_time[ew->arg];     /* the asm tests the table (s8), reads through the stepped copy (s6) */
    s16 *tt = tt0;
    s16 n = eft06_num[ew->arg];
    u16 all = eft06_all_time[ew->arg];
    s16 idx = eft06_index[ew->arg];
    s16 step = eft06_param[ew->arg];
    s16 fidx = eft06_fade_index[ew->arg];
    EMW *own = ew->owner;
    u16 time;
    u16 c;
    s16 old;
    s16 i;
    int moved = 0;
    f32 s;

    if (ew->x07 != 0xFF) {
        if (pw_die_ck(ew->owner) != 0) {
            ew->x07 = 0xFF;
        } else if (ew->arg == 7) {
            ew->x07 = 0;
        }
    }
    switch (ew->arg) {
    case 0:
        fidx += (s16)(ew->mode2 * 3);
        if (ew->x07 != 0xFF) {
            if (ew->timer == 10) {
                Eft06_set(ew->owner, 2, ew->mode2, ew->stg, ew->scale);
            }
            get_joint_pos(ew->owner, ew->stg, ew->pos);
        }
        break;
    case 1:
        if (ew->x07 != 0xFF) {
            get_joint_pos(ew->owner, ew->stg, ew->pos);
        }
        time = 11;
        break;
    case 2:
        time = 6;
        break;
    case 5:
        if (ew->x07 != 0xFF) {
            get_joint_pos(ew->owner, ew->stg, ew->pos);
            if (ew->timer == 0) {
                eft06_se_req(ew, ew->pos);
            }
        }
        time = 12;
        break;
    case 6:
        if (ew->x07 != 0xFF) {
            get_joint_pos(ew->owner, ew->stg, ew->pos);
            old = ew->x07;
            ew->x07 = *(s16 *)((u8 *)own + 0x87C) / 30;
            if (ew->x07 >= 3) {
                ew->x07 = 2;
            }
            if (old != ew->x07) {
                eft06_se_req(ew, ew->pos);
            }
        }
        break;
    case 7:
        if (ew->x07 == 0xFF) {
            return;
        }
        if (Pl_stg_ck(ew->owner) == 0 || game_w.info_stop == 1) {
            return;
        }
        get_joint_pos(ew->owner, ew->stg, jp);
        PointToPoint(dv, jp, ew->pos);
        flvecCopy(ew->pos, jp);
        get_joint_pos(ew->owner, 2, jp);
        jp[1] = ew->pos[1] - 20.0f;
        break;
    case 9:
        if (ew->x07 != 0xFF) {
            get_joint_pos(ew->owner, ew->stg, ew->pos);
            if (ew->timer == 0) {
                eft06_se_req(ew, ew->pos);
            }
        }
        time = 4;
        break;
    }
    if (ew->arg == 6) {
        if (ew->x07 != 0xFF) {
            c = own->char0;
            if (c != 0x3ED && c != 0x3EE && c != 0x3EF) {
                ew->x07 = 0xFF;
            }
        } else if (eft06_type_ck(w, n) != 0) {
            ew->mode++;
            ew->be_flag = 0;
            return;
        }
    } else if (ew->arg == 7) {
        switch (*((u8 *)own + 0x4D5) & 0x30) {
        default:
            ew->mode2 = 0;
            break;
        case 0x10:
            ew->mode2 = 1;
            break;
        case 0x20:
            ew->mode2 = 2;
            break;
        case 0x30:
            ew->mode2 = 3;
            break;
        }
        for (i = 0; i < n; i++, p++) {
            switch (ew->mode2) {
            case 0:
                return;
            case 1:
            case 2:
                p->lag += 2;
                if (p->no == 1) {
                    p->x32 += 4;
                } else {
                    p->x32 += 2;
                }
                break;
            case 3:
                p->lag += 4;
                if (p->no == 1) {
                    p->x32 += 6;
                } else {
                    p->x32 += 3;
                }
                break;
            }
            if (p->lag >= 0xF0) {
                p->lag = 0;
            }
            if (p->x32 >= 60) {
                if (++ew->u0A.joint >= 2) {
                    ew->u0A.joint = 0;
                    p->no = 1;
                } else {
                    p->no = 0;
                }
                p->rot[1] = ran_suu(1);
                p->x32 = 0;
                flvecCopy(p->pos, jp);
            } else {
                p->pos[0] += dv[0];
                p->pos[1] += dv[1];
                p->pos[2] += dv[2];
            }
            if (p->prim != 0) {
                flvecCopy(p->prim->pos, p->pos);
                add_prim(ot0, p->prim, 0x40, 0);
            }
        }
        return;
    } else if (++ew->timer >= all) {
        ew->mode++;
        ew->be_flag = 0;
        return;
    }
    for (i = 0; i < n; i++, tt++) {
        if (ew->arg == 6) {
            switch (p->no) {
            case 0:
                switch (p->x36) {
                case 0:
                case 1:
                    time = 14;
                    break;
                default:
                    time = 12;
                    break;
                }
                break;
            case 1:
                switch (ew->x07) {
                case 0:
                    time = 30;
                    break;
                case 1:
                    time = 30;
                    if (p->x36 == 0) {
                        p->lag = 0;
                        p->x36++;
                    }
                    break;
                case 2:
                    if (p->x36 == 1) {
                        p->lag = 0;
                        p->x36++;
                    }
                    time = 6;
                    break;
                default:
                    p->no = 0xFF;
                    goto skip;
                }
                break;
            case 2:
                switch (ew->x07) {
                case 0:
                case 1:
                    goto skip;
                default:
                    time = 4;
                    break;
                }
                break;
            }
        } else if (ew->arg == 1) {
            idx = eft06_index[ew->arg];
            fidx = ew->mode2 + eft06_fade_index[ew->arg];
        } else if (tt0 != 0) {
            time = *tt;
        } else {
            idx = eft06_index[ew->arg];
            fidx = eft06_fade_index[ew->arg];
        }
        if (++p->lag <= 0 || p->no == 0xFF) {
            goto skip;
        }
        if (p->lag > time) {
            switch (ew->arg) {
            case 6:
                switch (p->no) {
                case 0:
                    if (ew->x07 == 0 || ew->x07 == 1) {
                        p->lag = -6;
                    } else {
                        p->lag = -4;
                    }
                    break;
                case 1:
                    if (ew->x07 == 0 || ew->x07 == 1) {
                        p->lag = time;
                        break;
                    }
                    p->no = 0xFF;
                    goto skip;
                case 2:
                    p->no = 0xFF;
                    goto skip;
                }
                break;
            case 9:
                eft06_type9_init_sub(ew, p);
                break;
            case 2:
                eft06_type2_init_sub(ew, p, 1);
            default:
                goto skip;
            }
        }
        switch (ew->arg) {
        case 0:
        case 1:
        case 5:
        case 9:
            flvecCopy(p->pos, ew->pos);
            break;
        case 2:
            if (p->lag == 1) {
                if (ew->x07 == 0xFF) {
                    p->no = 0xFF;
                    goto skip;
                }
                eft06_type2_init_sub(ew, p, 0);
            }
            break;
        case 3:
            if (p->lag == 1) {
                if (ew->x07 == 0xFF) {
                    p->no = 0xFF;
                    goto skip;
                }
                if (p->no == 0) {
                    eft06_type3_init_sub(ew, p, i);
                }
            }
            if (p->no == 0) {
                p->pos[1] += ew->scale * (25.0f / time);
            } else {
                p->pos[1] += ew->scale * (50.0f / time);
            }
            break;
        case 4:
        case 8:
            if (p->no == 0 || p->no == 1) {
                dv[0] = ew->scale * ((ew->u0A.joint & 0xF) + 0xF) * (flSin(DEG2RAD(ANG2DEG(ew->timer << 11))) - 0.5f);
                dv[1] = 0.5f * ew->scale * ((((ew->u0A.joint & 0x300) >> 8) + 2) * ew->timer);
                dv[2] = 0.0f;
                flvecApplyMat33_2(dv, &rview_mat);
                moved = 1;
            }
            if (p->lag == 1) {
                if (p->no == 0 ? ew->x07 == 0xFF : w->no == 0xFF) {
                    p->no = 0xFF;
                    goto skip;
                }
                eft06_type4_init_sub(ew, w, i);
            }
            break;
        case 6:
            if (p->lag == 1) {
                if (ew->x07 == 0xFF) {
                    p->no = 0xFF;
                    goto skip;
                }
                if (p->no == 0) {
                    eft06_type6_init_sub(ew, p);
                }
            }
            switch (p->no) {
            case 0:
                if (p->x36 == 2) {
                    idx = 0x10;
                    fidx = 0x33;
                    if (ew->x07 != 0xFF) {
                        get_joint_pos(ew->owner, 0x12, p->pos);
                    }
                } else if (p->x36 == 1) {
                    idx = 0xF;
                    fidx = 0x32;
                    flvecCopy(p->pos, ew->pos);
                } else {
                    idx = 0xF;
                    fidx = 0x31;
                    flvecCopy(p->pos, ew->pos);
                }
                break;
            case 1:
                switch (ew->x07) {
                case 0:
                case 1:
                    s = 1.5f * p->lag / 30.0f;
                    if (ew->x07 == 1) {
                        s += 1.5f;
                    }
                    p->scale[0] = s;
                    p->scale[1] = s;
                    p->scale[2] = p->scale[0];
                    if (p->prim != 0) {
                        p->prim->pos[0] = ew->pos[0];
                        p->prim->pos[1] = ew->pos[1];
                        p->prim->pos[2] = ew->pos[2];
                        add_prim(ot0, p->prim, 0x40, 0);
                    }
                    goto skip;
                case 2:
                    idx = 0x11;
                    fidx = 0x34;
                    flvecCopy(p->pos, ew->pos);
                    break;
                }
                break;
            case 2:
                idx = 0x12;
                fidx = 0x35;
                if (ew->x07 == 0xFF) {
                    p->no = 0xFF;
                    goto skip;
                }
                get_joint_pos(ew->owner, 0x12, p->pos);
                break;
            }
            break;
        }
        if (step > 0) {
            eft_vec_linear(p->lag, eft06_data[idx++], p->scale);
        }
        p->rot[2] += p->drot;
        if (p->x34 != 0 && fidx != -1) {
            eft_rgba_linear(eft06_fade_data[fidx++], p->lag, &p->col);
        }
        if (p->prim != 0) {
            if (moved != 0) {
                moved = 0;
                p->prim->pos[0] = p->pos[0] + dv[0];
                p->prim->pos[1] = p->pos[1] + dv[1];
                p->prim->pos[2] = p->pos[2] + dv[2];
            } else {
                p->prim->pos[0] = p->pos[0];
                p->prim->pos[1] = p->pos[1];
                p->prim->pos[2] = p->pos[2];
            }
            if (ew->arg == 5 || ew->arg == 9) {
                add_prim(ot1, p->prim, 0x20, 0);
            } else {
                add_prim(ot0, p->prim, 0x40, 0);
            }
        }
        p++;
        continue;
    skip:
        eft06_continue(p, &idx, step, &fidx);
        p++;
    }
}
