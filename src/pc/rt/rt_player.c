/*
 * rt_player.c - the hunter driven by the game's own player code.
 *
 * Each tick (rt_player_tick): rt_pad_tick turns the host pad into Psw
 * (ioRead_sub), then pl_move (src/main/pl/pl48.c, matched) runs exactly
 * as on the PS2: pl_sw_set fills PLW.sw, pl_move_sub (pl_nm.c) runs the
 * timers, damage, the state machine (pl_move_sub_sub -> pl_normal /
 * pl_attack / pl_damage ... -> pl_mvNNN / pl_atNNN through their jump
 * tables), turning, the motion step (pl_chr_sub -> frame_init /
 * frame_move), the per-motion sound/effect hook (pl01_effect_move) and
 * the stage collision; hit_timer_calc_shl ages the attack shells.
 * The helpers that are not decompiled yet are in rt_pl.c (from the asm).
 *
 * Set-up (rt_player_game_init) does what init_pl_work (main 0x1116E0,
 * g_game_init) does for the master player in an offline quest: equipment
 * (+0x35E type/+0x360 weapon id, +0x34C = Ken_data[id][0], kind =
 * Battle_type[+0x34C]) and User_data for Get_equip_value; then the game's
 * pl_init(0) (pl01.c) places the hunter at stage_start_pos and starts the
 * idle motion (pl_init_sub -> normal_char_set).
 *
 * RT_PL_STANDIN=1 keeps the old host stand-in (turn/run/idle only).
 */
#include "rt.h"
#include "types.h"
#include "game.h"
#include "pl.h"
#include "fl.h"
#include "frame.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern FLMAT rview_mat;
void pl_sw_set(void);
void pl_move(void);
void pl_init(int mode);
void rt_pad_tick(void);
void hit_chk_init(void);
extern u8 Ken_data[][0x18];
extern u8 Gun_data[][0x14];
extern u8 Battle_type[];
extern u8 User_data[];

#define PF(pl, T, o) (*(T *)((u8 *)(pl) + (o)))

static int use_game = -1;

/* Weapon: Ken_data id (sword type 6) or Gun_data id (type 7). Default:
 * Ken 156, the first sword-and-shield (job 4) in the table. RT_WEAPON=id
 * picks another sword (e.g. 1 = the first great sword, job 0). */
void rt_player_game_init(int no)
{
    PLW *pl = &player_work[no];
    int wid = getenv("RT_WEAPON") ? atoi(getenv("RT_WEAPON")) : 156;
    int type = 6;
    if (wid < 0 || wid >= 234) wid = 156;
    /* init_pl_work's offline master path (Set_userdata) [read from the asm] */
    pl->be_flag = 1;
    pl->id = (u16)no;
    pl->stg = game_w.stage;
    PF(pl, u8, 0x35E) = 0;
    PF(pl, u8, 0x35F) = (u8)type;
    pl->wpn_kind = (u16)wid;
    pl->work34C = Ken_data[wid][0];
    pl->kind = Battle_type[pl->work34C];
    PF(User_data, u8, 0x3CD) = (u8)type;
    PF(User_data, u16, 0x3CE) = (u16)wid;
    game_w.pl_state[no] = 1;
#ifdef MH1_ONLINE
    {   /* co-op (rt_np.c): the session's slots, as init_pl_work sets them up online */
        void rt_np_init_slots(void);
        rt_np_init_slots();
    }
#endif
    pl_init(0);
    if (getenv("RT_PL_ITEMS")) {    /* no save data: pouch "id:n,id:n" (slots at +0x828, 4 bytes) */
        const char *q = getenv("RT_PL_ITEMS");
        int k = 0, id, num, used;
        while (k < 24 && sscanf(q, "%i:%i%n", &id, &num, &used) == 2) {
            PF(pl, s16, 0x828 + 4 * k) = (s16)id;
            PF(pl, s16, 0x82A + 4 * k) = (s16)num;
            k++;
            q += used;
            if (*q != ',')
                break;
            q++;
        }
    }
    {   /* Set_userdata copies the user's pouch (User_data+0x37C) to the
         * hunter; without save data the user's pouch starts as the one
         * pl_init / RT_PL_ITEMS gave him */
        void ItemCopy_Pl2Ud(PLW *), ItemCopy_Ud2Pl(PLW *);
        extern u8 User_data[];
        int k, any = 0;
        for (k = 0; k < 0x50; k++)
            any |= User_data[0x37C + k];
        if (any && !getenv("RT_PL_ITEMS"))
            ItemCopy_Ud2Pl(pl);
        else
            ItemCopy_Pl2Ud(pl);
    }
    if (getenv("RT_PL_TRACE"))
        fprintf(stderr, "rt_player: weapon %d model %d job %d at %.0f %.0f %.0f act %d/%d chr %d/%d\n",
                wid, pl->work34C, pl->kind, pl->pos[0], pl->pos[1], pl->pos[2], pl->flag14, pl->flag15,
                PF(pl, u16, 0x2DC), PF(pl, u16, 0x2DE));
}

