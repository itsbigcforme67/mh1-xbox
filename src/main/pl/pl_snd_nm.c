/* Player sound/effect wrappers (SLPM_654.95 main 0x24A240-0x24A790): pl_local_init (empty), pl01_effect_move,
 * sound_call* (play a sound once when the current motion reaches a frame), wall_sd_req/ashi_sd_req/yoroi_sd_req
 * (footstep, wall and armor sounds: the sound id is base*2 + random bit), ashi_eft_req (foot effect by kind),
 * move_default. pl01_effect_move drives ef_move_sub (the big per-motion effect/sound script at 0x24A790). */
#include "pl.h"
#include "game.h"
#include "plf.h"
#include "flow.h"

/* argument register the original leaves unset (stale value) in a few Code_Make/sound_call calls: -1 = no sound */
#define STALE (-1)

int frame_check(f32, PLW *, int);
void Pl_se_req2_com(PLW *, int, int, f32 *, int, int);
void Pl_se_req2(PLW *, int, int, f32 *, int, int);
void se_req2(int, int, int, f32 *, int, int);
void armor_sd_req(PLW *, int);
void eft13_set(PLW *, int, int);
void parts_chg();
struct PL01EX;
void ef_move_sub_0024A790(PLW *, struct PL01EX *);
int Code_Make(int, int, int, int);
void Eft13_set_scl(PLW *, int, int, f32);
void Eft20_set_pl(PLW *, int, int, int);
void func_54BA40(PLW *, int);
void func_555020(PLW *, int);
void func_60E2B0(PLW *, int);
void SetVector(f32 *, f32, f32, f32);
void Eft02_set_pos(f32 *, int, int);
int Pl_Skill_ck(PLW *, int);
int frame_check2(f32, PLW *, int);

static void sound_call_0024A2A0(PLW *pl, int frame, int se);
static void sound_call_h(PLW *pl, int frame, int se);
static void sound_call2(PLW *pl, int frame, int se);

void pl_local_init(void) {
}

void pl01_effect_move(PLW *pl) {
    u8 *ex = (u8 *)pl + 0x444;

    switch (ex[1]) {
    case 0:
        ex[1]++;
        *(s16 *)(ex + 0x12) = 0;
        break;
    case 1:
        ef_move_sub_0024A790(pl, (struct PL01EX *)ex);
        break;
    }
}

static void sound_call_0024A2A0(PLW *pl, int frame, int se) {
    if (frame_check((f32)frame, pl, 0) != 0) {
        Pl_se_req2_com(pl, se, 0, pl->pos, 1, 0);
    }
}

static void sound_call_h(PLW *pl, int frame, int se) {
    if (frame_check((f32)frame, pl, 0) != 0) {
        Pl_se_req2_com(pl, se, 0, pl->pos, 3, 0);
    }
}

static void sound_call2(PLW *pl, int frame, int se) {
    if (pl->work7ED == 0 && GW8(0x1DC) == 0 && frame_check((f32)frame, pl, 0) != 0) {
        Pl_se_req2(pl, se, 0, pl->pos, 1, 0);
    }
}

void wall_sd_req(f32 frame, PLW *pl, int base) {
    s16 mat;
    int i;
    PL_WALL *w;
    PL_WALL *p;

    mat = 0;
    if ((Pl_stg_ck(pl) & 0xFF) && frame_check(frame, pl, 0) != 0) {
        if (pl->work74C & 0xE0000007) {
            w = pl_wall_mat[pl->id];
            p = w;
            for (i = 0; i < 20; i++) {
                if (p->_08[0] != 0 && mat == 0) {
                    mat = w[i]._08[0];
                }
                p++;
            }
        }
        se_req2(7, base * 2 + ((u16)ran_suu(1) & 1), mat, pl->pos, 1, 0);
    }
}

static void ashi_sd_req_0024A510(f32 frame, PLW *pl, int base) {
    if ((Pl_stg_ck(pl) & 0xFF) && frame_check(frame, pl, 0) != 0) {
        se_req2(7, base * 2 + ((u16)ran_suu(1) & 1), PU8(pl, 0x70D), pl->pos, 1, 0);
    }
}

void ashi_eft_req(f32 frame, PLW *pl, int p, s16 kind) {
    if (frame_check(frame, pl, 0) != 0) {
        switch (kind) {
        case 0:
            eft13_set(pl, p, 0);
            break;
        case 1:
            eft13_set(pl, p, 3);
            break;
        case 2:
            eft13_set(pl, 5, 4);
            break;
        case 3:
            eft13_set(pl, p, 4);
            break;
        case 4:
            eft13_set(pl, p, 5);
            break;
        case 5:
            eft13_set(pl, p, 6);
            break;
        case 6:
            eft13_set(pl, p, 7);
            break;
        case 7:
            eft13_set(pl, p, 8);
            break;
        case 8:
            eft13_set(pl, p, 20);
            break;
        }
    }
}

void yoroi_sd_req(f32 frame, PLW *pl, int idx) {
    if ((Pl_stg_ck(pl) & 0xFF) && frame_check(frame, pl, 0) != 0) {
        armor_sd_req(pl, idx);
    }
}

static void move_default_0024A750(PLW *pl) {
    parts_chg(pl, 0x12, 0);
    parts_chg(pl, 0xE, 0);
}

/* Per-motion sound and effect script of player kind 1 (pl01_effect_move). pl is the player, w the effect work
 * (PL01EX at pl+0x444): w->anim is the current motion (pl->char0), w->tmr a cool-down for the skill effects
 * of other players. First the common motions (footsteps ashi_sd_req/ashi_eft_req, armor rattle yoroi_sd_req,
 * sound_call* one-shot sounds at a frame), then a switch on the weapon kind (pl->kind) with the weapon
 * specific motions 1002..1427. Generated from the disassembly (see docs/agents/agent-E.md); the Code_Make
 * calls show `Code_Make(X, n, Y, m)`: with probability n/8 sound X, else Y with m/8, else none; where the
 * original leaves a register argument unset the value is written as STALE. */
typedef struct PL01EX {
    u8 _00;
    u8 step;            /* 0x01 */
    u8 _02[4];
    s16 anim;           /* 0x06 motion number of the last frame */
    s8 x08;             /* 0x08 */
    u8 _09[9];
    u16 tmr;            /* 0x12 */
} PL01EX;

