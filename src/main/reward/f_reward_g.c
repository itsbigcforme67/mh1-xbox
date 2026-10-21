/* Monster Hunter G, PC build only: G's quest result module (SLPM_658.69 sub_main 0x4F3A50-0x4F7FDC), which replaces
 * MH1's f_reward.c / f_reward2.c / f_reward3.c in the G PC build (tools/build_pc.sh). Written from G's asm
 * (tools/gdraft.py drafts, read against tools/mhg_fdiff.py -g listings); field meanings are guesses.
 *
 * What G does after a quest (result_prog, 20 steps, game_w.step):
 *   0      quest end: special ends 7 / 8 (the pouch is put back to its state at departure); else the hand-in of G's five
 *          quest items (taken from the pouch, listed for sale), the hunting records, the clear bits, the reward list
 *   1      the reward list (reward_mv, f_reward4.c)            2 fade
 *   3      the money screen (gold_main / gold_disp)               4 fade
 *   5      "sale of special items": the handed-in items paid for at their price      6 fade
 *   7      online only: the hunter point screen                    8 fade
 *   9/10   "monster hunting record updated!!": a species' size record beaten (picture, old / new size, crowns)
 *   11/12  "added to the training school": a weapon class of a monster training opened by the kill count
 *   13/14  "added to the GALLERY": a monster ecology movie (by cleared quest)
 *   15/16  "added to the GALLERY": a gallery title (by event flag)
 *   17/18  online error                                             19 fade out to the village (game_w.mode 6)
 * Announcements chain: each one, when closed, looks for the next (record, training, movie, gallery title).
 * The reward list's "frozen game frame" backdrop (main 0x12EAF0, the GS's copy of the scene at the quest clear) is
 * the host's read-back of that frame (src/pc/rt/rt_2d.c rt_cap_*). */
#include "g_alias.h"
#include "reward.h"

/* PC / Xbox only (tools/build_pc.sh, VERSION=mhg); tools/build.py compiles every src file with MWCC, which gets an empty file */
#if defined(VERSION_MHG) && !defined(__MWERKS__)
int rt_cap_draw(void);          /* src/pc/rt/rt_2d.c: the quest clear's frame copy */
void rt_cap_drop(void);

/* G's result_w (sub_main 0x542490, 0x20 bytes); nothing outside this module reads it, so it is a host object */
typedef struct GRESULT_W {
    s32 gold;           /* 0x00 money / points left to pay out; the training row's monster on the training screen */
    s32 gold0;          /* 0x04 online: points before the death penalty (low-rank cut shown when lower than the quest's) */
    u8 n;               /* 0x08 sale: rows still to show; online: hunter rank now */
    u8 n0;              /* 0x09 sale: rows; online: hunter rank shown */
    u8 handin;          /* 0x0A some quest item was handed in */
    u8 rec_kind;        /* 0x0B the kind whose record screen is up (the next search starts after it) */
    PL_ITEM item[5];    /* 0x0C the handed-in items */
} GRESULT_W;
static GRESULT_W gres;

/* G tables (config/mhg/pc_data.txt) */
extern s16 result_item_ids[5];          /* sub_main 0x525F90: the five G quest items handed in at quest end */
extern s16 movie_add_tbl_g[10][3];      /* sub_main 0x525FC0: {quest, gallery flag, movie text index} */
extern s16 quest_size_flag_tbl[];       /* sub_main 0x526000: {event flag, gallery flag, quest}, -1 ends */
extern u8 train_tbl[];                  /* sub_main 0x526070: 12-byte rows {kind, training, u16 quest, u8 kills[5]}, 0xFF ends */
extern u8 hlog_list_row[0x23];          /* sub_main 0x526100: species kind -> hunting log row (0xFF none) */
extern f32 result_rank_mul[4];          /* sub_main 0x526130: online, point factor by how far below the hunter's rank */
extern u8 result_rank_lv[];             /* sub_main 0x526140: online, quest level expected per hunter rank */
extern char *movie_add_str_tbl[];       /* sub_main 0x5261A0: 6 movie names, then 15 gallery titles */
extern char *result_msg[10];            /* sub_main 0x526200: announcement texts */
extern char *result_str[31];            /* sub_main 0x526240: money / points / error screen texts */
extern char *result_str2[3];            /* sub_main 0x5262CC: sale title, low-rank cut, time left */
extern char result_sale_fmt[];          /* sub_main 0x537390: one sold item */
extern u32 result_frame[2];             /* sub_main 0x525F84: record screen frames (G card_list_frame_tbl [5] [6]) */
extern char *train_name[10];            /* sub_main 0x528A10: monster training names */
extern u8 train_job_map[8];             /* main 0x2EC5B8: training bit -> weapon class (menu_stat_job_str) */
extern char *menu_stat_job_str[];
extern char *hlog_str[12];
extern char *enemy_name[];
extern char *item_str[];
extern f32 em_base_size[52];
extern char *hunt_group_tbl[];
extern u8 my_user_handle[0x10];
extern u8 ud_pouch_backup[0x50];        /* src/main/ud/udmisc_nm.c: the master's pouch at departure (G 0x32C1C0) */

extern u8 Item_data[][0x14];
extern PLW player_work[];

int Event_flag_ck(int);
void Event_flag_set(int);
void ItemCopy_Ud2Pl();
int Pl_item_stack();
int func_0012D170(void);
void func_0012DD90(f32, int, int, int, int);
int func_00126320(int row, int x, int y, int size);     /* src/main/menu/menu_disp_nm.c: a hunting log crown */
void font_set_stack_no(int);
void font_set_palette();
void flfntSetSize();
void flfntLocate();
void font_print();
void flps0008();
void se_stop_all();
void all_reset();
int strlen();
int Hunter_point_add();
unsigned char Get_hunter_rank();
void Gold_add();
char *Quest_str_get();

