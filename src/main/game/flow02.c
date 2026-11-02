/* SLPM_654.95 0x001116D0-0x00111B1C: init_pl_work (matched by agent B: be_flag else-branch jumps to the next slot,
   the three weapon halfwords read before they are stored, (u16)i to a K&R Quest_pl_stage_init), init_view_work ..
   em_motion_load. See flow_nm.c. */
#include "g_alias.h"
#include "flow.h"
#include "em.h"

typedef struct PLSEL { u16 kind[4]; u16 pad[2]; } PLSEL;

#define EMB(p, o) (*(u8 *)((u8 *)(p) + MH2_OFS(p, o)))
#define PLH(p, o) (*(s16 *)((u8 *)(p) + MH2_OFS(p, o)))
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
















/* Clears the eight player works and sets up the joined hunters (id, stage, weapon, name or saved data). */
#if !defined(G_PART)
void init_pl_work(void) {
    s16 i;
    PLW *pl;
    u8 *g1;
    u8 *g6;
    u8 *g8;
    s16 w;
    s16 t;
    s16 t1;
    s16 t2;

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
            } else {
                pl->be_flag = 0;
                goto next;
            }
            {
                pl->id = i;
                pl->work011 = g1[0x30];
                EMB(pl, 0x616) = g1[0x70];
                *(s8 *)((u8 *)pl + 0x8BF) = *(s8 *)(g1 + 0x78);
                Quest_pl_stage_init((u16)i);
                if (Online_ck() == 0 && i > 0) {
                    t = *(s16 *)(g6 + 0x40);
                    t1 = *(s16 *)(g6 + 0x42);
                    t2 = *(s16 *)(g6 + 0x44);
                    PLH(pl, 0x35E) = t;
                    PLH(pl, 0x360) = t1;
                    PLH(pl, 0x362) = t2;
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
                    t1 = *(s16 *)(g6 + 0x42);
                    t2 = *(s16 *)(g6 + 0x44);
                    PLH(pl, 0x35E) = w;
                    PLH(pl, 0x360) = t1;
                    PLH(pl, 0x362) = t2;
                    EMB(pl, 0x34C) = Get_weapon_id(g6 + 0x40, w);
                    sprintf(pl->name, lit_277_00358218, i + 1);
                }
                Skill_set_PL(pl);
                pl->kind = Battle_type[EMB(pl, 0x34C)];
            }
        }
next:
        pl++;
        i++;
        g1 += 1;
        g6 += 6;
        g8 += 8;
    } while (i < 8);
}
#endif

#if !defined(G_PART) || G_PART == 1
void init_view_work(void) {
    view_reset();
}
#endif

#if !defined(G_PART) || G_PART == 1
void init_light_work(void) {
    memset(light_work, 0, 0x290);
    light_init(game_w.stage);
}
#endif

#if !defined(G_PART) || G_PART == 1
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
#endif

#if !defined(G_PART) || G_PART == 1
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
        st_model_work_set(m, stage_model, GV(0xEA, 0x111), STAGE_TEX[game_w.stage], 0);
        set_create_model(game_w.stage);
        LoadCameraData(game_w.stage);
    }
}
#endif

#if !defined(G_PART) || G_PART == 1
void com_motion_load(void) {
    load_plcom_motion();
    create_plcom_motion();
}
#endif

#if !defined(G_PART) || G_PART == 1
void pl_motion_load(int n) {
    load_pl_motion();
    create_pl_motion(n);
}
#endif

#if !defined(G_PART) || G_PART == 1
void em_motion_load(int a, int b) {
    load_em_motion();
    create_em_motion(a, b);
}
#endif