int rt_player_job(int no)
{
    return player_work[no].kind;
}

/* ---------------------------------------------- old host stand-in */
void HitWallPlayer(void *ent, int keep);
int GetFloorSlide(void *ent, f32 *out, int flag);
int GetGroundHitStatusAreaPl(void *ent, f32 *pos, void *attr, f32 *out, f32 *flag);
u8 Pl_stg_ck(void *);

static int moving[8];
static float fall_v[8];

static void set_motion(PLW *pl, int legs, int upper, int blend)
{
    FRW *w = (FRW *)pl;
    w->chr[0] = (u16)legs;
    w->chr[1] = (u16)upper;
    w->mt[0].spd = 1.0f;
    w->mt[1].spd = 1.0f;
    frame_init(w, 0, blend, 0);
    frame_init(w, 0, blend, 1);
}

static int ang_diff(int a, int b)
{
    return (s16)(u16)(b - a);
}

static void standin_collide(PLW *pl)
{
    f32 v[4], gy;
    int no = pl->id & 7;
    if (Pl_stg_ck(pl) & 0xFF)
        HitWallPlayer(pl, 0);
    if (pl->flag14 == 0 && (Pl_stg_ck(pl) & 0xFF))
        GetFloorSlide(pl, v, 1);
    if (GetGroundHitStatusAreaPl(pl, pl->pos, (u8 *)pl + 0x70C, &gy, (f32 *)((u8 *)pl + 0x7E4)) == 1)
        pl->x5AC = gy;
    if (!(Pl_stg_ck(pl) & 0xFF))
        return;
    gy = pl->x5AC;
    if (pl->pos[1] < gy || pl->pos[1] - gy < 30.0f) {
        pl->pos[1] = gy;
        fall_v[no] = 0;
    } else {
        fall_v[no] -= 3.0f;
        pl->pos[1] += fall_v[no];
        if (pl->pos[1] < gy) {
            pl->pos[1] = gy;
            fall_v[no] = 0;
        }
    }
}

static void standin_tick(int no)
{
    PLW *pl = &player_work[no];
    int want;
    PF(pl, f32, 0x5A0) = pl->pos[0];
    PF(pl, f32, 0x5A4) = pl->pos[1];
    PF(pl, f32, 0x5A8) = pl->pos[2];
    rt_pad_tick();
    pl_sw_set();
    want = pl->sw.pow[0] > 0;
    if (want) {
        float a = (float)pl->sw.ang[0] * (6.2831853f / 65536.0f);
        float sx = cosf(a), sy = sinf(a);
        float rx = rview_mat[0][0], rz = rview_mat[0][2];
        float fx = -rview_mat[2][0], fz = -rview_mat[2][2];
        float dx = rx * sx + fx * sy, dz = rz * sx + fz * sy;
        int target = (int)(atan2f(dx, dz) * (65536.0f / 6.2831853f)) & 0xFFFF;
        int d = ang_diff(pl->ang[1], target);
        if (d > 0x800) d = 0x800;
        if (d < -0x800) d = -0x800;
        pl->ang[1] = (pl->ang[1] + d) & 0xFFFF;
        pl->ang_y = (s16)pl->ang[1];
    }
    if (want != moving[no]) {
        moving[no] = want;
        set_motion(pl, want ? 3 : 1, want ? 103 : 101, 4);
    }
    rt_snd_player_motion(no);
    frame_move((FRW *)pl);
    standin_collide(pl);
}

/* ---------------------------------------------- tick */
int rt_player_uses_game(void)
{
    if (use_game < 0)
        use_game = getenv("RT_PL_STANDIN") == NULL;
    return use_game;
}

/* test aids RT_PL_AIM / RT_PL_WARP_EM / RT_DMG_MUL work on monster slot 0,
 * or on the slot RT_PL_TARGET="tick:slot,tick:slot,..." names from that
 * player tick on */
