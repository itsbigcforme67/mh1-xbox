/* SLPM_654.95 0x00226C30-0x002270A0: Quest_start .. Quest_start. See f_quest_nm.c. */
#include "quest.h"
#include "plf.h"

typedef char *va_list;
void str_gattai(char *dst, char *fmt, ...);
QEM *em_work_serch2(s16, s16);
u16 stolen_item_stack(int, s16);
void Quest_start();
void Quest_retire_set();
void Quest_error_set2();
void Quest_error_set();
void Quest_restart();
void Quest_em_init_set();
void Quest_enemy_die();
void Quest_enemy_escape();
void Quest_enemy_capture();
void Quest_enemy_hagi_set();
EMW * Em_direct_set();
void Quest_next_em_clr();
char * Quest_str_get();
void Item_stolen();
void Item_regained();
void Share_item_conv();
u16 Ext_pick_point_cnt_ck();
void Ext_pick_point_st();
u16 Ext_pick_point_ck();
void Ext_pick_point_st_clr();
s8 Em_hagi_point_set();
void Quest_forfeit_message();
QEM * em_work_serch();
void Quest_timer_reset();
void remuneration_item_set();
u16 * quest_supplies_get();
s16 quest_enemy_ck2();
void quest_presuccess_ptr_set();
void Quest_net_sub();
void quest_em_die();
void quest_timer_send();

extern s16 questName[];
int func_63AF40();
void load_file_mdl();
void Quest_pl_stage_init();
int Quest_time_get();
void quest_em_init();
void Ext_pick_point_init();
int Get_hunter_rank();
int Online_ck();
void net_send_sys();
void station_em_set();
void Quest_next_em_set();
QEM *em_work_serch();
void q_net_send_em_die();
void q_net_send_em_capture();
int quest_enemy_ck_sub();
int em_capture_conv();
void quest_failed_ptr_set();
int Quest_clear_ck();

extern char lit_656_0036B1F0[];
extern char lit_657_0036B210[];
extern char lit_658_0036B230[];
extern char lit_659_0036B250[];
extern char lit_660_0036B270[];
s16 quest_condition_prog();
void Quest_timer_calc();

void func_63ACA0();
void set01_set();
void set01_set2();

#define EM8(e, o) (*(s8 *)((u8 *)(e) + (o)))
void *pull_enemy_work();
void enemy_mv();

s32 *Em_data_com_adrs_get();
s32 *Em_data_st_adrs_get(s32 *, int, int, s8);
void func_5A8170();
void push_em_work();
void release_enemy_model();
void enemy_insurance_sub();
#define PL8(p, o) (*(u8 *)((u8 *)(p) + (o)))
#define PL32(p, o) (*(s32 *)((u8 *)(p) + (o)))

void em_next_tbl_ck();
void func_5589F0();
void em_create_model();
void em_herb_set();
EMW *Em_direct_set();

extern s8 item_regained_tbl[8];
char *func_5C5E20();
void adx_se_set();
s16 Share_item_num_ck();
int share_item_ck_ck();
s16 Quest_share_item_num_ck();
int Share_item_stack();

typedef struct HAGI {
    u16 id;             /* 0x00 */
    u16 cnt;            /* 0x02 */
    f32 rad;            /* 0x04 */
    s16 x08;            /* 0x08 */
    u8 x0A;             /* 0x0A */
    u8 joint;           /* 0x0B */
} HAGI;
extern HAGI *em_hagi_type_tbl[];
void get_joint_pos_em();
void ext_pick_point_fifo_ck();
void ext_pick_point_tbl_set();
void ext_pick_point_tbl_clr_ex();
void ext_pick_point_tbl_clr();
void Ext_pick_point_clr();
void Ext_pick_point_pos();
void Em_hagi_point_clr();
f32 flSqrt(f32);
int Item_get_ck();

int strlen();
int vsprintf();
void set01_set2_use_mem();

extern char lit_1730_0036B290[];

void em_data_st_adrs_set();

void quest_em_init_sub();
void quest_em_init_sub2();

int Quest_f_dra_ck();

extern u16 capture_type_tbl[];
int Pl_item_num_ck();
void em_herb_set();
int quest_enemy_ck_sub2();
void func_535D20();

extern s32 quest_timer_disp_tbl[][2];
extern char lit_2393[];
extern char lit_2394[];
extern char lit_2395[];
extern char lit_2396[];
void set01_set2();
void QuestClearCameraRequest();
int func_53B5C0();

int Net_Share_item_stack();

extern u16 rem_item_calc_tbl[];
extern s8 *rem_item_exit_sel_tbl[];

/* quest_w.x88 entry (8 bytes): kind, unused, offset of its item table in mission_area. */
typedef struct QREM {
    u16 id;             /* 0x00 condition kind, 0xFFFF ends the list */
    u16 x02;            /* 0x02 */
    s32 tbl;            /* 0x04 offset of the REMI table */
} QREM;