#define GW (&game_w)
#define QW16(o) (*(u16 *)((u8 *)&quest_w + (o)))        /* G quest_w: x16C / x1D4 per kind (include/quest.h) */
#define UDB ((u8 *)&User_data)
#define UD16(o) (*(u16 *)(UDB + (o)))
#define UD8(o) (*(u8 *)(UDB + (o)))
#define MASTER (&player_work[GW->master])
#define TRANS (*(void **)(card_prim + 0x14))
#define CX(w) ((s16)(0x140 - (w) * 18 / 4))
#define ITEM_PRICE(id) (*(s32 *)&Item_data[id][0x10])

typedef struct PFLPS2 {
    s16 s[4];
    u32 col;
    s16 uv[4];
} PFLPS2;

static void clear_prim(void)
{
    int i;

    for (i = 0; i < 2; i++)
        memset(card_prim + i * 0x20, 0, 0x20);
}

/* the hunting log helpers (sub_main 0x4C8510-0x4C8910): kills per kind at User_data+0xD76, size record per kind at +0xD10 */
static u8 *hgrp(int kind)
{
    return (u8 *)hunt_group_tbl[kind & 0xFF];
}

static int species_record(int kind, u8 *best)           /* 0x4C87C0: the biggest record of the species' kinds */
{
    u8 *l;
    int max = 0;

    for (l = hgrp(kind); *l != 0xFF; l++) {
        if (max < UD16(0xD10 + *l * 2)) {
            max = UD16(0xD10 + *l * 2);
            if (best)
                *best = *l;
        }
    }
    return max;
}

static int species_kills(int kind)                       /* 0x4C8870 */
{
    u8 *l;
    int n = 0;

    for (l = hgrp(kind); *l != 0xFF; l++)
        n = (n + UD16(0xD76 + *l * 2)) & 0xFFFF;
    return n >= 10000 ? 9999 : n;
}

static f32 size_cm(int kind, int size)                   /* 0x4C8750: size in cm (base size x size / 100) */
{
    return (f32)(u32)size * em_base_size[kind & 0xFF] / 100.0f;
}

static f32 species_record_cm(int kind)                   /* 0x4C8640(ud, kind, 1) */
{
    u8 best = kind;
    int r = species_record(kind, &best);

    return size_cm(best, r & 0xFFFF);
}


/* main 0x1262D0: the crown a size earns in hunting log row `row` (0 none, 1 silver, 2 gold); the thresholds are the
 * row's s16 crown[2] at +4 (hlog_monster_data, 12-byte rows) */
extern u8 hlog_monster_data[];
static int crown_level(int row, int size)
{
    s16 *c = (s16 *)(hlog_monster_data + (row & 0xFFFF) * 12 + 4);
    int n = 0;

    while (n < 2 && (s16)size >= c[n])
        n++;
    return n;
}

/* sub_main 0x4F3A50 / 0x4F3B00 (rows of quest_size_flag_tbl {event flag, gallery flag, quest}); 0x4F3B00 is
 * src/pc/rt/rt_quest.c's sub_main_func_004F3B00 (the training quests' random sizes) */
int sub_main_func_004F3A50(int quest, int what)
{
    s16 *p;
    int r = -1;

    for (p = quest_size_flag_tbl; p[0] != -1; p += 3) {
        if (p[0] == quest) {
            if (what)
                return p[2];
            if (Omake_flag_ck(p[1]) == 1)
                return 1;
            r = 0;
        }
    }
    return r;
}

int key_quest_ck(s16 no)                                /* 0x4F3B70, as MH1 */
{
    s16 *p;

    for (p = key_quest_tbl; *p != -1; p++)
        if (*p == no)
            return 1;
    return 0;
}

void disp_mark(void)                                    /* 0x4F3BC0, as MH1 */
{
    SPR2TF spr;

    SetFilterMode(1);
    reload_tex(1, GSLOT(0x157, 0x180));
    SetTextureStage(GSLOT(0x157, 0x180));
    spr.size[0] = 0xB9;
    spr.size[1] = 0xB9;
    spr.pos[0] = (0x280 - spr.size[0]) / 2;
    spr.pos[1] = (0x1C0 - spr.size[1]) / 2;
    spr.uv[0] = 0x95;
    spr.uv[1] = 0x69;
    spr.uv[2] = 0xDF;
    spr.uv[3] = 0xB3;
    spr.col = 0xFF808080;
    Put_2TF(spr.pos, spr.size);
}

/* 0x4F3C80: the reward list. G sets system_w+0x32 and draws the frozen game frame (main 0x12EAF0) where MH1 drew
 * disp_mark: the copy of the quest clear's scene, at half alpha over the backdrop (rt_2d.c rt_cap_draw) */
void trans_result_0(void)
{
    font_set_stack_no(4);
    Disp_back();
    rt_cap_draw();
    disp_reward();
}

void trans_result_1(void)                               /* 0x4F3CC0: money / sale / points screens */
{
    int i;

    font_set_stack_no(4);
    Disp_back();
    disp_mark();
    flSetRenderState(0x60, 0);
    SetTrnslMode(4, 5);
    for (i = 0; i < 5; i++)
        DispFrameListA(card_list_frame_tbl_00357850[i], 0, -1, 0xFF);
    if (GW->step == 5 || GW->step == 6) {               /* the sale screen has three more steps */
        if (GW->sub >= 9)
            func_0012DD90(1.0f, 0x14, 0x32, 0x16D, 0x141);
    } else if (GW->sub >= 6) {
        func_0012DD90(1.0f, 0x14, 0x32, 0x16D, 0x141);
    }
}

void trans_result_2(void)                               /* 0x4F3DE0: online error */
{
    font_set_stack_no(4);
    Disp_back();
    flSetRenderState(0x60, 0);
    SetTrnslMode(4, 5);
    DispFrameListA(card_list_frame_tbl_00357850[0], 0, -1, 0xFF);
}