static int pl_ticks;
int rt_tick_count(void) { return pl_ticks; }
int rt_test_target(void)
{
    const char *s = getenv("RT_PL_TARGET");
    int slot = 0;
    if (s && s[0] == 'k') {     /* "kN": the nearest living monster of kind N on the hunter's stage */
        extern u8 em_work[];
        PLW *p = &player_work[0];
        int kind = atoi(s + 1), i;
        float best = -1;
        int pass, none = 0;
        for (pass = 0; pass < 2 && best < 0; pass++) {      /* living first, then a dead one still lying there (carving) */
            for (i = 0; i < 20; i++) {
                u8 *e = em_work + 0xA10 * i;
                float dx, dz, d;
                int alive = *(s16 *)(e + 0x302) > 0;
                if (!e[0] || e[2] != kind || e[0x736] != p->stg || alive != (pass == 0))
                    continue;
                dx = *(f32 *)(e + 0xAC) - p->pos[0];
                dz = *(f32 *)(e + 0xB4) - p->pos[2];
                d = dx * dx + dz * dz;
                if (best < 0 || d < best) {
                    best = d;
                    slot = i;
                }
            }
        }
        if (best < 0) {     /* none on this stage: an unused slot, so WARP_EM / DMG_MUL / AIM do nothing */
            for (i = 0; i < 20; i++)
                if (!em_work[0xA10 * i]) {
                    none = i;
                    break;
                }
            slot = none;
        }
        return slot;
    }
    while (s && *s) {
        int t, n;
        if (sscanf(s, "%d:%d", &t, &n) == 2 && pl_ticks >= t)
            slot = n;
        s = strchr(s, ',');
        if (s)
            s++;
    }
    return slot >= 0 && slot < 20 ? slot : 0;
}

