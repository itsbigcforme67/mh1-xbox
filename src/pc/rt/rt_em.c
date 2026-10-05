/*
 * rt_em.c - the monster side of the port runtime: what the monster loop
 * (enemy_mv, src/main/em/f_em_nm.c) and game.bin's monster code call that is
 * not decompiled yet. Weak stand-ins are replaced by the real C when it is
 * linked in (per-monster AI files, the command interpreter em_cmd).
 */
#include "rt.h"
#include "types.h"
#include "em.h"
#include "game.h"
#include "quest.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WEAK __attribute__((weak))
static void once(const char *n) { if (getenv("RT_TRACE")) fprintf(stderr, "rt_em: %s not ported (stand-in)\n", n); }
#define WSTUB(name) WEAK void name(EMW *em) { static int o; (void)em; if (!o++) once(#name); }

#define PU8(p, o) (*(u8 *)((u8 *)(p) + (o)))
#define PS8(p, o) (*(s8 *)((u8 *)(p) + (o)))
#define PS16(p, o) (*(s16 *)((u8 *)(p) + (o)))
#define PS32(p, o) (*(s32 *)((u8 *)(p) + (o)))
#define PF(p, o) (*(f32 *)((u8 *)(p) + (o)))

extern s16 *em_dur_tbl[];
int ran_suu(int);
int act_ck(void *, int, int);
void rt_motion_attach(void *w);
void *Stage_data_get(int);

/* ------------------------------------------------ monster AI not linked yet
 * Each emNN file (game.bin, agent B and others) defines these; until it is
 * built for the PC the monster stands still (its init/main do nothing). */
