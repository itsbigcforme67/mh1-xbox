/* emmk01 - monster joint work (SLPM_654.95 0x0010A730-0x0010B054): old_pos_save (joint positions and matrices kept for
   riding), mlCalcTransEM (the per-kind special joints), enemy_mk (world matrix, pose), ride_ofs_calc. old_pos_save and
   mlCalcTransEM written from the asm by agent B (12 Oct 2026). */
#include "types.h"
#include "em.h"
void flmatMakeScale(f32 *, f32, f32, f32);
void cpRotMatrixYXZ2(s32 *, f32 *);
void flmatSetTrans(f32 *, f32, f32, f32);
void flmatMul33_2(f32 *, f32 *);
void flmatCopy();
static void mlCalcTransEM(EMW *, u8 *, f32 *);
void flCalcTransSI(u8 *, f32 *);
void flmatInvert(f32 *, f32 *);
void flmatGetTrans(f32 *, f32 *);
void flvecApplyMat33(f32 *, f32 *, f32 *);
extern f32 sys_old_mat[][16];
extern f32 sys_old_pos[][3];
#include "game.h"
extern s8 *em_neck_tbl[];        /* game.bin 0x63FF90 (main_aliases.txt): per monster kind, its neck joint numbers at +6..+9 */
f32 flConvertStoR(u16);
u8 *get_joint_mat_em(EMW *, s16);
void flmatRotX33(f32 *, f32);
void flmatRotY33(f32 *, f32);
void flmatRotZ33(f32 *, f32);
void flmatInit33(f32 *);
void flmatCopy33();
void flmatTranspose33(f32 *);
/* Saves every joint's position and matrix of the monster's skin (sys_old_pos / sys_old_mat, used for riding). */
#if !defined(G_PART) || G_PART == 1
void old_pos_save(u8 *em) {
    s16 n;
    s16 i;
    u8 *skin;

    if (em[0] == 0 || em[1] == 0) {
        return;
    }
    skin = *(u8 **)(*(u8 **)(em + MH2V(0x50C, 0x4EC)) + 0x24);
    n = *(s16 *)(skin + 0xC2);
    for (i = 0; i < n; i++, skin += 0x190) {
        sys_old_pos[i][0] = *(f32 *)(skin + 0x30);
        sys_old_pos[i][1] = *(f32 *)(skin + 0x34);
        sys_old_pos[i][2] = *(f32 *)(skin + 0x38);
        flmatCopy(sys_old_mat[i], (f32 *)skin);
    }
}
#endif

/* The per-kind special joints of a monster, applied to the local joint matrices before the skeleton is posed:
   the neck/head turn (kinds 1, 7, 0xB: four joints from the kind's joint table), the throat / wing joints that
   follow ex+4 (kind 6), a copied head rotation (kind 0xF), the tail spin from the frame counter (kind 0x21), and
   for kinds 8, 0x15, 0x22 four joints scaled 1.0 and turned by angles kept in the per-monster work. */