void rt_player_tick(int no)
{
    if (!rt_player_uses_game()) {
        standin_tick(no);
        return;
    }
    rt_pad_tick();
    pl_ticks++;
    if (getenv("RT_PL_AIM")) {      /* test aid: face monster 0 while standing (scripted fights) */
        extern u8 em_work[];
        u16 Em_Calc_angY(f32 *a, f32 *b);
        PLW *p = &player_work[no];
        u8 *tg = em_work + 0xA10 * rt_test_target();
        if (tg[0] && (p->flag14 == 0 || p->flag14 == 1) && (p->flag15 == 0 || p->flag15 == 2 || p->flag15 == 3 || p->flag15 == 4))
            p->ang[1] = Em_Calc_angY(p->pos, (f32 *)(tg + 0xAC));
    }
    if (getenv("RT_PL_WARP_EM")) {  /* test aid: at ticks "t1,t2-t3,..", next to monster 0's carve point (or its first body volume), facing it */
        static int tk;
        extern u8 em_work[], StiEM_data[];
        extern u8 *em_body_tbl[];
        int hit_data_expand(void *chr, void *body, f32 *cap, f32 *sph);
        u16 Em_Calc_angY(f32 *a, f32 *b);
        const char *s = getenv("RT_PL_WARP_EM");
        u8 *tg = em_work + 0xA10 * rt_test_target();
        int hit = 0;
        tk++;
        for (; *s; s++) {       /* "t" or a range "t1-t2" (every tick in it) */
            int a = atoi(s), b = a;
            const char *dash = strchr(s, '-'), *comma = strchr(s, ',');
            if (dash && (!comma || dash < comma))
                b = atoi(dash + 1);
            if (tk >= a && tk <= b)
                hit = 1;
            while (*s && *s != ',')
                s++;
            if (!*s)
                break;
        }
        if (hit && tg[0]) {
            PLW *p = &player_work[no];
            s8 hp = (s8)tg[0x88D];
            f32 *t = hp >= 0 ? (f32 *)(StiEM_data + 0x1C * hp) : (f32 *)(tg + 0xAC), cap[8], sph[4], r = 120.0f;
            u8 *bd = em_body_tbl[tg[2]];
            if (hp < 0 && *(s16 *)(tg + 0x302) <= 0) {
                /* dead, without EMW+0x88D: the nearest pick point the
                 * monster set itself (Fatalis' three, em02_hagi_set) */
                int k;
                f32 best = 1e9f;
                for (k = 0; k < 20; k++) {
                    u8 *e = StiEM_data + 0x1C * k;
                    f32 *ep = (f32 *)e, dx = ep[0] - *(f32 *)(tg + 0xAC), dz = ep[2] - *(f32 *)(tg + 0xB4);
                    if (*(u16 *)(e + 0x10) == 0xFFFF || *(s16 *)(e + 0x14) != 2 || e[0x18] != tg[0x736])
                        continue;
                    if (dx * dx + dz * dz < best) {
                        best = dx * dx + dz * dz;
                        t = ep;
                        hp = (s8)k;
                    }
                }
            }
            if (hp < 0 && bd) {     /* alive: its first body sphere (or the first capsule's middle) */
                u8 *b;
                int k = -1;
                for (b = bd; *(s16 *)b != -1 && k != 0; b += 0x28) {
                    int r1 = hit_data_expand(tg, b, cap, sph);
                    if (r1 == 0 && sph[3] > 0) {
                        k = 0;
                    } else if (r1 == 1 && k < 0) {
                        sph[0] = (cap[0] + cap[3]) / 2;
                        sph[1] = (cap[1] + cap[4]) / 2;
                        sph[2] = (cap[2] + cap[5]) / 2;
                        sph[3] = cap[6];
                        k = 1;
                    }
                }
                if (k >= 0 && sph[3] > 0) {
                    t = sph;
                    r = sph[3] + 40.0f;
                }
            }
            f32 d[2] = { p->pos[0] - t[0], p->pos[2] - t[2] }, l = sqrtf(d[0] * d[0] + d[1] * d[1]);
            if (l < 1) { d[0] = 1; l = 1; }
            p->pos[0] = t[0] + d[0] / l * r;
            p->pos[2] = t[2] + d[1] / l * r;
            if (hp >= 0)
                p->pos[1] = t[1];
            p->ang[1] = Em_Calc_angY(p->pos, t);
            if (getenv("RT_PL_TRACE") || getenv("RT_QUEST_TRACE"))
            fprintf(stderr, "rt_player: tick %d warped to %.0f %.0f (carve point %d at %.0f %.0f %.0f)\n",
                    tk, p->pos[0], p->pos[2], hp, t[0], t[1], t[2]);
        }
    }
    if (getenv("RT_PL_WARP")) {     /* test aid: "tick,x,z[,ang][;...]": put the hunter at x,z (same height), facing ang (hex) at those player ticks */
        static int tk;
        const char *s = getenv("RT_PL_WARP");
        tk++;
        while (s && *s) {
            int t = 0;
            unsigned a;
            float x, z, y = 0;
            int nf = sscanf(s, "%d,%f,%f,%x,%f", &t, &x, &z, &a, &y);
            if (nf >= 3 && tk == t) {
                player_work[no].pos[0] = x;
                player_work[no].pos[2] = z;
                if (nf >= 4)            /* optional facing angle (hex) */
                    player_work[no].ang[1] = (s32)(a & 0xFFFF);
                if (nf == 5)            /* optional height (a pick point below the ground level the warp lands on) */
                    player_work[no].pos[1] = y;
                fprintf(stderr, "rt_player: tick %d warped to %.0f %.0f\n", tk, x, z);
            }
            s = strchr(s, ';');
            if (s)
                s++;
        }
    }
    if (getenv("RT_PL_GOTO")) {     /* test aid: "tick,stage": from that player tick, walk the area exits
                                       (stage_mv_ck's STG_MV lists, shortest path) until the hunter is on that stage */
        static int tk, last;
        int t0 = 0, goal = -1;
        PLW *p = &player_work[no];
        tk++;
        int goto2_active = 0;
        sscanf(getenv("RT_PL_GOTO"), "%d,%d", &t0, &goal);
        if (getenv("RT_PL_GOTO2")) {        /* "tick,stage;tick,stage;...": from each tick on the goal is that stage (egg trips to the nest and the camp) */
            const char *g2 = getenv("RT_PL_GOTO2");
            int t2, s2;
            while (g2 && *g2) {
                if (sscanf(g2, "%d,%d", &t2, &s2) == 2 && tk >= t2) {
                    t0 = 0, goal = s2;
                    goto2_active = 1;
                }
                g2 = strchr(g2, ';');
                if (g2)
                    g2++;
            }
        }
        if (goto2_active) {
        } else if (strstr(getenv("RT_PL_GOTO"), ",f") && getenv("RT_PL_TARGET") && getenv("RT_PL_TARGET")[0] == 'k') {
            /* "tick,f" with RT_PL_TARGET=kN: follow the monster, goal = the stage a living monster of kind N is on now
             * (large monsters walk between areas) */
            extern u8 em_work[];
            int kind = atoi(getenv("RT_PL_TARGET") + 1), i;
            goal = -1;
            for (i = 0; i < 20 && goal < 0; i++) {
                u8 *e = em_work + 0xA10 * i;
                if (e[0] && e[2] == kind && *(s16 *)(e + 0x302) > 0 && e[0x736] < 0x58)
                    goal = e[0x736];
            }
        } else {   /* "tick,stageA,stageB,...": with RT_PL_TARGET=kN, go on to the next stage once no living monster of kind N is
             * left on the goal stage for 90 ticks (multi-stage hunts, tools/test_all_quests.py) */
            static int idx, calm;
            const char *g = getenv("RT_PL_GOTO"), *tt = getenv("RT_PL_TARGET");
            int n = 0, i;
            extern u8 em_work[];
            for (i = 0, g = strchr(g, ','); g; g = strchr(g + 1, ','), i++)
                ;
            if (idx >= i)       /* past the last stage: start over (stages restock when re-entered) */
                idx = 0;
            for (i = 0, g = strchr(getenv("RT_PL_GOTO"), ','); g; g = strchr(g + 1, ','), i++)
                if (i == idx)
                    goal = atoi(g + 1), n = 1;
            if (n && tk >= t0 && p->stg == goal && tt && tt[0] == 'k') {
                int alive = 0, kind = atoi(tt + 1);
                for (i = 0; i < 20; i++) {
                    u8 *e = em_work + 0xA10 * i;
                    if (e[0] && e[2] == kind && e[0x736] == p->stg && *(s16 *)(e + 0x302) > 0)
                        alive++;
                }
                calm = alive ? 0 : calm + 1;
                if (calm > 90) {
                    idx++;
                    calm = 0;
                    fprintf(stderr, "rt_player: tick %d stage %d cleared of kind %d, next goal\n", tk, p->stg, kind);
                }
            }
        }
        if (tk >= t0 && goal >= 0 && p->stg != goal && p->x738 == 0 && tk - last > 30) {
            void *Stage_mv_data_get(int st, int pl);
            static s16 prev[0x58];
            int q[0x58], qh = 0, qt = 0, st, nx = -1;
            memset(prev, 0xFF, sizeof prev);
            prev[p->stg] = p->stg;
            q[qt++] = p->stg;
            while (qh < qt && prev[goal] < 0) {         /* breadth first over the exit lists */
                u8 *m = Stage_mv_data_get(st = q[qh++], 0);
                for (; m && *(u16 *)m != 0xFFFF; m += 0x34)
                    if (*(u16 *)m < 0x58 && prev[*(u16 *)m] < 0) {
                        prev[*(u16 *)m] = (s16)st;
                        q[qt++] = *(u16 *)m;
                    }
            }
            if (prev[goal] >= 0) {
                for (nx = goal; prev[nx] != p->stg; nx = prev[nx])
                    ;
                {
                    u8 *m = Stage_mv_data_get(p->stg, 0);
                    for (; m && *(u16 *)m != 0xFFFF; m += 0x34)
                        if (*(u16 *)m == nx) {
                            f32 *e = (f32 *)(m + 4);
                            int kind = *(s16 *)(m + 2);
                            p->pos[0] = kind == 1 ? (e[0] + e[5]) * 0.5f : e[0];
                            p->pos[1] = e[1] + 1.0f;
                            p->pos[2] = kind == 1 ? (e[2] + e[7]) * 0.5f : e[2];
                            fprintf(stderr, "rt_player: tick %d stage %d -> exit to %d (goal %d)\n", tk, p->stg, nx, goal);
                            if (getenv("RT_GOTO_TRACE"))
                                fprintf(stderr, "  exit kind %d pos %.0f %.0f %.0f r/h %.0f %.0f box %.0f %.0f %.0f dest %.0f %.0f %.0f\n", kind,
                                        e[0], e[1], e[2], e[3], e[4], e[5], e[6], e[7], e[8], e[9], e[10]);
                            last = tk;
                            {   /* the warp lands on the ground, which may be above the exit's height window (stage 37 -> 40
                                 * stands at y 1000, the window is 500-1000): after two tries take the exit as stage_mv_ck
                                 * does when it hits (f_stage.c) */
                                static int tries, tnx = -1, tst = -1;
                                void Pl_ofs_set(PLW *pl, f32 *out, u16 ang);
                                tries = (tnx == nx && tst == p->stg) ? tries + 1 : 0;
                                tnx = nx; tst = p->stg;
                                if (tries >= 2) {
                                    PF(p, u8, 0x738) = 1;
                                    PF(p, u16, 0x73A) = (u16)nx;
                                    PF(p, f32, 0x73C) = e[8];
                                    PF(p, f32, 0x740) = e[9];
                                    PF(p, f32, 0x744) = e[10];
                                    PF(p, u16, 0x570) = (u16)(*(u16 *)(m + 0x30) + 0x4000);
                                    Pl_ofs_set(p, (f32 *)((u8 *)p + 0x73C), PF(p, u16, 0x570));
                                    fprintf(stderr, "rt_player: tick %d exit to %d taken directly\n", tk, nx);
                                    tries = 0;
                                }
                            }
                            break;
                        }
                }
            } else if (tk - last > 300) {
                fprintf(stderr, "rt_player: tick %d no path from stage %d to %d\n", tk, p->stg, goal);
                last = tk;
            }
        }
    }
    if (getenv("RT_PL_DIE")) {      /* test aid: "t1,t2,..": the hunter faints (the game's Pl_die_set) at those ticks */
        static int tk;
        void Pl_die_set(PLW *pl);
        const char *s = getenv("RT_PL_DIE");
        tk++;
        while (*s) {
            if (atoi(s) == tk) {
                fprintf(stderr, "rt_player: tick %d faint (Pl_die_set)\n", tk);
                PF(&player_work[no], s16, 0x302) = 0;
                Pl_die_set(&player_work[no]);
            }
            while (*s && *s != ',')
                s++;
            if (*s)
                s++;
        }
    }
    pl_move();
    {   /* PLW+0x60: the world matrix player_modify (weapon3.c, run from trans() on the
         * PS2) builds; the host poses the skeleton itself, but game code reads it
         * (demo cameras relative to the hunter: cmd_set_pos mode 0) */
        void cpAng2Rad_all(s32 *ang, f32 *out);
        void flmatMakeScale(FLMAT *m, f32 x, f32 y, f32 z);
        void flmatRotXYZ33(FLMAT *m, f32 x, f32 y, f32 z);
        void flmatSetTrans(FLMAT *m, f32 x, f32 y, f32 z);
        int k;
        for (k = 0; k < 8; k++) {
            PLW *p = &player_work[k];
            f32 a[3];
            FLMAT m;
            if (!p->be_flag || !p->x01)
                continue;
            cpAng2Rad_all((s32 *)&p->ang, a);
            flmatMakeScale(&m, p->scl[0], p->scl[1], p->scl[2]);
            flmatRotXYZ33(&m, a[0], a[1], a[2]);
            flmatSetTrans(&m, p->pos[0], p->pos[1], p->pos[2]);
            memcpy((u8 *)p + 0x60, &m, sizeof m);    /* 0x60-0x9F, as flmatCopy */
        }
    }
    if (getenv("RT_PL_GOD")) {      /* test aid: the hunter's vital (+0x302) back to 100 each tick, no stun gauge (+0x7AA) */
        PF(&player_work[no], s16, 0x302) = 100;
        PF(&player_work[no], s16, 0x7AA) = 0;
    }
    if (getenv("RT_PL_TRACE")) {
        PLW *pl = &player_work[no];
        printf("pl: act %d/%d step %d chr %d/%d fr %.1f spd %.1f pos %.0f %.0f %.0f ang %04X st %d sw %04X/%04X hp %d bite %d\n",
               pl->flag14, pl->flag15, PF(pl, u8, 5), PF(pl, u16, 0x2DC), PF(pl, u16, 0x2DE),
               PF(pl, f32, 0x19C), PF(pl, f32, 0x1A0), pl->pos[0], pl->pos[1], pl->pos[2],
               pl->ang[1] & 0xFFFF, pl->st, pl->sw.now, pl->sw.trg, PF(pl, s16, 0x302), pl->x881);
    }
}

