/* flow_nm - game start-up helpers (SLPM_654.95 0x00110F10-0x00111B1C, main.bin): game_init clears game_w, stage_load
 * loads the stage and character models and motions for a round (stage_init/st_model_load/load_eft/load_shadow),
 * round_init places the monsters of the quest (select_w+0x9C kind and +0xA4 count per slot, softdip 0x17 = the debug
 * layout of 4 kinds) and init_pl_work builds the player works of the session (name PLAYERn for the others).
 * Field names are guesses from use. */
#include "flow.h"
#include "em.h"

typedef struct PLSEL { u16 kind[4]; u16 pad[2]; } PLSEL;

#define EMB(p, o) (*(u8 *)((u8 *)(p) + (o)))
#define PLH(p, o) (*(s16 *)((u8 *)(p) + (o)))
#define SELB(o) FLD8(select_w, o)
#define SELH(o) FLD16(select_w, o)
#define SELA(o, i) (((u16 *)((u8 *)&select_w + (o)))[i])

extern PLW player_work[];
extern u8 pl_cnt_w[];
extern s32 stage_model;
extern s32 EFT_TEX[];
extern s32 eft_mdlw[];
extern s32 STAGE_TEX[];
extern u8 Battle_type[];
extern char lit_277_00358218[];
extern u8 swset_w[];
extern s16 Plsw_buff[4];
extern s16 Plan_buff[4];
extern s16 Plan_ang[4];
extern s16 Plan_pow[4];
extern u8 light_work[];

void *memset(void *, int, int);
int sprintf();
void Pit_init();
void func_63B020();
void Q_camera_init();
void all_model_free();
void all_sprite_free();
void flCompact();
void CameraWorkInit();
void CameraInit();
void LoadCameraData();
void all_reset();
void all_initmotion_free();
void load_pit();
void debug_model_load();
void load_stage_model();
void load_stage_hit();
void load_eft_model();
int get_start_mdlw();
s32 get_mdlw_ptr();
void set_used_mdlw();
void st_model_work_set(s16, s32, s16, s32, int);
void model_work_set(s16, s32, s16, s32, int, int);
void set_create_model();
void stage_w_init();
void fade_reset();
void set_viewproj();
void softdip_set();
void softdip_clear();
int softdip_ck();
void Quest_em_init_set();
void em_create_model();
EMW *pull_enemy_work();
void init_view_work();
void init_light_work();
void swset_w_init();
void st_model_load();
void init_pl_work();
void player_all_load();
void pl_create_model();
void player_init0();
void pl_motion_load();
void load_plcom_motion();
void create_plcom_motion();
void load_pl_motion();
void create_pl_motion();
void load_em_motion();
void create_em_motion();
void Quest_pl_stage_init();
int Online_ck();
int Pl_master_ck();
void Set_userdata();
void Set_mini_data_to_pl();
u8 Get_weapon_id();
void Skill_set_PL();
void view_reset();
void light_init();
void load_eft();
void load_shadow();
void com_motion_load();

void game_init(void) {
    memset(&game_w, 0, 0x224);
    all_model_free();
    all_sprite_free();
    flCompact();
    GWS16(0x1E) = 0;
    SELH(0xAC) = 0x88;
    GWS8(0x21) = 1;
    GWS8(0xD1) = 0;
    GWS8(0x20) = 0;
    GWS8(0x1DC) = 0;
    GWS8(0x17) = SELB(0xB6);
    CameraWorkInit();
}

void stage_init(void) {
    Pit_init();
    func_63B020();
    Q_camera_init();
}

void stage_load(void) {
    int i;

    all_reset();
    st_model_load(game_w.x15);
    com_motion_load(0);
    init_pl_work();
    i = 0;
    if (0 < game_w.pl_num) {
        do {
            if (game_w.pl_state[i] == 1) {
                player_all_load(i);
            } else {
                player_work[i].be_flag = 0;
                player_work[i].x01 = 0;
            }
            i++;
        } while (i < game_w.pl_num);
    }
    load_pit();
    load_eft();
    load_shadow();
    debug_model_load();
    all_initmotion_free();
}

void player_all_load(int n) {
    PLW *pl = &player_work[n];

    pl_motion_load(n, pl->kind);
    pl_create_model(n);
    player_init0(pl);
}