#if !defined(G_PART)
static void mlCalcTransEM(EMW *em, u8 *bone, f32 *unused) {
    f32 m2[16];
    f32 m1[16];
    s8 *t;
    u8 *x;

    t = em_neck_tbl[em->kind];
    switch (em->kind) {
    case 7:
    case 1:
    case 0xB:
        flmatRotY33((f32 *)(bone + t[9] * 0x190 + 0x40), flConvertStoR(em->neck[3]));
        flmatRotY33((f32 *)(bone + t[8] * 0x190 + 0x40), flConvertStoR(em->neck[2]));
        flmatRotY33((f32 *)(bone + t[7] * 0x190 + 0x40), flConvertStoR(em->neck[1]));
        flmatRotY33((f32 *)(bone + t[6] * 0x190 + 0x40), flConvertStoR(em->neck[0]));
        break;
    case 6:
        x = (u8 *)em + 0x444;
        if (em->x888 != 1) {
            if (x[4] > 0) {
                x[4]--;
            }
        } else if (x[4] < 30) {
            x[4]++;
        }
        flmatCopy(m2, get_joint_mat_em(em, 0x25));
        flmatInit33(m2);
        flmatRotX33(m2, -1.30899692f + 0.04363323f * x[4]);
        flmatCopy(get_joint_mat_em(em, 0x25), m2);
        flmatCopy(m2, get_joint_mat_em(em, 0x26));
        flmatInit33(m2);
        flmatRotY33(m2, 0.52359879f - 0.0174532924f * x[4]);
        flmatRotZ33(m2, 0.87266469f - 0.072722055f * x[4]);
        flmatCopy(get_joint_mat_em(em, 0x26), m2);
        flmatCopy(m2, get_joint_mat_em(em, 0x27));
        flmatInit33(m2);
        flmatRotY33(m2, -0.52359879f + 0.0174532924f * x[4]);
        flmatRotZ33(m2, -0.87266469f + 0.072722055f * x[4]);
        flmatCopy(get_joint_mat_em(em, 0x27), m2);
        break;
    case 0xF:
        flmatCopy(m1, get_joint_mat_em(em, 0x23));
        flmatTranspose33(m1);
        flmatCopy(m2, get_joint_mat_em(em, 0x25));
        flmatCopy33(m2, m1);
        flmatCopy(get_joint_mat_em(em, 0x25), m2);
        break;
    case 0x21:
        flmatCopy(m2, get_joint_mat_em(em, 0x18));
        flmatInit33(m2);
        flmatRotX33(m2, 2.0f * (3.1415927f * (((360.0f * (f32)(-((*(u16 *)&game_w.x1E & 0xF) << 12))) / 65536.0f) / 360.0f)));
        flmatCopy33(get_joint_mat_em(em, 0x18), m2);
        flmatCopy(m2, get_joint_mat_em(em, 0x19));
        flmatInit33(m2);
        flmatRotX33(m2, 2.0f * (3.1415927f * (((360.0f * (f32)(-(((*(u16 *)&game_w.x1E & 0xF) - 5) << 12))) / 65536.0f) / 360.0f)));
        flmatCopy33(get_joint_mat_em(em, 0x19), m2);
        break;
    }
    if (em->kind == 0x15 || em->kind == 8 || em->kind == 0x22) {
    *(f32 *)(bone + 0x3AD4) = 1.0f;
    *(f32 *)(bone + 0x3AD8) = 1.0f;
    *(f32 *)(bone + 0x3ADC) = 1.0f;
    *(f32 *)(bone + 0x3C64) = 1.0f;
    *(f32 *)(bone + 0x3C68) = 1.0f;
    *(f32 *)(bone + 0x3C6C) = 1.0f;
    *(f32 *)(bone + 0x3DF4) = 1.0f;
    *(f32 *)(bone + 0x3DF8) = 1.0f;
    *(f32 *)(bone + 0x3DFC) = 1.0f;
    *(f32 *)(bone + 0x1A04) = 1.0f;
    *(f32 *)(bone + 0x1A08) = 1.0f;
    *(f32 *)(bone + 0x1A0C) = 1.0f;
    x = (u8 *)em + 0x444;
    flmatRotX33((f32 *)(bone + 0x3A10), flConvertStoR(*(u16 *)(x + 0x52)));
    flmatRotX33((f32 *)(bone + 0x3BA0), flConvertStoR(*(u16 *)(x + 0x56)));
    flmatRotX33((f32 *)(bone + 0x3D30), flConvertStoR(*(u16 *)(x + 0x5A)));
    flmatRotX33((f32 *)(bone + 0x1940), flConvertStoR(*(u16 *)(x + 0x5E)));
    flmatRotY33((f32 *)(bone + 0x3A10), flConvertStoR(*(u16 *)(x + 0x54)));
    flmatRotY33((f32 *)(bone + 0x3BA0), flConvertStoR(*(u16 *)(x + 0x58)));
    flmatRotY33((f32 *)(bone + 0x3D30), flConvertStoR(*(u16 *)(x + 0x5C)));
    flmatRotY33((f32 *)(bone + 0x1940), flConvertStoR(*(u16 *)(x + 0x60)));
    }
}
#endif

/* Builds the monster's world matrix (scale, rotation, translation) into mat[], poses the per-kind special joints
   (mlCalcTransEM) and hands the skeleton to flCalcTransSI. */
#if !defined(G_PART)
void enemy_mk(EMW *em) {
    f32 m[16];
    f32 scale[16];
    EM_MDL *md = em->mdl;
    u8 kind;

    flmatMakeScale(scale, em->scale[0], em->scale[1], em->scale[2]);
    cpRotMatrixYXZ2(em->ang, m);
    flmatSetTrans(m, em->pos[0], em->pos[1], em->pos[2]);
    flmatMul33_2(m, scale);
    flmatCopy((f32 *)((u8 *)em + 0x60), m);
    kind = em->kind;
    if (kind == 1 || (u32)(kind - 6) <= 2 || kind == 0xB || (u32)(kind - 0xE) <= 1 || kind == 0x11 || (u32)(kind - 0x14) <= 2 || kind == 0x1A
        || kind == 0x21 || kind == 0x22) {
        mlCalcTransEM(em, md->bone, m);
    }
    flCalcTransSI(md->bone, m);
}
#endif
/* Position of the monster relative to saved old matrix #n (for riding), written to out as a vector. */
#if !defined(G_PART) || G_PART == 2
void ride_ofs_calc(EMW *em, int unused, s16 n, f32 *out) {
    f32 tr[4];
    f32 inv[16];
    f32 d[3];

    flmatInvert(inv, sys_old_mat[n]);
    flmatGetTrans(tr, sys_old_mat[n]);
    d[0] = em->pos[0] - tr[0];
    d[1] = em->pos[1] - tr[1];
    d[2] = em->pos[2] - tr[2];
    flvecApplyMat33(out, d, inv);
}
#endif