/* Read back what the game code saw (for tests): buttons, stick. */
void rt_player_sw(int no, int *now, int *ang, int *pow)
{
    PLW *pl = &player_work[no];
    *now = pl->sw.now;
    *ang = pl->sw.ang[0];
    *pow = pl->sw.pow[0];
}

void rt_player_set_ang(int no, int ang_y)
{
    player_work[no].ang[1] = ang_y & 0xFFFF;
    player_work[no].ang_y = (s16)ang_y;
}

/* ---------------------------------------------- weapon placement */
/* Where the weapon model's root bones go this frame, as weapon_trans
 * (main 0x167FE0? f_weapon; C in src/main/weapon/weapon3_nm.c) places them:
 * weapon_joint_calc (weapon_nm.c) says right hand (0), left hand (1) or
 * sheathed (2); the base is that part's world matrix (part 0x12 / 0xE;
 * sheathed: part 9 for sword and shield, else part 10) moved by the
 * weapon_disp_tbl_r/l/b[job] offset turned by the part (part 10 when
 * sheathed), and the root node gets the table's XYZ rotation and scale
 * (0.8, SnS 1.0, lance 0.9). For sword and shield the shield bones (the
 * second hierarchy, AHI group 1) follow joint 0x11 (left forearm) with a
 * fixed rotation and offset. Per-motion node scaling of the great sword,
 * lance, hammer and bowguns (weapon_dat_make tables) is not done.
 * Out: root0 / root1 = world matrices of the two hierarchy roots (row
 * vectors); returns the joint mode, -1 when the parts are not set. */