void load_eft(void) {
    s32 m;

    load_eft_model(0);
    m = get_start_mdlw(1);
    if (m >= 0) {
        eft_mdlw[0] = get_mdlw_ptr(m);
        set_used_mdlw(m, 1);
        model_work_set(m, stage_model, 0x10A, EFT_TEX[0], 0xB00, 0);
        load_eft_model(4);
        m = get_start_mdlw(1);
        if (m >= 0) {
            eft_mdlw[4] = get_mdlw_ptr(m);
            set_used_mdlw(m, 1);
            model_work_set(m, stage_model, 0x126, EFT_TEX[4], 0x800, 1);
        }
    }
}

void load_shadow(void) {
    int m;
    s32 *mdl;
    s32 *tex;
    int i;

    i = 1;
    mdl = &eft_mdlw[1];
    tex = &EFT_TEX[1];
loop:
    load_eft_model(i);
    m = get_start_mdlw(1);
    if (m >= 0) {
        *mdl = get_mdlw_ptr(m);
        set_used_mdlw(m, 1);
        model_work_set(m, stage_model, (s16)i + 0x127, *tex, 0x900, 1);
        i++;
        mdl++;
        tex++;
        if (i < 4) {
            goto loop;
        }
    }
}

void round_init(int mode) {
    u8 cnt[4];
    s16 k;
    s16 i;
    s16 j;
    EMW *e;

    swset_w_init();
    init_view_work();
    init_light_work();
    stage_w_init();
    fade_reset();
    GWS8(0x23) = 0;
    FLDS8(system_w, 0x2F) = 0xFF;
    GWS8(0x24) = 0;
    set_viewproj(0);
    CameraInit();
    if (mode != 0xFF) {
        if (game_w.stage == 0xD) {
            softdip_set(0x64);
            softdip_set(0x65);
            softdip_set(0x66);
        } else {
            softdip_clear(0x64);
            softdip_clear(0x65);
            softdip_clear(0x66);
            softdip_clear(0x67);
            softdip_clear(0x68);
        }
        if (game_w.quest != 0) {
            Quest_em_init_set(game_w.stage);
            return;
        }
        if (softdip_ck(0x17) == 0) {
            cnt[0] = 0;
            k = 0;
            cnt[1] = 0;
            i = 0;
            cnt[2] = 0;
            cnt[3] = 0;
            do {
                game_w.x28[i] = SELA(0x9C, i);
                if (game_w.x28[i] != 0) {
                    em_create_model(i);
                    j = 0;
                    if (SELA(0xA4, i) > 0) {
                        do {
                            e = pull_enemy_work();
                            if (e != 0) {
                                e->mdl_no = i;
                                e->kind = game_w.x28[e->mdl_no];
                                e->type = cnt[i]++;
                                e->id = k;
                                e->stg = game_w.stage;
                            }
                            j++;
                            k++;
                        } while (j < SELA(0xA4, i));
                    }
                }
                i++;
            } while (i < 4);
            return;
        }
        game_w.x28[0] = 0xB;
        game_w.x28[1] = 0xA;
        game_w.x28[2] = 3;
        game_w.x28[3] = 4;
        i = 0;
        do {
            em_create_model(i);
            i++;
        } while (i < 4);
        i = 0;
        do {
            if (softdip_ck(0x74) != 0) {
                if (game_w.stage != 0) {
                    goto next;
                }
            } else if (i != 0) {
                goto next;
            }
            e = pull_enemy_work();
            if (e != 0) {
                if (i == 0) {
                    e->mdl_no = 0;
                } else if (i > 0 && i < 3) {
                    e->mdl_no = 1;
                } else if (i >= 3 && i < 5) {
                    e->mdl_no = 2;
                } else {
                    e->mdl_no = 3;
                    if (i == 5) {
                        EMB(e, 0x442) = 1;
                    } else {
                        EMB(e, 0x442) = 0;
                    }
                }
                e->mdl_no = 0;
                e->kind = game_w.x28[e->mdl_no];
                if (e->kind == 1 && game_w.stage == 0xB) {
                    e->type = 1;
                } else {
                    e->type = 0;
                }
                e->id = i;
                e->stg = game_w.stage;
            }
next:
            i++;
        } while (i < 2);
        if (game_w.stage == 8) {
            e = pull_enemy_work(game_w.stage);
            if (e != 0) {
                e->mdl_no = 1;
                e->kind = game_w.x28[e->mdl_no];
                e->id = i;
                e->stg = game_w.stage;
            }
        }
    }
}