void trans_result_3(void)                               /* 0x4F3E40: announcements */
{
    font_set_stack_no(4);
    Disp_back();
    flSetRenderState(0x60, 0);
    SetTrnslMode(4, 5);
    DispFrameListA(card_list_frame_tbl_00357850[0], 0, -1, 0xFF);
    if (GW->sub != 0)
        func_0012DD90(1.0f, 0x14, 0x230, 0xF7, 0x141);
}

/* 0x4F3ED0: the record screen's frames, the monster's picture and the two crowns */
static void trans_result_record(void)
{
    PFLPS2 q;
    int k = hgrp(gres.rec_kind)[0];
    int row;
    u8 *m;

    if (k >= 0x23 || (row = hlog_list_row[k]) == 0xFF)
        return;
    m = hlog_monster_data + row * 12;
    font_set_stack_no(4);
    Disp_back();
    flSetRenderState(0x60, 0);
    SetTrnslMode(4, 5);
    DispFrameListA(result_frame[0], 0, -1, 0xFF);
    DispFrameListA(result_frame[1], 0, -1, 0xFF);
    if (GW->sub != 0)
        func_0012DD90(1.0f, 0x14, 0x118, 0x15B, 0x141);
    func_0012DD90(1.0f, 0xB, 0x176, 0xCF, 0x141);
    SetTrnslMode(4, 5);
    SetFilterMode(1);
    reload_tex(1, GSLOT(0x157, 0x180));
    SetTextureStage(GSLOT(0x157, 0x180));
    q.s[0] = 0x9C;
    q.s[1] = 0xB4;
    q.s[2] = 0x50;
    q.s[3] = 0x48;
    q.uv[0] = 8;
    q.uv[1] = 0x38;
    q.uv[2] = 0x88;
    q.uv[3] = 0xB8;
    q.col = -1;
    flps0008(&q);
    SetFilterMode(0);
    reload_tex(1, GSLOT(0x11B, 0x142));                  /* the hunting log's monster pictures */
    SetTextureStage(GSLOT(0x11B, 0x142));
    q.s[0] = 0xAA;
    q.s[1] = 0xB8;
    q.s[2] = 0x33;
    q.s[3] = 0x40;
    q.uv[0] = m[1];
    q.uv[1] = m[2];
    q.uv[2] = m[1] + 0x33;
    q.uv[3] = m[2] + 0x33;
    q.col = -1;
    flps0008(&q);
    func_00126320(row, 0x132, 0xB9, (s16)QW16(0x16C + gres.rec_kind * 2));
    func_00126320(row, 0x132, 0xE2, (s16)species_record(gres.rec_kind, 0));
}

void end_fade_set(void)                                 /* 0x4F4180 */
{
    str_fadein_vol(0, 0x1E, 0);
}

static void pouch_restore(void)                         /* 0x4F4190: the pouch back to its state at departure */
{
    memcpy(UDB + 0xC24, ud_pouch_backup, 0x50);
}

/* 0x4F41B0: a gallery title whose event flag is set and whose gallery flag is not: its index (flag - 13), else 0xFF.
 * Online only after a cleared quest; offline every quest end except a failed 10048 */
static int gallery_add_ck(int set)
{
    s16 *p;
    int r = 0xFF;

    if (Online_ck() == 1) {
        if (Quest_clear_ck(1) != 1)
            return 0xFF;
    } else if (quest_w.no == 10048 && Quest_clear_ck(1) != 1) {
        return 0xFF;
    }
    for (p = quest_size_flag_tbl; p[0] != -1; p += 3) {
        if (Event_flag_ck(p[0]) == 1 && Omake_flag_ck(p[1]) == 0) {
            if (set)
                Omake_flag_set(p[1]);
            r = p[1] - 13;
        }
    }
    return r;
}

/* 0x4F42A0: a monster ecology movie for a cleared quest (G's movie_add_ck): its text index, else 0xFF */
static int movie_add_ck_g(int no, int set)
{
    int i;

    if (Quest_clear_bit_ck(no & 0xFFFF) == 0)
        return 0xFF;
    for (i = 0; i < 10; i++) {
        if ((u16)movie_add_tbl_g[i][0] == (u16)(s16)no && Omake_flag_ck((u16)movie_add_tbl_g[i][1]) == 0) {
            if (set)
                Omake_flag_set((u16)movie_add_tbl_g[i][1]);
            return (u16)movie_add_tbl_g[i][2];
        }
    }
    return 0xFF;
}

static void announce(GAME_W *t, int step, int x03, void (*draw)(void))
{
    t->step = step;
    t->sub = 0;
    t->x03 = x03;
    t->x04 = 0x5A;
    t->x06 = 1;
    t->x08 = 0x384;
    str_play(1, 0x11);
    fade_set(2);
    clear_prim();
    TRANS = (void *)draw;
}

int movie_add_check(GAME_W *t, int go)                  /* 0x4F4360 */
{
    int r = movie_add_ck_g(quest_w.no, go);

    if (r == 0xFF) {
        r = gallery_add_ck(go);
        if (r == 0xFF) {
            if (go) {
                t->mode = 6;
                t->step = 0;
                t->x03 = 0;
                all_reset();
            }
            return 0;
        }
        if (go)
            announce(t, 15, r, trans_result_3);
        return 1;
    }
    if (go)
        announce(t, 13, r, trans_result_3);
    return 1;
}

/* 0x4F44F0: after the kills are added, each kind's size record (quest_w.x16C: the biggest seen this quest) and the
 * old records are kept for the record screen: x1D4 = the kind's old record (1 when a first record was set), x16C =
 * the species' best old record, under the species' first kind */
