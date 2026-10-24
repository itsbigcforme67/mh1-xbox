/* f_quest - the quest file (SLPM_654.95 0x00226C30-0x0022C66C), one translation unit in the original address order:
   quest start / clear / failure / retire, the quest's monster lists (Quest_next_em_set, Em_direct_set, stations),
   stolen and shared items, carving and gathering points (Ext_pick_point_*, Em_hagi_*), the clear conditions
   (quest_condition_prog), the reward (remuneration_item_set) and the online quest messages (Quest_net_sub, q_net_*).
   Built from the earlier per-run files f_quest, f_quest01..04, f_questb..r, qstb07, strg01 by tools/mktu.py
   (agent B, 12 Oct 2026); quest_em_init takes no parameters but is called with one (K&R `()` definition).
   stolen_item_stack matches here (a second pointer p for its second loop).
   13 functions are still kept as the original bytes (config/c_rawfuncs.txt; near-match C in f_quest_nm.c, which
   the PC build links weak): Em_direct_set, Quest_next_em_set, Item_regained, Share_item_stack, Net_Share_item_stack,
   Share_item_num_ck, Ext_pick_point_init, Em_hagi_point_cnt_ck, em_work_serch2, quest_condition_prog,
   remuneration_item_set, quest_item_ck2, Quest_net_sub. */

#include "g_alias.h"
#ifdef MHG_PS2
void sub_main_func_004B55F0();
#endif
#include "quest.h"
#include "plf.h"
#include "types.h"
#include "va.h"
typedef char *va_list;
extern s16 questName[];
int func_63AF40();
void load_file_mdl();
#ifdef __MWERKS__
int Get_hunter_rank();
#else
unsigned char Get_hunter_rank();   /* the real return type (tools/check_kr_returns.py) */
#endif
int Online_ck();
void net_send_sys();
extern char lit_656_0036B1F0[];
extern char lit_657_0036B210[];
extern char lit_658_0036B230[];
extern char lit_659_0036B250[];
extern char lit_660_0036B270[];
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
#define PL8(p, o) (*(u8 *)((u8 *)(p) + (o)))
#define PL32(p, o) (*(s32 *)((u8 *)(p) + (o)))
void func_5589F0();
void em_create_model();
extern s8 item_regained_tbl[8];
char *func_5C5E20();
void adx_se_set();
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
f32 flSqrt(f32);
#ifdef __MWERKS__
int Item_get_ck();
#else
unsigned short Item_get_ck();   /* the real return type (tools/check_kr_returns.py) */
#endif
int strlen();
int vsprintf();
void set01_set2_use_mem();
extern char lit_1730_0036B290[];
int Quest_f_dra_ck();
extern u16 capture_type_tbl[];
#ifdef __MWERKS__
int Pl_item_num_ck();
#else
short Pl_item_num_ck();   /* the real return type (tools/check_kr_returns.py) */
#endif
void func_535D20();
extern s32 quest_timer_disp_tbl[][2];
extern char lit_2393[];
extern char lit_2394[];
extern char lit_2395[];
extern char lit_2396[];
void QuestClearCameraRequest();
int func_53B5C0();
extern u16 rem_item_calc_tbl[];
extern s8 *rem_item_exit_sel_tbl[];
typedef struct QREM {
    u16 id;             /* 0x00 condition kind, 0xFFFF ends the list */
    u16 x02;            /* 0x02 */
    s32 tbl;            /* 0x04 offset of the REMI table */
} QREM;
typedef struct REMI {
    u16 w;              /* 0x00 weight */
    u16 id;             /* 0x02 item */
    u16 num;            /* 0x04 count */
} REMI;
extern u8 *mission_area;

/* prototypes of this file */
void Quest_start(void);
void Quest_retire_set(void);
void Quest_error_set2(void);
void Quest_error_set(void);
void Quest_pl_stage_init();
void Quest_restart(void);
int Quest_clear_ck();
int Quest_time_get();
void Quest_em_init_set();
int Quest_enemy_revival_ck();
int Quest_enemy_revival_set();
void Quest_enemy_die();
void Quest_enemy_escape();
void Quest_enemy_capture();
void Quest_enemy_hagi_set();
int Quest_remuneration_calc(void);
int Quest_condition_judging(void);
void Quest_next_em_clr();
char *Quest_str_get(int n);
static s16 stolen_item_num_ck();
static u16 stolen_item_stack(int item, s16 num);
void Item_stolen();
static int share_item_ck_ck(void);
s16 Quest_share_item_num_ck();
void Share_item_conv();
int Ext_pick_point_set();
u16 Ext_pick_point_cnt_ck();
void Ext_pick_point_clr();
void Ext_pick_point_pos();
void Ext_pick_point_st();
u16 Ext_pick_point_ck();
int Ext_pick_point_ck2();
static void ext_pick_point_fifo_ck(void);
static void ext_pick_point_tbl_clr();
static void ext_pick_point_tbl_clr_ex();
void Ext_pick_point_st_clr(void);
static void ext_pick_point_tbl_set();
s8 Em_hagi_point_set();
int Em_tail_hagi_point_set();
void Em_hagi_point_clr();
void Quest_forfeit_message(void);
static void str_gattai(char *dst, char *fmt, ...);
static void enemy_insurance_sub();
static void em_next_tbl_ck();
static void em_data_st_adrs_set();
static void quest_em_init();
static void quest_em_init_sub();
static void quest_em_init_sub2();
static void station_em_set(void);
static QEM *em_work_serch();
void Quest_timer_calc();
void Quest_timer_reset(void);
static u16 *quest_supplies_get();
int quest_item_set();
int quest_enemy_set();
static int quest_item_ck();
static int quest_item_ck3();
static int quest_share_item_ck();
static int quest_enemy_ck();
static s16 quest_enemy_ck2();
static int quest_enemy_ck_sub();
static int quest_enemy_ck_sub2();
static int em_capture_conv();
static void quest_failed_ptr_set();
static void quest_presuccess_ptr_set();
static void em_herb_set(void);
static void quest_em_die(void);
static void quest_timer_send(void);
static void q_net_send_em_die();
static void q_net_send_em_capture();
EMW *Em_direct_set(QEM *q);
void Quest_next_em_set();
void Item_regained();
int Share_item_stack();
int Net_Share_item_stack();
s16 Share_item_num_ck();
void Ext_pick_point_init(void);
int Em_hagi_point_cnt_ck();
static QEM *em_work_serch2();
s16 quest_condition_prog();
void remuneration_item_set(void);
int quest_item_ck2();
void Quest_net_sub(void);

#ifdef VERSION_MHG
/* G (sub_main 0x4B3AA0): quest numbers are u16 in groups (00xxx village, 01xxx-03xxx, 10xxx
 * training, 20xxx; 50000 and up are not on the disc); mission files mNNNNN.mib through the
 * per-group AFS index lists quest_file_tbl; a 0x48-byte mission header (bytes 0x40-0x47 new: base
 * size 0x40, start-area set 0x43, size class 0x44, 0x45-0x47); the quest's own random numbers
 * (quest_w.rnd, sub_main_func_004B5480) for the start area, the reward level bonus and the
 * monsters' sizes. Written from the asm for the PC (agent A, 14 Oct 2026); not byte-checked. */
typedef struct QSIZE_LIM { u8 b[10]; } QSIZE_LIM;
extern QSIZE_LIM quest_size_lim_def;    /* {min, max} percent per size class (5) */
extern QSIZE_LIM quest_size_lim;    /* the limits in use (the host's online) */
extern u8 quest_rnd_host[15];       /* the host's random bytes (online) */
extern s8 quest_bonus_tbl[32];       /* reward-level bonus by random number */
extern s16 *quest_file_tbl[];       /* AFS index of each quest file, per group */
extern s32 quest_data_tbl[];
int func_66E180();              /* Tutorial_quest_ck (game.bin 0x66E180) */
int ran_suu();
static void sub_main_func_004B52E0(void);
static u16 sub_main_func_004B5480(void);
#define MIB8(m, o) (((s8 *)(m))[o])

