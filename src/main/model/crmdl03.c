/* Pl_model_id_set (SLPM_654.95 main 0x00123F60-0x00124310): fills the six model numbers of a hunter (PLW+0x358..0x35D: legs, face, hair/helmet,
 * body, arms, waist) from the equipped parts (PLW+0x352..0x357). An armour entry (0x14 bytes) has a male model [0], a female model [1] and
 * availability bits [2] (1 male, 2 female); a part without a model for the sex gets the bare model 0 (the hair style PLW+0x34E for the head).
 * Face: PLW+0x353 - 1, plus the skin colour PLW+0x607 from skin_col_tbl_m/_f. Field names are guesses. */
#include "types.h"
typedef struct PLI {
    u8 _pad00[0x11];
    u8 sex;
    u8 _pad12[0x34E - 0x12];
    u8 hair;
    u8 _pad34F[0x352 - 0x34F];
    u8 leg, face, head, body, arm, waist;
    u8 m_leg, m_face, m_head, m_body, m_arm, m_waist;
    u8 _pad35E[0x607 - 0x35E];
    u8 skin;
} PLI;
typedef struct ARM { u8 m, f, flag; u8 _pad[0x11]; } ARM;
extern ARM Armor_Leg_Data[], Armor_Body_Data[], Armor_Arm_Data[], Armor_Waist_Data[], Armor_Head_Data[];
extern u8 parts_max_tbl[];
extern u8 skin_col_tbl_m[], skin_col_tbl_f[];

void Pl_model_id_set(PLI *pl) {
    s16 i;
    ARM *a;
    u8 f;

    i = 0;
    do {
        switch (i) {
        case 1:
            f = pl->face;
            if (f <= 0 && parts_max_tbl[pl->sex * 6 + 1] < f) {
                f = 1;
            }
            pl->m_face = f - 1;
            if (pl->sex == 0) {
                pl->skin = skin_col_tbl_m[pl->m_face];
            } else {
                pl->skin = skin_col_tbl_f[pl->m_face];
            }
            break;
        case 0:
            if (pl->sex == 0) {
                a = &Armor_Leg_Data[pl->leg];
                if (a->flag & 1) {
                    pl->m_leg = a->m;
                } else {
                    pl->m_leg = 0;
                }
            } else {
                a = &Armor_Leg_Data[pl->leg];
                if (a->flag & 2) {
                    pl->m_leg = a->f;
                } else {
                    pl->m_leg = 0;
                }
            }
            break;
        case 3:
            if (pl->sex == 0) {
                a = &Armor_Body_Data[pl->body];
                if (a->flag & 1) {
                    pl->m_body = a->m;
                } else {
                    pl->m_body = 0;
                }
            } else {
                a = &Armor_Body_Data[pl->body];
                if (a->flag & 2) {
                    pl->m_body = a->f;
                } else {
                    pl->m_body = 0;
                }
            }
            break;
        case 4:
            if (pl->sex == 0) {
                a = &Armor_Arm_Data[pl->arm];
                if (a->flag & 1) {
                    pl->m_arm = a->m;
                } else {
                    pl->m_arm = 0;
                }
            } else {
                a = &Armor_Arm_Data[pl->arm];
                if (a->flag & 2) {
                    pl->m_arm = a->f;
                } else {
                    pl->m_arm = 0;
                }
            }
            break;
        case 5:
            if (pl->sex == 0) {
                a = &Armor_Waist_Data[pl->waist];
                if (a->flag & 1) {
                    pl->m_waist = a->m;
                } else {
                    pl->m_waist = 0;
                }
            } else {
                a = &Armor_Waist_Data[pl->waist];
                if (a->flag & 2) {
                    pl->m_waist = a->f;
                } else {
                    pl->m_waist = 0;
                }
            }
            break;
        case 2:
            if (pl->head == 0) {
                pl->m_head = pl->hair;
            } else if (pl->sex == 0) {
                a = &Armor_Head_Data[pl->head];
                if (a->flag & 1) {
                    pl->m_head = a->m;
                } else {
                    pl->m_head = pl->hair;
                }
            } else {
                a = &Armor_Head_Data[pl->head];
                if (a->flag & 2) {
                    pl->m_head = a->f;
                } else {
                    pl->m_head = pl->hair;
                }
            }
            break;
        }
        i++;
    } while (i < 6);
}