static void records_update(void)
{
    int k, best;
    u16 old;
    u8 *l;

    for (k = 1; k < 0x33; k++) {
        old = UD16(0xD10 + k * 2);
        QW16(0x1D4 + k * 2) = old;
        if (old < QW16(0x16C + k * 2)) {
            UD16(0xD10 + k * 2) = QW16(0x16C + k * 2);
            if (QW16(0x1D4 + k * 2) == 0)
                QW16(0x1D4 + k * 2) = 1;
        }
        QW16(0x16C + k * 2) = 0;
    }
    for (k = 1; k < 0x33; k++) {
        best = 0;
        for (l = hgrp(k); *l != 0xFF; l++)
            if (best < QW16(0x1D4 + *l * 2))
                best = QW16(0x1D4 + *l * 2);
        QW16(0x16C + hgrp(k)[0] * 2) = best;
    }
}

/* 0x4F4620: the next species (after the one shown) whose size record this quest beat: its record screen */
static int record_add_check(GAME_W *t, int go)
{
    int k;

    for (k = gres.rec_kind + 1; k < 0x33; k++) {
        if (QW16(0x16C + k * 2) != 0 && QW16(0x16C + k * 2) < (u16)species_record(k, 0)) {
            if (go) {
                if (QW16(0x16C + k * 2) == 1)          /* first record: no old size */
                    QW16(0x16C + k * 2) = 0;
                gres.rec_kind = k;
                announce(t, 9, k, trans_result_record);
            }
            return 1;
        }
    }
    return 0;
}

/* 0x4F4780: a training course opened: per row (a monster's training, after its quest is cleared) five weapon
 * classes, each opened by a kill count of the row's kind; the opened bits are at User_data+0xDDC + training */
static int train_add_check(GAME_W *t, int go)
{
    u8 *r;
    int i;

    for (r = train_tbl; r[0] != 0xFF; r += 12) {
        if (Quest_clear_bit_ck(*(u16 *)(r + 2)) != 1)
            continue;
        for (i = 0; i < 5; i++) {
            if (species_kills(r[0]) >= r[4 + i] && !(UD8(0xDDC + r[1]) & (1 << i))) {
                if (go) {
                    gres.gold = r[1];
                    UD8(0xDDC + r[1]) |= 1 << i;    /* sub_main 0x4C93E0 */
                    t->step = 11;
                    t->sub = 0;
                    t->x03 = i;
                    t->x04 = 0x5A;
                    t->x06 = 1;
                    t->x08 = 0x384;
                    str_play(1, 0x11);
                    fade_set(2);
                    clear_prim();
                    TRANS = (void *)trans_result_3;
                }
                return 1;
            }
        }
    }
    return 0;
}

/* 0x4F4900: G's quest items (result_item_ids) leave the pouch at quest end and are listed for sale */
static int handin_items(void)
{
    PLW *pl = MASTER;
    int i, j, n = 0;

    gres.handin = 0;
    memset(gres.item, 0, sizeof(gres.item));
    for (i = 0; i < 5; i++) {
        for (j = 0; j < 20; j++) {
            if (pl->item[j].id == (u16)result_item_ids[i]) {
                gres.item[n].id = pl->item[j].id;
                gres.item[n].num = pl->item[j].num;
                Pl_item_stack(pl, pl->item[j].id, (s16)-pl->item[j].num);
                gres.handin = 1;
                n++;
            }
        }
    }
    return n;
}

static void kills_add(int kind, int n)                  /* sub_main 0x4C8510 */
{
    UD16(0xD76 + kind * 2) += n;
    if (UD16(0xD76 + kind * 2) >= 10000)
        UD16(0xD76 + kind * 2) = 9999;
}

static int step_wait(GAME_W *t)                         /* the blinking prompt and the 30 s timeout of an announcement */
{
    if (t->x04 > 0) {
        t->x04--;
    } else if (--t->x06 <= 0) {
        t->x06 = 0xF;
        t->sub ^= 1;
    }
    if (--t->x08 <= 0 || (t->x04 <= 0 && (Psw[2] & 0x20))) {
        t->step++;
        if (Psw[2] & 0x20)
            se_req(7, 0x13, 0);
        fade_set(1);
        t->x04 = 0x1E;
        return 1;
    }
    return 0;
}

static void sale_init(GAME_W *t);
static int sale_main(GAME_W *t);
static void sale_disp(GAME_W *t);
static void points_disp(GAME_W *t);
static void record_disp(GAME_W *t);
static void train_disp(GAME_W *t);
static void gallery_disp(GAME_W *t);

static void to_village(GAME_W *t)
{
    t->mode = 6;
    t->step = 0;
    all_reset();
    rt_cap_drop();                  /* the quest clear's frame copy (G's all_reset clears system_w+0x33 / its use) */
}

