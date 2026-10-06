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
extern s32 EDIT_TEX[];
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

void pl_create_model(int pl_no) {
    PLM *pl = &player_work[pl_no];
    int i;
    u8 *blk;

    blk = parts_work + pl->id * 0x1600;
    for (i = 0; i < 0x20; i += 2) {
        pl->part[i] = blk;
        pl->part[i + 1] = blk + 0xB0;
        blk += 0x160;
    }
    pl->id = pl_no;
    pl->x4E4 = 0;
}

void weapon_create_model(int wpn, int pl_no, int slot) {
    PLM *pl = &player_work[pl_no];
    int h;

    load_weapon_model(wpn);
    h = get_start_mdlw(1);
    if (h >= 0) {
        pl->wpn_no[slot] = h;
        pl->wpn_n[slot] = 1;
        pl->wpn_ptr[slot] = (s32)get_mdlw_ptr(h);
        set_used_mdlw(h, 1);
        model_work_set((s16)h, pl_area_top, (s16)(slot + (pl_no * 4 + 0x1A)), WEAPON_TEX[wpn], 0x900, 1);
    }
}

typedef struct STW {
    u8 _pad00[0x34];
    s16 n;                      /* 0x34 number of stage model slots */
    s16 _pad36;
    s16 first;                  /* 0x38 first slot */
} STW;

extern STW stage_work;

void release_stage_model(void) {
    s16 i;
    s16 t;
    STW *sw = &stage_work;
    u8 *h;

    i = stage_work.first;
    if (i < i + stage_work.n) {
        h = mdlw_heap + i;
        do {
            t = i;
            model_work_free(t);
            if (*h != 0) {
                clr_used_mdlw(t, 1);
            }
            i++;
            h++;
        } while (i < sw->first + sw->n);
    }
    if (set_top != -1) {
        t = set_top;
        model_work_free(t);
        if (mdlw_heap[t] != 0) {
            clr_used_mdlw(t, 1);
        }
    }
}

void set_create_model(int n) {
    int h;
    int off = n * 4;

    if (*(s32 *)((u8 *)set_model_data + off) != -1) {
        load_set_model(n);
        h = get_start_mdlw(1);
        if (h >= 0) {
            set_top = h;
            set_mdlw = (s32)get_mdlw_ptr(h);
            set_used_mdlw(h, 1);
            model_work_set((s16)h, stage_model, 0x12D, *(s32 *)((u8 *)SET_TEX + off), 0, 0);
        }
    } else {
        set_top = -1;
        set_mdlw = 0;
    }
}
void edit_create_model(void) {
    int a;
    int h;
    int i;

    for (i = 0; i < 2; i++) {
        load_edit_model(i);
        a = pl_area_top;
        h = get_start_mdlw(1);
        if (h < 0) {
            break;
        }
        edit_top[i] = h;
        edit_mdlw[i] = (s32)get_mdlw_ptr(h);
        set_used_mdlw(h, 1);
        model_work_set((s16)h, a, (s16)(10 + i * 0x32), EDIT_TEX[i], 0x900, 2);
    }
}

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

void release_enemy_model(s16 slot) {
    s16 i;
    s16 j;
    u8 *h;
    int end;
    int end2;
    int t;

    t = slot * 2;
    i = *(s16 *)((u8 *)game_w.mdl_no + t - (int)0 );
    end = i + *(s16 *)((u8 *)game_w.mdl_n + t);
    if (i < end) {
        h = mdlw_heap + i;
        do {
            model_work_free(i);
            if (*h != 0) {
                clr_used_mdlw(i, 1);
            }
            i++;
            h++;
        } while (i < end);
    }
    if (game_w.sub_n[slot] != -1) {
        j = game_w.sub_no[slot];
        end2 = j + game_w.sub_n[slot];
        if (j < end2) {
            h = mdlw_heap + j;
            do {
                model_work_free(j);
                if (*h != 0) {
                    clr_used_mdlw(j, 1);
                }
                j++;
                h++;
            } while (j < end2);
        }
    }
    em_motion_free(slot);
    game_w.em_kind[slot] = 0;
}

void em_create_model(int slot) {
    int area;
    int o20;
    GWM *g = &game_w;
    int o4;
    int o2;
    int h;
    u8 *kp;

    if (g->em_kind[slot] == 0) {
        g->mdl_no[slot] = 0;
        g->mdl_n[slot] = 0;
        g->mdl_ptr[slot] = 0;
        return;
    }
    kp = &g->em_kind[slot];
    load_enemy_model(*kp);
    area = pl_area_top;
    h = get_start_mdlw(1);
    if (h < 0) {
        return;
    }
    o2 = slot * 2;
    g->mdl_no[slot] = h;
    g->mdl_n[slot] = 1;
    o4 = slot * 4;
    g->mdl_ptr[slot] = (s32)get_mdlw_ptr(h);
    set_used_mdlw(h, 1);
    o20 = (o4 + slot) * 4;
    em_model_work_set((s16)h, area, (s16)(o20 + 0x9A), ENEMY_TEX[*kp], 0x900, slot & 0xFF);
    em_motion_load(slot, *kp);
    if (em_sub_model_data[*kp] != -1) {
        load_enemy_sub_model(*kp, -1, *kp * 4);
        area = pl_area_top;
        h = get_start_mdlw(1);
        if (h < 0) {
            g->sub_n[slot] = -1;
            return;
        }
        g->sub_no[slot] = h;
        g->sub_n[slot] = 1;
        g->sub_ptr[slot] = (s32)get_mdlw_ptr(h);
        set_used_mdlw(h, 1);
        model_work_set((s16)h, area, (s16)(o20 + 0xA6), ENEMY_SUB_TEX[*kp], 0x900, 0);
        return;
    }
    g->sub_n[slot] = -1;
}

void npc_create_model(int slot) {
    int h;
    int area;

    load_npc_model(slot);
    area = pl_area_top;
    h = get_start_mdlw(1);
    if ((s16)h >= 0) {
        game_w.mdl_no[slot] = h;
        game_w.mdl_n[slot] = 1;
        game_w.mdl_ptr[slot] = (s32)get_mdlw_ptr((s16)h);
        set_used_mdlw((s16)h, 1);
        switch (slot) {
        case 0:
            pl_model_work_set((s16)h, area, (s16)(slot * 0x14 + 0x9A), NPC_TEX[slot], 0x900, 1);
            return;
        case 1:
            em_model_work_set((s16)h, area, (s16)(slot * 0x14 + 0x9A), NPC_TEX[slot], 0x900, 1);
            em_motion_load(slot, 0xA);
            return;
        case 2:
            em_model_work_set((s16)h, area, (s16)(slot * 0x14 + 0x9A), NPC_TEX[slot], 0x900, 1);
            em_motion_load(slot, 9);
            return;
        case 3:
            em_model_work_set((s16)h, area, (s16)(slot * 0x14 + 0x9A), NPC_TEX[slot], 0x900, 1);
            em_motion_load(slot, 0x20);
            break;
        }
    }
}