#define EM_SET(nn) \
    WSTUB(em##nn##_init) WSTUB(em##nn##_main) \
    WEAK void em##nn##_act_set(EMW *em, int a, int b, int c) { (void)em; (void)a; (void)b; (void)c; } \
    WSTUB(em##nn##_local_area_move_init)
EM_SET(01) EM_SET(02) EM_SET(03) EM_SET(04) EM_SET(07) EM_SET(08) EM_SET(09) EM_SET(10)
EM_SET(12) EM_SET(14) EM_SET(15) EM_SET(16) EM_SET(17) EM_SET(18) EM_SET(19) EM_SET(20)
EM_SET(21) EM_SET(27) EM_SET(29) EM_SET(33)

/* ------------------------------------------------ main f_em / f_pl helpers */
/* em_dur_init (main 0x10A070): the breakable parts' durability from
 * em_dur_tbl[kind] (9 s16 values, then the part count byte at +0x953):
 * per part 8 bytes at +0x304 {on, index, 0, 0, s16 durability, 0}. */
void em_dur_init(EMW *em)
{
    s16 *t = em_dur_tbl[em->kind];
    u8 *e = (u8 *)em;
    int i;
    for (i = 0; i < 9; i++, t++, e += 8) {
        if (*t > 0) {
            e[0x304] = 1;
            e[0x305] = (u8)i;
            e[0x306] = 0;
            e[0x307] = 0;
            *(s16 *)(e + 0x308) = *t;
            e[0x30A] = 0;
        } else {
            e[0x304] = 0;
        }
    }
    PU8(em, 0x953) = (u8)*t;
}



/* hit_line2_pk (0x28CC40): packs a line p0..p1: +0 p0, +0xC p1, +0x18
 * p1 - p0, +0x24 centre, +0x30 half length. */
f32 flvecCalcDistance(f32 *, f32 *);
void flvecCopy(f32 *, f32 *);
void hit_line2_pk(f32 *p0, f32 *p1, f32 *pk)
{
    flvecCopy(pk, p0);
    flvecCopy(pk + 3, p1);
    pk[6] = p1[0] - p0[0];
    pk[7] = p1[1] - p0[1];
    pk[8] = p1[2] - p0[2];
    pk[9] = 0.5f * (p0[0] + p1[0]);
    pk[10] = 0.5f * (p0[1] + p1[1]);
    pk[11] = 0.5f * (p0[2] + p1[2]);
    pk[12] = flvecCalcDistance(pk, pk + 9);
}

/* act_set (0x14EB00): start action (mode a, sub-mode b) of a player or
 * monster. Monsters (+0x10 != 0) skip the player-only parts (old action,
 * random +0x39A, timers, network). */
void action_timer_calc(void *, int);
int Pl_master_ck(void *);
void pl_to_normal(void *, int, int, int);
WEAK void act_set(void *w, int a, int b)
{
    u8 *p = w;
    if (p[0x10] == 0) {
        p[0x16] = p[0x14];
        p[0x17] = p[0x15];
    }
    p[0x14] = (u8)a;
    p[0x15] = (u8)b;
    p[5] = p[6] = p[7] = 0;
    if (p[0x10] == 0)
        PS16(p, 0x39A) = (s16)ran_suu(1);
    if (p[0x10] != 0 || !((s16)act_ck(p, 1, 8) || (s16)act_ck(p, 1, 2) || (s16)act_ck(p, 1, 10))) {
        int i;
        for (i = 0; i < 4; i++) {
            p[0x720 + i] = 0;
            PS16(p, 0x724 + 2 * i) = 0;
            PS16(p, 0x72C + 2 * i) = 0;
        }
    }
    if (p[0x10] == 0 && p[0x14] == 2) {
        p[0x615] = 0;
        action_timer_calc(p, 0);
    }
    PS32(p, 0x390) &= 0x100100;
    PS32(p, 0x394) = 0;
    if (p[0x10] == 0 && p[0x1E] == 0 && Pl_master_ck(p) != 1 && (s16)act_ck(p, 0, 0))
        pl_to_normal(p, 0, 4, 0);
}

/* GetEmMaterialData (0x16AB30): only kinds 9 and 23 take the ground's
 * diffuse colour (GetPlayerDiffuseData); the host lights monsters itself. */
void GetEmMaterialData(EMW *em) { (void)em; }

/* enemy_trans: the monster's draw prim; the host draws the monster model
 * itself (viewer.c), so the prim does nothing here. */
void enemy_trans(void *prim) { (void)prim; }

/* em_work_set (0x16A010): takes a model work for the monster and copies the
 * model data game_w+0x88[mdl_no]; the host has its own model and only needs
 * the motion model work (frame.h FRMDL) for frame_init/frame_move. */
void em_work_set(EMW *em)
{
    if (!em->mdl)
        rt_motion_attach(em);
}

/* push_em_work (0x169EB0): frees the draw prim and model work, clears the
 * first 0x12 bytes (be_flag ... ) */
void release_prim(s16);
void push_em_work(EMW *em)
{
    if (PS32(em, 0x564))
        release_prim(PS16(em, 0x568));
    memset(em, 0, 0x12);
}

/* pull_enemy_work (0x16A1A0): first free em_work slot, cleared and set up
 * (slot number at +0x13 and +0xC, +0x88D = -1, +0x798 = 1.0). */
void *rt_pull_enemy_work(void)
{
    int i;
    for (i = 0; i < 20; i++) {
        EMW *em = &em_work[i];
        if (em->be_flag == 0) {
            void *mdl = em->mdl;            /* host: keep the motion model work */
            memset(em, 0, sizeof *em);
            em->x13 = (u8)i;
            em->be_flag = 1;
            em->x10 = 1;
            PS8(em, 0x88D) = -1;
            em->id = (u16)i;
            em->mdl = mdl;
            PU8(em, 0x8C3) = PU8(&game_w, 0xD1);
            PF(em, 0x798) = 1.0f;
            return em;
        }
    }
    return NULL;
}

/* ------------------------------------------------ quest (main f_quest, not built) */
WEAK void Quest_enemy_die(EMW *em) { fprintf(stderr, "rt_em: monster %d (kind %d) down\n", em->id, em->kind); }
void Ext_pick_point_clr(int);
/* Em_hagi_point_clr (0x229A50) */
WEAK void Em_hagi_point_clr(EMW *em)
{
    Ext_pick_point_clr(PS8(em, 0x88D));
    PS8(em, 0x88D) = -1;
}

/* ------------------------------------------------ network (offline: nothing) */
WEAK void net_receive_em_act(EMW *em) { (void)em; }
WEAK void net_send_em(EMW *em, int a, int b) { (void)em; (void)a; (void)b; }

/* ------------------------------------------------ em_cmd (game.bin 0x55B060.., agent D) */
WSTUB(em_cmd_init)
WEAK void NextStage_No_Set(EMW *em) { (void)em; }

/* Em_Mode_Chg (0x566500): switch the monster's mode +0x888 (0 calm, 1
 * attack mode with timer +0x886); back to calm clears the hate/target
 * work and the players' +0x7EE marks it set. Returns 0 if unchanged. */
WEAK int Em_Mode_Chg(EMW *em, int mode, int timer)
{
    int i;
    u8 *pl;
    if (PU8(em, 0x888) == (u8)mode)
        return 0;
    if ((s16)timer > 0)
        PS16(em, 0x886) = (s16)timer;
    PU8(em, 0x888) = (u8)mode;
    PU8(em, 0x83B) &= 0xC0;
    if (PU8(em, 0x888) == 0) {
        PU8(em, 0x88C) = 0;
        PU8(em, 0x88F) = 0;
        for (i = 0, pl = (u8 *)player_work; i < 4; i++, pl += 0xA00) {
            PS32(em, 0x8F4 + 4 * i) = 0;
            PS32(em, 0x918 + 4 * i) = 0;
            PS16(em, 0x890 + 2 * i) = 0;
            if ((PU8(em, 0x7EE) & (1 << i)) && pl[0] && pl[0x7EE])
                pl[0x7EE] = 0;
        }
        PU8(em, 0x7EE) = 0;
    }
    return 1;
}

/* em_cdm_act_flag_ck (0x565DC0): +0x82B 1: target the first player whose
 * bit in +0x914 is set (+0x880/+0x881 = 1, +0x883 = index); none
 * flagged: +0x880 = 0. */
WEAK void em_cdm_act_flag_ck(EMW *em)
{
    int n = PU8(&game_w, 0xD3), i, c = 0;
    if (PU8(em, 0x82B) != 1) {
        if (PU8(em, 0x82B) == 0)
            PU8(em, 0x880) = 0;
        return;
    }
    for (i = 0; i < n; i++)
        if (PU8(em, 0x914) & (1 << i))
            c++;
    if (c == 0) {
        PU8(em, 0x880) = 0;
        return;
    }
    PU8(em, 0x880) = 1;
    PU8(em, 0x881) = 1;
    PU8(em, 0x882) = 0;
    for (i = 0; i < n; i++)
        if (PU8(em, 0x914) & (1 << i))
            break;
    PU8(em, 0x883) = (u8)i;
}

/* ------------------------------------------------ callees of em01 AI / em_cmd / Em_Dmg_Sys */
/* em_dur_set (main 0x10A0F0): refill part n's durability (+0x308 of the
 * 8-byte part slot) from em_dur_tbl[kind][n], if the part is on. */
void em_dur_set(EMW *em, int n)
{
    u8 *e = (u8 *)em + 8 * n;
    if ((unsigned)n >= 9 || e[0x304] == 0)
        return;
    *(s16 *)(e + 0x308) = em_dur_tbl[em->kind][n];
}

/* GetWaterData (main 0x16AB80): byte +0xC of the ground entry the work
 * stands on (ground_tbl_add[stage +0x736][+0x70C]): non-zero = water.
 * em_cmd_water_ck calls it with no arguments (a0 = em left over); its
 * build passes em (build_pc.sh). */
extern u8 *ground_tbl_add[];
int GetWaterData(void *w)
{
    u8 *p = w, *t = ground_tbl_add[p[0x736]];
    return t ? t[p[0x70C] * 16 + 0xC] : 0;
}

/* Quest side (main f_quest; f_quest_nm.c is not on the PC yet): carving
 * points are not set up (no carving yet), so they report "none". */
WEAK s8 Em_hagi_point_set(EMW *em, int n) { (void)n; PS8(em, 0x88D) = -1; return -1; }
WEAK int Em_hagi_point_cnt_ck(EMW *em) { (void)em; return -1; }
WEAK void Quest_enemy_capture(EMW *em) { fprintf(stderr, "rt_em: monster %d (kind %d) captured\n", em->id, em->kind); }
/* Quest_enemy_hagi_set (main 0x2276C0): quest_w+0x13C |= b */
WEAK void Quest_enemy_hagi_set(int a, int b) { (void)a; quest_w.x13C |= b; }
/* WyvernAreaMove (menu16.c): the map's monster-moved marker; no map yet. */
WEAK void WyvernAreaMove(void *em) { (void)em; }
/* wyvern_kill_cnt_up (ud_nm.c): online-only kill counter. */
WEAK void wyvern_kill_cnt_up(void *u, int n) { (void)u; (void)n; }

/* ------------------------------------------------ quest monster set-up
 * The parts of the quest start (main f_quest, Quest_init 0x2263xx /
 * Em_direct_set) the PC needs to put a quest's monster on the stage:
 * the mission file questName[no] (AFS_DATA, Meltw) is read into a host
 * mission_area and quest_w's table pointers set from its header as
 * Quest_init does (x64 header, x74 per-stage monster lists, x78 the
 * quest's own (big) monsters, x80 stage data, x94 info, x14E). */
void rt_quest_mem_init(void);
void Quest_init(void);
void Quest_start(void);
void Quest_timer_reset(void);
void Quest_em_init_set(int stage);
s32 *Em_data_com_adrs_get(s32 *p, int which);

/* --quest N: the game's own quest start. Quest_init (free-hunt tables),
 * then Quest_start as game11 runs it (select_w+0xAC = quest number): it
 * loads the mission file questName[no] into mission_area, points quest_w
 * at its tables, sets the stage (Quest_pl_stage_init), the time limit and
 * the monster states (quest_em_init). src/main/quest/f_quest*_nm.c. */
int rt_quest_load(int no)
{
    if (no <= 0 || no >= 0xB2)
        return -1;
    rt_quest_mem_init();
    Quest_init();
    game_w.master = 0;
    game_w.pl_num = 1;
    game_w.pl_state[0] = 1;
    *((u8 *)&select_w + 0xAC) = (u8)no;
    *((u8 *)&select_w + 0xAD) = 0;
    Quest_start();
    /* game13's start of the hunt: mode 2 (game2), timers */
    game_w.mode = 2;
    game_w.step = 0;
    Quest_timer_reset();
    return quest_w.no == no && quest_w.x94 ? 0 : -1;
}

/* no quest (free play in the viewer): Quest_init's free-hunt tables, so
 * the HUD and quest helpers have their data */
void rt_quest_free_hunt(void)
{
    rt_quest_mem_init();
    Quest_init();
}

/* the quest's own monsters (QEM list 1 of quest_w.x78), NULL for none */
static void *quest_com_list(int which)
{
    s32 *p;
    if (quest_w.no == 0)
        return NULL;
    p = Em_data_com_adrs_get(quest_w.x78, which);
    return p == NULL || p == (s32 *)-1 ? NULL : p;
}

/* The quest's first own monster (QEM, 0x3C bytes): its stage (QEM+7)
 * is where the hunt takes place; -1 without one. */
int rt_quest_monster_stage(int *kind)
{
    QEM *q = quest_com_list(1);
    if (!q || q->id < 0)
        return -1;
    if (kind)
        *kind = q->id;
    return (u8)q->x07;
}

/* Em_direct_set (main 0x2273xx, f_quest_nm.c) for one QEM entry: a free
 * em_work, the entry's kind, variant, stage, hunger/thirst/sleep,
 * position and angle, then enemy_mv's first step (em_init: em01_init sets
 * the hit points). Model slot 0 (game_w+0x28[0]) is the host's em01 model. */
int enemy_mv(EMW *em);
EMW *rt_monster_spawn_qem(const QEM *q)
{
    EMW *em = rt_pull_enemy_work();
    if (!em)
        return NULL;
    game_w.x28[0] = (u8)q->id;
    em->mdl_no = 0;
    em->kind = (u8)q->id;
    em->type = (u8)q->x02;
    PU8(em, 0x9EB) = (u8)q->x2C;
    em->stg = game_w.stage;
    em->hungry = q->x0C;
    em->thirst = q->x10;
    em->x8A0 = q->x14;
    PU8(em, 0x95B) = (u8)q->x06;
    em->pos[0] = q->pos[0];
    em->pos[1] = q->pos[1];
    em->pos[2] = q->pos[2];
    em->ang[1] = q->x1C;
    enemy_mv(em);
    return em;
}

/* The quest's own monster on the current stage (quest loaded with
 * rt_quest_load); without a quest, kind `kind` at pos facing ang_y.
 * Returns the em_work index or -1. */
int rt_monster_spawn(int kind, const float pos[3], int ang_y)
{
    QEM *q, dflt;
    EMW *em;
    if (quest_w.no != 0) {
        /* the quest's monsters, as the game sets them up when the stage
         * is entered: station_em_set (the quest's own) + Quest_next_em_set
         * (the stage's small monsters) -> Em_direct_set */
        int i;
        Quest_em_init_set(game_w.stage);
        for (i = 0; i < 20; i++)
            if (em_work[i].be_flag && getenv("RT_EM_TRACE"))
                fprintf(stderr, "rt_em: quest monster %d kind %d stage %d at %.0f %.0f %.0f hp %d\n",
                        i, em_work[i].kind, em_work[i].stg, em_work[i].pos[0], em_work[i].pos[1],
                        em_work[i].pos[2], PS16(&em_work[i], 0x302));
        em = &em_work[0];
        if (!em->be_flag)
            return -1;
        if (getenv("RT_EM_POS")) {      /* test aid: monster 0 at the given x,z */
            sscanf(getenv("RT_EM_POS"), "%f,%f", &em->pos[0], &em->pos[2]);
        }
        if (getenv("RT_EM_HP"))         /* test aid: monster 0's hit points */
            PS16(em, 0x302) = (s16)atoi(getenv("RT_EM_HP"));
        return 0;
    }
    q = &dflt;
    memset(&dflt, 0, sizeof dflt);
    dflt.id = (s16)kind;
    dflt.pos[0] = pos[0];
    dflt.pos[1] = pos[1];
    dflt.pos[2] = pos[2];
    dflt.x1C = ang_y & 0xFFFF;
    em = rt_monster_spawn_qem(q);
    if (!em)
        return -1;
    em->ang[1] = q->x1C;            /* em_status_init zeroes the angle in free hunts */
    if (getenv("RT_EM_TRACE"))
        fprintf(stderr, "rt_em: monster %d kind %d at %.0f %.0f %.0f ang %04X hp %d/%d\n",
                em->id, em->kind, em->pos[0], em->pos[1], em->pos[2], em->ang[1] & 0xFFFF,
                PS16(em, 0x302), PS16(em, 0x792));
    if (getenv("RT_EM_TRACE")) {
        int i;
        fprintf(stderr, "rt_em: part durability (kind %d):", PU8(em, 0x953));
        for (i = 0; i < 9; i++)
            fprintf(stderr, " %d", PU8(em, 0x304 + 8 * i) ? PS16(em, 0x308 + 8 * i) : -1);
        fprintf(stderr, "\n");
    }
    return em->id;
}

/* One game tick of monster no: the game's enemy_mv (src/main/em/f_em_nm.c). */
int rt_monster_tick(int no)
{
    EMW *em = &em_work[no];
    if (!em->be_flag)
        return 0;
    if (getenv("RT_EM_TRACE"))
        printf("em%d: step %d act %d/%d char %d frame %.1f pos %.0f %.0f %.0f ang %04X hp %d mode %d\n",
                no, em->x04, PU8(em, 0x14), PU8(em, 0x15), PS16(em, 0x2DC), PF(em, 0x19C),
                em->pos[0], em->pos[1], em->pos[2], em->ang[1] & 0xFFFF, PS16(em, 0x302), PU8(em, 0x888));
    return enemy_mv(em);
}

/* em_sleep_eff_set (game 0x53xxxx, em_master_nm.c): sleep bubbles at
 * joint a every 90 ticks (3 puffs, 10 apart). Callers pass (em, joint,
 * pos, scale); the PS2 definition reads two ints and leaves the scale in
 * f12 for Eft06_set2, which x86 cannot do: this is the PC definition. */
void Eft06_set2(f32 scale, void *chr, s16 arg, int joint, f32 *pos);
void em_sleep_eff_set(EMW *em, int a, f32 *pos, f32 scale)
{
    u16 t = *(u16 *)((u8 *)&game_w + 0x1E) % 90;
    if (t == 0 || t == 10 || t == 20)
        Eft06_set2(scale, em, 4, a, pos);
}

/* drawn by the host: the work is in use and on the current stage */
int rt_monster_shown(int no)
{
    EMW *em = &em_work[no];
    return em->be_flag && em->stg == game_w.stage;
}

/* all em_work slots free (the host's quest restart) */
void rt_monster_clear_all(void)
{
    int i;
    for (i = 0; i < 20; i++)
        em_work[i].be_flag = 0;
}