typedef struct { f32 p[3]; f32 r[3]; } RT_WDISP;
extern RT_WDISP weapon_disp_tbl_r[], weapon_disp_tbl_l[], weapon_disp_tbl_b[];
s16 weapon_joint_calc(PLW *pl);
void flmatInit(FLMAT *m);
void flmatCopy(FLMAT *d, FLMAT *s);
void flmatSetXYZ33(FLMAT *m, f32 x, f32 y, f32 z);
void flmatMakeScale(FLMAT *m, f32 x, f32 y, f32 z);
void flmatMul33_2(FLMAT *a, FLMAT *b);
void flmatMul(FLMAT *d, FLMAT *a, FLMAT *b);
void flvecApplyMat33(f32 *out, f32 *v, FLMAT *m);
FLMAT *get_joint_wmat(void *chr, int joint);

static FLMAT *part_w(PLW *pl, int i)
{
    u8 *b = (u8 *)(uintptr_t)PF(pl, u32, 0x110 + i * 4);
    return b ? (FLMAT *)(b + 0x40) : NULL;
}

int rt_player_weapon(int no, float *root0, float *root1)
{
    PLW *pl = &player_work[no];
    FLMAT m0, nd, m2, w;
    f32 p[3], r[3], o[3], sc = 1.0f;
    RT_WDISP *tb;
    int jt, k = pl->kind;
    if (!part_w(pl, 9) || k > 5)
        return -1;
    jt = weapon_joint_calc(pl);
    if (jt != 2) {
        int pi = jt == 0 ? 0x12 : 0xE;
        flmatCopy(&m0, part_w(pl, pi));
        tb = jt == 0 ? &weapon_disp_tbl_r[k] : &weapon_disp_tbl_l[k];
        memcpy(p, tb->p, sizeof p);
        memcpy(r, tb->r, sizeof r);
        flvecApplyMat33(o, p, part_w(pl, pi));
    } else {
        flmatCopy(&m0, part_w(pl, k == 4 ? 9 : 10));
        tb = &weapon_disp_tbl_b[k];
        memcpy(p, tb->p, sizeof p);
        memcpy(r, tb->r, sizeof r);
        flvecApplyMat33(o, p, part_w(pl, 10));
        sc = k == 4 || k == 5 ? 1.0f : k == 3 ? 0.9f : 0.8f;
    }
    m0[3][0] += o[0];
    m0[3][1] += o[1];
    m0[3][2] += o[2];
    flmatInit(&nd);
    flmatSetXYZ33(&nd, r[0], r[1], r[2]);
    flmatMakeScale(&m2, sc, sc, sc);
    flmatMul33_2(&nd, &m2);
    flmatMul(&w, &nd, &m0);
    memcpy(root0, w, sizeof w);
    if (k == 3 || k == 4) {     /* shield (lance / sword and shield) */
        FLMAT n2;
        flmatCopy(&m0, get_joint_wmat(pl, 0x11));
        flmatInit(&n2);
        if (k == 4) {
            flmatSetXYZ33(&n2, -0.453785628f, -0.0523598827f, 0.139626354f);
            p[0] = -21.0f; p[1] = 2.2f; p[2] = 5.0f;
        } else {
            flmatSetXYZ33(&n2, 0.366519153f, -3.00196648f, -0.0436332338f);
            p[0] = -18.0f; p[1] = 2.0f; p[2] = -14.0f;
        }
        flvecApplyMat33(o, p, &m0);
        m0[3][0] += o[0];
        m0[3][1] += o[1];
        m0[3][2] += o[2];
        flmatMul(&w, &n2, &m0);
        memcpy(root1, w, sizeof w);
    } else
        memcpy(root1, root0, sizeof w);
    return jt;
}