#if !defined(G_PART)
void Quest_start(void)
{
    MISSION *m;
    s32 *p;
    int i;
    int no;

    game_w.x0D0 = 0;
    quest_w.no = game_w.quest = select_w.xAC;
    game_w.x0D5 = 2;
    game_w.x1E7 = func_66E180(game_w.quest);
    Ext_pick_point_init();
    if (Online_ck() == 0) {
        for (i = 0; i < 15; i++) {
            quest_w.rnd[i] = ran_suu(0);
        }
        quest_size_lim = quest_size_lim_def;
    } else {
        for (i = 0; i < 15; i++) {
            quest_w.rnd[i] = quest_rnd_host[i];
        }
    }
    sub_main_func_004B52E0();
    sub_main_func_004B5480();
    for (i = 0; i < 32; i++) {
        quest_w.xBC[i].v = 0;
        quest_w.xBC[i].f = 0;
    }
    for (i = 0; i < 52; i++) {
        quest_w.x16C[i] = 0;
        quest_w.x1D4[i] = 0;
    }
    for (i = 0; i < 4; i++) {
        quest_w.rnd[i] = sub_main_func_004B5480();
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
    quest_w.x23C = 0;
    quest_w.x23E = 0;
    quest_w.x240 = 0;
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
    no = quest_w.no;
    if (no != 0) {
        if (no < 50000) {
            if (no < 10000) {
                i = no / 1000;
            } else if (no < 20000) {
                i = 4;
            } else {
                i = 5;
            }
            load_file_mdl(mission_area, quest_file_tbl[i][no % 1000]);
        }
        m = (MISSION *)mission_area;
        quest_w.x64 = m;
        quest_w.x94 = (MISSION2 *)(m->o[0] + (int)m);
        Quest_pl_stage_init(0xFF);
        quest_w.x10 = Quest_time_get(1);
        quest_w.xAF = (quest_w.x10 - 1) / 9000;
        if (quest_w.xAF > 2) {
            quest_w.xAF = 2;
        }
        quest_w.x38 = 0;
        quest_w.x6C = (s32 *)(m->o[4] + (int)mission_area);
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
        if (!(quest_w.x40 & 0x4000)) {
            quest_w.x14E += quest_bonus_tbl[sub_main_func_004B5480() & 0x1F];
        }
        if (quest_w.x14E >= 16) {
            quest_w.x14E = 15;
        }
        if (quest_w.x14E < 0) {
            quest_w.x14E = 0;
        }
        p = (s32 *)(m->o[9] + (int)mission_area);
        game_w.x2E = p[0];
        game_w.x2F = p[1];
        quest_w.x165[0] = MIB8(m, 0x45);
        quest_w.x165[1] = MIB8(m, 0x46);
        quest_w.x165[2] = MIB8(m, 0x47);
        quest_w.x169 = (quest_w.x10 - 1) / 9000;
        quest_w.x168 = 0;
        quest_em_init();
    } else {
        s16 st = select_w.x0A;

        quest_w.x64 = (MISSION *)quest_data_tbl;
        game_w.x2E = 2;
        game_w.stage = st;
        game_w.x15 = st;
    }
}
#endif
#else
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

#endif

#if !defined(G_PART) || G_PART == 1
void Quest_retire_set(void)
{
    if (Quest_clear_ck(1) == 0) {
        game_w.x0D5 = 7;
        quest_w.x06 = 7;
        quest_w.x36 = -1;
        if (Online_ck() != 0) {
            net_send_sys(0xC, game_w.master);
        }
    }
}
#endif

#if !defined(G_PART) || G_PART == 1
void Quest_error_set2(void)
{
    game_w.x0D5 = 8;
    quest_w.x06 = 8;
    quest_w.x36 = -1;
}
#endif

#if !defined(G_PART) || G_PART == 1
void Quest_error_set(void)
{
    if (Quest_clear_ck(1) == 0) {
        Quest_error_set2();
    }
}
#endif

#ifdef VERSION_MHG
/* G (sub_main 0x4B4370): a mission whose header byte 0x43 is set starts each hunter in an
 * area drawn from a weighted list ({weight, stage} pairs, 0xFF ends): start_area_tbl per mission map
 * (MISSION2+0x14), list number byte 0x43 - 1, drawn with that player's quest random byte. */
extern u8 **start_area_tbl[];

#if !defined(G_PART)
void Quest_pl_stage_init(arg)
int arg;
{
    u8 **lists = start_area_tbl[quest_w.x94->x14];
    u8 *tbl;
    u8 *t;
    int sel;
    int tot;
    int r;

    if (quest_w.no == 0) {
        if ((u16)arg == 0xFF) {
            game_w.stage = select_w.x0A;
            game_w.x15 = select_w.x0A;
        } else {
            player_work[(u16)arg].stg = game_w.stage;
        }
        return;
    }
    sel = MIB8(quest_w.x64, 0x43);
    if (sel != 0) {
        t = lists[sel - 1];
        tot = 0;
        for (tbl = t; *tbl != 0xFF; tbl += 2) {
            tot += *tbl;
        }
        arg = (u16)arg;
        if (arg == 0xFF) {
            r = quest_w.rnd[game_w.master] % tot;
        } else {
            r = quest_w.rnd[arg] % tot;
        }
        if (*t != 0xFF) {
            tot = 0;
            for (;;) {
                tot += *t++;
                if (r < tot) {
                    break;
                }
                t++;
                if (*t == 0xFF) {
                    break;
                }
            }
        }
        if (arg == 0xFF) {
            game_w.x15 = game_w.stage = *t;
        } else {
            player_work[arg].stg = *t;
        }
        return;
    }
    tbl = (u8 *)(*(s32 *)(mission_area + 4) + (int)mission_area);
    arg = (u16)arg;
    if (arg == 0xFF) {
        tbl += game_w.master * 0x10;
        game_w.x15 = game_w.stage = *(s32 *)tbl;
    } else {
        player_work[arg].stg = *(s32 *)(tbl + arg * 0x10);
    }
}
#endif
#else
void Quest_pl_stage_init(arg)
int arg;
{
    u8 *tbl;

    if (quest_w.no == 0) {
        if ((u16)arg == 0xFF) {
            game_w.stage = select_w.x0A;
            game_w.x15 = select_w.x0A;
        } else {
            player_work[(u16)arg].stg = game_w.stage;
        }
        return;
    }
    tbl = (u8 *)(*(s32 *)(mission_area + 4) + (int)mission_area);
    arg = (u16)arg;
    if (arg == 0xFF) {
        tbl += game_w.master * 0x10;
        game_w.x15 = game_w.stage = *(s32 *)tbl;
    } else {
        player_work[arg].stg = *(s32 *)(tbl + arg * 0x10);
    }
}

#endif

#if !defined(G_PART) || G_PART == 2
void Quest_restart(void)
{
    s32 *p = quest_w.x6C;
    s16 i = 0;

    if (Quest_clear_ck(1) == 0) {
        for (;;) {
            if (*(s16 *)p == 0x16) {
                break;
            }
            p += 2;
            i++;
        }
        game_w.x11 = 0;
        quest_w.x36 = i + 1;
    }
}
#endif

#if !defined(G_PART) || G_PART == 2
int Quest_clear_ck(arg)
int arg;
{
    int v = game_w.x0D5;

    if (v == 3 || v == 4) {
        return 1;
    }
    if ((v == 5 || v == 6 || v == 7) && arg != 0) {
        return -1;
    }
    return 0;
}
#endif

#if !defined(G_PART) || G_PART == 2
int Quest_time_get(arg)
int arg;
{
    switch (arg) {
    case 0:
        if (quest_w.x10 < 0) {
            return 0;
        }
        return quest_w.x10;
    case 1:
        return quest_w.x94->x10;
    default:
        if (quest_w.x10 < 0) {
            return 0;
        }
        return quest_w.x10;
    }
}
#endif

#if !defined(G_PART) || G_PART == 2
void Quest_em_init_set(arg)
int arg;
{
    station_em_set();
    Quest_next_em_set(arg);
}
#endif

#if !defined(G_PART) || G_PART == 2
int Quest_enemy_revival_ck(arg)
int arg;
{
    QEM *q;

    if (Quest_clear_ck(0) != 0) {
        return 0;
    }
    q = em_work_serch(arg);
    if (q != 0) {
        if (q->x2E & 1) {
            return 0;
        }
        if (q->x04 > 0) {
            return 1;
        }
    }
    return 0;
}
#endif

#ifdef VERSION_MHG
extern u16 em_size_max_tbl[];   /* G 0x4FD8E0: per species, the largest size a returning monster grows to */
extern char *quest_kill_msg;    /* G 0x52633C: message when the mission's kill count is reached */
extern u8 *em_species_tbl[];
static int sub_main_func_004B7920(s16 kind);
void em_size_scale_set(EMW *em);
void em_dur_init(EMW *em);
void se_req(int, int, int);

/* G (sub_main 0x4B7A60): kinds that grow 5% each time they come back (sub_main_func_004B7920's
 * list without kind 0x21) */
#if !defined(G_PART)
static int em_revival_grow_ck(s16 kind)
{
    return kind != 0x21 && sub_main_func_004B7920(kind) == 1;
}
#endif

/* G (sub_main 0x4B55F0): mission header 0x45 == 2: count kills of kind header 0x46; at the
 * count in header 0x47 show the message and play SE 0x77 once */
#if !defined(G_PART)
static void quest_kill_count_ck(EMW *e)
{
    if (quest_w.x165[0] == 0 || quest_w.x165[0] != 2) {
        return;
    }
    if (quest_w.x165[1] != e->kind) {
        return;
    }
    if (--quest_w.x165[2] > 0) {
        return;
    }
    quest_w.x165[0] = 0;
    set01_set2(quest_kill_msg);
    se_req(1, 0x77, 0);
}
#endif
#endif

#if !defined(G_PART)
int Quest_enemy_revival_set(e)
EMW *e;
{
    QEM *q = em_work_serch(e);

    if (q != 0) {
        if (q->x2E & 1) {
            return 0;
        }
        q->x2E = 2;
#ifdef VERSION_MHG
        /* G (sub_main 0x4B47B0): a big monster comes back 5% bigger (up to its species' limit),
         * and stronger: the quest's level and its own go up by one (up to 14; not for
         * missions with flag 0x8000) */
        if (em_revival_grow_ck(e->kind) == 1) {
            u16 max = em_size_max_tbl[*em_species_tbl[e->kind]];

            e->size += 5;
            if (max < e->size) {
                e->size = max;
            }
            if (quest_w.x14E < 14) {
                if (!(quest_w.x40 & 0x8000)) {
                    quest_w.x14E++;
                }
                if (quest_w.x14E >= 15) {
                    quest_w.x14E = 14;
                }
            }
            if (e->lv < 14) {
                if (!(quest_w.x40 & 0x8000)) {
                    e->lv++;
                }
                if (e->lv >= 15) {
                    e->lv = 14;
                }
            }
        }
        q->x08 = e->x302;
        e->pos[0] = q->pos[0];
        e->pos[1] = q->pos[1];
        e->pos[2] = q->pos[2];
        e->ang[1] = q->x1C;
        e->stg = q->x07;
        em_dur_init(e);
        em_size_scale_set(e);
        return q->x04;
#else
        q->x08 = e->x302;
        e->pos[0] = q->pos[0];
        e->pos[1] = q->pos[1];
        e->pos[2] = q->pos[2];
        e->ang[1] = q->x1C;
        e->stg = q->x07;
        return q->x04;
#endif
    }
    return 0;
}
#endif

#if !defined(G_PART) || G_PART == 19
void Quest_enemy_die(e)
EMW *e;
{
    QEM *q;

    if ((q = em_work_serch(e)) != 0) {
        if (!(q->x2E & 5)) {
            if (q->x05 != 0) {
                quest_w.x34--;
            }
            q->x04--;
#ifdef VERSION_MHG
            /* G (sub_main 0x4B4940): the kill-count message, the "N of kind" target count, the
             * per-kind kill counts (capped 9999) and the biggest size killed per kind */
#ifdef MHG_PS2
            sub_main_func_004B55F0(e);   /* the static quest_kill_count_ck (G keeps it outside this part) */
#else
            quest_kill_count_ck(e);
#endif
            if (quest_w.x240 != 0 && quest_w.x23C == e->kind && quest_w.x23E > 0) {
                quest_w.x23E--;
            }
            quest_w.x1D4[e->kind]++;
            if (quest_w.x1D4[e->kind] > 9999) {
                quest_w.x1D4[e->kind] = 9999;
            }
            if (sub_main_func_004B7920(e->kind) == 1 && e->size > quest_w.x16C[e->kind]) {
                quest_w.x16C[e->kind] = e->size;
            }
#endif
            if (q->x04 > 0) {
                q->x2E = 4;
            } else {
                q->x0A = -1;
                q->x2E = 1;
            }
            if (e->x8C3 == 0) {
                q_net_send_em_die(q, e);
            }
            if (quest_enemy_ck_sub(&quest_w, e->kind) != 0) {
                quest_w.x3C = e;
            }
        }
    }
}
#endif

#if !defined(G_PART) || G_PART == 3
void Quest_enemy_escape(e)
EMW *e;
{
    QEM *q = em_work_serch(e);

    if (q != 0 && !(q->x2E & 1)) {
        q->x08 = e->x302;
        q->x0A = -1;
    }
}
#endif

#if !defined(G_PART) || G_PART == 3
void Quest_enemy_capture(e)
EMW *e;
{
    QEM *q;

    if ((q = em_work_serch(e)) != 0 && !(q->x2E & 8)) {
        em_capture_conv(e->kind);
        if (!(q->x2E & 5)) {
            q_net_send_em_capture(q, e);
            if (q->x05 != 0) {
                quest_w.x34--;
            }
            q->x04--;
            if (q->x04 > 0) {
                q->x2E = 4;
            } else {
                q->x0A = -1;
                q->x2E = 1;
            }
        }
        q->x2E |= 8;
    }
}
#endif

#if !defined(G_PART) || G_PART == 3
void Quest_enemy_hagi_set(a, b)
int a;
int b;
{
    quest_w.x13C = quest_w.x13C | b;
}
#endif

#if !defined(G_PART) || G_PART == 3
int Quest_remuneration_calc(void)
{
    int v;

    if (quest_w.x14 <= 0) {
        return 2;
    }
    v = quest_w.x14 - quest_w.x18;
    quest_w.x14 = v;
    if (v <= 0) {
        if (Quest_clear_ck(1) == 0) {
            quest_failed_ptr_set(0);
        }
        if (quest_w.x14 <= 0) {
            quest_w.x14 = 0;
        }
        return 1;
    }
    return 0;
}
#endif

#if !defined(G_PART)
int Quest_condition_judging(void)
{
    int i;
    int sp3C;
    s8 t;
    int m;
    PLW *pl;
    u8 *rank;

    m = game_w.master;
    pl = &player_work[m];
    for (i = 0; i < 4; i++) {
        if (game_w.pl_state[i] != 1) {
            quest_w.x140 |= 1 << (i + 8);
        }
    }
    if (quest_w.no == 0) {
        return 0;
    }
    t = quest_w.x06;
    if (t == 7) {
        return -2;
    }
    if (t == 8) {
        return -2;
    }
    switch (quest_w.x00) {
    case 0:
        if (t == 4) {
            return 1;
        }
        if (t == 6) {
            return -1;
        }
        if (quest_w.x36 == -1) {
            return 1;
        }
        quest_w.x36 = quest_condition_prog(pl, quest_w.x36, &sp3C, t);
        if (game_w.x1E7 != 0) {
            func_63ACA0();
        }
        return 0;
    case 1:
        switch (quest_w.x01) {
        case 0:
            quest_w.x01++;
            quest_w.x10 = 0x96;
            break;
        case 1:
            Quest_timer_calc(1, 2, quest_w.x00, t);
            if (quest_w.x10 <= 0) {
                quest_w.x01++;
                if (quest_w.x40 & 2) {
                    quest_w.x10 = 0x258;
                    if (Online_ck() == 1) {
                        set01_set2(lit_656_0036B1F0);
                    } else {
                        set01_set2(lit_657_0036B210);
                    }
                } else {
                    quest_w.x10 = 0x708;
                    if (Online_ck() == 1) {
                        set01_set2(lit_658_0036B230);
                    } else {
                        set01_set2(lit_659_0036B250);
                    }
                }
            }
            break;
        case 2:
            Quest_timer_calc(1, 2, quest_w.x00, t);
            if (quest_w.x10 <= 0) {
                quest_w.x01 = 0;
                quest_w.x00++;
                set01_set2(lit_660_0036B270);
                set01_set(0, 0xF, 0);
            }
            break;
        }
        break;
    case 2:
        switch (quest_w.x01) {
        case 0:
            quest_w.x01++;
            quest_w.x06 = 4;
            quest_w.x14 = quest_w.x14 / game_w.pl_num;
            rank = (u8 *)quest_w.xB4 + (m & 0xFF) * 2;
            *rank = Get_hunter_rank(&User_data);
            quest_w.x182 = 0;
            quest_w.x181 = 0xC;
            quest_w.x184 = *rank;
            quest_w.x186 = 0;
            net_send_sys(6, m);
            quest_w.x140 |= 1 << game_w.master;
            break;
        case 1:
            for (i = 0; i < 4; i++) {
                if (game_w.pl_state[i] != 1) {
                    quest_w.x140 |= 1 << i;
                }
            }
            if ((quest_w.x140 & 0xF) == 0xF || game_w.pl_state[m] != 1) {
                quest_w.x182 = 0;
                quest_w.x181 = 0xD;
                quest_w.x01++;
                quest_w.x184 = (u32)quest_w.x13C >> 16;
                quest_w.x186 = quest_w.x13C;
                net_send_sys(6, m);
                quest_w.x140 |= 1 << (game_w.master + 4);
            }
            break;
        case 2:
            for (i = 0; i < 4; i++) {
                if (game_w.pl_state[i] != 1) {
                    quest_w.x140 |= 1 << (i + 4);
                }
            }
            if ((u8)quest_w.x140 == 0xFF || game_w.pl_state[m] != 1) {
                quest_w.x01++;
                game_w.x0D5 = 4;
            }
            break;
        case 3:
            break;
        }
        break;
    case 3:
        break;
    }
    return 0;
}
#endif

/* Em_direct_set: original bytes kept (near-match C in src/main/quest/f_quest_nm.c, linked weak by the PC build): build/raw/Em_direct_set.inc,
   config/c_rawfuncs.txt */
#ifdef __MWERKS__
#if !defined(G_PART)
asm EMW *Em_direct_set(QEM *q)
{
#include "Em_direct_set.inc"
}
#endif
#endif

#ifdef VERSION_MHG
/* G (sub_main 0x4B52E0): seed the quest's generator from its random bytes 0x155..0x162 (seven
 * little-endian halfwords), 0x1C3 if they add up to 0 */
#if !defined(G_PART) || G_PART == 4
static void sub_main_func_004B52E0(void)
{
    int i;

    quest_w.rnd_seed = 0;
    for (i = 0; i < 14; i += 2) {
        quest_w.rnd_seed += (u16)(quest_w.rnd[i] + (u16)(quest_w.rnd[i + 1] << 8));
    }
    if (quest_w.rnd_seed == 0) {
        quest_w.rnd_seed = 0x1C3;
    }
}
#endif

/* G (sub_main 0x4B5480): the quest's generator, seed * 176 mod 65363 (the same numbers on
 * every machine of an online session: the host sends the bytes) */
#if !defined(G_PART) || G_PART == 4
static u16 sub_main_func_004B5480(void)
{
    u32 v = quest_w.rnd_seed;

    if (v == 0) {
        v = 1;
    }
#ifdef VERSION_MHG
    v = (v * 176) % 0xFF53;     /* G returns the value it stored without reading it back */
    quest_w.rnd_seed = v;
    return v;
#else
    quest_w.rnd_seed = (v * 176) % 0xFF53;
    return quest_w.rnd_seed;
#endif
}
#endif

#endif

#if !defined(G_PART) || G_PART == 5
void Quest_next_em_clr(arg0, arg1)
int arg0;
int arg1;
{
    s32 *list;
    EMW *e;
    PLW *pl;
    int i;
    int found;
    QEM *q;
    s32 *p;
    int j;
    int v;
    u8 *g;
    s32 *p2;
    int v2;
    s32 *t74;

    e = em_work;
    pl = player_work;
    list = Em_data_com_adrs_get(quest_w.x78, 0);
    for (i = 0; i < 20; i++, e++) {
        if (e->be_flag != 0) {
            if (list != 0) {
                p = list;
                for (;;) {
                    v = *p;
                    if (v == -1) {
                        found = 0;
                        break;
                    }
                    if (e->x765 != 0) {
                        found = 1;
                        break;
                    }
                    if (e->kind == v) {
                        found = 1;
                        break;
                    }
                    p++;
                }
            } else {
                found = 0;
            }
            if ((q = em_work_serch(e)) != 0 && q->x2E != 1 && found == 0) {
                if (e->kind == 0x17) {
                    func_5A8170(e);
                }
                if (e->x04 >= 2 || e->mode == 5) {
                    Quest_enemy_die(e);
                    if (q->x2E & 4) {
                        q->x2E = 2;
                        q->x08 = -1;
                    }
                } else {
                    q->x08 = e->x302;
                }
                q->x0A = -1;
            }
            if (found == 0) {
                if (e->kind == 0x12) {
                    for (j = 0; j < game_w.pl_num; j++, pl++) {
                        if (pl->be_flag != 0 && pl->x824 == e) {
                            pl->x824 = 0;
                        }
                    }
                }
                push_em_work(e);
            }
        }
    }
    enemy_insurance_sub(arg0);
    for (i = 0, g = (u8 *)&game_w; i < 4; i++, g++) {
        v = g[0x28];
        if (v > 0) {
            if (list != 0) {
                p2 = list;
                for (;;) {
                    v2 = *p2;
                    if (v2 == -1) {
                        found = 0;
                        break;
                    }
                    if (v == v2) {
                        found = 1;
                        break;
                    }
                    p2++;
                }
                if (found != 0) {
                    continue;
                }
            }
            t74 = quest_w.x74;
            list = Em_data_st_adrs_get(t74, arg1, 0, quest_w.x3A);
            if (list != 0) {
                for (;;) {
                    if (*list == -1) {
                        found = 0;
                        break;
                    }
                    if (g[0x28] == *list) {
                        found = 1;
                        break;
                    }
                    list++;
                }
                if (found != 0) {
                    continue;
                }
            }
            release_enemy_model((s16)i);
            g[0x28] = 0;
        }
    }
}
#endif

/* Quest_next_em_set: original bytes kept (near-match C in src/main/quest/f_quest_nm.c, linked weak by the PC build): build/raw/Quest_next_em_set.inc,
   config/c_rawfuncs.txt */
#ifdef __MWERKS__
#if !defined(G_PART)
asm void Quest_next_em_set(int n)
{
#include "Quest_next_em_set.inc"
}
#endif
#endif

#if !defined(G_PART) || G_PART == 6
char *Quest_str_get(int n)
{
    s32 *p;
    if (game_w.x1DC) {
        return func_5C5E20();
    }
    p = quest_w.x84;
    p += n;
    return (char *)((u32)*p + (int)mission_area);
}
#endif

#if !defined(G_PART) || G_PART == 7
static s16 stolen_item_num_ck(item)
u16 item;
{
    s16 i;
    u8 *q;

    for (i = 0, q = (u8 *)&quest_w; i < 5; i++, q += 4) {
        if (*(u16 *)(q + 0x98) == item) {
            return quest_w.x98[i].f;
        }
    }
    return 0;
}
#endif

#if !defined(G_PART)
static u16 stolen_item_stack(int item, s16 num)
{
    s16 i;
    u16 r;
    s8 mx;
    u8 *q;
    u8 *p;

    if (stolen_item_num_ck(item) == 0) {
        r = 5;
        for (i = 0, q = (u8 *)&quest_w; i < 5; i++, q += 4) {
            if (*(u16 *)(q + 0x98) == 0 && num > 0) {
                quest_w.x98[i].v = item;
                quest_w.x98[i].f = num;
                r = 0;
                break;
            }
        }
        if (r == 5) {
            quest_w.x98[quest_w.xAC].v = item;
            quest_w.x98[quest_w.xAC].f = num;
            quest_w.xAC++;
            if (quest_w.xAC >= 5) {
                quest_w.xAC = 0;
            }
        }
    } else {
        for (i = 0, p = (u8 *)&quest_w; i < 5; i++, p += 4) {
            if (*(u16 *)(p + 0x98) == (u16)item) {
                mx = *((s8 *)((s8 *)Item_data + 3) + (item & 0xFFFF) * 16);
                if (num > 0 && quest_w.x98[i].f >= mx) {
                    quest_w.x98[i].f = mx;
                    r = 3;
                } else {
                    quest_w.x98[i].f += (s8)num;
                    if (quest_w.x98[i].f <= 0) {
                        r = 4;
                        quest_w.x98[i].v = 0;
                        quest_w.x98[i].f = 0;
                    } else if (mx < quest_w.x98[i].f) {
                        quest_w.x98[i].f = mx;
                        r = 2;
                    } else {
                        r = 1;
                    }
                }
                break;
            }
        }
    }
    return r;
}
#endif

#if !defined(G_PART) || G_PART == 8
void Item_stolen(pl, item, num)
void *pl;
int item;
s16 num;
{
    game_w.xCC = item;
    game_w.xCE = num;
    game_w.x0D0 = 1;
    stolen_item_stack(item, -num);
    quest_w.xAD = 1;
}
#endif

/* Item_regained: original bytes kept (near-match C in src/main/quest/f_quest_nm.c, linked weak by the PC build): build/raw/Item_regained.inc,
   config/c_rawfuncs.txt */
#ifdef __MWERKS__
#if !defined(G_PART)
asm void Item_regained(PLW *pl, int unused)
{
#include "Item_regained.inc"
}
#endif
#endif

/* Share_item_stack: original bytes kept (near-match C in src/main/quest/f_quest_nm.c, linked weak by the PC build): build/raw/Share_item_stack.inc,
   config/c_rawfuncs.txt */
#ifdef __MWERKS__
#if !defined(G_PART) || G_PART == 9
asm int Share_item_stack(PLW *pl, int item, int num)
{
#include "Share_item_stack.inc"
}
#endif
#endif

/* Net_Share_item_stack: original bytes kept (near-match C in src/main/quest/f_quest_nm.c, linked weak by the PC build): build/raw/Net_Share_item_stack.inc,
   config/c_rawfuncs.txt */
#ifdef __MWERKS__
#if !defined(G_PART) || G_PART == 9
asm int Net_Share_item_stack(PLW *pl, int item, int num)
{
#include "Net_Share_item_stack.inc"
}
#endif
#endif

/* Share_item_num_ck: original bytes kept (near-match C in src/main/quest/f_quest_nm.c, linked weak by the PC build): build/raw/Share_item_num_ck.inc,
   config/c_rawfuncs.txt */
#ifdef __MWERKS__
#if !defined(G_PART)
asm s16 Share_item_num_ck(int item, int mode)
{
#include "Share_item_num_ck.inc"
}
#endif
#endif

#if !defined(G_PART) || G_PART == 10
static int share_item_ck_ck(void)
{
    return (quest_w.x40 & 0x100) != 0;
}
#endif

#if !defined(G_PART) || G_PART == 10
s16 Quest_share_item_num_ck(item)
int item;
{
    s16 i;
    u8 *q;

    for (i = 0, q = (u8 *)&quest_w; i < 4; i++, q += 2) {
        if (*(s16 *)(q + 0x1C) == (u16)item) {
            return i;
        }
    }
    return -1;
}
#endif

#if !defined(G_PART)
void Share_item_conv(pl)
PLW *pl;
{
    int i;
    int j;
    int any;
    u8 *q;

    if (Quest_clear_ck(1) == 0) {
        any = 0;
        for (i = 0, q = (u8 *)&quest_w; i < 4; i++, q += 2) {
            if (*(s16 *)(q + 0x1C) != 0) {
                for (j = 0; j < 20; j++) {
                    if (pl->item[j].id == *(s16 *)(q + 0x1C) && (u16)Share_item_stack(pl, pl->item[j].id, pl->item[j].num) > 0) {
                        any = 1;
                        quest_w.x182 = 0;
                        quest_w.x181 = 9;
                        quest_w.x184 = *(s16 *)(q + 0x1C);
                        quest_w.x186 = Share_item_num_ck((u16) * (s16 *)(q + 0x1C), 1);
                        net_send_sys(6, game_w.master);
                        set01_set(1, 8, *(s16 *)(q + 0x1C));
                    }
                }
            }
        }
        if (any != 0) {
            adx_se_set(pl, 7);
        }
    }
}
#endif

/* Ext_pick_point_init: clears the 20 gathering/carving point slots (agent B; the plain loop, MWCC unrolls it 5x). */
#if !defined(G_PART) || G_PART == 11
void Ext_pick_point_init(void)
{
    STIEM *s;
    int i;

    quest_w.x3B = 0;
    s = StiEM_data;
    for (i = 0; i < 20; i++) {
        s->id = 0xFFFF;
        stiem_stack_tbl[i] = -1;
        s++;
    }
}
#endif

#if !defined(G_PART) || G_PART == 11
int Ext_pick_point_set(a, pos)
STIEM *a;
f32 *pos;
{
    STIEM *s = StiEM_data;
    int i;

    ext_pick_point_fifo_ck();
    for (i = 0; i < 20; i++, s++) {
        if (s->id == 0xFFFF) {
            if (pos == 0) {
                s->pos[0] = a->pos[0];
                s->pos[1] = a->pos[1];
                s->pos[2] = a->pos[2];
            } else {
                s->pos[0] = pos[0];
                s->pos[1] = pos[1];
                s->pos[2] = pos[2];
            }
            s->rad = a->rad;
            s->id = a->id;
            s->cnt = a->cnt;
            s->x14 = 2;
            s->stg = a->stg;
            s->x19 = a->x19;
            s->x1A = a->x1A;
            ext_pick_point_tbl_set((s8)i);
            return i;
        }
    }
    return -1;
}
#endif

#if !defined(G_PART) || G_PART == 11
u16 Ext_pick_point_cnt_ck(n)
int n;
{
    return StiEM_data[n].cnt;
}
#endif

#if !defined(G_PART) || G_PART == 11
void Ext_pick_point_clr(n)
int n;
{
    if (n != -1) {
        StiEM_data[n].id = 0xFFFF;
        ext_pick_point_tbl_clr_ex((s8)n);
    }
}
#endif

#if !defined(G_PART) || G_PART == 11
void Ext_pick_point_pos(n, p)
int n;
f32 *p;
{
    StiEM_data[n].pos[0] = p[0];
    StiEM_data[n].pos[1] = p[1];
    StiEM_data[n].pos[2] = p[2];
}
#endif

#if !defined(G_PART) || G_PART == 11
void Ext_pick_point_st(n, st)
int n;
s8 st;
{
    StiEM_data[n].stg = st;
}
#endif

#if !defined(G_PART) || G_PART == 11
u16 Ext_pick_point_ck(pl)
PLW *pl;
{
    STIEM *s;
    int i;
    f32 dx, dz;
    u16 r;

    s = StiEM_data;
    for (i = 0; i < 20; i++, s++) {
        if (s->id != 0xFFFF && s->stg == pl->stg) {
            if (!(pl->pos[1] < s->pos[1] - 200.0f) && pl->pos[1] < s->pos[1] + 100.0f) {
                dx = pl->pos[0] - s->pos[0];
                dz = pl->pos[2] - s->pos[2];
                if (flSqrt(dx * dx + dz * dz) <= s->rad) {
                    r = s->id;
                    if (pl->pos[1] + 80.0f <= s->pos[1]) {
                        r |= 0x8000;
                    }
                    return r;
                }
            }
        }
    }
    return 0xFFFF;
}
#endif

#if !defined(G_PART) || G_PART == 11
int Ext_pick_point_ck2(pl)
PLW *pl;
{
    STIEM *s;
    int i;
    f32 dx, dz;
    int r;
    u16 c;

    s = StiEM_data;
    for (i = 0; i < 20; i++, s++) {
        if (s->id != 0xFFFF && s->stg == pl->stg) {
            if (!(pl->pos[1] < s->pos[1] - 200.0f) && pl->pos[1] < s->pos[1] + 100.0f) {
                dx = pl->pos[0] - s->pos[0];
                dz = pl->pos[2] - s->pos[2];
                if (flSqrt(dx * dx + dz * dz) <= s->rad) {
                    if ((s32)s->cnt > 0) {
                        r = Item_get_ck(s->id & 0x7FFF) & 0xFFFF;
                        if (s->cnt != 0xFF) {
                            s->cnt = s->cnt - 1;
                        }
                    } else {
                        r = 0xFFFE;
                        r = r & 0xFFFF;
                    }
                    return r;
                }
            }
        }
    }
    return 0;
}
#endif

#if !defined(G_PART) || G_PART == 11
static void ext_pick_point_fifo_ck(void)
{
    int n;
    int i;
    s8 *t;

    n = quest_w.x3B;
    if (n >= 20) {
        n--;
        i = 0;
        if (0 < n) {
            t = stiem_stack_tbl;
            do {
                if (!(StiEM_data[*t].x19 & 1)) {
                    break;
                }
                i++;
                t++;
            } while (i < n);
        }
        Ext_pick_point_clr(stiem_stack_tbl[i]);
    }
}
#endif

#if !defined(G_PART) || G_PART == 11
static void ext_pick_point_tbl_clr(arg)
int arg;
{
    int n = (s8)arg;
    s8 *t;
    int last;

    last = quest_w.x3B - 1;
    if (n < last) {
        t = stiem_stack_tbl + n;
        do {
            n++;
            t[0] = t[1];
            t++;
        } while (n < last);
    }
    stiem_stack_tbl[n] = -1;
    quest_w.x3B--;
}
#endif

#if !defined(G_PART) || G_PART == 11
static void ext_pick_point_tbl_clr_ex(n)
int n;
{
    int i;
    s8 *t;

    i = 0;
    if (0 < quest_w.x3B) {
        t = stiem_stack_tbl;
        do {
            if ((s8)n == *t) {
                ext_pick_point_tbl_clr((s8)i);
                return;
            }
            i++;
            t++;
        } while (i < quest_w.x3B);
    }
}
#endif

#if !defined(G_PART) || G_PART == 11
void Ext_pick_point_st_clr(void)
{
    int i;
    s8 *t;

    i = quest_w.x3B - 1;
    if (i >= 0) {
        t = stiem_stack_tbl + i;
        do {
            if (!(StiEM_data[*t].x19 & 1)) {
                Ext_pick_point_clr(*t);
            }
            i--;
            t--;
        } while (i >= 0);
    }
}
#endif

#if !defined(G_PART) || G_PART == 11
static void ext_pick_point_tbl_set(n)
s8 n;
{
    stiem_stack_tbl[quest_w.x3B] = n;
    quest_w.x3B++;
}
#endif

#if !defined(G_PART) || G_PART == 11
s8 Em_hagi_point_set(em, n)
EMW *em;
int n;
{
    STIEM sp;
    f32 jp[3];
    u8 *h;
    u8 kind;

    kind = em->kind;
#ifdef VERSION_MHG      /* G (sub_main 0x4B7250): carve tables per quest rank (mission header 0x42) */
    h = (u8 *)((HAGI **)em_hagi_type_tbl[MIB8(quest_w.x64, 0x42)])[kind];
    if (h == 0) {
        em->x88D = -1;
    } else {
    if (kind == 3 && em->type == 1) {
        h += 0xC;
    }
#else
    h = (u8 *)em_hagi_type_tbl[kind];
    if (h == 0) {
        em->x88D = -1;
    } else {
    if (quest_w.x14E > 0) {
        h += 0xC;
    }
    if (kind == 3 && em->type == 1) {
        h += 0x18;
    }
#endif
    h += n * 0xC;
    sp.rad = *(f32 *)(h + 4);
    sp.id = *(u16 *)h;
    sp.cnt = *(u16 *)(h + 2);
    sp.stg = em->stg;
    sp.x1A = *(s16 *)(h + 8);
    sp.x19 = h[0xA];
    em->x876 = h[0xB];
    if (em->x876 != 0) {
        get_joint_pos_em(em, em->x876, jp);
        jp[1] = em->x5AC;
        em->x88D = Ext_pick_point_set(&sp, jp);
    } else {
        em->x88D = Ext_pick_point_set(&sp, em->pos);
    }
    }
    return em->x88D;
}
#endif

#if !defined(G_PART) || G_PART == 11
int Em_tail_hagi_point_set(tail)
u8 *tail;
{
    STIEM sp;
    u8 *h;

#ifdef VERSION_MHG      /* G (sub_main 0x4B7360): per rank, the tail is record 1 */
    h = (u8 *)((HAGI **)em_hagi_type_tbl[MIB8(quest_w.x64, 0x42)])[(*(EMW **)(tail + 0x34))->kind];
    if (h == 0) {
        return -1;
    }
    h += 0xC;
#else
    h = (u8 *)em_hagi_type_tbl[(*(EMW **)(tail + 0x34))->kind];
    if (h == 0) {
        return -1;
    }
    h += 0x18;
    if (quest_w.x14E > 0) {
        h += 0xC;
    }
#endif
    sp.rad = *(f32 *)(h + 4);
    sp.id = *(u16 *)h;
    sp.cnt = *(u16 *)(h + 2);
    sp.stg = tail[6];
    sp.x1A = *(s16 *)(h + 8);
    sp.x19 = h[0xA];
    return Ext_pick_point_set(&sp, (f32 *)(tail + 0x24));
}
#endif

/* Em_hagi_point_cnt_ck: original bytes kept (near-match C in src/main/quest/f_quest_nm.c, linked weak by the PC build): build/raw/Em_hagi_point_cnt_ck.inc,
   config/c_rawfuncs.txt */
#ifdef __MWERKS__
#if !defined(G_PART) || G_PART == 11
asm int Em_hagi_point_cnt_ck(EMW *em)
{
#include "Em_hagi_point_cnt_ck.inc"
}
#endif
#endif

#if !defined(G_PART) || G_PART == 11
void Em_hagi_point_clr(em)
EMW *em;
{
    Ext_pick_point_clr(em->x88D);
    em->x88D = -1;
}
#endif

#if !defined(G_PART) || G_PART == 11
void Quest_forfeit_message(void)
{
    char buf[0x20];
    int n;

    str_gattai(buf, GSTR(lit_1730_0036B290, gptr_00526324), quest_w.x18);
    n = strlen(buf);
    if (n & 1) {
        buf[n] = 0x20;
        buf[n + 1] = 0;
    }
    buf[0x1F] = 0;
    set01_set2_use_mem(buf);
}
#endif

#if !defined(G_PART) || G_PART == 11
static void str_gattai(char *dst, char *fmt, ...)
{
    va_list ap;

#ifndef __MWERKS__
    {   /* PC text layer: the format is one of the game's strings (the quest time limit), so look it up */
        extern const char *rt_text_tr(const char *p);
        fmt = (char *)rt_text_tr(fmt);
    }
#endif
    va_start(ap, fmt);
    vsprintf(dst, fmt, ap);
}
#endif

#if !defined(G_PART) || G_PART == 11
static void enemy_insurance_sub(n)
int n;
{
    QEM *e;

    if (Em_data_st_adrs_get(quest_w.x74, n, 0, quest_w.x3A) != 0) {
        e = (QEM *)Em_data_st_adrs_get(quest_w.x74, n, 1, quest_w.x3A);
        for (;;) {
            if (e->id < 0) {
                break;
            }
            if (e->x2E & 4) {
                e->x2E = 2;
                e->x08 = -1;
            }
            e++;
        }
    }
}
#endif

#if !defined(G_PART) || G_PART == 11
static void em_next_tbl_ck(n)
int n;
{
    s32 *l0;
    QEM *e;
    int any;

    l0 = Em_data_st_adrs_get(quest_w.x74, n, 0, quest_w.x3A);
    if (l0 != 0) {
        e = (QEM *)Em_data_st_adrs_get(quest_w.x74, n, 1, quest_w.x3A);
        any = 0;
        for (;;) {
            if (e->id < 0) {
                break;
            }
            if (!(e->x2E & 5)) {
                any = 1;
            }
            e++;
        }
        if (any == 0 && e->id == -2) {
            em_data_st_adrs_set(quest_w.x74, n, e + 1, 1);
            em_data_st_adrs_set(quest_w.x74, n, (int)l0 + 0x10, 0);
        }
    }
}
#endif

#if !defined(G_PART) || G_PART == 11
static void em_data_st_adrs_set(tbl, n, val, k)
s32 *tbl;
int n;
int val;
int k;
{
    s32 *p;
    s32 base;
    s32 v;
    s32 w;

    tbl += quest_w.x3A;
    base = *tbl;
    if (base != 0 && quest_w.no != 0) {
        p = (s32 *)(base + (int)mission_area);
loop:
        v = *p;
        if (v != 0) {
            if (v != n) {
                p += 4;
                goto loop;
            }
            p += k + 2;
            w = *p;
            if (w == 0 || w == -1) {
                return;
            }
            *p = val - (int)mission_area;
        }
    }
}
#endif

#if !defined(G_PART) || G_PART == 11
static void quest_em_init()
{
    int n;

    n = 0;
    quest_em_init_sub(quest_w.x78, &n);
    quest_em_init_sub2(quest_w.x74, &n);
}
#endif

#ifdef VERSION_MHG
extern u8 *quest_size_dist_tbl[];        /* size distributions (32 random slots each) */
extern u8 quest_size_dist_sel[32];       /* random number -> distribution */
int func_0012D1B0(void);        /* main: a G work byte (0x35F014) != 0 */
int sub_main_func_004F3B00();   /* 1: the quest's event flag allows random sizes */

/* G (sub_main 0x4B7810): a random size in percent, clamped to the class limits quest_size_lim
 * (mission header byte 0x44) */
#if !defined(G_PART)
static u16 sub_main_func_004B7810(void)
{
    u8 *d = quest_size_dist_tbl[quest_size_dist_sel[sub_main_func_004B5480() & 0x1F]];
    u16 v = d[sub_main_func_004B5480() & 0x1F];
    int c = MIB8(quest_w.x64, 0x44);

    if (v < quest_size_lim.b[c * 2]) {
        v = quest_size_lim.b[c * 2];
    }
    if (quest_size_lim.b[c * 2 + 1] < v) {
        v = quest_size_lim.b[c * 2 + 1];
    }
    return v;
}
#endif

/* G (sub_main 0x4B78D0): MH1's per-entry set-up of quest_em_init_sub, made a function */
#if !defined(G_PART) || G_PART == 12
static void sub_main_func_004B78D0(QEM *e, int *cnt)
{
    e->x2E = 2;
    e->x08 = -1;
    e->x0A = -1;
    e->x2C = *cnt;
    if (e->x05 != 0) {
        quest_w.x34 += e->x04;
    }
    (*cnt)++;
}
#endif

/* G (sub_main 0x4B7920): monster kinds that get a random size */
#if !defined(G_PART)
static int sub_main_func_004B7920(s16 kind)
{
    switch (kind) {
    case 0x21: case 0x1F: case 0x1C: case 0x1B: case 0x2C: case 0x1A: case 0x30: case 0x16:
    case 0x2E: case 0x15: case 0x27: case 0x14: case 0x2F: case 0x11: case 0x2D: case 0x0F:
    case 0x2B: case 0x0E: case 0x31: case 0x29: case 0x0B: case 0x08: case 0x26: case 0x06:
    case 0x25: case 0x2A: case 0x01:
        return 1;
    }
    return 0;
}
#endif

/* G (sub_main 0x4B7BA0): as MH1, plus each monster's size (QEM+0x30, percent): the mission's
 * base size (header 0x40) times a random size for the kinds above when the work byte or the
 * quest's event flag allows it; 100 otherwise */
#if !defined(G_PART)
static void quest_em_init_sub(p, cnt)
s32 *p;
int *cnt;
{
    s32 *l0;
    QEM *e;

    for (;;) {
        l0 = Em_data_com_adrs_get(p, 0);
        e = (QEM *)Em_data_com_adrs_get(p, 1);
        if (l0 == 0) {
            break;
        }
        if (e != 0) {
            for (;;) {
                if (e->id == -1) {
                    break;
                }
                sub_main_func_004B78D0(e, cnt);
                if (sub_main_func_004B7920(e->id) == 1) {
                    e->x30 = *(s16 *)((u8 *)quest_w.x64 + 0x40);
                    e->x30 = (u16)e->x30 * sub_main_func_004B7810() / 100;
                    if (func_0012D1B0() != 1 && sub_main_func_004F3B00(quest_w.no) != 1) {
                        e->x30 = 100;
                    }
                } else {
                    e->x30 = 100;
                }
                e++;
            }
        }
        p += 4;
    }
}
#endif

/* G (sub_main 0x4B7CD0): as MH1, sizes 100 */
#if !defined(G_PART)
static void quest_em_init_sub2(p, cnt)
s32 *p;
int *cnt;
{
    s32 *l0;
    QEM *e;
    s32 *q;

    for (;;) {
        q = (s32 *)*p;
        if (q == 0) {
            break;
        }
        if (quest_w.no == 0) {
            q = (s32 *)(int)q;
        } else {
            q = (s32 *)((int)q + (int)mission_area);
        }
        for (;;) {
            l0 = Em_data_com_adrs_get(q, 0);
            e = (QEM *)Em_data_com_adrs_get(q, 1);
            q += 4;
            if (l0 == 0) {
                break;
            }
            if (e != 0) {
                for (;;) {
                    if (e->id == -1) {
                        break;
                    }
                    sub_main_func_004B78D0(e, cnt);
                    e->x30 = 100;
                    e++;
                }
            }
        }
        p++;
    }
}
#endif
#else
static void quest_em_init_sub(p, cnt)
s32 *p;
int *cnt;
{
    s32 *l0;
    QEM *e;

    for (;;) {
        l0 = Em_data_com_adrs_get(p, 0);
        e = (QEM *)Em_data_com_adrs_get(p, 1);
        if (l0 == 0) {
            break;
        }
        if (e != 0) {
            for (;;) {
                if (e->id == -1) {
                    break;
                }
                e->x2E = 2;
                e->x08 = -1;
                e->x0A = -1;
                e->x2C = *cnt;
                if (e->x05 != 0) {
                    quest_w.x34 += e->x04;
                }
                e++;
                (*cnt)++;
            }
        }
        p += 4;
    }
}

static void quest_em_init_sub2(p, cnt)
s32 *p;
int *cnt;
{
    s32 *l0;
    QEM *e;
    s32 *q;

    for (;;) {
        q = (s32 *)*p;
        if (q == 0) {
            break;
        }
        if (quest_w.no == 0) {
            q = (s32 *)(int)q;
        } else {
            q = (s32 *)((int)q + (int)mission_area);
        }
        for (;;) {
            l0 = Em_data_com_adrs_get(q, 0);
            e = (QEM *)Em_data_com_adrs_get(q, 1);
            q += 4;
            if (l0 == 0) {
                break;
            }
            if (e != 0) {
                for (;;) {
                if (e->id == -1) {
                    break;
                }
                    e->x2E = 2;
                    e->x08 = -1;
                    e->x0A = -1;
                    e->x2C = *cnt;
                    if (e->x05 != 0) {
                        quest_w.x34 += e->x04;
                    }
                    e++;
                    (*cnt)++;
                }
            }
        }
        p++;
    }
}

#endif

#if !defined(G_PART)
static void station_em_set(void)
{
    QEM *e;
    EMW *em;
    s32 *l;
    u8 *h;
    int i;
    u8 *g;
    int j;
    s32 v;

    l = Em_data_com_adrs_get(quest_w.x78, 0);
    e = (QEM *)Em_data_com_adrs_get(quest_w.x78, 1);
    if (l != 0 && e != 0) {
        for (;;) {
            v = *l;
            if (v == -1) {
                break;
            }
            for (i = 0, g = (u8 *)&game_w; i < 4; i++, g++) {
                if (g[0x28] == v) {
                    break;
                }
            }
            if (i < 4) {
                l++;
                continue;
            }
            for (j = 0, h = (u8 *)&game_w; j < 4; j++, h++) {
                if (h[0x28] <= 0) {
                    ((u8 *)&game_w)[0x28 + j] = v;
                    em_create_model(j);
                    break;
                }
            }
            l++;
        }
        for (;;) {
            if (e->id < 0) {
                break;
            }
            if ((em = Em_direct_set(e)) != 0) {
#ifdef VERSION_MHG      /* G (sub_main 0x4B7DA0): the subspecies 36 of kind 2 too */
                if ((em->kind == 2 || em->kind == 0x24) && Quest_f_dra_ck(*(u8 *)&quest_w.no) != 0) {
#else
                if (em->kind == 2 && Quest_f_dra_ck(*(u8 *)&quest_w.no) != 0) {
#endif
                    quest_w.x14C = em->x302;
                }
                em->stg = e->x07;
            }
            e++;
        }
    }
}
#endif

#if !defined(G_PART) || G_PART == 13
static QEM *em_work_serch(em)
EMW *em;
{
    int i;
    s32 *l0;
    QEM *e;

    i = 0;
    if (0 > quest_w.x3A) {
    } else {
        do {
            l0 = Em_data_st_adrs_get(quest_w.x74, em->stg, 0, (s8)i);
            e = (QEM *)Em_data_st_adrs_get(quest_w.x74, em->stg, 1, (s8)i);
            if (l0 != 0) {
                for (;;) {
                    if (e->id < 0) {
                        break;
                    }
                    if (em->id == e->x0A) {
                        return e;
                    }
                    e++;
                }
            }
            i++;
        } while (quest_w.x3A >= i);
    }
    l0 = Em_data_com_adrs_get(quest_w.x78, 0);
    e = (QEM *)Em_data_com_adrs_get(quest_w.x78, 1);
    if (l0 == 0) {
        return 0;
    }
    if (e == 0) {
        return 0;
    }
    for (;;) {
        if (e->id < 0) {
            break;
        }
        if (em->id == e->x0A) {
            return e;
        }
        e++;
    }
    return 0;
}
#endif

/* em_work_serch2: K&R definition (the original sign-extends both s16 arguments itself), a for loop over the
   quest's stage lists (agent B). Its caller quest_em_die casts the arguments to s16 (the original extends them at
   the call as with an s16 prototype; MWCC accepts no prototype next to the K&R definition). */
#if !defined(G_PART)
static QEM *em_work_serch2(no, stg)
s16 no;
s16 stg;
{
    int i;
    s32 *l0;
    QEM *e;

    for (i = 0; i <= quest_w.x3A; i++) {
        l0 = Em_data_st_adrs_get(quest_w.x74, stg, 0, quest_w.x3A);
        e = (QEM *)Em_data_st_adrs_get(quest_w.x74, stg, 1, quest_w.x3A);
        if (l0 != 0) {
            for (;;) {
                if (e->id < 0) {
                    break;
                }
                if (no == e->x2C) {
                    return e;
                }
                e++;
            }
        }
    }
    l0 = Em_data_com_adrs_get(quest_w.x78, 0);
    e = (QEM *)Em_data_com_adrs_get(quest_w.x78, 1);
    if (l0 == 0) {
        return 0;
    }
    if (e == 0) {
        return 0;
    }
    for (;;) {
        if (e->id < 0) {
            break;
        }
        if (no == e->x2C) {
            return e;
        }
        e++;
    }
    return 0;
}
#endif

#if !defined(G_PART) || G_PART == 14
void Quest_timer_calc(n)
int n;
{
    if (game_w.info_stop == 0 && quest_w.x10 > 0) {
        quest_w.x10 -= n;
        if (quest_w.x10 < 0) {
            quest_w.x10 = 0;
        }
    }
}
#endif

#if !defined(G_PART) || G_PART == 14
void Quest_timer_reset(void)
{
    quest_w.x10 = Quest_time_get(1);
}
#endif

/* quest_condition_prog: original bytes kept (near-match C in src/main/quest/f_quest_nm.c, linked weak by the PC build): build/raw/quest_condition_prog.inc,
   config/c_rawfuncs.txt */
#ifdef __MWERKS__
#if !defined(G_PART)
asm s16 quest_condition_prog(PLW *pl, int i)
{
#include "quest_condition_prog.inc"
}
#endif
#endif

/* remuneration_item_set: original bytes kept (near-match C in src/main/quest/f_quest_nm.c, linked weak by the PC build): build/raw/remuneration_item_set.inc,
   config/c_rawfuncs.txt */
#ifdef __MWERKS__
#if !defined(G_PART)
asm void remuneration_item_set(void)
{
#include "remuneration_item_set.inc"
}
#endif
#endif

#if !defined(G_PART) || G_PART == 15
static u16 *quest_supplies_get(pl, p)
PLW *pl;
u16 *p;
{
    u16 a = *p;
    s16 b;

    p++;
    b = *p;
    p++;
    Pl_item_stack(pl, a, b);
    return p;
}
#endif

#if !defined(G_PART) || G_PART == 15
int quest_item_set(id, num)
s16 id;
s16 num;
{
    int i;
    u8 *q;
    int t;

    for (i = 0, q = (u8 *)&quest_w; i < 4; i++, q += 2) {
        if (*(s16 *)(q + 0x1C) == 0) {
            t = i * 2;
            *(s16 *)((u8 *)quest_w.x1C + t) = id;
            *(s16 *)((u8 *)quest_w.x24 + t) = num;
            return 0;
        }
    }
    return -1;
}
#endif

#if !defined(G_PART) || G_PART == 15
int quest_enemy_set(id, num)
s16 id;
s16 num;
{
    int i;
    u8 *q;
    int t;

    for (i = 0, q = (u8 *)&quest_w; i < 2; i++, q += 2) {
        if (*(s16 *)(q + 0x2C) == 0) {
            t = i * 2;
            *(s16 *)((u8 *)quest_w.x2C + t) = id;
            *(s16 *)((u8 *)quest_w.x144 + t) = id;
            *(s16 *)((u8 *)quest_w.x30 + t) = num;
            *(s16 *)((u8 *)quest_w.x148 + t) = num;
            return 0;
        }
    }
    return -1;
}
#endif

#if !defined(G_PART) || G_PART == 16
static int quest_item_ck(q)
QUEST_W *q;
{
    return (s16)Pl_item_num_ck(&player_work[game_w.master], (u16)q->x1C[0]) >= q->x24[0];
}
#endif

/* quest_item_ck2: original bytes kept (near-match C in src/main/quest/f_quest_nm.c, linked weak by the PC build): build/raw/quest_item_ck2.inc,
   config/c_rawfuncs.txt */
#ifdef __MWERKS__
#if !defined(G_PART)
asm int quest_item_ck2(QUEST_W *q, QEM *em)
{
#include "quest_item_ck2.inc"
}
#endif
#endif

#if !defined(G_PART) || G_PART == 17
static int quest_item_ck3(unused, p)
int unused;
s16 *p;
{
    u16 id;
    s16 num;

    p++;
    id = *p;
    p++;
    num = *p;
    return (s16)Pl_item_num_ck(&player_work[game_w.master], id, id) >= num;
}
#endif

#if !defined(G_PART) || G_PART == 17
static int quest_share_item_ck(q)
QUEST_W *q;
{
    int i;

    for (i = 0; i < 4; i++) {
        if (q->x1C[i] != 0 && quest_w.x24[i] - (s16)Share_item_num_ck((u16)q->x1C[i], share_item_ck_ck()) > 0) {
            return 0;
        }
    }
    return 1;
}
#endif

#if !defined(G_PART) || G_PART == 17
static int quest_enemy_ck(q)
QUEST_W *q;
{
    int i;

    for (i = 0; i < 2; i++) {
        if (q->x2C[i] != 0) {
            return 0;
        }
    }
    return 1;
}
#endif

#if !defined(G_PART) || G_PART == 17
static s16 quest_enemy_ck2(q, kind)
QUEST_W *q;
int kind;
{
    int i;

    for (i = 0; i < 2; i++) {
        if (q->x2C[i] == (kind & 0xFF)) {
            return q->x30[i];
        }
    }
    return 0;
}
#endif

#if !defined(G_PART) || G_PART == 17
static int quest_enemy_ck_sub(q, kind)
QUEST_W *q;
int kind;
{
    int i;
    s16 t;
    s16 v;

    for (i = 0; i < 2; i++) {
        v = q->x2C[i];
        switch (v) {
        case 0x63:
            t = q->x30[i] - 1;
            q->x30[i] = t;
            if (t <= 0) {
                q->x2C[i] = 0;
                return 1;
            }
            break;
        default:
            if (v == (kind & 0xFF)) {
                t = q->x30[i] - 1;
                q->x30[i] = t;
                if (t <= 0) {
                    q->x2C[i] = 0;
                    return 1;
                }
            }
            break;
        }
    }
    return 0;
}
#endif

#if !defined(G_PART) || G_PART == 17
static int quest_enemy_ck_sub2(q, kind)
QUEST_W *q;
int kind;
{
    int i;
    s16 v;

    for (i = 0; i < 2; i++) {
        v = q->x2C[i];
        switch (v) {
        case 0x63:
            if (q->x30[i] < 2) {
                return 1;
            }
            break;
        default:
            if (v == (kind & 0xFF) && q->x30[i] < 2) {
                return 1;
            }
            break;
        }
    }
    return 0;
}
#endif

#if !defined(G_PART) || G_PART == 17
static int em_capture_conv(kind)
int kind;
{
    u16 *p;

    p = capture_type_tbl;
    while (*p != 0xFF) {
        if ((kind & 0xFF) == (s8)*p) {
            Share_item_stack(&player_work[game_w.master], p[1], 1);
            return 1;
        }
        p += 2;
    }
    return 0;
}
#endif

#if !defined(G_PART) || G_PART == 17
static void quest_failed_ptr_set(flag)
int flag;
{
    s16 *p;
    s16 i;
    s16 k;

    k = flag == 0 ? -2 : 0x25;
    p = (s16 *)quest_w.x6C;
    i = 0;
    for (;;) {
        if (*p == k) {
            break;
        }
        p += 4;
        i++;
    }
    quest_w.x36 = i;
}
#endif

#if !defined(G_PART) || G_PART == 17
static void quest_presuccess_ptr_set(flag)
int flag;
{
    s16 *p;
    s16 i;
    s16 k;

    k = flag == 0 ? -1 : 0x26;
    p = (s16 *)quest_w.x6C;
    i = 0;
    for (;;) {
        if (*p == k) {
            break;
        }
        p += 4;
        i++;
    }
    quest_w.x36 = i;
}
#endif

#if !defined(G_PART)
static void em_herb_set(void)
{
    QEM q;

    q.id = 0xA;
    q.x02 = 0;
    q.x08 = -1;
    q.pos[0] = 0;
    q.pos[1] = 0;
    q.pos[2] = 0;
    q.x1C = 0;
    Em_direct_set(&q);
}
#endif

/* Quest_net_sub: original bytes kept (near-match C in src/main/quest/f_quest_nm.c, linked weak by the PC build): build/raw/Quest_net_sub.inc,
   config/c_rawfuncs.txt */
#ifdef __MWERKS__
#if !defined(G_PART)
asm void Quest_net_sub(void)
{
#include "Quest_net_sub.inc"
#ifndef __MWERKS__
short Pl_item_num_ck();   /* (declared before its first use) */
unsigned char Get_hunter_rank();   /* (declared before its first use) */
unsigned short Item_get_ck();   /* (declared before its first use) */
#endif
}
#endif
#endif

#if !defined(G_PART) || G_PART == 18
static void quest_em_die(void)
{
    QEM *e;
    EMW *em;
    int hp;
    int i;
    int fl;
    int no;

    em = em_work;
    hp = (u16)quest_w.x186 >> 8 & 0xFF;
    if ((e = em_work_serch2((s16)quest_w.x184, (s16)(quest_w.x186 & 0xFF))) != 0 && ((fl = e->x2E) & 6) != 0) {
        no = e->x0A;
        i = 0;
        for (;;) {
            if (em->id == no && *(u8 *)((u8 *)em + GV(0x9EB, 0x9E3)) == *(u8 *)&e->x2C) {
                if (em->kind != e->id) {
                    return;
                }
                if (em->be_flag == 0) {
                    return;
                }
                if (!(fl & 4)) {
                    if (em->x04 >= 2) {
                        return;
                    }
                    if (em->mode == 5) {
                        return;
                    }
                } else if (!(e->x04 > (u16)hp)) {
                    return;
                }
                {
                    if (em->x04 >= 2 || em->mode == 5) {
                        Em_hagi_point_clr(em, fl, no, i);
                    }
                    func_535D20(em, 5, 0);
                    *(s32 *)((u8 *)em + 0x798) = 0x3F800000;
                    e->x2E = 2;
                    em->x302 = 0;
                    if (quest_enemy_ck_sub2(&quest_w, em->kind) != 0) {
                        quest_w.x3C = em;
                    }
                }
                return;
            }
            i++;
            em++;
            if (i >= 20) {
                break;
            }
        }
        if (e->x04 > (u16)hp) {
            quest_enemy_ck_sub(&quest_w, (u8)e->id, no, i);
            if (e->x05 != 0) {
                quest_w.x34--;
            }
            e->x04--;
            if (e->x04 > 0) {
                e->x2E = 2;
                return;
            }
            e->x2E = 1;
            e->x0A = -1;
        }
    }
}
#endif

#if !defined(G_PART) || G_PART == 18
static void quest_timer_send(void)
{
    if (game_w.info_stop == 0) {
        quest_w.x182 = 0;
        quest_w.x181 = 5;
        quest_w.x186 = quest_w.x10;
        quest_w.x184 = (u32)quest_w.x10 >> 16;
        net_send_sys(6, game_w.master);
    }
}
#endif

#if !defined(G_PART) || G_PART == 18
static void q_net_send_em_die(e, em)
QEM *e;
EMW *em;
{
    quest_w.x182 = 0;
    quest_w.x181 = 7;
    quest_w.x184 = e->x2C;
    quest_w.x186 = em->stg | (u16)(e->x04 << 8);
    net_send_sys(6, game_w.master);
}
#endif

#if !defined(G_PART) || G_PART == 18
static void q_net_send_em_capture(e, em)
QEM *e;
EMW *em;
{
    quest_w.x182 = 0;
    quest_w.x181 = 8;
    quest_w.x184 = e->x2C;
    quest_w.x186 = em->stg;
    net_send_sys(6, game_w.master);
}
#endif
