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
    pl_init(0);
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

void rt_player_tick(int no)
{
    if (!rt_player_uses_game()) {
        standin_tick(no);
        return;
    }
    rt_pad_tick();
    pl_move();
    if (getenv("RT_PL_TRACE")) {
        PLW *pl = &player_work[no];
        printf("pl: act %d/%d step %d chr %d/%d fr %.1f spd %.1f pos %.0f %.0f %.0f ang %04X st %d sw %04X/%04X\n",
               pl->flag14, pl->flag15, PF(pl, u8, 5), PF(pl, u16, 0x2DC), PF(pl, u16, 0x2DE),
               PF(pl, f32, 0x19C), PF(pl, f32, 0x1A0), pl->pos[0], pl->pos[1], pl->pos[2],
               pl->ang[1] & 0xFFFF, pl->st, pl->sw.now, pl->sw.trg);
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
    u8 *e = em_work;
    hit_check();
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