void result_prog(void)                                  /* 0x4F4A60 */
{
    GAME_W *t = GW;
    PLW *pl;
    int i, r;

    font_set_stack_no(4);
    switch (t->step) {
    case 0:
        se_stop_all();                                  /* (G calls the twin of MH1's se_stat, which is G's flSndAllStop) */
        t->x0A = 0;
        stage_work.x01 = 0;
        gres.rec_kind = 0;
        clear_prim();
        if (func_0012D170() == 1)
            ItemCopy_Ud2Pl(MASTER);
        if (t->x0D5 == 8) {
            t->step = 17;
            t->sub = 0;
            t->x04 = 0x96;
            t->x06 = 1;
            Quest_price_return();
            fade_set(2);
            TRANS = (void *)trans_result_2;
            pouch_restore();
            break;
        }
        if (t->x0D5 == 7) {
            Quest_price_return();
            pouch_restore();
            r = gallery_add_ck(1);
            if (r == 0xFF) {
                t->step = 19;
                t->x04 = 0x1E;
                fade_set(10);
                end_fade_set();
            } else {
                announce(t, 15, r, trans_result_3);
            }
            return;
        }
        /* (G 0x12AE80 also stores a new random number of its channel-2 generator at User_data+0xD0A here and at
         * title time; no reader found; the PC's Rnd_w has two channels: not ported) */
        quest_price = 0;
        TRANS = (void *)trans_result_0;
        for (i = 0; i < 20; i++) {
            pl = MASTER;
            if (Item_data[pl->item[i].id][4] & 0x10)
                Pl_item_stack(pl, pl->item[i].id, (s16)-pl->item[i].num);
        }
        handin_items();
        for (i = 0; i < 0x33; i++) {
            if (QW16(0x1D4 + i * 2) == 0)
                continue;
            if (quest_w.no == 3003 && Event_flag_ck(0x2B) && t->x21A != 0 && (i == 7 || i == 0x32))
                Event_flag_set(0x1F);
            kills_add(i, QW16(0x1D4 + i * 2));
        }
        records_update();
        if (Quest_clear_ck(1) == 1) {
            if (Online_ck() == 1) {
                if (Quest_f_dra_ck((u8)quest_w.no) == 0 && (key_quest_ck(quest_w.no) != 1 || t->x21A != 0))
                    Quest_clear_bit_set((u16)quest_w.no);
            } else {
                Quest_clear_bit_set((u16)quest_w.no);
            }
            if (Quest_f_dra_ck((u8)quest_w.no) != 0) {
                if (quest_w.x34 == 0) {
                    quest_w.x14C = 0;
                    if (t->x21A != 0) {
                        Quest_clear_bit_set(3004);
                        Quest_clear_bit_set(3005);
                        Quest_clear_bit_set(3006);
                        Quest_clear_bit_set(3007);
                    }
                }
                if (t->x21A != 0)
                    USER_x1A = quest_w.x14C;
            }
            remuneration_item_set();
            t->step++;
            t->sub = 0;
            t->x04 = 0x5A;
            reward_init();
            fade_set(2);
        } else if (func_0012D170() == 1) {
            t->step = 19;
            t->x04 = 0x1E;
            fade_set(10);
            end_fade_set();
        } else {
            t->step = 3;
            ItemCopy_Pl2Ud(MASTER);
            gold_init(t);
        }
        break;
    case 1:
        if ((s16)reward_mv() <= 0) {
            t->step++;
            fade_set(1);
            t->x04 = 0x1E;
            ItemCopy_Pl2Ud(MASTER);
        }
        break;
    case 2:
        if (--t->x04 <= 0) {
            t->step++;
            gold_init(t);
        }
        break;
    case 3:
        if (gold_main(t)) {
            t->step++;
            fade_set(1);
            t->x04 = 0x1E;
            if (Online_ck() != 1 && !train_add_check(t, 0) && !record_add_check(t, 0) && !movie_add_check(t, 0)
                && gres.handin == 0)
                end_fade_set();
        }
        gold_disp(t);
        break;
    case 4:
        if (--t->x04 <= 0) {
            if (gres.handin != 0 && func_0012D170() == 0) {
                t->step++;
                sale_init(t);
                break;
            }
            t->step = 7;
            if (result_init(t))
                return;
            break;
        }
        gold_disp(t);
        break;
    case 5:
        if (sale_main(t)) {
            ItemCopy_Pl2Ud(MASTER);
            t->step++;
            fade_set(1);
            t->x04 = 0x1E;
            if (Online_ck() != 1 && !train_add_check(t, 0) && !record_add_check(t, 0) && !movie_add_check(t, 0))
                end_fade_set();
        }
        sale_disp(t);
        break;
    case 6:
        if (--t->x04 <= 0) {
            t->step = 7;
            if (result_init(t))
                return;
            break;
        }
        sale_disp(t);
        break;
    case 7:
        if (result_main(t)) {
            t->step++;
            fade_set(1);
            t->x04 = 0x1E;
            if (!train_add_check(t, 0) && !record_add_check(t, 0) && !movie_add_check(t, 0))
                end_fade_set();
        }
        points_disp(t);
        break;
    case 8:
        if (--t->x04 <= 0) {
            if (!record_add_check(t, 1) && !train_add_check(t, 1) && !movie_add_check(t, 1))
                return;
            break;
        }
        points_disp(t);
        break;
    case 9:
        if (step_wait(t) && !train_add_check(t, 0) && !record_add_check(t, 0) && !movie_add_check(t, 0))
            end_fade_set();
        record_disp(t);
        break;
    case 10:
        if (--t->x04 <= 0) {
            if (!record_add_check(t, 1) && !train_add_check(t, 1) && !movie_add_check(t, 1)) {
                to_village(t);
                return;
            }
            break;
        }
        record_disp(t);
        break;
    case 11:
        if (step_wait(t) && !train_add_check(t, 0) && !movie_add_check(t, 0))
            end_fade_set();
        train_disp(t);
        break;
    case 12:
        if (--t->x04 <= 0) {
            if (!train_add_check(t, 1) && !movie_add_check(t, 1)) {
                to_village(t);
                return;
            }
            break;
        }
        train_disp(t);
        break;
    case 13:
        if (step_wait(t) && gallery_add_ck(0) == 0xFF)
            end_fade_set();
        add_disp(t);
        break;
    case 14:
        if (--t->x04 <= 0) {
            r = gallery_add_ck(1);
            if (r == 0xFF)
                to_village(t);
            else
                announce(t, 15, r, trans_result_3);
            return;
        }
        add_disp(t);
        break;
    case 15:
        if (step_wait(t))
            end_fade_set();
        gallery_disp(t);
        break;
    case 16:
        if (--t->x04 <= 0) {
            to_village(t);
            return;
        }
        gallery_disp(t);
        break;
    case 17:
        if (--t->x04 <= 0) {
            t->step++;
            fade_set(1);
        }
        error_disp();
        break;
    case 18:
        if (--t->x04 <= 0) {
            to_village(t);
            return;
        }
        error_disp();
        break;
    case 19:
        if (--t->x04 <= 0) {
            to_village(t);
            return;
        }
        break;
    }
    if (TRANS != 0)
        add_prim2(ot8, card_prim, 0, 1);
}

