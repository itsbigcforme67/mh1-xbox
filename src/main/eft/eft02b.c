/* eft02b - SLPM_654.95 0x0027E940-0x0027EF58: eft02's spawners (see eft02.c).
 * eft02 - whole file 0x0027D6E0-0x0027EF58. Hit sparks and blood: one
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

void eft02_set(PLW *pl, int arg, f32 *pos) {
    EFTW *ew;

    if (pl != 0 && Pl_stg_ck(pl) != 0) {
        ew = pull_eft_work(0);
        if (ew != 0) {
            ew->type = 2;
            ew->move = eft02_move;
            ew->arg = arg;
            ew->owner = (EMW *)pl;
            ew->scale = 1.0f;
            ew->pos[0] = pos[0];
            ew->pos[1] = pos[1];
            ew->pos[2] = pos[2];
            ew->u0A.ang = pl->x3EC + 0x8000;
        }
    }
}

void Eft02_set2(u16 a, u16 b, int arg, f32 *pos) {
    EFTW *ew;

    if ((ew = pull_eft_work(0)) != 0) {
        ew->type = 2;
        ew->move = eft02_move;
        ew->arg = arg;
        ew->scale = 1.0f;
        ew->pos[0] = pos[0];
        ew->pos[1] = pos[1];
        ew->pos[2] = pos[2];
        ew->stg = a >> 8;
        ew->x07 = b >> 8;
        ew->u0A.joint = ran_suu(1);
    }
}

void Eft02_set3(EMW *em, int ang, int arg, int joint, f32 *pos, f32 scale) {
    EFTW *ew;

    if (Pl_stg_ck(em) != 0) {
        ew = pull_eft_work(0);
        if (ew != 0) {
            ew->type = 2;
            ew->move = eft02_move;
            ew->arg = arg;
            ew->owner = em;
            ew->scale = scale;
            ew->pos[0] = pos[0];
            ew->pos[1] = pos[1];
            ew->pos[2] = pos[2];
            ew->mode2 = joint;
            ew->x07 = 0;
            ew->u0A.joint = ang;
            if (em->x10 != 0 && (em->kind == 0x13 || em->kind == 0x18)) {
                ew->x1E = 2;
            } else {
                ew->x1E = 0;
            }
        }
    }
}

void Eft02_set4(u16 a, int ang, int arg, f32 *pos, f32 scale) {
    EFTW *ew = pull_eft_work(0);

    if (ew != 0) {
        ew->type = 2;
        ew->move = eft02_move;
        ew->arg = arg;
        ew->owner = 0;
        ew->scale = scale;
        ew->pos[0] = pos[0];
        ew->pos[1] = pos[1];
        ew->pos[2] = pos[2];
        ew->x07 = a >> 8;
        ew->u0A.joint = ang;
    }
}

void Eft02_set5(EMW *em, s16 kind, int arg, int joint, f32 *pos, f32 scale) {
    EFTW *ew;

    if (Pl_stg_ck(em) != 0) {
        ew = pull_eft_work(0);
        if (ew != 0) {
            ew->type = 2;
            ew->move = eft02_move;
            ew->arg = arg;
            ew->scale = scale;
            ew->pos[0] = pos[0];
            ew->pos[1] = pos[1];
            ew->pos[2] = pos[2];
            ew->mode2 = joint;
            ew->x1E = 0;
            switch (kind) {
            case 9:
            case 10:
            case 11:
            case 12:
            case 13:
            case 14:
            case 15:
            case 16:
                ew->stg = 1;
                break;
            default:
                ew->stg = 0;
                if (em->x10 != 0 && (em->kind == 0x13 || em->kind == 0x18)) {
                    ew->x1E = 2;
                }
                break;
            }
            ew->x07 = 0;
            ew->u0A.joint = 0;
        }
    }
}

void Eft02_set6(EMW *em, int arg, int joint, f32 scale) {
    EFTW *ew;

    if (Pl_stg_ck(em) != 0) {
        ew = pull_eft_work(0);
        if (ew != 0) {
            ew->type = 2;
            ew->move = eft02_move;
            ew->arg = arg;
            ew->owner = em;
            ew->scale = scale;
            ew->mode2 = joint;
            ew->x07 = 0;
            ew->u0A.joint = 0;
        }
    }
}

void Eft02_set_pos(f32 *pos, int arg, int ang) {
    EFTW *ew = pull_eft_work(0);

    if (ew != 0) {
        ew->type = 2;
        ew->move = eft02_move;
        ew->arg = arg;
        ew->owner = 0;
        ew->scale = 1.0f;
        ew->pos[0] = pos[0];
        ew->pos[1] = pos[1];
        ew->pos[2] = pos[2];
        ew->u0A.joint = ang;
    }
}

void Eft02_set_pos2(int ang, int arg, int joint, f32 *pos, f32 scale) {
    EFTW *ew = pull_eft_work(0);

    if (ew != 0) {
        ew->type = 2;
        ew->move = eft02_move;
        ew->arg = arg;
        ew->owner = 0;
        ew->scale = scale;
        ew->pos[0] = pos[0];
        ew->pos[1] = pos[1];
        ew->pos[2] = pos[2];
        ew->mode2 = joint;
        ew->x07 = 0;
        ew->u0A.joint = ang;
        ew->x1E = 0;
    }
}
