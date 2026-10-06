/* NONMATCHING (not built). armor_create_model (SLPM_654.95 0x00124310-0x001246D4): per part (legs, face, hair, body, arms, waist) loads the armour
 * model, reserves a model work slot and calls model_work_set2. Every instruction matches (check.py 105 lines differ only by the s-register
 * numbers of i/p2/p4/off: orig i=s2 p2=s1 p4=s7 off=s6 id=s3 h=s4, ours i=s6 p2=s2 p4=s7 off=s4 id=s3). Declaration order does not matter, nor
 * does an `s16` or prototyped load_armor_model. Field names are guesses. */
#include "types.h"
typedef struct PLA {
    u8 _pad00[0xC];
    u16 id;                     /* 0x00C */
    u8 _pad0E[0x11 - 0xE];
    u8 sex;                     /* 0x011 */
    u8 _pad12[0x358 - 0x12];
    u8 mdl[6];                  /* 0x358 model number of the six parts */
    u8 _pad35E[0x508 - 0x35E];
    s16 body_n;                 /* 0x508 */
    s16 body_no;                /* 0x50A */
    s32 body_ptr;               /* 0x50C */
    u8 _pad510[0x534 - 0x510];
    s32 part_ptr[6];            /* 0x534 */
    s16 part_n[6];              /* 0x54C */
    s16 part_no[6];             /* 0x558 */
    u8 _pad564[0x607 - 0x564];
    u8 skin;                    /* 0x607 */
} PLA;

extern s16 reg_nude_model[], body_nude_model[], arm_nude_model[];
extern s32 parts_tex[];
extern s32 pl_area_top;

void Pl_model_id_set();
void load_armor_model();
int get_start_mdlw();
int get_mdlw_ptr();
void set_used_mdlw();
void model_work_set2();

void armor_create_model(PLA *pl) {
    int tex;
    u8 *p2;
    s16 i;
    int id;
    s16 h;
    int off;
    u8 *p4;

    Pl_model_id_set(pl);
    i = 0;
    p2 = (u8 *)pl;
    p4 = (u8 *)pl;
    off = 0;
    do {
        id = (s16)pl->mdl[i];
        switch (i) {
        case 1:
        case 2:
        case 5:
            break;
        case 0:
            if (pl->sex == 0) {
                if (id == 0 || (u32)(id - 13) <= 1 || id == 15) {
                    id = reg_nude_model[pl->skin];
                }
            } else if (id == 0 || (u32)(id - 11) <= 1 || id == 13) {
                id = reg_nude_model[pl->skin + 4];
            }
            break;
        case 3:
            if (pl->sex == 0) {
                if (id == 0 || (u32)(id - 16) <= 1 || id == 18) {
                    id = body_nude_model[pl->skin];
                }
            } else if (id == 0 || (u32)(id - 12) <= 1 || id == 14) {
                id = body_nude_model[pl->skin + 4];
            }
            break;
        case 4:
            if (pl->sex == 0) {
                if (id == 0 || (u32)(id - 16) <= 1 || id == 18) {
                    id = arm_nude_model[pl->skin];
                }
            } else if (id == 0 || (u32)(id - 12) <= 1 || id == 14) {
                id = arm_nude_model[pl->skin + 4];
            }
            break;
        }
        load_armor_model((s16)id, i, pl->sex);
        h = get_start_mdlw(1);
        if (h < 0) {
            return;
        }
        if (i == 0) {
            pl->body_no = h;
            pl->body_n = 1;
            pl->body_ptr = get_mdlw_ptr(h);
        } else {
            *(s16 *)(p2 + 0x558) = h;
            *(s16 *)(p2 + 0x54C) = 1;
            *(s32 *)(p4 + 0x534) = get_mdlw_ptr(h);
        }
        set_used_mdlw(h, 1);
        tex = parts_tex[pl->sex * 6 + i] + id * 8;
        if (i == 0) {
            model_work_set2(h, pl_area_top, (s16)(pl->id * 12 + 0x3A + off), tex, 0x900, (u16)(pl->skin + pl->sex * 4), 2);
        } else {
            model_work_set2(h, pl_area_top, (s16)(pl->id * 12 + 0x3A + off), tex, 0x900, (u16)(pl->skin + pl->sex * 4), 1);
        }
        i++;
        p2 += 2;
        p4 += 4;
        off += 2;
    } while (i < 6);
}
