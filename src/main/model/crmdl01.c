/* crmdl01 - armor model release (SLPM_654.95 0x001246E0-0x00124818): armor_model_free. Whole file in crmdl_nm.c. */
/* crmdl_nm - SLPM_654.95 0x00123DC0-0x00125060: model creation per kind (player parts, weapon, armor, monster,
   npc, set, edit) and the matching releases. mdlw slots come from get_start_mdlw/set_used_mdlw. Working file. */
#include "types.h"
#include "mdlw.h"
typedef struct PLM {            /* PLW fields touched here (player_work[], 0xA00 each) */
    u8 _pad00[0xC];
    u16 id;                     /* 0x00C */
    u8 _pad0E[0x11 - 0xE];
    u8 sex;                     /* 0x011 0 male, 1 female */
    u8 _pad12[0x110 - 0x12];
    void *part[32];             /* 0x110 matrix block pointers (pairs: block, block + 0xB0) */
    u8 _pad190[0x4E4 - 0x190];
    s16 x4E4;                   /* 0x4E4 */
    u8 _pad4E6[0x508 - 0x4E6];
    s16 mdl_n;                  /* 0x508 slots used by the body model */
    s16 mdl_no;                 /* 0x50A first slot */
    s32 mdl_ptr;                /* 0x50C */
    u8 _pad510[4];
    s32 wpn_ptr[4];             /* 0x514 */
    s16 wpn_n[4];               /* 0x524 */
    s16 wpn_no[4];              /* 0x52C */
    s32 part_ptr[6];            /* 0x534 armor part model pointers */
    s16 part_n[6];              /* 0x54C slots used by the armor parts */
    s16 part_no[6];             /* 0x558 first slot */
    u8 _pad564[0xA00 - 0x564];
} PLM;
extern PLM player_work[];
extern u8 parts_work[];
extern s32 WEAPON_TEX[];
extern s32 pl_area_top;
extern u8 mdlw_heap[0x80];
extern s32 set_top;
extern s32 set_mdlw;
extern s32 stage_model;
extern s32 set_model_data[];
extern s32 SET_TEX[];
extern s32 EDIT_TEX[2];
extern s32 edit_mdlw[2];
extern s32 edit_top[2];
void load_weapon_model(int);
void load_set_model(int);
void load_edit_model(int);
int get_start_mdlw(int);
MDLW *get_mdlw_ptr(int);
void set_used_mdlw(int, int);
void clr_used_mdlw(int, int);
MDLW *model_work_set(int, int, int, int, int, int);
void model_work_free(int);
typedef struct STW {
    u8 _pad00[0x34];
    s16 n;                      /* 0x34 number of stage model slots */
    s16 _pad36;
    s16 first;                  /* 0x38 first slot */
} STW;
extern STW stage_work;
void armor_model_free(PLM *pl) {
    int i;
    u8 *h;
    int k;
    u8 *a;
    u8 *b;
    u8 *g;
    int j;

    i = pl->mdl_no;
    if (i < i + pl->mdl_n) {
        h = mdlw_heap + i;
        do {
            model_work_free(i);
            if (*h != 0) {
                clr_used_mdlw(i, 1);
            }
            pl->mdl_ptr = 0;
            pl->mdl_no = 0;
            i++;
            pl->mdl_n = 0;
            h++;
        } while (i < pl->mdl_no + pl->mdl_n);
    }
    k = 0;
    a = (u8 *)pl;
    b = (u8 *)pl;
    do {
        j = *(s16 *)(a + 0x558);
        if (j < j + *(s16 *)(a + 0x54C)) {
            g = mdlw_heap + j;
            do {
                model_work_free(j);
                if (*g != 0) {
                    clr_used_mdlw(j, 1);
                }
                j++;
                g++;
            } while (j < *(s16 *)(a + 0x558) + *(s16 *)(a + 0x54C));
        }
        *(s32 *)(b + 0x534) = 0;
        *(s16 *)(a + 0x558) = 0;
        k++;
        *(s16 *)(a + 0x54C) = 0;
        b += 4;
        a += 2;
    } while (k < 6);
}
typedef struct GWM {            /* game_w: monster model bookkeeping, 4 slots */
    u8 _pad00[0x28];
    u8 em_kind[4];              /* 0x28 monster kind per slot, 0 = none */
    u8 _pad2C[0x88 - 0x2C];
    s32 mdl_ptr[4];             /* 0x88 */
    s16 mdl_n[4];               /* 0x98 */
    s16 mdl_no[4];              /* 0xA0 */
    s32 sub_ptr[4];             /* 0xA8 */
    s16 sub_n[4];               /* 0xB8 -1 = none */
    s16 sub_no[4];              /* 0xC0 */
} GWM;
extern GWM game_w;
extern s32 ENEMY_TEX[];
extern s32 ENEMY_SUB_TEX[];
extern s32 NPC_TEX[];
extern s32 em_sub_model_data[];
void em_motion_free(int);
void em_motion_load(int, int);
void load_enemy_model(int);
void load_enemy_sub_model(int, int, int);
void load_npc_model(int);
MDLW *em_model_work_set(int, int, int, int, int, int);
MDLW *pl_model_work_set(int, int, int, int, int, int);