void init_pl_work(void) {
    s16 i;
    PLW *pl;
    u8 *g1;
    u8 *g6;
    u8 *g8;
    s16 w;
    s16 t;

    memset(pl_cnt_w, 0, 0x10);
    g1 = (u8 *)&game_w;
    i = 0;
    pl = player_work;
    g6 = (u8 *)&game_w;
    g8 = (u8 *)&game_w;
    do {
        memset(pl, 0, 0xA00);
        if (i < game_w.pl_num) {
            if (g1[0x208] == 1) {
                pl->be_flag = 1;
                pl->id = i;
                pl->work011 = g1[0x30];
                EMB(pl, 0x616) = g1[0x70];
                *(s8 *)((u8 *)pl + 0x8BF) = *(s8 *)(g1 + 0x78);
                Quest_pl_stage_init(i & 0xFFFF);
                if (Online_ck() == 0 && i > 0) {
                    t = *(s16 *)(g6 + 0x40);
                    PLH(pl, 0x35E) = t;
                    PLH(pl, 0x360) = *(s16 *)(g6 + 0x42);
                    PLH(pl, 0x362) = *(s16 *)(g6 + 0x44);
                    EMB(pl, 0x34C) = Get_weapon_id(g6 + 0x40, t);
                    sprintf(pl->name, lit_277_00358218, i + 1);
                } else if (g1[0x70] == 0) {
                    if (Pl_master_ck(pl) == 1) {
                        Set_userdata(pl);
                    } else {
                        Set_mini_data_to_pl(g8 + 0x1E8, pl);
                    }
                } else {
                    w = *(s16 *)(g6 + 0x40);
                    PLH(pl, 0x35E) = w;
                    PLH(pl, 0x360) = *(s16 *)(g6 + 0x42);
                    PLH(pl, 0x362) = *(s16 *)(g6 + 0x44);
                    EMB(pl, 0x34C) = Get_weapon_id(g6 + 0x40, w);
                    sprintf(pl->name, lit_277_00358218, i + 1);
                }
                Skill_set_PL(pl);
                pl->kind = Battle_type[EMB(pl, 0x34C)];
            } else {
                pl->be_flag = 0;
            }
        }
        pl++;
        i++;
        g1 += 1;
        g6 += 6;
        g8 += 8;
    } while (i < 8);
}

void init_view_work(void) {
    view_reset();
}

void init_light_work(void) {
    memset(light_work, 0, 0x290);
    light_init(game_w.stage);
}

void swset_w_init(void) {
    memset(swset_w, 0, 0x20);
    Plsw_buff[0] = 0;
    Plsw_buff[1] = 0;
    Plan_buff[0] = 0;
    Plan_buff[1] = 0;
    Plan_ang[0] = 0;
    Plan_ang[1] = 0;
    Plan_pow[0] = 0;
    Plan_pow[1] = 0;
    Plsw_buff[2] = 0;
    Plsw_buff[3] = 0;
    Plan_buff[2] = 0;
    Plan_buff[3] = 0;
    Plan_ang[2] = 0;
    Plan_ang[3] = 0;
    Plan_pow[2] = 0;
    Plan_pow[3] = 0;
}

void st_model_load(stage)
u8 stage;
{
    int m;
    STGW *sw = &stage_work;

    game_w.stage = stage;
    sw->stage = stage;
    load_stage_model(game_w.stage);
    load_stage_hit(game_w.stage);
    m = get_start_mdlw(1);
    if (m >= 0) {
        sw->x38 = m;
        sw->x34 = 1;
        sw->mdls = (void *)get_mdlw_ptr(m);
        set_used_mdlw(m, 1);
        st_model_work_set(m, stage_model, 0xEA, STAGE_TEX[game_w.stage], 0);
        set_create_model(game_w.stage);
        LoadCameraData(game_w.stage);
    }
}

void com_motion_load(void) {
    load_plcom_motion();
    create_plcom_motion();
}

void pl_motion_load(int n) {
    load_pl_motion();
    create_pl_motion(n);
}

void em_motion_load(int a, int b) {
    load_em_motion();
    create_em_motion(a, b);
}