/* PLW+0x5FC: the hair colour (0xFFRRGGBB, set from test_hair_col by the
 * edit / load code); 0 = none set */
unsigned rt_player_hair_col(int no)
{
    return *(u32 *)((u8 *)&player_work[no] + 0x5FC);
}

int rt_player_weapon_model(int no)
{
    return player_work[no].work34C;
}

int rt_weapon_afs(int model, int tex)
{
    extern s32 weapon_model_data[], WEAPON_TEX[];
    if (model < 0 || model >= 124)
        return -1;
    return tex ? WEAPON_TEX[model] : weapon_model_data[model];
}

void rt_actor_joints(const void *chr, const float *mats, int n);
extern u8 em_work[];
void rt_monster_joints(int no, const float *world, int n)
{
    rt_actor_joints(em_work + 0xA10 * no, world, n);
}

void hit_check(void);
void rt_hit_check(void)
{
    static int tr = -1;
    u8 *e = em_work + 0xA10 * rt_test_target();
    hit_check();
    if (getenv("RT_PL_SLAY")) {     /* test aid "tick[,kind]": from that player tick on, the living monsters of that kind (default: the
                                       RT_PL_TARGET one) take a lethal hit each tick, as if hit_check had found one: a boss that
                                       stays out of reach (Rathalos aloft, a submerged Plesioth) still ends the quest
                                       (tools/test_all_quests.py) */
        int t = 0, kind = -1, i, nk = 0, kinds[16];
        const char *g = getenv("RT_PL_SLAY");
        t = atoi(g);
        while ((g = strchr(g, ',')) != NULL && nk < 16)       /* "tick,kind,kind,...": every monster of these kinds */
            kinds[nk++] = atoi(++g);
        if (nk)
            kind = 0;
        for (i = 0; i < 20 && rt_tick_count() >= t && (rt_tick_count() - t) % 120 == 0; i++) {   /* every 120 ticks: a hit every tick keeps a reacting monster in its flinch (the Plesioth in fly18 sets x8BB, which floors hp at 1) */
            u8 *tg = em_work + 0xA10 * i;
            int match = 0, k;
            for (k = 0; k < nk; k++)
                match |= tg[2] == kinds[k];
            if (tg[0] && *(s16 *)(tg + 0x302) > 0 && (kind >= 0 ? match : tg == e)) {
                static int once;
                tg[0x38D] = 1;
                PF(tg, s16, 0x766) = 30000;
                if (!once++)
                    fprintf(stderr, "rt_player: RT_PL_SLAY: monster slot %d (kind %d) takes a lethal hit\n", i, tg[2]);
            }
        }
    }
    if (e[0x38D] && getenv("RT_DMG_MUL")) {     /* test aid: scale this tick's damage to monster 0 */
        int k, m = atoi(getenv("RT_DMG_MUL"));
        for (k = 0; k < 8; k++)
            PF(e, s16, 0x766 + 2 * k) = (s16)(PF(e, s16, 0x766 + 2 * k) * m);
    }
    if (tr < 0) tr = getenv("RT_HIT_DM") != NULL;
    if (tr > 1 || (tr && getenv("RT_HIT_DM")[0] == '2')) {   /* RT_HIT_DM=2: live shells each tick */
        extern u8 *shell_w_top;
        u8 *sh;
        tr = 2;
        for (sh = shell_w_top; sh; sh = *(u8 **)(sh + 0x10)) {
            if (sh[0] && sh[0xB] == 2 && *(u8 **)(sh + 0x88)) {
                extern u8 *em_body_tbl[];
                int hit_data_expand(void *chr, void *body, f32 *cap, f32 *sph);
                u8 *b = *(u8 **)(sh + 0x88);
                f32 cap[8], sph[4];
                int k;
                for (; *(s16 *)b != -1; b += 0x28) {
                    k = hit_data_expand(&player_work[sh[0xA]], b, cap, sph);
                    printf("  sb j %d t %d -> %d cap %.0f %.0f %.0f - %.0f %.0f %.0f r %.0f sph %.0f %.0f %.0f r %.0f\n",
                           *(s16 *)b, *(s16 *)(b + 2), k, cap[0], cap[1], cap[2], cap[3], cap[4], cap[5], cap[6],
                           sph[0], sph[1], sph[2], sph[3]);
                }
                printf("  em0 be %d x10 %d mode %d stg %d (game %d) x40C %d x40A %d dm %d\n", e[0], e[0x10], e[0x14], e[0x736],
                       rt_game_stage(), PF(e, u16, 0x40C), e[0x40A], e[0x38D]);
                for (b = em_body_tbl[e[2]]; b && *(s16 *)b != -1; b += 0x28) {
                    k = hit_data_expand(e, b, cap, sph);
                    printf("  eb j %d t %d -> %d cap %.0f %.0f %.0f - %.0f %.0f %.0f r %.0f sph %.0f %.0f %.0f r %.0f\n",
                           *(s16 *)b, *(s16 *)(b + 2), k, cap[0], cap[1], cap[2], cap[3], cap[4], cap[5], cap[6],
                           sph[0], sph[1], sph[2], sph[3]);
                }
            }
            if (sh[0])
                printf("shl: type %d arg %d mode %d hit_mode %d x08 %d no %d atk %d body %p x7B %d stg %d pos2 %.0f %.0f %.0f\n",
                       sh[2], sh[3], sh[4], sh[0xB], sh[8], sh[0xA], sh[0x63], *(void **)(sh + 0x88), sh[0x7B], sh[0xCA],
                       PF(sh, f32, 0x30), PF(sh, f32, 0x34), PF(sh, f32, 0x38));
        }
    }
    if (tr && e[0x38D]) {
        int k;
        printf("hit: em0 dm_flag %d part %d pos %.0f %.0f %.0f ang %04X vals", e[0x38D], e[0x38E],
               PF(e, f32, 0x430), PF(e, f32, 0x434), PF(e, f32, 0x438), PF(e, u16, 0x3EC));
        for (k = 0; k < 8; k++)
            printf(" %d", PF(e, s16, 0x766 + 2 * k));
        printf("\n");
    }
}