/* ---- the money screen (0x4F5980 / 0x4F5AA0 / 0x4F5D40) ---- */
int gold_init(GAME_W *t)
{
    t->sub = 0;
    t->x03 = 0;
    t->x08 = 0xE10;
    gres.gold = 0;
    t->x06 = 0x14;
    clear_prim();
    if (Quest_clear_ck(1) == 1) {
        gres.gold = quest_w.x14;
        if (Online_ck() == 1) {
            if (t->x21A != 0)
                gres.gold += quest_w.x94[1] * 2;
        } else {
            gres.gold += quest_w.x94[1];
        }
    }
    TRANS = (void *)trans_result_1;
    fade_set(2);
    return 0;
}

/* the count-up of gold_main / sale_main / result_main: start sounds, then the payout (circle: faster) */
static int pay_step(GAME_W *t, int (*pay)(int))
{
    int amt;

    if (gres.gold == 0)
        return 0;
    amt = 1;
    if (Psw[0] & 0x20)
        amt += 0x4D8;
    if (gres.gold > 0) {
        if (gres.gold < amt)
            amt = gres.gold;
    } else {
        amt = -amt;
        if (amt < gres.gold)
            amt = gres.gold;
    }
    se_req(7, 8, 0);
    pay(amt);
    gres.gold -= amt;
    return 1;
}

static int gold_pay(int amt)
{
    Gold_add(amt);
    return 0;
}

int gold_main(GAME_W *t)
{
    static const u8 se[5] = { 0x10, 0x11, 0x11, 0x13, 0 };

    if (t->x08 > 0)
        t->x08--;
    switch (t->sub) {
    case 0: case 1: case 2: case 3: case 4:
        if (--t->x06 < 0) {
            t->sub++;
            t->x06 = 0x14;
            if (se[t->sub - 1])
                se_req(7, se[t->sub - 1], 0);
        }
        break;
    case 5:
        if (!pay_step(t, gold_pay)) {
            t->sub++;
            se_req(7, 0x19, 0);
        }
        break;
    case 6:
        if (t->x08 <= 0 || ((Psw[2] & 0x20) && t->x08 < 0xDB7)) {
            se_req(7, 0x13, 0);
            return 1;
        }
        break;
    }
    return 0;
}

static void print_c(int y, char *s)                     /* centred by the proportional width */
{
    flfntLocate(CX(strlen_sp(s)), y);
    font_print_sp("%s", s);
}

static void print_c2(int y, char *s)                    /* centred by the byte count (G's strlen calls) */
{
    flfntLocate(CX(strlen(s)), y);
    font_print("%s", s);
}

/* the lines every money / points screen ends with: time left, quest score (the hunter's name), money or points */
static void screen_footer(GAME_W *t, char *time_fmt, char *score_fmt, char *total_fmt, s32 total)
{
    char buf[0x70];

    flfntLocate(0x17F, 0x16F);
    font_print_sp(time_fmt, t->x08 / 30);
    sprintf(buf, score_fmt, Online_ck() == 1 ? (char *)my_user_handle : (char *)UDB + 8);
    print_c2(0x46, buf);
    sprintf(buf, total_fmt, total);
    print_c(0x127, buf);
}

void gold_disp(GAME_W *t)
{
    char buf[0x70];
    int v;
    char *s;

    font_set_palette(0);
    flfntSetSize(0x12, 0x12);
    switch (t->sub) {
    default:
        flfntLocate(0x4A, 0x16F);
        font_print_sp(result_str[0]);                   /* end the payout */
    case 4:
    case 5:
        sprintf(buf, result_str[1], gres.gold);
        print_c(0x103, buf);
    case 3:
        v = 0;
        if (Quest_clear_ck(1) == 1) {
            if (Online_ck() == 1) {
                if (t->x21A != 0)
                    v = quest_w.x94[1];
            } else {
                v = quest_w.x94[1];
            }
        }
        flfntLocate(0x101, 0xCD);
        if (Quest_clear_ck(1) == 1 && Online_ck() == 1 && t->x21A != 0)
            font_print_sp(result_str[2], v);            /* contract fee x2 */
        else
            font_print_sp(result_str[3], v);
    case 2:
        v = Quest_clear_ck(1) == 1 ? quest_w.x14 : 0;
        flfntLocate(0x101, 0x97);
        font_print_sp(result_str[4], v);
    case 1:
        sprintf(buf, Quest_clear_ck(1) == 1 ? result_str[5] : result_str[6]);
        print_c2(0x61, buf);
        s = Quest_str_get(0);
        print_c(0x73, s);
        break;
    case 0:
        break;
    }
    screen_footer(t, result_str[7], result_str[8], result_str[9], *(s32 *)(UDB + 0x20));
}

/* ---- the sale of the handed-in items (0x4F6140 / 0x4F6260 / 0x4F64F0) ---- */
static void sale_init(GAME_W *t)
{
    int i;

    t->sub = 0;
    t->x03 = 0;
    t->x08 = 0xE10;
    gres.gold = 0;
    gres.n = 0;
    t->x06 = 0x14;
    clear_prim();
    for (i = 0; i < 5; i++) {
        if (gres.item[i].id != 0) {
            gres.n++;
            gres.gold += gres.item[i].num * ITEM_PRICE(gres.item[i].id);
        }
    }
    TRANS = (void *)trans_result_1;
    gres.n0 = gres.n;
    fade_set(2);
}

static int sale_main(GAME_W *t)
{
    if (t->x08 > 0)
        t->x08--;
    switch (t->sub) {
    case 0:
    case 1:
        if (--t->x06 < 0) {
            t->sub++;
            t->x06 = 0x14;
            se_req(7, t->sub == 1 ? 0x10 : 0x11, 0);
        }
        break;
    case 2: case 3: case 4: case 5: case 6:             /* one row per step */
        if (--t->x06 < 0) {
            if (--gres.n == 0) {
                t->sub = 7;
                se_req(7, 0x13, 0);
            } else {
                t->sub++;
                se_req(7, 0x11, 0);
            }
            t->x06 = 0x14;
        }
        break;
    case 7:
        if (--t->x06 < 0) {
            t->sub++;
            t->x06 = 0x14;
        }
        break;
    case 8:
        if (!pay_step(t, gold_pay)) {
            t->sub++;
            se_req(7, 0x19, 0);
        }
        break;
    case 9:
        if (t->x08 <= 0 || ((Psw[2] & 0x20) && t->x08 < 0xDB7)) {
            se_req(7, 0x13, 0);
            return 1;
        }
        break;
    }
    return 0;
}

