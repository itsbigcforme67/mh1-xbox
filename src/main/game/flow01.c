/* SLPM_654.95 0x00110F10-0x001111E8: game_init .. load_eft. See flow_nm.c. */
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
