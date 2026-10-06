/* lb_gq01 - stage load 0x005D7050-0x005D778C: Lb_stage_load. Whole file in lb_q.c. */
#include "lobby_f.h"
typedef struct LBSP { f32 x, y, z; u16 ang; u8 _pad[2]; } LBSP;
extern LBSP lb_start_pos[];
extern LBSP lb_return_pos[];
extern f32 stage_start_pos[88][3];
typedef struct LBUNI { u8 _pad00[4]; LBV3 pos; u8 _pad10[4]; u16 ang; u8 _pad16[2]; } LBUNI;
extern LBUNI *St_unique_tbl[];
extern PLW player_work[];
void Lb_load_player_all();
void stage_w_init();
void set_viewproj();
void view_reset();
void fade_reset();
void st_model_load();
void Lbc_connect();
void CameraInit();
void Lb_pl_init();
void light_init();
void stage_fog_set(u8);
void stage_set_set(u8);
void Set06_set();
int Event_flag_ck();
void Lb_pl_to_normal();
void Lb_pl_chr_set();
void Lb_send_pl_warp();
void Lb_player_load();

static inline void lb_pl_place(PLW *pl, LBV3 *v, u16 ang) {
    *(LBV3 *)pl->pos = *v;
    pl->ang[1] = *(u16 *)&pl->ang_y = ang;
}
#define LBPLACE(pl, p) \
    *(LBV3 *)(pl)->pos = *(LBV3 *)(p); \
    ang = (p)->ang; \
    (pl)->ang_y = ang; \
    (pl)->ang[1] = ang


extern u8 ClassInfo[];
int Get_sw2();
int SoftKeyboard_alive_check();
int Lbs_RoomExit();
void Lbc_SendMiniData();
void NPCZoomInCameraCancel();
void str_fadeout();
int Fade_busy_ck();
void Lbc_set_prim();
void Pit_reset();
int Lb_menu_move();
void Lbc_init_network_work();
int Lb_select();
void *Lbs_GetRoomInfo();
void Lb_shop();
void Lb_mix();
void Lb_armor_shop();
void Lb_process_shop();
void Lb_guild();
void Lb_event_market();
void Lb_event_GH();
void Lb_event_localLv5End();
void Lb_event_guild();
void Lb_event_guildstart();
void Lb_event_localLast();
void Lb_join();
int Lbs_MatchEntry();
void SetDialogData_HTML();
void Lb_Matching();
int Lbs_MatchEntryCancel();
int Lb_pl_status_m();
int Lb_talk();
void Lb_send_pl_chidori_off();
void cnWrap_SoundRequest();
void Lb_eat();
int Lb_ItemBox_mv();
int Lb_gh_board();
int Lbs_LobbyExit();
void SoftKeyboard_exit();
void To_EnterPlaza2Lobby();
void Lb_ck_menu();
void Lb_menu_exit();

typedef struct LBSYSB { u8 _pad00[0x74]; s16 x74; } LBSYSB;

void Lb_stage_load(int stage) {
    PLW *pl = &player_work[game_w.master];
    int i;
    LBSP *p;
    LBUNI *u;
    u16 ang;
    if (lb_sys.x70 == 0) {
        Lb_load_player_all();
    }
    flCompact();
    game_w.stage = stage;
    pl->stg = stage;
    lb_sys.x6C = 0;
    stage_w_init();
    *(s8 *)0x3D8231 = 1;
    set_viewproj(0);
    view_reset();
    fade_reset();
    st_model_load(stage);
    Lbc_connect();
    CameraInit();
    Lb_pl_init();
    light_init(stage);
    *(u8 *)0x3F36BF = 0xFF;
    stage_fog_set(stage);
    stage_set_set(stage);
    if (lb_sys.x68 != 0x21) {
        lb_sys.x68 = 0;
    }
    PLU8(pl, 0x8EC) = 0;
    PLU8(pl, 0x90F) = 0;
    pl->fish878 = 0;
    pl->work936 = 0;
    pl->work7ED = 0;
    if (game_w.stage == 0x4E) {
        switch (*(s32 *)(cw + 0xBF3C) & 3) {
        case 1:
            Set06_set(2);
            break;
        case 2:
            Set06_set(0);
            break;
        case 3:
            Set06_set(1);
            break;
        }
    } else if (game_w.stage == 0x57 && Event_flag_ck(1) == 0) {
        Set06_set(0);
    }
    if (lb_sys.x71 == -1) {
        switch (game_w.stage) {
        case 0x57:
            p = &lb_start_pos[13];
            LBPLACE(pl, p);
            break;
        case 0x56:
            if (lb_sys.x68 == 0x21) {
                p = &lb_start_pos[14];
                LBPLACE(pl, p);
                Lb_act_set(pl, 0, 0x4F);
                Lb_pl_chr_set(pl, 0x1AB, 0, 0);
            } else {
                lb_pl_place(pl, (LBV3 *)stage_start_pos[stage], 0);
            }
            break;
        case 0x4C:
            p = &lb_start_pos[cw[0x35D1] % 8];
            LBPLACE(pl, p);
            break;
        default:
            lb_pl_place(pl, (LBV3 *)stage_start_pos[stage], 0);
            break;
        }
    } else if (lb_sys.x71 == 0) {
        switch (stage) {
        case 0x4D:
            if (cw[0x35D2] == 0) {
                p = &lb_start_pos[8];
                LBPLACE(pl, p);
            } else {
                p = &lb_return_pos[cw[0x35D1] % 4];
                LBPLACE(pl, p);
            }
            break;
        case 0x50:
            p = &lb_start_pos[9];
            LBPLACE(pl, p);
            break;
        case 0x52:
            p = &lb_start_pos[10];
            LBPLACE(pl, p);
            break;
        case 0x53:
            p = &lb_start_pos[11];
            LBPLACE(pl, p);
            break;
        case 0x55:
            p = &lb_start_pos[12];
            LBPLACE(pl, p);
            break;
        case 0x56:
            p = &lb_start_pos[10];
            LBPLACE(pl, p);
            break;
        case 0x57:
            p = &lb_start_pos[15];
            LBPLACE(pl, p);
            break;
        default:
            u = (LBUNI *)((int)St_unique_tbl[stage] + lb_sys.x71 * 24);
            *(LBV3 *)pl->pos = u->pos;
            pl->ang[1] = u->ang;
            pl->ang_y = u->ang;
            break;
        }
    } else {
        u = (LBUNI *)((int)St_unique_tbl[stage] + lb_sys.x71 * 24);
        *(LBV3 *)pl->pos = u->pos;
        pl->ang[1] = u->ang;
        pl->ang_y = u->ang;
    }
    pl->x3B0 = 0;
    if (lb_sys.x68 != 0x21) {
        if (pl->work011 == 1) {
            Lb_pl_to_normal(pl, 0, 0, 0);
            Lb_pl_chr_set(pl, 0x3F, 0, 0x6A);
        } else {
            Lb_pl_to_normal(pl, 0, 0, 0);
        }
    }
    if (Online_ck() == 1) {
        Lb_send_stage();
    }
    Lb_send_pl_warp(pl);
    Lb_send_pl_status(pl);
    if (game_w.stage == 0x4C || game_w.stage == 0x4D) {
        s8 n = 0;
        for (pl = player_work; n < 8; n++, pl++) {
            if (*(u8 *)pl != 0) {
                u8 *q = cw + n + 0x2BFE;
                if (*(s8 *)q == 0) {
                    *q = 1;
                    Lb_player_load(pl, q);
                    Lbc_connect();
                }
            }
        }
    }
}