static void sale_disp(GAME_W *t)
{
    char buf[0x70];
    int i, n, y;

    font_set_palette(0);
    flfntSetSize(0x12, 0x12);
    switch (t->sub) {
    default:
        flfntLocate(0x4A, 0x16F);
        font_print_sp(result_str[0]);
    case 7:
    case 8:
        sprintf(buf, result_str[1], gres.gold);
        print_c(0x103, buf);
    case 2: case 3: case 4: case 5: case 6:
        n = t->sub - 1;
        if (n >= 6)
            n = 5;
        for (i = 0, y = 0x85; i < n; i++, y += 0x18) {
            if (i < gres.n0) {
                sprintf(buf, result_sale_fmt, item_str[gres.item[i].id], gres.item[i].num,
                        gres.item[i].num * ITEM_PRICE(gres.item[i].id));
                print_c(y, buf);
            }
        }
    case 1:
        sprintf(buf, result_str2[0]);                   /* sale of special items */
        print_c2(0x6A, buf);
        break;
    case 0:
        break;
    }
    screen_footer(t, result_str[7], result_str[8], result_str[9], *(s32 *)(UDB + 0x20));
}

/* ---- online: the hunter point screen (0x4F6880 / 0x4F6AC0 / 0x4F6DF0); offline result_init only looks for the
 * announcements ---- */
int result_init(GAME_W *t)
{
    int d;
    u8 lv;

    if (Online_ck() != 1) {
        if (record_add_check(t, 1) || train_add_check(t, 1))
            return 1;
        return movie_add_check(t, 1) != 0;
    }
    t->sub = 0;
    t->x03 = 0;
    t->x08 = 0xE10;
    gres.gold = 0;
    t->x06 = 0x14;
    gres.n0 = gres.n = Get_hunter_rank(&User_data, 1);
    clear_prim();
    gres.gold = Quest_clear_ck(1) == 1 ? quest_w.x64[0xE] : quest_w.x64[0xF];
    if ((u16)quest_w.no < 50000) {                      /* a quest below the hunter's rank pays less */
        lv = result_rank_lv[gres.n];
        if (((u8 *)quest_w.x94)[2] < lv) {
            d = lv - ((u8 *)quest_w.x94)[2];
            if (d >= 4)
                d = 3;
            gres.gold = (s32)((f32)gres.gold * result_rank_mul[d]);
        }
    }
    gres.gold0 = gres.gold;
    if (MASTER->work91E > 0)
        gres.gold -= MASTER->work91E * 10;
    TRANS = (void *)trans_result_1;
    fade_set(2);
    return 0;
}

static int point_pay(int amt)
{
    return Hunter_point_add(amt);
}

int result_main(GAME_W *t)
{
    static const u8 se[5] = { 0x10, 0x11, 0x11, 0x13, 0 };
    int amt, r;

    if (t->x08 > 0)
        t->x08--;
    switch (t->sub) {
    case 0: case 1: case 2: case 3: case 4:
        if (--t->x06 < 0) {
            t->sub++;
            t->x06 = 0x14;
            if (se[t->sub - 1])
                se_req(7, se[t->sub - 1], 0);
        }
        break;
    case 5:
        if (gres.gold != 0) {
            amt = 1;
            if (Psw[0] & 0x20)
                amt += 0x4D8;
            if (gres.gold > 0) {
                if (gres.gold < amt)
                    amt = gres.gold;
            } else {
                amt = -amt;
                if (amt < gres.gold)
                    amt = gres.gold;
            }
            se_req(7, 8, 0);
            r = point_pay(amt);
            if (t->x03 == 0)
                t->x03 = r;
            gres.n = Get_hunter_rank(&User_data);
            if (gres.n != gres.n0) {
                if (gres.n0 < gres.n)
                    str_play(1, 0x11);
                else
                    se_req(7, 0xD, 0);
                gres.n0 = gres.n;
            }
            gres.gold -= amt;
        } else {
            t->sub++;
            se_req(7, 0x19, 0);
        }
        break;
    case 6:
        if (t->x08 <= 0 || ((Psw[2] & 0x20) && t->x08 < 0xDB7)) {
            se_req(7, 0x13, 0);
            return 1;
        }
        break;
    }
    return 0;
}

