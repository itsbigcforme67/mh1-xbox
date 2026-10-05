/* eft02 - SLPM_654.95 0x0027D6E0-0x0027DDB8 (first matching run; eft02_t
 * is still asm, near-match in eft02_nm.c, setters in eft02b.c). Whole file
 * 0x0027D6E0-0x0027EF58. Hit sparks and blood: one
 * model per effect, picked by arg (0-11), animated by stepping through
 * consecutive clay models every two frames (clay[timer / 2]), with its own
 * life time per arg (eft02_m). Blood colours come from Eft_blood_rgb (row 2
 * for monster kinds 0x13 and 0x18). Several spawners: on a player
 * (eft02_set), at a point (Eft02_set_pos/_pos2, set2, set4), on a monster
 * joint (set3, set5, set6). Names of the arg kinds are not known. */
#include "eft.h"
#include "em.h"
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

#define ANG2DEG(a) (360.0f * (f32)(a) / 65536.0f)
#define DEG2RAD(d) (2.0f * (3.1415927f * ((d) / 360.0f)))

extern EFT_MDLW *eft_mdlw[5];
extern FLMAT rview_mat;
extern u8 Eft_blood_rgb[6][4];
extern f32 scale32_34_00354E00[];

u32 ran_suu(int);
u8 Pl_stg_ck(void *);
void release_prim(s16);
FLMAT *get_joint_wmat(void *, int);
void flmatCopy(FLMAT *, FLMAT *);
void flmatGetTrans(f32 *, FLMAT *);
void flvecApplyMat33_2(f32 *, FLMAT *);
void flmatRotZ33(FLMAT *, f32);
void flmatRotZXY33(FLMAT *, f32, f32, f32);
void flmatRotXYZ33(FLMAT *, f32, f32, f32);
void RotateZ(FLMAT *, f32);
void SetFilterMode(int);
void Material_set_sub(void *, CLAY *);
void Eft_rendope_set(u16);
void eft_vec_linear(f32, f32 *, f32 *);

void eft02_move(EFTW *ew);
static void eft02_i(EFTW *ew);
static void eft02_m(EFTW *ew);
static void eft02_d(EFTW *ew);
static void eft02_e(EFTW *ew);
void eft02_t(PRIM *pr);

void eft02_move(EFTW *ew) {
    switch (ew->mode) {
    case 0:
        eft02_i(ew);
        break;
    case 1:
        eft02_m(ew);
        break;
    case 2:
        eft02_d(ew);
        break;
    case 3:
        eft02_e(ew);
        break;
    }
}

static void eft02_i(EFTW *ew) {
    FLMAT m;
    f32 v[3];

    ew->mode++;
    ew->be_flag = 1;
    ew->work14 = 0;
    ew->timer = 0;
    switch (ew->arg) {
    case 0:
        ew->timer = -4;
        break;
    case 4:
        ew->u0A.joint = ran_suu(1);
        break;
    case 5:
        ew->scale *= 0.8f + (0.4f / 1000.0f) * ((u16)ran_suu(1) & 0x3FF);
        break;
    case 7:
        ew->scale *= 0.8f + (0.4f / 1000.0f) * ((u16)ran_suu(1) & 0x3FF);
        ew->u0A.joint = ran_suu(1);
        break;
    case 6:
        ew->stg = (u16)ran_suu(1) % 3;
        break;
    case 8:
        v[0] = (0.1f * 0.1f) * (((u16)ran_suu(1) & 0x3FF) - 0x200);
        v[1] = 5.0f + (0.1f * 0.1f) * ((u16)ran_suu(1) & 0x3FF);
        v[2] = 0.0f;
        flmatCopy(&m, get_joint_wmat(ew->owner, ew->mode2));
        flvecApplyMat33_2(v, &m);
        flmatGetTrans(ew->pos, &m);
        ew->pos[0] += v[0];
        ew->pos[1] += v[1];
        ew->pos[2] += v[2];
        break;
    case 9:
    case 10:
    case 11:
        if (ew->mode2 & 1) {
            ew->timer = -8;
        }
        ew->stg = (u16)ran_suu(1) >> 8;
        ew->x07 = (u16)ran_suu(1) >> 8;
        ew->u0A.joint = ran_suu(1);
        break;
    case 2:
        break;
    case 1:
    case 3:
    default:
        ew->stg = (u16)ran_suu(1) >> 8;
        ew->x07 += (u8)((((u16)ran_suu(1) & 0xFFF) - 0x800) >> 8);
        ew->u0A.joint += (s16)(((u16)ran_suu(1) & 0xFFF) - 0x800);
        break;
    }
    ew->prim_no = get_prim();
    if (ew->prim_no != -1) {
        ew->prim = get_prim_ptr(ew->prim_no);
        ew->prim->owner = ew;
        ew->prim->trans = eft02_t;
    } else {
        push_eft_work(ew);
    }
}

static void eft02_m(EFTW *ew) {
    f32 v[3];
    s16 n;

    ew->timer += 2;
    switch (ew->arg) {
    case 4:
        n = 8;
        break;
    case 5:
        n = 0x18;
        break;
    case 6:
        n = 8;
        break;
    case 7:
        n = 0x18;
        break;
    case 8:
        n = 0x20;
        break;
    case 9:
    case 10:
    case 11:
        n = 0x10;
        break;
    default:
        n = 0x1E;
        break;
    }
    if (ew->timer > 0) {
        if (ew->timer > n) {
            ew->mode++;
            ew->be_flag = 0;
        } else if (ew->prim != 0) {
            switch (ew->arg) {
            case 0:
            case 2:
                if (ew->timer >= 16) {
                    ew->pos[1] -= 4.0f;
                }
                break;
            case 3:
                if (ew->timer >= 10) {
                    ew->pos[1] -= 5.0f;
                }
                break;
            case 4:
                v[0] = 0.0f;
                v[1] = 0.0f;
                v[2] = -20.0f;
                flvecApplyMat33_2(v, &rview_mat);
                ew->prim->pos[0] = ew->pos[0] + v[0];
                ew->prim->pos[1] = ew->pos[1] + v[1];
                ew->prim->pos[2] = ew->pos[2] + v[2];
                add_prim(ot0, ew->prim, 0x40, 0);
                return;
            case 6:
                v[0] = 0.0f;
                v[1] = 0.0f;
                v[2] = 50.0f;
                flvecApplyMat33_2(v, &rview_mat);
                ew->prim->pos[0] = ew->pos[0] + v[0];
                ew->prim->pos[1] = ew->pos[1] + v[1];
                ew->prim->pos[2] = ew->pos[2] + v[2];
                add_prim(ot0, ew->prim, 0x40, 0);
                return;
            case 8:
                ew->pos[1] += -0.4f * ew->timer;
                break;
            }
            ew->prim->pos[0] = ew->pos[0];
            ew->prim->pos[1] = ew->pos[1];
            ew->prim->pos[2] = ew->pos[2];
            add_prim(ot1, ew->prim, 0x20, 0);
        }
    }
}

static void eft02_d(EFTW *ew) {
    ew->mode++;
    release_prim(ew->prim_no);
}

static void eft02_e(EFTW *ew) {
    push_eft_work(ew);
}