void ef_move_sub_0024A790(PLW *pl, PL01EX *w)
{
    int i;
    int j;
    PLW *p;
    f32 v[3];
    f32 v2[3];

    if (pl->char0 != w->anim) {
        w->x08 = 0;
        w->anim = pl->char0;
    }
    if (w->tmr != 0) {
        w->tmr--;
    }
    if (Pl_master_ck(pl) != 1 && PU8(pl, 0x14) != 3 && GW8(0xD3) > 0) {
        p = player_work;
        for (i = 0; i < GW8(0xD3); i++, p++) {
            if ((s16)i == pl->id || PU8(p, 0x917) == 0 || PU8(p, 0x14) != 0) {
                continue;
            }
            switch (PU8(p, 0x15)) {
            case 99:
                if (w->tmr == 0) {
                    w->tmr = 60;
                    switch (PU16(p, 0x88A)) {
                    case 138:
                        Eft06_set(4.0f, pl, 0, 0, 10);
                        break;
                    case 139:
                        Eft06_set(4.0f, pl, 0, 1, 10);
                        break;
                    case 140:
                        Eft06_set(4.0f, pl, 0, 2, 10);
                        break;
                    case 141:
                        Eft06_set(4.0f, pl, 0, 3, 10);
                        break;
                    }
                }
                break;
            case 113:
                switch (PU16(p, 0x88A)) {
                case 154:
                    if (w->tmr == 0) {
                        w->tmr = 60;
                        Eft06_set(4.0f, pl, 0, 0, 10);
                    }
                    break;
                case 1:
                    if (Pl_Skill_ck(p, 28) == 1 && w->tmr == 0) {
                        w->tmr = 60;
                        Eft06_set(4.0f, pl, 0, 0, 10);
                    }
                    break;
                case 5:
                    if (Pl_Skill_ck(p, 29) == 1 && w->tmr == 0) {
                        w->tmr = 60;
                        Eft06_set(4.0f, pl, 0, 1, 10);
                    }
                    break;
                case 83:
                    if (Pl_Skill_ck(p, 30) == 1 && w->tmr == 0) {
                        w->tmr = 60;
                        Eft06_set(4.0f, pl, 0, 2, 10);
                    }
                    break;
                case 84:
                    if (Pl_Skill_ck(p, 31) == 1 && w->tmr == 0) {
                        w->tmr = 60;
                        Eft06_set(4.0f, pl, 0, 3, 10);
                    }
                    break;
                }
                break;
            }
        }
    }
    switch (w->anim) {
    case 2:
        ashi_sd_req_0024A510(20.0f, pl, 0);
        ashi_eft_req(20.0f, pl, 8, 8);
        ashi_sd_req_0024A510(54.0f, pl, 0);
        ashi_eft_req(54.0f, pl, 5, 8);
        ashi_sd_req_0024A510(88.0f, pl, 0);
        ashi_eft_req(88.0f, pl, 8, 8);
        yoroi_sd_req(22.0f, pl, 0);
        yoroi_sd_req(56.0f, pl, 0);
        yoroi_sd_req(90.0f, pl, 0);
        break;
    case 3:
        sound_call2(pl, 56, Code_Make(39, 1, 40, 1));
        ashi_sd_req_0024A510(8.0f, pl, 2);
        ashi_eft_req(12.0f, pl, 8, 0);
        ashi_eft_req(16.0f, pl, 5, 0);
        ashi_sd_req_0024A510(30.0f, pl, 2);
        ashi_eft_req(32.0f, pl, 8, 0);
        ashi_sd_req_0024A510(54.0f, pl, 2);
        ashi_eft_req(56.0f, pl, 5, 0);
        yoroi_sd_req(32.0f, pl, 0);
        yoroi_sd_req(54.0f, pl, 0);
        break;
    case 4:
        ashi_sd_req_0024A510(6.0f, pl, 2);
        ashi_sd_req_0024A510(26.0f, pl, 2);
        yoroi_sd_req(10.0f, pl, 0);
        yoroi_sd_req(30.0f, pl, 0);
        ashi_sd_req_0024A510(46.0f, pl, 2);
        ashi_sd_req_0024A510(66.0f, pl, 2);
        yoroi_sd_req(50.0f, pl, 0);
        ashi_eft_req(12.0f, pl, 5, 1);
        ashi_eft_req(30.0f, pl, 8, 1);
        ashi_eft_req(50.0f, pl, 5, 1);
        break;
    case 5:
        if (frame_check(2.0f, pl, 0)) {
            Eft13_set_scl(pl, 8, 3, 0.699999988079071f);
        }
        if (frame_check(20.0f, pl, 0)) {
            Eft13_set_scl(pl, 5, 3, 0.699999988079071f);
        }
        ashi_eft_req(30.0f, pl, 8, 3);
        ashi_sd_req_0024A510(4.0f, pl, 2);
        ashi_sd_req_0024A510(18.0f, pl, 1);
        ashi_sd_req_0024A510(42.0f, pl, 0);
        yoroi_sd_req(42.0f, pl, 0);
        sound_call2(pl, 80, Code_Make(41, 3, 41, 2));
        sound_call2(pl, 164, Code_Make(41, 3, 41, 2));
        break;
    case 6:
        ashi_sd_req_0024A510(34.0f, pl, 0);
        yoroi_sd_req(34.0f, pl, 0);
        break;
    case 7:
    case 9:
        sound_call_0024A2A0(pl, 2, 74);
        yoroi_sd_req(4.0f, pl, 0);
        yoroi_sd_req(4.0f, pl, 4);
        break;
    case 10:
        yoroi_sd_req(14.0f, pl, 4);
        ashi_sd_req_0024A510(30.0f, pl, 1);
        sound_call_0024A2A0(pl, 38, 68);
        yoroi_sd_req(84.0f, pl, 4);
        ashi_sd_req_0024A510(100.0f, pl, 1);
        sound_call_0024A2A0(pl, 108, 68);
        break;
    case 12:
        sound_call2(pl, 2, Code_Make(STALE, 2, STALE, 2));
        ashi_sd_req_0024A510(2.0f, pl, 3);
        yoroi_sd_req(4.0f, pl, 0);
        break;
    case 13:
        ashi_eft_req(2.0f, pl, 10, 6);
        ashi_sd_req_0024A510(2.0f, pl, 3);
        yoroi_sd_req(4.0f, pl, 0);
        sound_call_0024A2A0(pl, 4, 58);
        break;
    case 14:
        ashi_sd_req_0024A510(4.0f, pl, 3);
        yoroi_sd_req(4.0f, pl, 0);
        sound_call_0024A2A0(pl, 4, 74);
        break;
    case 15:
        ashi_sd_req_0024A510(4.0f, pl, 0);
        ashi_sd_req_0024A510(10.0f, pl, 3);
        yoroi_sd_req(10.0f, pl, 0);
        ashi_eft_req(2.0f, pl, 8, 1);
        break;
    case 16:
        ashi_sd_req_0024A510(4.0f, pl, 3);
        yoroi_sd_req(8.0f, pl, 0);
        break;
    case 17:
        yoroi_sd_req(2.0f, pl, 4);
        sound_call_0024A2A0(pl, 2, 75);
        break;
    case 18:
        sound_call2(pl, 2, Code_Make(STALE, 3, STALE, 3));
        yoroi_sd_req(12.0f, pl, 4);
        yoroi_sd_req(56.0f, pl, 4);
        ashi_sd_req_0024A510(88.0f, pl, 3);
        yoroi_sd_req(88.0f, pl, 0);
        ashi_sd_req_0024A510(112.0f, pl, 0);
        sound_call_0024A2A0(pl, 94, 68);
        break;
    case 19:
        ashi_eft_req(2.0f, pl, 8, 7);
        sound_call_0024A2A0(pl, 2, 64);
        yoroi_sd_req(2.0f, pl, 0);
        yoroi_sd_req(20.0f, pl, 4);
        ashi_sd_req_0024A510(100.0f, pl, 2);
        yoroi_sd_req(100.0f, pl, 0);
        break;
    case 20:
        ashi_eft_req(2.0f, pl, 8, 7);
        sound_call_0024A2A0(pl, 2, 66);
        yoroi_sd_req(20.0f, pl, 3);
        ashi_sd_req_0024A510(68.0f, pl, 1);
        yoroi_sd_req(74.0f, pl, 4);
        ashi_sd_req_0024A510(96.0f, pl, 2);
        yoroi_sd_req(96.0f, pl, 0);
        break;
    case 21:
        ashi_sd_req_0024A510(34.0f, pl, 0);
        yoroi_sd_req(40.0f, pl, 0);
        ashi_sd_req_0024A510(76.0f, pl, 0);
        yoroi_sd_req(76.0f, pl, 0);
        break;
    case 22:
        ashi_sd_req_0024A510(6.0f, pl, 3);
        ashi_sd_req_0024A510(14.0f, pl, 3);
        yoroi_sd_req(14.0f, pl, 4);
        break;
    case 28:
        sound_call2(pl, 2, Code_Make(STALE, 4, STALE, 4));
        ashi_sd_req_0024A510(2.0f, pl, 3);
        sound_call_0024A2A0(pl, 2, 63);
        yoroi_sd_req(20.0f, pl, 3);
        sound_call_0024A2A0(pl, 28, 65);
        ashi_sd_req_0024A510(46.0f, pl, 0);
        yoroi_sd_req(52.0f, pl, 0);
        if (frame_check(22.0f, pl, 0)) {
            eft13_set(pl, 10, 11);
        }
        ashi_eft_req(4.0f, pl, 10, 0);
        break;
    case 29:
        ashi_sd_req_0024A510(20.0f, pl, 0);
        yoroi_sd_req(20.0f, pl, 0);
        ashi_sd_req_0024A510(48.0f, pl, 0);
        yoroi_sd_req(48.0f, pl, 0);
        ashi_sd_req_0024A510(82.0f, pl, 0);
        yoroi_sd_req(82.0f, pl, 0);
        ashi_sd_req_0024A510(116.0f, pl, 0);
        yoroi_sd_req(116.0f, pl, 0);
        ashi_sd_req_0024A510(148.0f, pl, 0);
        yoroi_sd_req(148.0f, pl, 0);
        ashi_sd_req_0024A510(184.0f, pl, 0);
        yoroi_sd_req(184.0f, pl, 0);
        break;
    case 30:
        ashi_eft_req(4.0f, pl, 8, 2);
        ashi_sd_req_0024A510(6.0f, pl, 3);
        yoroi_sd_req(15.0f, pl, 0);
        break;
    case 31:
        ashi_eft_req(16.0f, pl, 2, 4);
        ashi_eft_req(20.0f, pl, 2, 4);
        break;
    case 32:
        if (PS32(pl, 0x39C) % 3 == 0) {
            eft13_set(pl, 2, 5);
        }
        break;
    case 34:
        ashi_sd_req_0024A510(4.0f, pl, 0);
        yoroi_sd_req(18.0f, pl, 0);
        wall_sd_req(50.0f, pl, 0);
        yoroi_sd_req(56.0f, pl, 4);
        wall_sd_req(98.0f, pl, 0);
        yoroi_sd_req(104.0f, pl, 4);
        break;
    case 35:
        yoroi_sd_req(38.0f, pl, 0);
        wall_sd_req(38.0f, pl, 1);
        yoroi_sd_req(82.0f, pl, 0);
        wall_sd_req(82.0f, pl, 1);
        break;
    case 36:
        ashi_sd_req_0024A510(2.0f, pl, 0);
        yoroi_sd_req(2.0f, pl, 0);
        break;
    case 37:
        ashi_sd_req_0024A510(16.0f, pl, 1);
        ashi_sd_req_0024A510(36.0f, pl, 1);
        ashi_eft_req(12.0f, pl, 8, 8);
        ashi_eft_req(32.0f, pl, 5, 8);
        break;
    case 38:
        sound_call2(pl, 16, Code_Make(39, 3, 40, 2));
        ashi_sd_req_0024A510(16.0f, pl, 2);
        ashi_sd_req_0024A510(32.0f, pl, 2);
        yoroi_sd_req(16.0f, pl, 0);
        yoroi_sd_req(32.0f, pl, 0);
        if (frame_check(4.0f, pl, 0)) {
            Eft13_set_scl(pl, 8, 0, 1.5f);
        }
        if (frame_check(20.0f, pl, 0)) {
            Eft13_set_scl(pl, 5, 0, 1.5f);
        }
        break;
    case 39:
        sound_call2(pl, STALE, STALE);
        ashi_sd_req_0024A510(18.0f, pl, 2);
        ashi_sd_req_0024A510(48.0f, pl, 2);
        ashi_sd_req_0024A510(72.0f, pl, 2);
        ashi_sd_req_0024A510(100.0f, pl, 2);
        yoroi_sd_req(48.0f, pl, 0);
        yoroi_sd_req(100.0f, pl, 0);
        if (frame_check(2.0f, pl, 0)) {
            Eft13_set_scl(pl, 5, 3, 0.800000011920929f);
        }
        if (frame_check(26.0f, pl, 0)) {
            Eft13_set_scl(pl, 8, 3, 0.800000011920929f);
        }
        if (frame_check(54.0f, pl, 0)) {
            Eft13_set_scl(pl, 5, 3, 0.800000011920929f);
        }
        if (frame_check(78.0f, pl, 0)) {
            Eft13_set_scl(pl, 8, 3, 0.800000011920929f);
        }
        break;
    case 41:
        ashi_sd_req_0024A510(10.0f, pl, 0);
        yoroi_sd_req(6.0f, pl, 4);
        break;
    case 42:
        ashi_sd_req_0024A510(20.0f, pl, 0);
        yoroi_sd_req(24.0f, pl, 0);
        ashi_eft_req(26.0f, pl, 8, 1);
        ashi_eft_req(18.0f, pl, 8, 1);
        ashi_eft_req(46.0f, pl, 8, 1);
        break;
    case 43:
        ashi_sd_req_0024A510(20.0f, pl, 0);
        break;
    case 44:
    case 45:
        if (w->anim == 44) {
            ashi_eft_req(30.0f, pl, 5, 3);
            ashi_sd_req_0024A510(22.0f, pl, 0);
            sound_call_0024A2A0(pl, 30, 69);
        } else {
            ashi_eft_req(30.0f, pl, 8, 3);
            ashi_sd_req_0024A510(22.0f, pl, 0);
            sound_call_0024A2A0(pl, 30, 69);
        }
        if (frame_check(30.0f, pl, 0) != 0 && ((u16)ran_suu(1) & 3) == 0) {
            func_555020(pl, 0);
        }
        break;
    case 46:
        ashi_sd_req_0024A510(4.0f, pl, 2);
        yoroi_sd_req(4.0f, pl, 0);
        ashi_eft_req(10.0f, pl, 5, 1);
        break;
    case 48:
        ashi_sd_req_0024A510(16.0f, pl, 3);
        sound_call_0024A2A0(pl, 4, 66);
        yoroi_sd_req(4.0f, pl, 3);
        sound_call_0024A2A0(pl, 60, 75);
        ashi_sd_req_0024A510(80.0f, pl, 1);
        ashi_sd_req_0024A510(100.0f, pl, 0);
        yoroi_sd_req(100.0f, pl, 0);
        if (frame_check2(16.0f, pl, 0) == 0 && (GW16(0x1E) & 1) != 0) {
            eft13_set(pl, 10, 10);
        }
        break;
    case 49:
        ashi_sd_req_0024A510(40.0f, pl, 1);
        ashi_sd_req_0024A510(102.0f, pl, 1);
        break;
    case 50:
        sound_call2(pl, 26, Code_Make(35, 2, 36, 2));
        ashi_sd_req_0024A510(20.0f, pl, 3);
        sound_call_0024A2A0(pl, 20, 62);
        yoroi_sd_req(32.0f, pl, 0);
        break;
    case 52:
        ashi_sd_req_0024A510(18.0f, pl, 0);
        yoroi_sd_req(18.0f, pl, 0);
        ashi_sd_req_0024A510(48.0f, pl, 0);
        yoroi_sd_req(48.0f, pl, 0);
        ashi_eft_req(16.0f, pl, 8, 8);
        ashi_eft_req(46.0f, pl, 5, 8);
        break;
    case 53:
        sound_call2(pl, 4, Code_Make(39, 2, 40, 2));
        ashi_sd_req_0024A510(20.0f, pl, 2);
        yoroi_sd_req(20.0f, pl, 0);
        ashi_sd_req_0024A510(42.0f, pl, 2);
        yoroi_sd_req(42.0f, pl, 0);
        ashi_eft_req(28.0f, pl, 8, 1);
        ashi_eft_req(8.0f, pl, 5, 1);
        break;
    case 54:
        ashi_sd_req_0024A510(32.0f, pl, 0);
        yoroi_sd_req(32.0f, pl, 0);
        ashi_sd_req_0024A510(76.0f, pl, 1);
        ashi_sd_req_0024A510(162.0f, pl, 0);
        ashi_sd_req_0024A510(192.0f, pl, 1);
        sound_call2(pl, 36, Code_Make(38, 2, 38, 2));
        break;
    case 55:
        yoroi_sd_req(8.0f, pl, 4);
        ashi_sd_req_0024A510(16.0f, pl, 0);
        yoroi_sd_req(16.0f, pl, 0);
        ashi_sd_req_0024A510(32.0f, pl, 1);
        break;
    case 57:
        sound_call_0024A2A0(pl, 4, STALE);
        ashi_sd_req_0024A510(88.0f, pl, 0);
        yoroi_sd_req(88.0f, pl, 0);
        ashi_eft_req(4.0f, pl, 2, 6);
        break;
    case 58:
        ashi_sd_req_0024A510(6.0f, pl, 3);
        yoroi_sd_req(10.0f, pl, 0);
        if (frame_check(6.0f, pl, 0)) {
            Eft13_set_scl(pl, 2, 7, 0.699999988079071f);
        }
        break;
    case 59:
        sound_call2(pl, 4, Code_Make(39, 2, 40, 2));
        ashi_sd_req_0024A510(10.0f, pl, 2);
        yoroi_sd_req(10.0f, pl, Code_Make(0, 2, 0, 2));
        ashi_sd_req_0024A510(24.0f, pl, 2);
        yoroi_sd_req(24.0f, pl, Code_Make(0, 2, 0, 2));
        ashi_eft_req(16.0f, pl, 8, 1);
        ashi_eft_req(2.0f, pl, 5, 1);
        break;
    case 60:
        yoroi_sd_req(20.0f, pl, 4);
        if (frame_check2(18.0f, pl, 0) != 0 && (GW16(0x1E) & 0x1F) == 0) {
            Eft13_set_scl(pl, 14, 26, 0.4000000059604645f);
        }
        break;
    case 62:
        yoroi_sd_req(4.0f, pl, 0);
        ashi_sd_req_0024A510(4.0f, pl, 3);
        ashi_sd_req_0024A510(10.0f, pl, 2);
        if (frame_check(2.0f, pl, 0)) {
            Eft13_set_scl(pl, 10, 7, 0.6000000238418579f);
        }
        break;
    case 63:
        yoroi_sd_req(4.0f, pl, 0);
        ashi_sd_req_0024A510(70.0f, pl, 1);
        ashi_sd_req_0024A510(100.0f, pl, 1);
        break;
    case 64:
        ashi_sd_req_0024A510(26.0f, pl, 0);
        yoroi_sd_req(26.0f, pl, 0);
        ashi_sd_req_0024A510(60.0f, pl, 0);
        yoroi_sd_req(60.0f, pl, 0);
        break;
    case 202:
        ashi_sd_req_0024A510(12.0f, pl, 0);
        yoroi_sd_req(12.0f, pl, 0);
        sound_call_0024A2A0(pl, 26, 64);
        yoroi_sd_req(12.0f, pl, 3);
        if (frame_check(24.0f, pl, 0)) {
            Eft13_set_scl(pl, 2, 7, 0.5f);
        }
        break;
    case 204:
        sound_call_0024A2A0(pl, 14, 64);
        yoroi_sd_req(14.0f, pl, 0);
        ashi_sd_req_0024A510(26.0f, pl, 0);
        yoroi_sd_req(26.0f, pl, 4);
        if (frame_check(16.0f, pl, 0)) {
            Eft13_set_scl(pl, 2, 7, 0.800000011920929f);
        }
        break;
    case 206:
        sound_call_0024A2A0(pl, 4, 60);
        yoroi_sd_req(4.0f, pl, 3);
        sound_call_0024A2A0(pl, 32, 66);
        sound_call_0024A2A0(pl, 116, 58);
        yoroi_sd_req(54.0f, pl, 4);
        yoroi_sd_req(84.0f, pl, 0);
        yoroi_sd_req(116.0f, pl, 4);
        if (frame_check(6.0f, pl, 0)) {
            Eft13_set_scl(pl, 10, 7, 1.5f);
        }
        if (frame_check(34.0f, pl, 0)) {
            Eft13_set_scl(pl, 10, 17, 1.5f);
        }
        if (frame_check(50.0f, pl, 0) != 0 || frame_check(54.0f, pl, 0) != 0 || frame_check(58.0f, pl, 0) != 0) {
            Eft13_set_scl(pl, 8, 18, 1.0f);
        }
        break;
    case 207:
        yoroi_sd_req(10.0f, pl, 4);
        ashi_sd_req_0024A510(30.0f, pl, 1);
        yoroi_sd_req(30.0f, pl, 0);
        ashi_sd_req_0024A510(74.0f, pl, 2);
        yoroi_sd_req(74.0f, pl, 0);
        break;
    case 209:
        sound_call_0024A2A0(pl, 4, 60);
        yoroi_sd_req(4.0f, pl, 3);
        sound_call_0024A2A0(pl, 32, 66);
        sound_call_0024A2A0(pl, 116, 58);
        yoroi_sd_req(54.0f, pl, 4);
        yoroi_sd_req(84.0f, pl, 0);
        yoroi_sd_req(116.0f, pl, 4);
        if (frame_check(6.0f, pl, 0)) {
            Eft13_set_scl(pl, 20, 7, 1.5f);
        }
        if (frame_check(28.0f, pl, 0)) {
            Eft13_set_scl(pl, 10, 7, 1.5f);
        }
        if (frame_check(42.0f, pl, 0) != 0 || frame_check(46.0f, pl, 0) != 0 || frame_check(50.0f, pl, 0) != 0) {
            Eft13_set_scl(pl, 8, 18, 1.0f);
        }
        break;
    case 210:
        sound_call_0024A2A0(pl, 66, 58);
        yoroi_sd_req(66.0f, pl, 0);
        sound_call_0024A2A0(pl, 146, 75);
        yoroi_sd_req(146.0f, pl, 4);
        sound_call_0024A2A0(pl, 150, 60);
        if (frame_check(152.0f, pl, 0)) {
            Eft13_set_scl(pl, 2, 7, 1.0f);
        }
        break;
    case 211:
        sound_call_0024A2A0(pl, 12, 74);
        sound_call_0024A2A0(pl, 20, 75);
        yoroi_sd_req(16.0f, pl, 4);
        yoroi_sd_req(28.0f, pl, 0);
        ashi_sd_req_0024A510(26.0f, pl, 0);
        break;
    case 212:
        yoroi_sd_req(32.0f, pl, 4);
        yoroi_sd_req(92.0f, pl, 0);
        ashi_sd_req_0024A510(94.0f, pl, 1);
        ashi_sd_req_0024A510(114.0f, pl, 1);
        break;
    case 214:
        yoroi_sd_req(66.0f, pl, 0);
        yoroi_sd_req(16.0f, pl, 0);
        break;
    case 215:
        sound_call_0024A2A0(pl, 4, 60);
        yoroi_sd_req(4.0f, pl, 3);
        sound_call_0024A2A0(pl, 106, 64);
        yoroi_sd_req(110.0f, pl, 4);
        ashi_eft_req(2.0f, pl, 19, 5);
        break;
    case 216:
        sound_call_0024A2A0(pl, 14, 68);
        yoroi_sd_req(4.0f, pl, 4);
        ashi_sd_req_0024A510(44.0f, pl, 1);
        ashi_sd_req_0024A510(64.0f, pl, 3);
        yoroi_sd_req(74.0f, pl, 0);
        break;
    case 218:
        sound_call_0024A2A0(pl, 10, 74);
        yoroi_sd_req(10.0f, pl, 4);
        break;
    case 220:
        yoroi_sd_req(18.0f, pl, 0);
        break;
    case 221:
        yoroi_sd_req(56.0f, pl, 4);
        yoroi_sd_req(22.0f, pl, 0);
        break;
    case 222:
        sound_call_0024A2A0(pl, 38, 66);
        yoroi_sd_req(46.0f, pl, 0);
        yoroi_sd_req(52.0f, pl, 4);
        if (frame_check(40.0f, pl, 0)) {
            Eft13_set_scl(pl, 2, 7, 1.0f);
        }
        if (frame_check(46.0f, pl, 0)) {
            Eft13_set_scl(pl, 2, 20, 1.2000000476837158f);
        }
        break;
    case 224:
        ashi_sd_req_0024A510(20.0f, pl, 0);
        yoroi_sd_req(20.0f, pl, 0);
        sound_call2(pl, 64, 41);
        sound_call_0024A2A0(pl, 64, 59);
        break;
    case 225:
        yoroi_sd_req(54.0f, pl, 4);
        yoroi_sd_req(120.0f, pl, 0);
        sound_call2(pl, 64, 41);
        sound_call_0024A2A0(pl, 118, 59);
        sound_call_0024A2A0(pl, 170, 65);
        break;
    case 401:
        yoroi_sd_req(20.0f, pl, 4);
        ashi_sd_req_0024A510(16.0f, pl, 3);
        sound_call_0024A2A0(pl, 20, 62);
        yoroi_sd_req(40.0f, pl, 0);
        break;
    case 402:
        yoroi_sd_req(48.0f, pl, 4);
        yoroi_sd_req(84.0f, pl, 4);
        ashi_sd_req_0024A510(46.0f, pl, 1);
        sound_call_0024A2A0(pl, 122, 75);
        sound_call_0024A2A0(pl, 142, 75);
        yoroi_sd_req(142.0f, pl, 4);
        break;
    case 403:
        ashi_sd_req_0024A510(22.0f, pl, 1);
        yoroi_sd_req(22.0f, pl, 0);
        sound_call_0024A2A0(pl, 28, Code_Make(48, 3, 48, 2));
        sound_call_0024A2A0(pl, 226, Code_Make(48, 2, 48, 2));
        break;
    case 405:
        ashi_sd_req_0024A510(16.0f, pl, 0);
        yoroi_sd_req(40.0f, pl, 4);
        sound_call_0024A2A0(pl, 70, 76);
        yoroi_sd_req(72.0f, pl, 4);
        sound_call_0024A2A0(pl, 116, 76);
        yoroi_sd_req(120.0f, pl, 4);
        sound_call_0024A2A0(pl, 120, 76);
        sound_call_0024A2A0(pl, 146, 76);
        yoroi_sd_req(192.0f, pl, 4);
        sound_call_0024A2A0(pl, 188, 76);
        ashi_sd_req_0024A510(230.0f, pl, 0);
        yoroi_sd_req(230.0f, pl, 0);
        break;
    case 406:
        sound_call_0024A2A0(pl, 36, 73);
        break;
    case 407:
        yoroi_sd_req(52.0f, pl, 4);
        sound_call_0024A2A0(pl, 38, 74);
        yoroi_sd_req(50.0f, pl, 0);
        break;
    case 408:
        sound_call2(pl, 18, Code_Make(42, 5, 43, 2));
        sound_call2(pl, 38, Code_Make(42, 2, 43, 4));
        sound_call2(pl, 66, Code_Make(42, 4, 43, 2));
        sound_call2(pl, 84, Code_Make(42, 2, 43, 5));
        break;
    case 409:
        yoroi_sd_req(10.0f, pl, 4);
        sound_call_0024A2A0(pl, 20, 75);
        break;
    case 410:
        sound_call_0024A2A0(pl, 42, 75);
        sound_call_0024A2A0(pl, 78, 75);
        break;
    case 413:
        ashi_sd_req_0024A510(50.0f, pl, 0);
        yoroi_sd_req(52.0f, pl, 0);
        sound_call2(pl, 30, 10);
        sound_call2(pl, 324, 11);
        sound_call_0024A2A0(pl, 100, 72);
        yoroi_sd_req(100.0f, pl, 4);
        sound_call_0024A2A0(pl, 146, 72);
        sound_call_0024A2A0(pl, 192, 72);
        yoroi_sd_req(192.0f, pl, 4);
        sound_call_0024A2A0(pl, 240, 72);
        ashi_sd_req_0024A510(346.0f, pl, 0);
        sound_call_0024A2A0(pl, 274, 54);
        if (frame_check(274.0f, pl, 0)) {
            Eft13_set_scl(pl, 14, 21, 1.0f);
        }
        break;
    case 414:
        ashi_sd_req_0024A510(22.0f, pl, 0);
        yoroi_sd_req(22.0f, pl, 0);
        sound_call_0024A2A0(pl, 62, 74);
        yoroi_sd_req(46.0f, pl, 4);
        break;
    case 415:
        sound_call_0024A2A0(pl, 8, 61);
        sound_call_0024A2A0(pl, 26, 63);
        sound_call_0024A2A0(pl, 42, 47);
        ashi_sd_req_0024A510(8.0f, pl, 0);
        yoroi_sd_req(22.0f, pl, 0);
        ashi_sd_req_0024A510(38.0f, pl, 3);
        yoroi_sd_req(38.0f, pl, 0);
        ashi_sd_req_0024A510(78.0f, pl, 0);
        yoroi_sd_req(78.0f, pl, 0);
        ashi_sd_req_0024A510(92.0f, pl, 0);
        yoroi_sd_req(92.0f, pl, 0);
        ashi_sd_req_0024A510(136.0f, pl, 0);
        yoroi_sd_req(138.0f, pl, 0);
        ashi_sd_req_0024A510(162.0f, pl, 1);
        if (frame_check(50.0f, pl, 0) != 0 && (Pl_stg_ck(pl) & 0xFF) == 1) {
            SetVector(v, 0.0f, 0.0f, 120.0f);
            flvecApplyMat33(v2, v, (f32 *)((u8 *)pl + 0x60));
            v[0] = pl->pos[0] + v2[0];
            v[1] = pl->pos[1] + v2[1];
            v[2] = pl->pos[2] + v2[2];
            Eft02_set_pos(v, 7, PU16(pl, 0xA4));
        }
        break;
    case 416:
        sound_call_0024A2A0(pl, 2, 74);
        ashi_sd_req_0024A510(12.0f, pl, 0);
        sound_call_0024A2A0(pl, 50, 55);
        sound_call_0024A2A0(pl, 68, 55);
        sound_call_0024A2A0(pl, 78, 55);
        sound_call_0024A2A0(pl, 94, 55);
        sound_call_0024A2A0(pl, 110, 55);
        sound_call_0024A2A0(pl, 132, 55);
        sound_call_0024A2A0(pl, 146, 55);
        sound_call_0024A2A0(pl, 152, 55);
        sound_call_0024A2A0(pl, 172, 73);
        break;
    case 417:
        sound_call_0024A2A0(pl, 12, 62);
        yoroi_sd_req(16.0f, pl, 4);
        ashi_sd_req_0024A510(12.0f, pl, 0);
        yoroi_sd_req(70.0f, pl, 0);
        ashi_sd_req_0024A510(70.0f, pl, 0);
        break;
    case 418:
        sound_call_0024A2A0(pl, 32, 42);
        yoroi_sd_req(14.0f, pl, 4);
        sound_call_0024A2A0(pl, 104, 76);
        ashi_sd_req_0024A510(28.0f, pl, 0);
        yoroi_sd_req(104.0f, pl, 4);
        yoroi_sd_req(194.0f, pl, 0);
        ashi_sd_req_0024A510(194.0f, pl, 0);
        if (frame_check(36.0f, pl, 0)) {
            Eft13_set_scl(pl, 18, 24, 0.30000001192092896f);
        }
        break;
    case 419:
        sound_call_0024A2A0(pl, 30, 50);
        sound_call_0024A2A0(pl, 64, 51);
        sound_call_0024A2A0(pl, 90, 53);
        sound_call_0024A2A0(pl, 172, 53);
        sound_call_0024A2A0(pl, 228, 51);
        sound_call_0024A2A0(pl, 228, 50);
        sound_call_0024A2A0(pl, 286, 75);
        ashi_sd_req_0024A510(24.0f, pl, 0);
        ashi_sd_req_0024A510(58.0f, pl, 0);
        yoroi_sd_req(58.0f, pl, 0);
        ashi_sd_req_0024A510(316.0f, pl, 0);
        yoroi_sd_req(318.0f, pl, 0);
        ashi_sd_req_0024A510(336.0f, pl, 0);
        break;
    case 420:
        sound_call_0024A2A0(pl, 6, 74);
        sound_call_h(pl, 70, 119);
        yoroi_sd_req(118.0f, pl, 4);
        ashi_sd_req_0024A510(24.0f, pl, 0);
        yoroi_sd_req(24.0f, pl, 0);
        break;
    case 421:
        ashi_sd_req_0024A510(26.0f, pl, 0);
        yoroi_sd_req(26.0f, pl, 0);
        break;
    case 422:
        ashi_sd_req_0024A510(14.0f, pl, 0);
        yoroi_sd_req(18.0f, pl, 0);
        ashi_sd_req_0024A510(48.0f, pl, 0);
        yoroi_sd_req(48.0f, pl, 0);
        sound_call_0024A2A0(pl, 28, 50);
        sound_call_0024A2A0(pl, 58, 51);
        break;
    case 423:
        ashi_sd_req_0024A510(18.0f, pl, 0);
        yoroi_sd_req(18.0f, pl, 0);
        ashi_sd_req_0024A510(30.0f, pl, 0);
        yoroi_sd_req(30.0f, pl, 0);
        break;
    case 424:
        ashi_sd_req_0024A510(24.0f, pl, 1);
        yoroi_sd_req(24.0f, pl, 0);
        yoroi_sd_req(30.0f, pl, 4);
        sound_call_h(pl, 72, 120);
        break;
    case 425:
        sound_call_0024A2A0(pl, 58, 74);
        ashi_sd_req_0024A510(48.0f, pl, 1);
        yoroi_sd_req(48.0f, pl, 0);
        ashi_sd_req_0024A510(78.0f, pl, 0);
        yoroi_sd_req(78.0f, pl, 0);
        break;
    case 426:
        sound_call_0024A2A0(pl, 168, 61);
        sound_call_0024A2A0(pl, 204, 59);
        yoroi_sd_req(210.0f, pl, 3);
        yoroi_sd_req(234.0f, pl, 0);
        yoroi_sd_req(324.0f, pl, 4);
        ashi_sd_req_0024A510(46.0f, pl, 0);
        ashi_sd_req_0024A510(74.0f, pl, 0);
        ashi_sd_req_0024A510(166.0f, pl, 3);
        ashi_sd_req_0024A510(172.0f, pl, 3);
        if (frame_check(190.0f, pl, 0)) {
            Eft13_set_scl(pl, 2, 23, 0.6000000238418579f);
        }
        break;
    case 428:
        yoroi_sd_req(30.0f, pl, 4);
        break;
    case 429:
        sound_call_0024A2A0(pl, 30, 67);
        yoroi_sd_req(56.0f, pl, 4);
        yoroi_sd_req(122.0f, pl, 4);
        sound_call_0024A2A0(pl, 192, 75);
        sound_call_0024A2A0(pl, 282, 69);
        yoroi_sd_req(294.0f, pl, 0);
        sound_call_0024A2A0(pl, 338, 69);
        yoroi_sd_req(350.0f, pl, 0);
        yoroi_sd_req(408.0f, pl, 0);
        ashi_sd_req_0024A510(408.0f, pl, 0);
        ashi_sd_req_0024A510(470.0f, pl, 0);
        yoroi_sd_req(470.0f, pl, 0);
        break;
    case 431:
        ashi_sd_req_0024A510(26.0f, pl, 1);
        yoroi_sd_req(26.0f, pl, 0);
        if ((GW16(0x1E) & 0x1F) == 0) {
            Eft13_set_scl(pl, 20, 22, 1.0f);
        }
        break;
    case 432:
        ashi_sd_req_0024A510(8.0f, pl, 0);
        yoroi_sd_req(8.0f, pl, 0);
        sound_call2(pl, 30, 38);
        sound_call_0024A2A0(pl, 78, 60);
        ashi_sd_req_0024A510(106.0f, pl, 0);
        yoroi_sd_req(46.0f, pl, 4);
        ashi_eft_req(78.0f, pl, 1, 6);
        break;
    case 600:
        sound_call_0024A2A0(pl, 4, 67);
        sound_call_0024A2A0(pl, 158, 67);
        ashi_sd_req_0024A510(16.0f, pl, 0);
        yoroi_sd_req(16.0f, pl, 0);
        ashi_sd_req_0024A510(46.0f, pl, 0);
        yoroi_sd_req(46.0f, pl, 0);
        ashi_sd_req_0024A510(74.0f, pl, 0);
        ashi_sd_req_0024A510(98.0f, pl, 1);
        ashi_sd_req_0024A510(144.0f, pl, 0);
        yoroi_sd_req(144.0f, pl, 0);
        ashi_sd_req_0024A510(178.0f, pl, 0);
        yoroi_sd_req(178.0f, pl, 0);
        break;
    case 602:
        sound_call_0024A2A0(pl, 48, 80);
        sound_call_0024A2A0(pl, 62, 80);
        break;
    case 603:
        ashi_sd_req_0024A510(16.0f, pl, 1);
        ashi_sd_req_0024A510(48.0f, pl, 1);
        sound_call_0024A2A0(pl, 34, 49);
        sound_call_0024A2A0(pl, 48, 49);
        sound_call_0024A2A0(pl, 62, 49);
        yoroi_sd_req(120.0f, pl, 0);
        break;
    case 604:
        sound_call_0024A2A0(pl, 12, 68);
        break;
    case 605:
        ashi_sd_req_0024A510(52.0f, pl, 0);
        yoroi_sd_req(52.0f, pl, 0);
        sound_call_0024A2A0(pl, 22, 74);
        break;
    case 607:
        ashi_sd_req_0024A510(34.0f, pl, 0);
        yoroi_sd_req(34.0f, pl, 0);
        break;
    case 608:
        ashi_sd_req_0024A510(34.0f, pl, 0);
        yoroi_sd_req(34.0f, pl, 0);
        ashi_sd_req_0024A510(54.0f, pl, 1);
        sound_call_0024A2A0(pl, 68, 74);
        break;
    case 610:
        ashi_sd_req_0024A510(14.0f, pl, 0);
        ashi_sd_req_0024A510(64.0f, pl, 1);
        yoroi_sd_req(64.0f, pl, 0);
        sound_call_0024A2A0(pl, 12, 68);
        break;
    case 611:
        sound_call_0024A2A0(pl, 42, 81);
        sound_call_0024A2A0(pl, 64, 81);
        sound_call_0024A2A0(pl, 108, 83);
        yoroi_sd_req(108.0f, pl, 4);
        break;
    case 612:
        sound_call_0024A2A0(pl, 124, 75);
        sound_call_0024A2A0(pl, 42, 61);
        sound_call_0024A2A0(pl, 56, 82);
        yoroi_sd_req(48.0f, pl, 4);
        sound_call_0024A2A0(pl, 128, 83);
        break;
    case 613:
        sound_call_0024A2A0(pl, 72, 58);
        yoroi_sd_req(76.0f, pl, 4);
        sound_call_0024A2A0(pl, 132, 84);
        sound_call_0024A2A0(pl, 206, 85);
        break;
    case 614:
        sound_call_0024A2A0(pl, 38, 68);
        yoroi_sd_req(62.0f, pl, 4);
        break;
    case 615:
        sound_call_0024A2A0(pl, 42, 74);
        yoroi_sd_req(70.0f, pl, 4);
        yoroi_sd_req(154.0f, pl, 4);
        yoroi_sd_req(220.0f, pl, 4);
        break;
    case 616:
        sound_call_0024A2A0(pl, 32, 67);
        yoroi_sd_req(82.0f, pl, 4);
        yoroi_sd_req(102.0f, pl, 0);
        sound_call_0024A2A0(pl, 74, 64);
        sound_call_0024A2A0(pl, 126, 84);
        sound_call_0024A2A0(pl, 246, 85);
        break;
    case 617:
        sound_call_0024A2A0(pl, 46, 67);
        ashi_sd_req_0024A510(62.0f, pl, 1);
        ashi_sd_req_0024A510(102.0f, pl, 3);
        yoroi_sd_req(104.0f, pl, 0);
        ashi_sd_req_0024A510(140.0f, pl, 3);
        yoroi_sd_req(142.0f, pl, 0);
        ashi_sd_req_0024A510(176.0f, pl, 0);
        yoroi_sd_req(176.0f, pl, 0);
        break;
    case 618:
        sound_call_0024A2A0(pl, 38, 83);
        sound_call_0024A2A0(pl, 126, 58);
        yoroi_sd_req(42.0f, pl, 4);
        ashi_sd_req_0024A510(68.0f, pl, 0);
        ashi_sd_req_0024A510(94.0f, pl, 0);
        yoroi_sd_req(96.0f, pl, 0);
        sound_call_0024A2A0(pl, 126, 61);
        yoroi_sd_req(132.0f, pl, 4);
        sound_call_0024A2A0(pl, 190, 61);
        yoroi_sd_req(192.0f, pl, 4);
        break;
    case 619:
        ashi_sd_req_0024A510(40.0f, pl, 0);
        ashi_sd_req_0024A510(54.0f, pl, 1);
        yoroi_sd_req(40.0f, pl, 0);
        break;
    case 620:
        ashi_sd_req_0024A510(16.0f, pl, 1);
        yoroi_sd_req(42.0f, pl, 0);
        ashi_sd_req_0024A510(42.0f, pl, 0);
        yoroi_sd_req(62.0f, pl, 4);
        break;
    case 621:
        ashi_sd_req_0024A510(50.0f, pl, 0);
        yoroi_sd_req(50.0f, pl, 0);
        sound_call_0024A2A0(pl, 10, 68);
        ashi_sd_req_0024A510(114.0f, pl, 0);
        break;
    case 622:
        ashi_sd_req_0024A510(22.0f, pl, 3);
        ashi_sd_req_0024A510(10.0f, pl, 3);
        yoroi_sd_req(22.0f, pl, 0);
        yoroi_sd_req(48.0f, pl, 4);
        sound_call_0024A2A0(pl, 54, 61);
        yoroi_sd_req(70.0f, pl, 0);
        ashi_sd_req_0024A510(66.0f, pl, 3);
        sound_call_0024A2A0(pl, 66, 58);
        break;
    case 623:
        ashi_sd_req_0024A510(30.0f, pl, 0);
        yoroi_sd_req(30.0f, pl, 0);
        sound_call2(pl, 46, 41);
        sound_call_0024A2A0(pl, 66, 64);
        sound_call_0024A2A0(pl, 80, 58);
        yoroi_sd_req(80.0f, pl, 4);
        break;
    case 624:
        sound_call_0024A2A0(pl, 4, 68);
        break;
    case 625:
        ashi_sd_req_0024A510(28.0f, pl, 0);
        yoroi_sd_req(28.0f, pl, 0);
        sound_call_0024A2A0(pl, 28, 58);
        ashi_sd_req_0024A510(56.0f, pl, 0);
        yoroi_sd_req(56.0f, pl, 0);
        sound_call_0024A2A0(pl, 56, 58);
        break;
    case 626:
        ashi_sd_req_0024A510(34.0f, pl, 0);
        yoroi_sd_req(34.0f, pl, 0);
        break;
    case 627:
        ashi_sd_req_0024A510(16.0f, pl, 0);
        yoroi_sd_req(16.0f, pl, 0);
        ashi_sd_req_0024A510(32.0f, pl, 1);
        sound_call_0024A2A0(pl, 30, 49);
        sound_call_0024A2A0(pl, 42, 49);
        break;
    case 628:
        ashi_sd_req_0024A510(24.0f, pl, 0);
        yoroi_sd_req(24.0f, pl, 0);
        sound_call_0024A2A0(pl, 32, 49);
        sound_call_0024A2A0(pl, 50, 49);
        sound_call_0024A2A0(pl, 68, 49);
        sound_call_0024A2A0(pl, 86, 49);
        ashi_sd_req_0024A510(120.0f, pl, 0);
        yoroi_sd_req(120.0f, pl, 0);
        ashi_sd_req_0024A510(132.0f, pl, 0);
        sound_call_0024A2A0(pl, 130, 61);
        yoroi_sd_req(130.0f, pl, 0);
        sound_call_0024A2A0(pl, 176, 61);
        yoroi_sd_req(176.0f, pl, 0);
        sound_call_0024A2A0(pl, 228, 61);
        yoroi_sd_req(228.0f, pl, 0);
        ashi_sd_req_0024A510(282.0f, pl, 0);
        sound_call_0024A2A0(pl, 300, 67);
        ashi_sd_req_0024A510(306.0f, pl, 0);
        yoroi_sd_req(228.0f, pl, 0);
        sound_call_0024A2A0(pl, 316, 58);
        yoroi_sd_req(362.0f, pl, 4);
        yoroi_sd_req(386.0f, pl, 4);
        yoroi_sd_req(436.0f, pl, 4);
        break;
    case 629:
        sound_call_0024A2A0(pl, 316, 69);
        ashi_sd_req_0024A510(26.0f, pl, 0);
        yoroi_sd_req(26.0f, pl, 0);
        break;
    case 630:
        sound_call_0024A2A0(pl, 20, 67);
        sound_call_0024A2A0(pl, 40, 61);
        yoroi_sd_req(40.0f, pl, 0);
        ashi_sd_req_0024A510(84.0f, pl, 0);
        sound_call_0024A2A0(pl, 106, 69);
        ashi_sd_req_0024A510(130.0f, pl, 3);
        yoroi_sd_req(132.0f, pl, 0);
        ashi_sd_req_0024A510(158.0f, pl, 3);
        yoroi_sd_req(158.0f, pl, 0);
        ashi_sd_req_0024A510(184.0f, pl, 3);
        yoroi_sd_req(184.0f, pl, 0);
        ashi_sd_req_0024A510(210.0f, pl, 3);
        yoroi_sd_req(210.0f, pl, 0);
        break;
    case 631:
        ashi_sd_req_0024A510(42.0f, pl, 0);
        yoroi_sd_req(42.0f, pl, 0);
        ashi_sd_req_0024A510(198.0f, pl, 0);
        yoroi_sd_req(198.0f, pl, 0);
        break;
    case 632:
        ashi_sd_req_0024A510(28.0f, pl, 0);
        yoroi_sd_req(28.0f, pl, 0);
        ashi_sd_req_0024A510(174.0f, pl, 0);
        break;
    case 633:
        ashi_sd_req_0024A510(26.0f, pl, 0);
        yoroi_sd_req(26.0f, pl, 0);
        sound_call_0024A2A0(pl, 42, 62);
        yoroi_sd_req(42.0f, pl, 4);
        break;
    case 635:
        sound_call_0024A2A0(pl, 138, 86);
        sound_call_0024A2A0(pl, 222, 53);
        sound_call_0024A2A0(pl, 234, 52);
        sound_call_0024A2A0(pl, 256, 55);
        sound_call_0024A2A0(pl, 270, 50);
        sound_call_0024A2A0(pl, 286, 53);
        sound_call_0024A2A0(pl, 310, 51);
        sound_call_0024A2A0(pl, 328, 51);
        sound_call_0024A2A0(pl, 380, 52);
        sound_call_0024A2A0(pl, 388, 52);
        sound_call_0024A2A0(pl, 412, 55);
        sound_call_0024A2A0(pl, 434, 50);
        sound_call_0024A2A0(pl, 478, 53);
        sound_call_0024A2A0(pl, 504, 51);
        sound_call_0024A2A0(pl, 504, 52);
        sound_call_0024A2A0(pl, 230, 87);
        sound_call_0024A2A0(pl, 250, 87);
        sound_call_0024A2A0(pl, 290, 87);
        sound_call_0024A2A0(pl, 350, 87);
        sound_call_0024A2A0(pl, 380, 87);
        sound_call_0024A2A0(pl, 478, 87);
        sound_call_0024A2A0(pl, 412, 87);
        sound_call_0024A2A0(pl, 440, 87);
        sound_call_0024A2A0(pl, 560, 87);
        yoroi_sd_req(26.0f, pl, 0);
        ashi_sd_req_0024A510(26.0f, pl, 0);
        break;
    case 636:
        sound_call_0024A2A0(pl, 22, 74);
        sound_call_0024A2A0(pl, 22, 75);
        ashi_sd_req_0024A510(26.0f, pl, 0);
        yoroi_sd_req(26.0f, pl, 0);
        ashi_sd_req_0024A510(56.0f, pl, 2);
        yoroi_sd_req(56.0f, pl, 0);
        ashi_sd_req_0024A510(68.0f, pl, 2);
        yoroi_sd_req(68.0f, pl, 0);
        ashi_sd_req_0024A510(80.0f, pl, 2);
        yoroi_sd_req(80.0f, pl, 0);
        ashi_sd_req_0024A510(90.0f, pl, 2);
        yoroi_sd_req(90.0f, pl, 0);
        ashi_sd_req_0024A510(100.0f, pl, 2);
        yoroi_sd_req(100.0f, pl, 0);
        ashi_sd_req_0024A510(110.0f, pl, 2);
        yoroi_sd_req(110.0f, pl, 0);
        ashi_sd_req_0024A510(126.0f, pl, 2);
        yoroi_sd_req(126.0f, pl, 0);
        sound_call_0024A2A0(pl, 150, 58);
        break;
    case 637:
        ashi_sd_req_0024A510(30.0f, pl, 0);
        yoroi_sd_req(30.0f, pl, 0);
        sound_call_0024A2A0(pl, 44, 67);
        yoroi_sd_req(44.0f, pl, 4);
        sound_call_0024A2A0(pl, 60, 67);
        yoroi_sd_req(60.0f, pl, 4);
        sound_call_0024A2A0(pl, 80, 86);
        yoroi_sd_req(80.0f, pl, 4);
        ashi_sd_req_0024A510(146.0f, pl, 0);
        yoroi_sd_req(146.0f, pl, 0);
        break;
    case 640:
        yoroi_sd_req(14.0f, pl, 4);
        sound_call_0024A2A0(pl, 14, 69);
        break;
    case 642:
        yoroi_sd_req(14.0f, pl, 4);
        ashi_sd_req_0024A510(86.0f, pl, 0);
        yoroi_sd_req(86.0f, pl, 0);
        ashi_sd_req_0024A510(98.0f, pl, 1);
        yoroi_sd_req(98.0f, pl, 0);
        sound_call_0024A2A0(pl, 116, 83);
        break;
    case 643:
        sound_call_0024A2A0(pl, 24, 83);
        yoroi_sd_req(24.0f, pl, 4);
        break;
    case 647:
        if (frame_check(236.0f, pl, 0)) {
            Eft13_set_scl(pl, 0, 33, 0.699999988079071f);
        }
        if (frame_check(314.0f, pl, 0)) {
            func_60E2B0(pl, 6);
            func_60E2B0(pl, 7);
        }
        ashi_sd_req_0024A510(26.0f, pl, 1);
        sound_call_0024A2A0(pl, 78, 91);
        yoroi_sd_req(78.0f, pl, 0);
        sound_call_0024A2A0(pl, 116, 92);
        yoroi_sd_req(116.0f, pl, 0);
        sound_call_0024A2A0(pl, 192, 75);
        sound_call_0024A2A0(pl, 230, 88);
        yoroi_sd_req(244.0f, pl, 4);
        sound_call_0024A2A0(pl, 286, 89);
        yoroi_sd_req(300.0f, pl, 0);
        sound_call_0024A2A0(pl, 300, 93);
        sound_call_0024A2A0(pl, 390, 94);
        sound_call_0024A2A0(pl, 416, 61);
        sound_call_0024A2A0(pl, 566, 90);
        yoroi_sd_req(430.0f, pl, 0);
        ashi_sd_req_0024A510(430.0f, pl, 0);
        yoroi_sd_req(430.0f, pl, 0);
        yoroi_sd_req(580.0f, pl, 0);
        ashi_sd_req_0024A510(580.0f, pl, 0);
        yoroi_sd_req(608.0f, pl, 0);
        ashi_sd_req_0024A510(608.0f, pl, 0);
        sound_call_0024A2A0(pl, 282, 62);
        yoroi_sd_req(42.0f, pl, 4);
        break;
    case 648:
        ashi_sd_req_0024A510(12.0f, pl, 1);
        sound_call_0024A2A0(pl, 46, 91);
        yoroi_sd_req(46.0f, pl, 0);
        sound_call_0024A2A0(pl, 86, 92);
        yoroi_sd_req(86.0f, pl, 0);
        sound_call_0024A2A0(pl, 102, 93);
        sound_call_0024A2A0(pl, 140, 75);
        sound_call_0024A2A0(pl, 158, 88);
        yoroi_sd_req(168.0f, pl, 4);
        sound_call_0024A2A0(pl, 254, 88);
        sound_call_0024A2A0(pl, 276, 88);
        yoroi_sd_req(280.0f, pl, 4);
        sound_call_0024A2A0(pl, 300, 88);
        yoroi_sd_req(302.0f, pl, 4);
        sound_call_0024A2A0(pl, 332, 88);
        yoroi_sd_req(336.0f, pl, 4);
        sound_call_0024A2A0(pl, 368, 88);
        sound_call_0024A2A0(pl, 398, 88);
        yoroi_sd_req(406.0f, pl, 4);
        sound_call_0024A2A0(pl, 250, 91);
        yoroi_sd_req(468.0f, pl, 4);
        yoroi_sd_req(466.0f, pl, 0);
        sound_call_0024A2A0(pl, 466, 92);
        yoroi_sd_req(466.0f, pl, 0);
        sound_call_0024A2A0(pl, 486, 91);
        yoroi_sd_req(486.0f, pl, 0);
        sound_call_0024A2A0(pl, 502, 92);
        yoroi_sd_req(556.0f, pl, 4);
        yoroi_sd_req(556.0f, pl, 4);
        yoroi_sd_req(698.0f, pl, 0);
        sound_call_0024A2A0(pl, 698, 92);
        yoroi_sd_req(740.0f, pl, 0);
        sound_call_0024A2A0(pl, 740, 91);
        yoroi_sd_req(792.0f, pl, 0);
        ashi_sd_req_0024A510(792.0f, pl, 0);
        yoroi_sd_req(820.0f, pl, 0);
        ashi_sd_req_0024A510(820.0f, pl, 0);
        break;
    case 677:
        ashi_sd_req_0024A510(40.0f, pl, 1);
        yoroi_sd_req(46.0f, pl, 0);
        sound_call_0024A2A0(pl, 96, 75);
        yoroi_sd_req(96.0f, pl, 4);
        sound_call_0024A2A0(pl, 96, 67);
        sound_call_0024A2A0(pl, 140, 58);
        sound_call_0024A2A0(pl, 140, 74);
        break;
    case 679:
        sound_call_0024A2A0(pl, 38, 67);
        ashi_sd_req_0024A510(30.0f, pl, 1);
        ashi_sd_req_0024A510(60.0f, pl, 0);
        yoroi_sd_req(60.0f, pl, 0);
        ashi_sd_req_0024A510(110.0f, pl, 1);
        break;
    case 800:
        sound_call_0024A2A0(pl, 24, 74);
        sound_call_0024A2A0(pl, 72, 58);
        yoroi_sd_req(54.0f, pl, 0);
        ashi_sd_req_0024A510(54.0f, pl, 0);
        if (frame_check(24.0f, pl, 0)) {
            Eft13_set_scl(pl, 0, 25, 0.800000011920929f);
        }
        break;
    case 801:
        sound_call2(pl, 4, 37);
        sound_call_0024A2A0(pl, 4, 74);
        ashi_sd_req_0024A510(18.0f, pl, 0);
        yoroi_sd_req(18.0f, pl, 0);
        break;
    case 802:
        sound_call_0024A2A0(pl, 4, 74);
        sound_call2(pl, 56, 41);
        ashi_sd_req_0024A510(118.0f, pl, 0);
        yoroi_sd_req(118.0f, pl, 0);
        break;
    case 803:
        sound_call_0024A2A0(pl, 58, 40);
        sound_call_0024A2A0(pl, 58, 41);
        sound_call_0024A2A0(pl, 120, 41);
        sound_call_0024A2A0(pl, 180, 41);
        sound_call_0024A2A0(pl, 180, 40);
        sound_call_0024A2A0(pl, 242, 41);
        if (w->anim == 803 && GW16(0x1E) % 20 == 0) {
            func_54BA40(pl, 6);
        }
        break;
    case 805:
        ashi_sd_req_0024A510(60.0f, pl, 1);
        yoroi_sd_req(94.0f, pl, 0);
        ashi_sd_req_0024A510(94.0f, pl, 1);
        ashi_sd_req_0024A510(142.0f, pl, 1);
        sound_call_0024A2A0(pl, 100, 43);
        yoroi_sd_req(106.0f, pl, 4);
        break;
    case 807:
        ashi_sd_req_0024A510(32.0f, pl, 3);
        yoroi_sd_req(32.0f, pl, 0);
        ashi_sd_req_0024A510(54.0f, pl, 1);
        yoroi_sd_req(54.0f, pl, 0);
        ashi_sd_req_0024A510(84.0f, pl, 1);
        yoroi_sd_req(32.0f, pl, 0);
        yoroi_sd_req(34.0f, pl, 4);
        sound_call_0024A2A0(pl, 52, 75);
        sound_call_0024A2A0(pl, 54, 45);
        sound_call_0024A2A0(pl, 96, 45);
        sound_call_0024A2A0(pl, 210, 45);
        sound_call_0024A2A0(pl, 160, 45);
        break;
    case 808:
        sound_call_0024A2A0(pl, 12, 43);
        sound_call_0024A2A0(pl, 22, 45);
        sound_call_0024A2A0(pl, 76, 75);
        ashi_sd_req_0024A510(24.0f, pl, 0);
        yoroi_sd_req(24.0f, pl, 0);
        ashi_sd_req_0024A510(76.0f, pl, 0);
        ashi_sd_req_0024A510(100.0f, pl, 1);
        ashi_sd_req_0024A510(180.0f, pl, 1);
        break;
    case 809:
        sound_call_0024A2A0(pl, 28, 43);
        ashi_sd_req_0024A510(66.0f, pl, 0);
        yoroi_sd_req(94.0f, pl, 0);
        ashi_sd_req_0024A510(94.0f, pl, 1);
        ashi_sd_req_0024A510(212.0f, pl, 1);
        ashi_sd_req_0024A510(226.0f, pl, 1);
        break;
    case 810:
        sound_call_0024A2A0(pl, 10, 43);
        sound_call_0024A2A0(pl, 10, 45);
        sound_call_0024A2A0(pl, 54, 65);
        yoroi_sd_req(118.0f, pl, 0);
        sound_call_0024A2A0(pl, 122, 75);
        ashi_sd_req_0024A510(176.0f, pl, 1);
        if (frame_check(52.0f, pl, 0) == 0) break;
        Eft13_set_scl(pl, 2, 7, 0.5f);
        break;
    }
    switch (pl->kind) {
    case 0:
        switch (w->anim) {
        case 1002:
            sound_call2(pl, 2, 1);
            sound_call2(pl, 76, 2);
            break;
        case 1003:
            sound_call2(pl, 2, 2);
            sound_call2(pl, 86, 6);
            break;
        case 1004:
            ashi_sd_req_0024A510(2.0f, pl, 0);
            ashi_sd_req_0024A510(32.0f, pl, 0);
            yoroi_sd_req(2.0f, pl, 0);
            yoroi_sd_req(32.0f, pl, 0);
            ashi_eft_req(2.0f, pl, 5, 8);
            ashi_eft_req(32.0f, pl, 8, 8);
            break;
        case 1009:
            ashi_sd_req_0024A510(2.0f, pl, 0);
            ashi_sd_req_0024A510(24.0f, pl, 0);
            ashi_sd_req_0024A510(50.0f, pl, 0);
            ashi_sd_req_0024A510(78.0f, pl, 0);
            yoroi_sd_req(2.0f, pl, 0);
            yoroi_sd_req(24.0f, pl, 0);
            yoroi_sd_req(50.0f, pl, 0);
            yoroi_sd_req(78.0f, pl, 0);
            sound_call2(pl, 2, 2);
            sound_call2(pl, 46, 6);
            ashi_eft_req(22.0f, pl, 8, 8);
            ashi_eft_req(76.0f, pl, 8, 8);
            ashi_eft_req(50.0f, pl, 5, 8);
            ashi_eft_req(104.0f, pl, 5, 8);
            break;
        case 1401:
        case 1402:
        case 1403:
            ashi_sd_req_0024A510(92.0f, pl, 0);
            ashi_sd_req_0024A510(108.0f, pl, 0);
            yoroi_sd_req(108.0f, pl, 0);
            ashi_eft_req(12.0f, pl, 8, 1);
            ashi_eft_req(12.0f, pl, 5, 1);
            ashi_eft_req(16.0f, pl, 5, 1);
            break;
        case 1404:
            sound_call2(pl, 44, 13);
            sound_call2(pl, 66, 14);
            ashi_sd_req_0024A510(28.0f, pl, 0);
            yoroi_sd_req(28.0f, pl, 0);
            sound_call2(pl, 10, 3);
            sound_call2(pl, 104, 4);
            break;
        case 1408:
            ashi_sd_req_0024A510(44.0f, pl, 0);
            yoroi_sd_req(64.0f, pl, 0);
            ashi_sd_req_0024A510(64.0f, pl, 0);
            ashi_sd_req_0024A510(130.0f, pl, 0);
            yoroi_sd_req(130.0f, pl, 0);
            ashi_eft_req(64.0f, pl, 8, 1);
            ashi_eft_req(50.0f, pl, 5, 1);
            ashi_eft_req(54.0f, pl, 5, 1);
            break;
        case 1409:
        case 1410:
            ashi_sd_req_0024A510(108.0f, pl, 0);
            ashi_sd_req_0024A510(136.0f, pl, 0);
            yoroi_sd_req(136.0f, pl, 0);
            ashi_eft_req(8.0f, pl, 8, 1);
            ashi_eft_req(8.0f, pl, 5, 1);
            ashi_eft_req(12.0f, pl, 5, 1);
            break;
        case 1411:
            sound_call2(pl, 2, 4);
            sound_call2(pl, 68, 9);
            sound_call2(pl, 180, 1);
            break;
        case 1412:
            sound_call2(pl, 12, 8);
            break;
        case 1413:
            sound_call2(pl, 24, Code_Make(35, 4, 36, 4));
            sound_call2(pl, 4, 7);
            sound_call2(pl, 12, 1);
            sound_call_0024A2A0(pl, 8, 61);
            break;
        default:
            move_default_0024A750(pl);
            break;
        }
        break;
    case 1:
        switch (w->anim) {
        case 1002:
            sound_call2(pl, 2, 1);
            yoroi_sd_req(24.0f, pl, 4);
            sound_call2(pl, 66, 2);
            ashi_sd_req_0024A510(60.0f, pl, 3);
            yoroi_sd_req(60.0f, pl, 0);
            break;
        case 1003:
            sound_call2(pl, 2, 2);
            sound_call2(pl, 2, 3);
            yoroi_sd_req(24.0f, pl, 4);
            ashi_sd_req_0024A510(72.0f, pl, 0);
            yoroi_sd_req(72.0f, pl, 0);
            break;
        case 1004:
        case 1005:
            sound_call2(pl, 6, Code_Make(39, 1, 40, 1));
            ashi_sd_req_0024A510(2.0f, pl, 0);
            ashi_sd_req_0024A510(22.0f, pl, 2);
            ashi_sd_req_0024A510(56.0f, pl, 2);
            yoroi_sd_req(2.0f, pl, 0);
            yoroi_sd_req(22.0f, pl, 0);
            yoroi_sd_req(56.0f, pl, 0);
            ashi_eft_req(4.0f, pl, 5, 1);
            ashi_eft_req(28.0f, pl, 8, 1);
            break;
        case 1006:
            ashi_sd_req_0024A510(4.0f, pl, 0);
            ashi_sd_req_0024A510(30.0f, pl, 3);
            yoroi_sd_req(30.0f, pl, 0);
            sound_call2(pl, 20, 5);
            sound_call2(pl, 4, 2);
            break;
        case 1009:
            sound_call2(pl, 2, 2);
            sound_call2(pl, 2, 3);
            yoroi_sd_req(24.0f, pl, 4);
            ashi_sd_req_0024A510(36.0f, pl, 0);
            ashi_sd_req_0024A510(64.0f, pl, 0);
            yoroi_sd_req(36.0f, pl, 0);
            yoroi_sd_req(64.0f, pl, 0);
            ashi_eft_req(60.0f, pl, 8, 8);
            ashi_eft_req(32.0f, pl, 5, 8);
            ashi_eft_req(76.0f, pl, 5, 8);
            break;
        case 1010:
            sound_call2(pl, 2, 1);
            sound_call2(pl, 12, 2);
            yoroi_sd_req(4.0f, pl, 0);
            break;
        case 1011:
            sound_call2(pl, 2, Code_Make(35, 2, 36, 2));
            ashi_sd_req_0024A510(2.0f, pl, 3);
            sound_call_0024A2A0(pl, 2, 63);
            yoroi_sd_req(20.0f, pl, 3);
            sound_call_0024A2A0(pl, 28, 65);
            ashi_sd_req_0024A510(56.0f, pl, 0);
            yoroi_sd_req(52.0f, pl, 0);
            ashi_eft_req(4.0f, pl, 10, 0);
            if (frame_check(20.0f, pl, 0)) {
                eft13_set(pl, 10, 11);
            }
            break;
        case 1012:
        case 1013:
            sound_call2(pl, 2, Code_Make(35, 2, 36, 2));
            ashi_sd_req_0024A510(2.0f, pl, 3);
            sound_call_0024A2A0(pl, 2, 63);
            yoroi_sd_req(20.0f, pl, 3);
            sound_call_0024A2A0(pl, 28, 65);
            if (w->anim == 1012) {
                ashi_eft_req(2.0f, pl, 8, 1);
            } else {
                ashi_eft_req(2.0f, pl, 5, 1);
            }
            ashi_eft_req(18.0f, pl, 10, 6);
            break;
        case 1401:
            sound_call2(pl, 28, Code_Make(35, 2, 36, 2));
            sound_call2(pl, 4, 2);
            sound_call2(pl, 26, 4);
            yoroi_sd_req(4.0f, pl, 4);
            ashi_sd_req_0024A510(28.0f, pl, 0);
            yoroi_sd_req(28.0f, pl, 0);
            break;
        case 1402:
            if (frame_check(68.0f, pl, 0)) {
                Eft13_set_scl(pl, 14, 12, 1.7999999523162842f);
            }
            sound_call2(pl, 62, Code_Make(37, 6, 36, 2));
            sound_call2(pl, 68, 7);
            sound_call2(pl, 14, 5);
            yoroi_sd_req(14.0f, pl, 4);
            sound_call2(pl, 60, 4);
            ashi_sd_req_0024A510(64.0f, pl, 3);
            yoroi_sd_req(64.0f, pl, 0);
            sound_call2(pl, 116, 5);
            yoroi_sd_req(128.0f, pl, 4);
            break;
        case 1403:
            if (frame_check2(10.0f, pl, 0) != 0 && frame_check2(66.0f, pl, 0) == 0 && (GW16(0x1E) & 3) == 0) {
                eft13_set(pl, 2, 13);
            }
            sound_call2(pl, 4, Code_Make(38, 3, 38, 3));
            ashi_sd_req_0024A510(2.0f, pl, 2);
            yoroi_sd_req(2.0f, pl, 0);
            ashi_sd_req_0024A510(8.0f, pl, 2);
            yoroi_sd_req(8.0f, pl, 0);
            sound_call2(pl, 10, 6);
            sound_call2(pl, 38, 6);
            break;
        case 1404:
            if (frame_check(38.0f, pl, 0)) {
                Eft13_set_scl(pl, 14, 15, 1.5f);
            }
            sound_call2(pl, 192, Code_Make(38, 3, 38, 3));
            sound_call2(pl, 38, 7);
            sound_call2(pl, 2, 1);
            ashi_sd_req_0024A510(36.0f, pl, 2);
            yoroi_sd_req(36.0f, pl, 0);
            ashi_sd_req_0024A510(56.0f, pl, 0);
            sound_call2(pl, 116, 2);
            sound_call2(pl, 176, 5);
            ashi_sd_req_0024A510(168.0f, pl, 0);
            yoroi_sd_req(168.0f, pl, 0);
            break;
        case 1405:
            sound_call2(pl, 24, Code_Make(35, 4, 35, 4));
            sound_call2(pl, 66, Code_Make(36, 2, 36, 4));
            sound_call2(pl, 12, 4);
            sound_call2(pl, 64, 4);
            sound_call2(pl, 64, 5);
            yoroi_sd_req(4.0f, pl, 4);
            yoroi_sd_req(68.0f, pl, 4);
            ashi_sd_req_0024A510(10.0f, pl, 0);
            yoroi_sd_req(10.0f, pl, 0);
            ashi_sd_req_0024A510(48.0f, pl, 2);
            yoroi_sd_req(48.0f, pl, 0);
            ashi_sd_req_0024A510(92.0f, pl, 0);
            ashi_sd_req_0024A510(112.0f, pl, 0);
            yoroi_sd_req(48.0f, pl, 0);
            break;
        case 1406:
            if (frame_check(76.0f, pl, 0)) {
                Eft13_set_scl(pl, 14, 19, 0.6000000238418579f);
            }
            sound_call2(pl, 20, 38);
            sound_call2(pl, 72, 37);
            sound_call2(pl, 74, 8);
            sound_call2(pl, 90, 15);
            sound_call2(pl, 2, 4);
            sound_call2(pl, 62, 4);
            sound_call2(pl, 182, 38);
            sound_call2(pl, 186, 5);
            sound_call2(pl, 238, 2);
            yoroi_sd_req(30.0f, pl, 4);
            ashi_sd_req_0024A510(70.0f, pl, 3);
            yoroi_sd_req(70.0f, pl, 0);
            ashi_sd_req_0024A510(238.0f, pl, 1);
            yoroi_sd_req(190.0f, pl, 4);
            break;
        case 1407:
            sound_call2(pl, 28, 5);
            sound_call2(pl, 58, 2);
            yoroi_sd_req(4.0f, pl, 4);
            yoroi_sd_req(68.0f, pl, 4);
            ashi_sd_req_0024A510(12.0f, pl, 2);
            yoroi_sd_req(12.0f, pl, 0);
            ashi_sd_req_0024A510(38.0f, pl, 0);
            yoroi_sd_req(48.0f, pl, 0);
            break;
        case 1408:
            ashi_sd_req_0024A510(12.0f, pl, 2);
            yoroi_sd_req(12.0f, pl, 0);
            ashi_sd_req_0024A510(32.0f, pl, 0);
            yoroi_sd_req(32.0f, pl, 0);
            sound_call2(pl, 62, 1);
            yoroi_sd_req(90.0f, pl, 4);
            break;
        case 1409:
            sound_call2(pl, 4, 5);
            yoroi_sd_req(4.0f, pl, 4);
            break;
        case 1410:
            sound_call2(pl, 2, 4);
            break;
        case 1411:
            sound_call2(pl, 38, 37);
            ashi_sd_req_0024A510(130.0f, pl, 0);
            yoroi_sd_req(130.0f, pl, 0);
            ashi_sd_req_0024A510(154.0f, pl, 2);
            yoroi_sd_req(154.0f, pl, 0);
            sound_call2(pl, 12, 6);
            sound_call2(pl, 156, 2);
            sound_call2(pl, 30, 15);
            if (frame_check(28.0f, pl, 0)) {
                Eft13_set_scl(pl, 10, 12, 1.2000000476837158f);
                eft13_set(pl, 10, 31);
            }
            break;
        case 1412:
            if (frame_check(52.0f, pl, 0)) {
                Eft13_set_scl(pl, 14, 12, 0.699999988079071f);
            }
            sound_call2(pl, 50, Code_Make(35, 1, 36, 2));
            sound_call2(pl, 4, 2);
            sound_call2(pl, 42, 4);
            yoroi_sd_req(4.0f, pl, 4);
            ashi_sd_req_0024A510(50.0f, pl, 0);
            yoroi_sd_req(50.0f, pl, 0);
            sound_call2(pl, 56, 7);
            break;
        default:
            move_default_0024A750(pl);
            break;
        }
        break;
    case 2:
        switch (w->anim) {
        case 1002:
            sound_call2(pl, 2, 1);
            ashi_sd_req_0024A510(24.0f, pl, 0);
            yoroi_sd_req(24.0f, pl, 0);
            break;
        case 1003:
            sound_call2(pl, 2, 6);
            ashi_sd_req_0024A510(60.0f, pl, 0);
            yoroi_sd_req(60.0f, pl, 0);
            break;
        case 1004:
            sound_call2(pl, 56, Code_Make(39, 1, 40, 1));
            ashi_sd_req_0024A510(2.0f, pl, 0);
            ashi_sd_req_0024A510(24.0f, pl, 0);
            yoroi_sd_req(2.0f, pl, 0);
            yoroi_sd_req(24.0f, pl, 0);
            ashi_eft_req(2.0f, pl, 5, 1);
            ashi_eft_req(26.0f, pl, 8, 1);
            break;
        case 1009:
            ashi_sd_req_0024A510(2.0f, pl, 0);
            ashi_sd_req_0024A510(16.0f, pl, 0);
            ashi_sd_req_0024A510(40.0f, pl, 0);
            ashi_sd_req_0024A510(54.0f, pl, 3);
            yoroi_sd_req(2.0f, pl, 0);
            yoroi_sd_req(16.0f, pl, 0);
            yoroi_sd_req(54.0f, pl, 0);
            sound_call2(pl, 10, 6);
            ashi_eft_req(22.0f, pl, 5, 1);
            break;
        case 1010:
            ashi_sd_req_0024A510(2.0f, pl, 0);
            ashi_sd_req_0024A510(18.0f, pl, 0);
            ashi_sd_req_0024A510(40.0f, pl, 0);
            yoroi_sd_req(2.0f, pl, 0);
            yoroi_sd_req(18.0f, pl, 0);
            yoroi_sd_req(48.0f, pl, 0);
            sound_call2(pl, 10, 1);
            ashi_eft_req(18.0f, pl, 8, 1);
            break;
        case 1401:
        case 1402:
        case 1403:
            ashi_sd_req_0024A510(92.0f, pl, 0);
            ashi_sd_req_0024A510(106.0f, pl, 0);
            yoroi_sd_req(78.0f, pl, 0);
            ashi_eft_req(12.0f, pl, 8, 1);
            ashi_eft_req(12.0f, pl, 5, 1);
            ashi_eft_req(16.0f, pl, 5, 1);
            break;
        case 1404:
            sound_call2(pl, 44, 13);
            sound_call2(pl, 66, 14);
            ashi_sd_req_0024A510(28.0f, pl, 0);
            yoroi_sd_req(28.0f, pl, 0);
            sound_call2(pl, 10, 3);
            sound_call2(pl, 104, 4);
            break;
        case 1408:
            ashi_sd_req_0024A510(44.0f, pl, 0);
            yoroi_sd_req(64.0f, pl, 0);
            ashi_sd_req_0024A510(64.0f, pl, 0);
            ashi_sd_req_0024A510(130.0f, pl, 0);
            yoroi_sd_req(130.0f, pl, 0);
            ashi_eft_req(64.0f, pl, 8, 1);
            ashi_eft_req(50.0f, pl, 5, 1);
            ashi_eft_req(54.0f, pl, 5, 1);
            break;
        case 1409:
        case 1410:
            ashi_sd_req_0024A510(108.0f, pl, 0);
            ashi_sd_req_0024A510(136.0f, pl, 0);
            yoroi_sd_req(136.0f, pl, 0);
            ashi_eft_req(8.0f, pl, 8, 1);
            ashi_eft_req(8.0f, pl, 5, 1);
            ashi_eft_req(12.0f, pl, 5, 1);
            break;
        case 1411:
            sound_call2(pl, 2, 9);
            sound_call2(pl, 180, 7);
            break;
        case 1412:
            sound_call2(pl, 12, 8);
            break;
        case 1413:
            sound_call2(pl, 24, Code_Make(35, 4, 36, 4));
            sound_call2(pl, 70, 7);
            sound_call2(pl, 20, 1);
            sound_call_0024A2A0(pl, 8, 61);
            ashi_sd_req_0024A510(26.0f, pl, 3);
            ashi_sd_req_0024A510(72.0f, pl, 0);
            yoroi_sd_req(26.0f, pl, 0);
            break;
        case 1414:
        case 1415:
            ashi_sd_req_0024A510(28.0f, pl, 0);
            ashi_sd_req_0024A510(128.0f, pl, 0);
            sound_call2(pl, 4, 3);
            sound_call2(pl, 100, 4);
            break;
        case 1416:
        case 1417:
            sound_call2(pl, 12, 8);
            break;
        default:
            move_default_0024A750(pl);
            break;
        }
        break;
    case 3:
        switch (w->anim) {
        case 1002:
            sound_call2(pl, 2, 2);
            sound_call2(pl, 10, 3);
            yoroi_sd_req(18.0f, pl, 4);
            ashi_sd_req_0024A510(42.0f, pl, 3);
            yoroi_sd_req(42.0f, pl, 0);
            sound_call2(pl, 56, 1);
            break;
        case 1003:
            sound_call2(pl, 22, 4);
            sound_call2(pl, 32, 3);
            sound_call2(pl, 58, 2);
            yoroi_sd_req(54.0f, pl, 4);
            ashi_sd_req_0024A510(72.0f, pl, 3);
            yoroi_sd_req(72.0f, pl, 0);
            break;
        case 1004:
            ashi_sd_req_0024A510(22.0f, pl, 3);
            yoroi_sd_req(22.0f, pl, 4);
            ashi_sd_req_0024A510(56.0f, pl, 3);
            yoroi_sd_req(56.0f, pl, 0);
            ashi_eft_req(18.0f, pl, 8, 8);
            ashi_eft_req(52.0f, pl, 5, 8);
            break;
        case 1005:
        case 1007:
            sound_call_0024A2A0(pl, 2, 74);
            break;
        case 1009:
            sound_call2(pl, 22, 4);
            sound_call2(pl, 32, 3);
            sound_call2(pl, 58, 2);
            ashi_sd_req_0024A510(22.0f, pl, 3);
            yoroi_sd_req(22.0f, pl, 0);
            ashi_sd_req_0024A510(62.0f, pl, 3);
            yoroi_sd_req(62.0f, pl, 0);
            ashi_sd_req_0024A510(94.0f, pl, 3);
            yoroi_sd_req(94.0f, pl, 0);
            ashi_eft_req(18.0f, pl, 5, 8);
            ashi_eft_req(96.0f, pl, 5, 8);
            ashi_eft_req(60.0f, pl, 8, 8);
            break;
        case 1010:
            sound_call2(pl, 28, 37);
            sound_call2(pl, 2, 2);
            sound_call2(pl, 10, 3);
            sound_call2(pl, 28, 1);
            sound_call2(pl, 40, 6);
            ashi_sd_req_0024A510(18.0f, pl, 3);
            yoroi_sd_req(18.0f, pl, 4);
            ashi_sd_req_0024A510(38.0f, pl, 3);
            yoroi_sd_req(38.0f, pl, 0);
            ashi_eft_req(2.0f, pl, 8, 1);
            ashi_eft_req(44.0f, pl, 5, 1);
            ashi_eft_req(46.0f, pl, 5, 1);
            break;
        case 1011:
            sound_call_0024A2A0(pl, 12, 69);
            ashi_sd_req_0024A510(4.0f, pl, 0);
            yoroi_sd_req(4.0f, pl, 0);
            ashi_sd_req_0024A510(32.0f, pl, 1);
            yoroi_sd_req(32.0f, pl, 0);
            ashi_eft_req(8.0f, pl, 8, 8);
            ashi_eft_req(36.0f, pl, 5, 8);
            break;
        case 1014:
        case 1015:
            ashi_sd_req_0024A510(4.0f, pl, 3);
            sound_call_0024A2A0(pl, 12, 67);
            yoroi_sd_req(4.0f, pl, 0);
            ashi_sd_req_0024A510(22.0f, pl, 3);
            yoroi_sd_req(22.0f, pl, 0);
            ashi_sd_req_0024A510(30.0f, pl, 0);
            yoroi_sd_req(30.0f, pl, 0);
            if (w->anim == 1014) {
                ashi_eft_req(4.0f, pl, 8, 1);
            } else {
                ashi_eft_req(2.0f, pl, 5, 1);
            }
            break;
        case 1016:
            ashi_sd_req_0024A510(4.0f, pl, 3);
            sound_call_0024A2A0(pl, 12, 67);
            yoroi_sd_req(4.0f, pl, 0);
            ashi_sd_req_0024A510(26.0f, pl, 3);
            yoroi_sd_req(26.0f, pl, 0);
            ashi_sd_req_0024A510(30.0f, pl, 0);
            yoroi_sd_req(30.0f, pl, 0);
            if (frame_check(4.0f, pl, 0)) {
                Eft13_set_scl(pl, 8, 3, 0.800000011920929f);
            }
            if (frame_check(8.0f, pl, 0)) {
                Eft13_set_scl(pl, 5, 3, 0.800000011920929f);
            }
            break;
        case 1018:
            sound_call2(pl, 2, 2);
            sound_call2(pl, 6, 3);
            sound_call2(pl, 18, 1);
            ashi_sd_req_0024A510(2.0f, pl, 0);
            yoroi_sd_req(18.0f, pl, 4);
            ashi_sd_req_0024A510(22.0f, pl, 0);
            yoroi_sd_req(22.0f, pl, 0);
            break;
        case 1401:
            if (frame_check(46.0f, pl, 0)) {
                eft13_set(pl, 10, 16);
            }
            if (frame_check(48.0f, pl, 0)) {
                Eft20_set_pl(pl, 2, 3, 0.6000000238418579f);
            }
            if (frame_check(52.0f, pl, 0)) {
                Eft20_set_pl(pl, 3, 3, 0.6000000238418579f);
            }
            sound_call2(pl, 4, Code_Make(35, 2, 36, 2));
            sound_call2(pl, 12, 5);
            sound_call2(pl, 52, 7);
            yoroi_sd_req(26.0f, pl, 0);
            break;
        case 1402:
            if (frame_check(4.0f, pl, 0)) {
                Eft20_set_pl(pl, 2, 2, 0.4000000059604645f);
            }
            if (frame_check(22.0f, pl, 0)) {
                Eft20_set_pl(pl, 3, 2, 0.4000000059604645f);
            }
            sound_call2(pl, 4, 7);
            sound_call2(pl, 22, 7);
            yoroi_sd_req(16.0f, pl, 0);
            ashi_sd_req_0024A510(16.0f, pl, 2);
            yoroi_sd_req(32.0f, pl, 0);
            ashi_sd_req_0024A510(32.0f, pl, 2);
            break;
        case 1403:
            sound_call2(pl, 66, 5);
            yoroi_sd_req(4.0f, pl, 0);
            ashi_sd_req_0024A510(4.0f, pl, 3);
            yoroi_sd_req(8.0f, pl, 0);
            ashi_sd_req_0024A510(8.0f, pl, 2);
            yoroi_sd_req(64.0f, pl, 0);
            ashi_sd_req_0024A510(64.0f, pl, 1);
            ashi_eft_req(10.0f, pl, 8, 1);
            ashi_eft_req(12.0f, pl, 8, 1);
            ashi_eft_req(12.0f, pl, 5, 1);
            ashi_eft_req(14.0f, pl, 5, 1);
            break;
        case 1404:
            sound_call2(pl, 70, 5);
            yoroi_sd_req(20.0f, pl, 0);
            ashi_sd_req_0024A510(20.0f, pl, 3);
            yoroi_sd_req(50.0f, pl, 0);
            ashi_sd_req_0024A510(42.0f, pl, 0);
            yoroi_sd_req(74.0f, pl, 0);
            ashi_sd_req_0024A510(74.0f, pl, 0);
            ashi_eft_req(8.0f, pl, 8, 1);
            ashi_eft_req(10.0f, pl, 8, 1);
            ashi_eft_req(16.0f, pl, 5, 1);
            break;
        case 1405:
            sound_call2(pl, 26, Code_Make(35, 2, 36, 3));
            sound_call2(pl, 18, 6);
            sound_call2(pl, 92, 5);
            yoroi_sd_req(22.0f, pl, 0);
            ashi_sd_req_0024A510(22.0f, pl, 3);
            yoroi_sd_req(98.0f, pl, 4);
            ashi_sd_req_0024A510(60.0f, pl, 0);
            ashi_sd_req_0024A510(100.0f, pl, 0);
            ashi_eft_req(16.0f, pl, 5, 1);
            ashi_eft_req(20.0f, pl, 5, 1);
            ashi_eft_req(24.0f, pl, 8, 1);
            break;
        case 1406:
            sound_call2(pl, 18, Code_Make(35, 3, 36, 2));
            sound_call2(pl, 18, 6);
            sound_call2(pl, 92, 5);
            yoroi_sd_req(16.0f, pl, 0);
            ashi_sd_req_0024A510(16.0f, pl, 3);
            yoroi_sd_req(102.0f, pl, 4);
            ashi_sd_req_0024A510(58.0f, pl, 0);
            ashi_sd_req_0024A510(100.0f, pl, 1);
            ashi_eft_req(22.0f, pl, 5, 1);
            ashi_eft_req(26.0f, pl, 5, 1);
            ashi_eft_req(18.0f, pl, 8, 1);
            break;
        case 1407:
            sound_call2(pl, 28, 5);
            sound_call_0024A2A0(pl, 80, 69);
            sound_call_0024A2A0(pl, 80, 74);
            ashi_sd_req_0024A510(26.0f, pl, 0);
            yoroi_sd_req(26.0f, pl, 0);
            ashi_sd_req_0024A510(74.0f, pl, 1);
            ashi_sd_req_0024A510(98.0f, pl, 1);
            yoroi_sd_req(98.0f, pl, 0);
            break;
        case 1409:
            sound_call2(pl, 52, 5);
            sound_call_0024A2A0(pl, 6, 69);
            sound_call_0024A2A0(pl, 6, 74);
            ashi_sd_req_0024A510(26.0f, pl, 0);
            yoroi_sd_req(26.0f, pl, 0);
            ashi_sd_req_0024A510(50.0f, pl, 0);
            yoroi_sd_req(50.0f, pl, 0);
            ashi_sd_req_0024A510(66.0f, pl, 1);
            break;
        case 1411:
            sound_call2(pl, 2, Code_Make(36, 2, 37, 3));
            sound_call2(pl, 6, 6);
            ashi_sd_req_0024A510(8.0f, pl, 3);
            sound_call_0024A2A0(pl, 10, 68);
            yoroi_sd_req(10.0f, pl, 0);
            ashi_eft_req(16.0f, pl, 8, 1);
            ashi_eft_req(18.0f, pl, 8, 1);
            break;
        case 1412:
            sound_call2(pl, 6, 37);
            sound_call2(pl, 12, 6);
            sound_call2(pl, 116, 5);
            sound_call_0024A2A0(pl, 14, 68);
            ashi_sd_req_0024A510(14.0f, pl, 3);
            yoroi_sd_req(14.0f, pl, 0);
            ashi_sd_req_0024A510(84.0f, pl, 0);
            yoroi_sd_req(84.0f, pl, 0);
            ashi_sd_req_0024A510(118.0f, pl, 0);
            yoroi_sd_req(118.0f, pl, 0);
            ashi_eft_req(10.0f, pl, 8, 1);
            ashi_eft_req(14.0f, pl, 8, 1);
            ashi_eft_req(12.0f, pl, 5, 1);
            ashi_eft_req(16.0f, pl, 5, 1);
            break;
        case 1415:
            ashi_sd_req_0024A510(8.0f, pl, 3);
            yoroi_sd_req(10.0f, pl, 0);
            sound_call_0024A2A0(pl, 26, 68);
            for (j = 0; j < 9; j++) {
                if (frame_check(26.0f, pl, 0)) {
                    Eft13_set_scl(pl, 5, 3, 1.2999999523162842f);
                    Eft13_set_scl(pl, 8, 3, 1.2999999523162842f);
                }
            }
            break;
        case 1416:
            sound_call2(pl, 6, Code_Make(35, 2, 36, 2));
            sound_call2(pl, 10, 6);
            break;
        case 1417:
            sound_call2(pl, 88, 5);
            ashi_sd_req_0024A510(28.0f, pl, 3);
            yoroi_sd_req(28.0f, pl, 0);
            ashi_sd_req_0024A510(48.0f, pl, 3);
            yoroi_sd_req(48.0f, pl, 0);
            ashi_sd_req_0024A510(92.0f, pl, 1);
            break;
        default:
            move_default_0024A750(pl);
            break;
        }
        break;
    case 4:
        switch (w->anim) {
        case 1002:
            sound_call2(pl, 2, 1);
            ashi_sd_req_0024A510(12.0f, pl, 0);
            break;
        case 1003:
            sound_call2(pl, 28, 2);
            ashi_sd_req_0024A510(32.0f, pl, 0);
            yoroi_sd_req(34.0f, pl, 0);
            ashi_sd_req_0024A510(40.0f, pl, 1);
            break;
        case 1004:
            ashi_sd_req_0024A510(2.0f, pl, 0);
            yoroi_sd_req(6.0f, pl, 0);
            sound_call2(pl, 6, Code_Make(39, 1, 40, 1));
            ashi_sd_req_0024A510(22.0f, pl, 3);
            yoroi_sd_req(22.0f, pl, 0);
            ashi_eft_req(2.0f, pl, 5, 1);
            ashi_eft_req(26.0f, pl, 8, 1);
            break;
        case 1005:
        case 1007:
            sound_call_0024A2A0(pl, 2, 74);
            ashi_sd_req_0024A510(10.0f, pl, 1);
            break;
        case 1009:
            ashi_sd_req_0024A510(2.0f, pl, 3);
            yoroi_sd_req(2.0f, pl, 0);
            ashi_sd_req_0024A510(22.0f, pl, 3);
            yoroi_sd_req(24.0f, pl, 0);
            ashi_sd_req_0024A510(46.0f, pl, 0);
            yoroi_sd_req(48.0f, pl, 0);
            sound_call2(pl, 4, 3);
            sound_call2(pl, 20, 2);
            ashi_eft_req(6.0f, pl, 5, 1);
            break;
        case 1010:
            sound_call2(pl, 2, 1);
            break;
        case 1012:
        case 1013:
            sound_call2(pl, 2, Code_Make(35, 2, 36, 2));
            ashi_sd_req_0024A510(2.0f, pl, 3);
            sound_call_0024A2A0(pl, 2, 63);
            yoroi_sd_req(20.0f, pl, 3);
            sound_call_0024A2A0(pl, 20, 65);
            if (w->anim == 1012) {
                ashi_eft_req(2.0f, pl, 8, 1);
                ashi_eft_req(16.0f, pl, 10, 6);
            } else {
                ashi_eft_req(2.0f, pl, 5, 1);
                ashi_eft_req(18.0f, pl, 10, 6);
            }
            break;
        case 1018:
            ashi_sd_req_0024A510(12.0f, pl, 3);
            yoroi_sd_req(12.0f, pl, 0);
            sound_call2(pl, 4, 1);
            break;
        case 1401:
            sound_call2(pl, 16, Code_Make(35, 2, 36, 2));
            sound_call2(pl, 14, 4);
            ashi_sd_req_0024A510(12.0f, pl, 3);
            yoroi_sd_req(12.0f, pl, 4);
            ashi_sd_req_0024A510(58.0f, pl, 0);
            if (frame_check(6.0f, pl, 0)) {
                func_543690(pl, 11, 1);
            }
            break;
        case 1402:
            sound_call2(pl, 2, Code_Make(35, 2, 36, 2));
            sound_call2(pl, 2, 3);
            sound_call2(pl, 4, 4);
            ashi_sd_req_0024A510(14.0f, pl, 1);
            if (frame_check(2.0f, pl, 0)) {
                func_543690(pl, 9, 1);
            }
            break;
        case 1403:
            sound_call2(pl, 18, Code_Make(35, 4, 36, 4));
            sound_call2(pl, 54, 37);
            sound_call2(pl, 4, 3);
            sound_call2(pl, 18, 6);
            sound_call2(pl, 40, 5);
            ashi_sd_req_0024A510(8.0f, pl, 0);
            ashi_sd_req_0024A510(22.0f, pl, 0);
            ashi_sd_req_0024A510(54.0f, pl, 3);
            yoroi_sd_req(60.0f, pl, 4);
            yoroi_sd_req(54.0f, pl, 0);
            ashi_sd_req_0024A510(108.0f, pl, 0);
            if (frame_check(52.0f, pl, 0)) {
                func_543690(pl, 9, 1);
            }
            break;
        case 1404:
            sound_call2(pl, 26, Code_Make(35, 4, 36, 4));
            sound_call2(pl, 8, 6);
            yoroi_sd_req(60.0f, pl, 4);
            sound_call2(pl, 24, 5);
            ashi_sd_req_0024A510(6.0f, pl, 3);
            ashi_sd_req_0024A510(10.0f, pl, 3);
            yoroi_sd_req(28.0f, pl, 0);
            ashi_sd_req_0024A510(28.0f, pl, 3);
            ashi_sd_req_0024A510(32.0f, pl, 3);
            ashi_sd_req_0024A510(76.0f, pl, 0);
            if (frame_check(24.0f, pl, 0)) {
                func_543690(pl, 14, 1);
            }
            ashi_eft_req(6.0f, pl, 8, 1);
            ashi_eft_req(26.0f, pl, 5, 3);
            break;
        case 1405:
            sound_call2(pl, 28, Code_Make(37, 4, 36, 2));
            sound_call2(pl, 4, 6);
            sound_call2(pl, 14, 5);
            ashi_sd_req_0024A510(8.0f, pl, 0);
            ashi_sd_req_0024A510(28.0f, pl, 3);
            yoroi_sd_req(30.0f, pl, 0);
            ashi_sd_req_0024A510(84.0f, pl, 0);
            yoroi_sd_req(42.0f, pl, 4);
            if (frame_check(20.0f, pl, 0)) {
                func_543690(pl, 13, 1);
            }
            break;
        case 1406:
            ashi_sd_req_0024A510(4.0f, pl, 3);
            yoroi_sd_req(4.0f, pl, 0);
            ashi_sd_req_0024A510(20.0f, pl, 3);
            yoroi_sd_req(20.0f, pl, 0);
            ashi_sd_req_0024A510(68.0f, pl, 0);
            sound_call2(pl, 16, 36);
            sound_call2(pl, 10, 5);
            if (frame_check(10.0f, pl, 0)) {
                func_543690(pl, 12, 1);
            }
            break;
        case 1408:
            ashi_sd_req_0024A510(22.0f, pl, 0);
            yoroi_sd_req(22.0f, pl, 0);
            ashi_sd_req_0024A510(36.0f, pl, 0);
            yoroi_sd_req(36.0f, pl, 0);
            break;
        case 1413:
            ashi_sd_req_0024A510(16.0f, pl, 0);
            ashi_sd_req_0024A510(32.0f, pl, 3);
            yoroi_sd_req(16.0f, pl, 0);
            yoroi_sd_req(32.0f, pl, 0);
            break;
        case 1415:
            ashi_sd_req_0024A510(30.0f, pl, 3);
            ashi_sd_req_0024A510(40.0f, pl, 0);
            ashi_sd_req_0024A510(124.0f, pl, 0);
            yoroi_sd_req(40.0f, pl, 0);
            sound_call_0024A2A0(pl, 46, 64);
            for (j = 0; j < 7; j++) {
                ashi_eft_req(46.0f, pl, 5, 1);
                ashi_eft_req(46.0f, pl, 8, 1);
            }
            break;
        case 1416:
            sound_call2(pl, 20, Code_Make(35, 2, 35, 2));
            sound_call2(pl, 14, 4);
            ashi_sd_req_0024A510(10.0f, pl, 3);
            yoroi_sd_req(10.0f, pl, 0);
            if (frame_check(16.0f, pl, 0)) {
                func_543690(pl, 8, 1);
            }
            break;
        default:
            move_default_0024A750(pl);
            break;
        }
        break;
    case 5:
    default:
        switch (w->anim) {
        case 1002:
            sound_call2(pl, 32, 12);
            sound_call2(pl, 18, 1);
            sound_call2(pl, 20, 2);
            ashi_sd_req_0024A510(36.0f, pl, 3);
            yoroi_sd_req(36.0f, pl, 0);
            break;
        case 1003:
            sound_call2(pl, 10, 3);
            sound_call2(pl, 32, 1);
            ashi_sd_req_0024A510(56.0f, pl, 0);
            yoroi_sd_req(56.0f, pl, 0);
            break;
        case 1004:
            ashi_sd_req_0024A510(2.0f, pl, 0);
            ashi_sd_req_0024A510(30.0f, pl, 0);
            yoroi_sd_req(4.0f, pl, 0);
            yoroi_sd_req(34.0f, pl, 0);
            ashi_eft_req(62.0f, pl, 8, 8);
            ashi_eft_req(30.0f, pl, 5, 8);
            break;
        case 1012:
        case 1013:
            sound_call2(pl, 12, Code_Make(37, 2, 36, 2));
            sound_call_0024A2A0(pl, 20, 65);
            yoroi_sd_req(40.0f, pl, 3);
            sound_call2(pl, 58, 1);
            ashi_sd_req_0024A510(40.0f, pl, 3);
            ashi_sd_req_0024A510(66.0f, pl, 0);
            ashi_sd_req_0024A510(126.0f, pl, 0);
            yoroi_sd_req(40.0f, pl, 0);
            yoroi_sd_req(66.0f, pl, 0);
            if (w->anim == 1012) {
                ashi_eft_req(6.0f, pl, 5, 0);
            } else {
                ashi_eft_req(6.0f, pl, 8, 0);
            }
            ashi_eft_req(18.0f, pl, 10, 6);
            break;
        case 1016:
            sound_call2(pl, 12, 1);
            sound_call2(pl, 20, 2);
            ashi_sd_req_0024A510(12.0f, pl, 0);
            ashi_sd_req_0024A510(30.0f, pl, 3);
            yoroi_sd_req(12.0f, pl, 0);
            yoroi_sd_req(30.0f, pl, 0);
            ashi_eft_req(4.0f, pl, 5, 1);
            break;
        case 1017:
            sound_call2(pl, 8, 3);
            sound_call2(pl, 42, 1);
            ashi_sd_req_0024A510(32.0f, pl, 0);
            ashi_sd_req_0024A510(60.0f, pl, 0);
            yoroi_sd_req(34.0f, pl, 0);
            yoroi_sd_req(62.0f, pl, 0);
            ashi_eft_req(60.0f, pl, 8, 8);
            ashi_eft_req(32.0f, pl, 5, 8);
            ashi_eft_req(74.0f, pl, 5, 8);
            break;
        case 1401:
            sound_call2(pl, 92, Code_Make(37, 3, 36, 2));
            sound_call2(pl, 8, 4);
            sound_call2(pl, 54, 5);
            sound_call2(pl, 100, 7);
            sound_call2(pl, 200, 1);
            yoroi_sd_req(26.0f, pl, 4);
            yoroi_sd_req(192.0f, pl, 4);
            ashi_sd_req_0024A510(18.0f, pl, 0);
            ashi_sd_req_0024A510(100.0f, pl, 3);
            ashi_sd_req_0024A510(178.0f, pl, 0);
            ashi_sd_req_0024A510(208.0f, pl, 0);
            yoroi_sd_req(18.0f, pl, 0);
            yoroi_sd_req(100.0f, pl, 0);
            break;
        case 1404:
            sound_call2(pl, 84, 1);
            ashi_sd_req_0024A510(14.0f, pl, 0);
            ashi_sd_req_0024A510(28.0f, pl, 0);
            ashi_sd_req_0024A510(100.0f, pl, 0);
            ashi_sd_req_0024A510(230.0f, pl, 0);
            yoroi_sd_req(28.0f, pl, 0);
            break;
        case 1406:
            sound_call2(pl, 48, Code_Make(37, 3, 36, 2));
            sound_call2(pl, 16, 5);
            sound_call2(pl, 130, 1);
            ashi_sd_req_0024A510(16.0f, pl, 3);
            ashi_sd_req_0024A510(40.0f, pl, 0);
            ashi_sd_req_0024A510(76.0f, pl, 3);
            ashi_sd_req_0024A510(158.0f, pl, 1);
            ashi_sd_req_0024A510(184.0f, pl, 1);
            yoroi_sd_req(16.0f, pl, 0);
            yoroi_sd_req(76.0f, pl, 0);
            yoroi_sd_req(52.0f, pl, 4);
            yoroi_sd_req(178.0f, pl, 4);
            break;
        case 1407:
            sound_call2(pl, 76, Code_Make(35, 3, 36, 3));
            sound_call2(pl, 228, 38);
            sound_call2(pl, 8, 4);
            sound_call2(pl, 50, 5);
            sound_call2(pl, 114, 8);
            sound_call2(pl, 190, 1);
            sound_call2(pl, 224, 2);
            sound_call2(pl, 272, 1);
            ashi_sd_req_0024A510(28.0f, pl, 0);
            ashi_sd_req_0024A510(110.0f, pl, 3);
            ashi_sd_req_0024A510(264.0f, pl, 0);
            ashi_sd_req_0024A510(282.0f, pl, 0);
            yoroi_sd_req(28.0f, pl, 0);
            yoroi_sd_req(110.0f, pl, 0);
            yoroi_sd_req(198.0f, pl, 4);
            yoroi_sd_req(274.0f, pl, 4);
            break;
        case 1410:
            sound_call2(pl, 34, Code_Make(35, 4, 36, 4));
            sound_call2(pl, 2, 1);
            sound_call2(pl, 26, 2);
            ashi_sd_req_0024A510(12.0f, pl, 3);
            yoroi_sd_req(12.0f, pl, 0);
            break;
        case 1411:
            sound_call2(pl, 6, Code_Make(35, 3, 36, 1));
            sound_call_0024A2A0(pl, 6, 61);
            sound_call2(pl, 48, 1);
            ashi_sd_req_0024A510(48.0f, pl, 0);
            yoroi_sd_req(48.0f, pl, 0);
            break;
        case 1412:
            sound_call2(pl, 4, 1);
            sound_call2(pl, 20, 6);
            ashi_sd_req_0024A510(20.0f, pl, 1);
            break;
        case 1414:
            sound_call2(pl, 20, 1);
            ashi_sd_req_0024A510(30.0f, pl, 0);
            yoroi_sd_req(284.0f, pl, 4);
            break;
        case 1415:
            sound_call2(pl, 56, 1);
            sound_call2(pl, 56, 2);
            ashi_sd_req_0024A510(28.0f, pl, 1);
            ashi_sd_req_0024A510(36.0f, pl, 1);
            ashi_sd_req_0024A510(52.0f, pl, 1);
            ashi_sd_req_0024A510(72.0f, pl, 1);
            yoroi_sd_req(40.0f, pl, 0);
            yoroi_sd_req(72.0f, pl, 0);
            yoroi_sd_req(84.0f, pl, 4);
            for (j = 0; j < 6; j++) {
                if (frame_check(64.0f, pl, 0)) {
                    Eft13_set_scl(pl, 5, 3, 1.2999999523162842f);
                    Eft13_set_scl(pl, 8, 3, 1.2999999523162842f);
                }
            }
            break;
        case 1416:
            sound_call2(pl, 8, 4);
            sound_call2(pl, 48, 5);
            ashi_sd_req_0024A510(46.0f, pl, 3);
            yoroi_sd_req(46.0f, pl, 0);
            break;
        case 1417:
            sound_call2(pl, 30, 5);
            ashi_sd_req_0024A510(26.0f, pl, 0);
            yoroi_sd_req(26.0f, pl, 0);
            break;
        case 1418:
            sound_call2(pl, 30, 5);
            ashi_sd_req_0024A510(16.0f, pl, 0);
            break;
        case 1419:
            sound_call2(pl, 28, 5);
            ashi_sd_req_0024A510(12.0f, pl, 0);
            break;
        case 1420:
            ashi_sd_req_0024A510(8.0f, pl, 0);
            ashi_sd_req_0024A510(40.0f, pl, 3);
            yoroi_sd_req(40.0f, pl, 0);
            break;
        case 1421:
            sound_call2(pl, 16, 5);
            ashi_sd_req_0024A510(26.0f, pl, 0);
            break;
        case 1422:
            sound_call2(pl, 8, 4);
            sound_call2(pl, 48, 5);
            ashi_sd_req_0024A510(46.0f, pl, 3);
            yoroi_sd_req(46.0f, pl, 0);
            break;
        case 1423:
            sound_call2(pl, 30, 5);
            ashi_sd_req_0024A510(26.0f, pl, 0);
            yoroi_sd_req(46.0f, pl, 0);
            break;
        case 1424:
            sound_call2(pl, 30, 5);
            ashi_sd_req_0024A510(16.0f, pl, 0);
            break;
        case 1425:
            sound_call2(pl, 28, 5);
            ashi_sd_req_0024A510(12.0f, pl, 0);
            break;
        case 1426:
            ashi_sd_req_0024A510(8.0f, pl, 0);
            ashi_sd_req_0024A510(40.0f, pl, 3);
            yoroi_sd_req(8.0f, pl, 0);
            break;
        case 1427:
            sound_call2(pl, 16, 5);
            ashi_sd_req_0024A510(26.0f, pl, 0);
            break;
        default:
            move_default_0024A750(pl);
            break;
        }
        break;
    }
}