/* Reward item table entry (6 bytes), table ends with weight 0xFFFF. */
typedef struct REMI {
    u16 w;              /* 0x00 weight */
    u16 id;             /* 0x02 item */
    u16 num;            /* 0x04 count */
} REMI;

void Quest_start(void)
{
    MISSION *m;
    s32 *p;
    int i;

    game_w.x0D0 = 0;
    quest_w.no = game_w.quest = select_w.xAC;
    game_w.x0D5 = 2;
    game_w.x1E7 = func_63AF40(game_w.quest);
    Ext_pick_point_init();
    for (i = 0; i < 32; i++) {
        quest_w.xBC[i].v = 0;
        quest_w.xBC[i].f = 0;
    }
    quest_w.x140 = 0;
    quest_w.x13C = 0;
    quest_w.x150 = 0;
    quest_w.x14F = 0;
    quest_w.x0B = 0;
    quest_w.xB4[0].a = 0;
    quest_w.xB4[0].b = 0;
    quest_w.xB4[1].a = 0;
    quest_w.xB4[1].b = 0;
    quest_w.xB4[2].a = 0;
    quest_w.xB4[2].b = 0;
    quest_w.xB4[3].a = 0;
    quest_w.xB4[3].b = 0;
    quest_w.xB4[game_w.master].b = Get_hunter_rank(&User_data);
    quest_w.xAE = 1;
    game_w.x28[0] = 0;
    game_w.x28[1] = 0;
    game_w.x28[2] = 0;
    game_w.x28[3] = 0;
    quest_w.x98[0].v = 0;
    quest_w.x98[0].f = 0;
    quest_w.x98[1].v = 0;
    quest_w.x98[1].f = 0;
    quest_w.x98[2].v = 0;
    quest_w.x98[2].f = 0;
    quest_w.x98[3].v = 0;
    quest_w.x98[3].f = 0;
    quest_w.x98[4].v = 0;
    quest_w.x98[4].f = 0;
    quest_w.xAC = 0;
    quest_w.xAD = 0;
    quest_w.x03 = 0;
    quest_w.x00 = 0;
    quest_w.x01 = 0;
    quest_w.x3A = 0;
    quest_w.x3C = 0;
    quest_w.x10 = 0;
    quest_w.x04 = 0;
    quest_w.x1C[0] = 0;
    quest_w.x24[0] = 0;
    quest_w.x1C[1] = 0;
    quest_w.x24[1] = 0;
    quest_w.x1C[2] = 0;
    quest_w.x24[2] = 0;
    quest_w.x1C[3] = 0;
    quest_w.x24[3] = 0;
    quest_w.x2C[0] = 0;
    quest_w.x30[0] = 0;
    quest_w.x2C[1] = 0;
    quest_w.x30[1] = 0;
    if (quest_w.no != 0) {
        if (quest_w.no < 0xC8) {
            load_file_mdl(mission_area, questName[quest_w.no]);
        }
        m = (MISSION *)mission_area;
        Quest_pl_stage_init(0xFF);
        quest_w.x94 = (MISSION2 *)((u32)m->o[0] + (int)mission_area);
        quest_w.x10 = Quest_time_get(1);
        quest_w.xAF = quest_w.x10 / 9000;
        if (quest_w.xAF > 2) {
            quest_w.xAF = 2;
        }
        quest_w.x64 = m;
        quest_w.x6C = (s32 *)(m->o[4] + (int)mission_area);
        quest_w.x38 = 0;
        quest_w.x36 = 0;
        quest_w.x7C = (s32 *)(m->o[7] + (int)mission_area);
        quest_w.x70 = (s32 *)(m->o[12] + (int)mission_area);
        quest_w.x84 = (s32 *)(quest_w.x94->x18 + (int)mission_area);
        quest_w.x74 = (s32 *)(m->o[5] + (int)mission_area);
        quest_w.x78 = (s32 *)(m->o[6] + (int)mission_area);
        quest_w.x80 = (s32 *)(m->o[8] + (int)mission_area);
        quest_w.x8C = (s32 *)(m->o[10] + (int)mission_area);
        quest_w.x90 = (s32 *)(m->o[11] + (int)mission_area);
        quest_w.x14 = quest_w.x94->x08;
        quest_w.x18 = quest_w.x94->x0C;
        quest_w.x88 = (s32 *)(m->o[3] + (int)mission_area);
        quest_w.x40 = quest_w.x94->x00;
        quest_w.x14E = m->o[13];
        p = (s32 *)(m->o[9] + (int)mission_area);
        game_w.x2E = p[0];
        game_w.x2F = p[1];
        quest_em_init(quest_w.x94);
    } else {
        s16 st = select_w.x0A;

        game_w.x2E = 2;
        game_w.stage = st;
        game_w.x15 = st;
    }
}