static void points_disp(GAME_W *t)
{
    char buf[0x70];
    int v, n;
    s32 q;

    font_set_palette(0);
    flfntSetSize(0x12, 0x12);
    switch (t->sub) {
    default:
        flfntLocate(0x4A, 0x16F);
        font_print_sp(result_str[10]);
    case 4:
    case 5:
        sprintf(buf, result_str[11], gres.gold);
        print_c(0x103, buf);
    case 3:
        n = MASTER->work91E;
        v = n > 0 ? n * -10 : 0;
        flfntLocate(0x7A, 0xCD);
        font_print_sp(result_str[12], v);               /* deducted points */
        if (MASTER->work91E > 0) {
            flfntLocate(0x176, 0xCD);
            font_print_sp(result_str[13], MASTER->work91E);    /* <deaths x%d> */
        }
    case 2:
        flfntLocate(0x176, 0x97);
        q = Quest_clear_ck(1) == 1 ? quest_w.x64[0xE] : quest_w.x64[0xF];
        font_print_sp(Quest_clear_ck(1) == 1 ? result_str[15] : result_str[14]);
        if (gres.gold0 < q) {                           /* the low-rank cut */
            font_set_palette(2);
            sprintf(buf, result_str2[1]);
            print_c2(0xA9, buf);
            font_set_palette(0);
        }
        flfntLocate(0x7A, 0x97);
        font_print_sp(result_str[16], q);
    case 1:
        sprintf(buf, Quest_clear_ck(1) == 1 ? result_str[17] : result_str[18]);
        print_c2(0x61, buf);
        print_c(0x73, Quest_str_get(0));
        break;
    case 0:
        break;
    }
    flfntLocate(0x17F, 0x16F);
    font_print_sp(result_str[19], t->x08 / 30);
    sprintf(buf, result_str[20], Online_ck() == 1 ? (char *)my_user_handle : (char *)UDB + 8);
    print_c2(0x46, buf);
    sprintf(buf, result_str[21], *(s32 *)(UDB + 0x1C));
    print_c(0x127, buf);
    sprintf(buf, result_str[22], gres.n0, hunter_appellation[gres.n0]);
    print_c(0x14B, buf);
    switch (t->x03) {
    case 0:
        return;
    case 1:
        font_set_palette(5);
        sprintf(buf, result_str[23]);
        break;
    case 2:
        font_set_palette(5);
        sprintf(buf, result_str[24]);
        break;
    default:
        font_set_palette(5);
        sprintf(buf, result_str[25]);
        break;
    }
    print_c(0x139, buf);
}

/* ---- the announcements' texts ---- */
void error_disp(void)                                   /* 0x4F7530 */
{
    font_set_palette(0);
    flfntSetSize(0x12, 0x12);
    flfntLocate(0x113, 0x61);
    font_print_sp(result_str[26]);
    flfntLocate(0xB0, 0x85);
    font_print_sp(result_str[27]);
    flfntLocate(0xB0, 0x97);
    font_print_sp(result_str[28]);
    flfntLocate(0xB0, 0xA9);
    font_print_sp(result_str[29]);
    flfntLocate(0xB0, 0xBB);
    font_print_sp(result_str[30]);
}

/* "update / added to the GALLERY / <name> / was added", time left (add_disp 0x4F75D0, 0x4F77D0) */
static void gallery_text(GAME_W *t, char *name)
{
    char buf[0x70];

    font_set_palette(0);
    flfntSetSize(0x12, 0x12);
    sprintf(buf, result_msg[0]);
    print_c(0x61, buf);
    sprintf(buf, result_msg[1]);
    print_c(0x85, buf);
    sprintf(buf, name);
    print_c(0x97, buf);
    sprintf(buf, result_msg[2]);
    print_c(0xA9, buf);
    flfntLocate(0x3B, 0x103);
    font_print_sp(result_str2[2], t->x08 / 30);
}

void add_disp(GAME_W *t)
{
    gallery_text(t, movie_add_str_tbl[t->x03]);
}

static void gallery_disp(GAME_W *t)
{
    gallery_text(t, movie_add_str_tbl[6 + t->x03]);
}

static void record_disp(GAME_W *t)                      /* 0x4F79D0 */
{
    char buf[0x70];
    int k = hgrp(gres.rec_kind)[0];
    int row, old;
    f32 cm;

    if (k >= 0x23 || (row = hlog_list_row[k]) == 0xFF)
        return;
    font_set_palette(0);
    flfntSetSize(0x12, 0x12);
    sprintf(buf, result_msg[6]);                        /* monster hunting record updated!! */
    print_c(0x61, buf);
    sprintf(buf, hlog_str[2], enemy_name[k]);
    print_c(0x85, buf);
    old = (s16)QW16(0x16C + gres.rec_kind * 2);
    cm = size_cm(gres.rec_kind, old);
    switch (crown_level(row, old)) {
    case 1: font_set_palette(0xB); break;
    case 2: font_set_palette(7); break;
    default: font_set_palette(0); break;
    }
    if (cm == 0.0f)
        sprintf(buf, result_msg[9]);                    /* length: - - - */
    else
        sprintf(buf, result_msg[7], (double)cm);
    flfntLocate(0x152, 0xB9);
    font_print_sp("%s", buf);
    switch (crown_level(row, (s16)species_record(gres.rec_kind, 0))) {
    case 1: font_set_palette(0xB); break;
    case 2: font_set_palette(7); break;
    default: font_set_palette(0); break;
    }
    sprintf(buf, result_msg[7], (double)species_record_cm(gres.rec_kind));
    flfntLocate(0x152, 0xE6);
    font_print_sp("%s", buf);
    font_set_palette(0);
    if (t->x04 <= 0 || t->step == 10) {
        sprintf(buf, result_msg[8]);                    /* end */
        flfntLocate(0x140, 0x15D);
        font_print_sp("%s", buf);
    }
    flfntLocate(0x4D, 0x139);
    font_print_sp(result_str2[2], t->x08 / 30);
}

static void train_disp(GAME_W *t)                       /* 0x4F7DB0 */
{
    char buf[0x70];

    font_set_palette(0);
    flfntSetSize(0x12, 0x12);
    sprintf(buf, result_msg[0]);
    print_c(0x61, buf);
    sprintf(buf, result_msg[4]);                        /* to the training school */
    print_c(0x97, buf);
    sprintf(buf, result_msg[5], menu_stat_job_str[train_job_map[t->x03]], train_name[gres.gold]);
    print_c(0xA9, buf);
    sprintf(buf, result_msg[2]);
    print_c(0xBB, buf);
    flfntLocate(0x3B, 0x103);
    font_print_sp(result_str2[2], t->x08 / 30);
}

void reward_init(void)                                  /* 0x4F7FE0, as MH1 */
{
    *(s32 *)&reward_w = 0;
    reward_w.x4 = 0;
    reward_w.xB = -1;
    reward_w.xC = 0xFF;
    reward_w.x6 = 0xE10;
    PitMenu.x11 = 6;
    PitMenu.x10 = 0;
    reward_key_repeat(0, 0);
}

#endif
