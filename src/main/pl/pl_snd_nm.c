/* Player sound/effect wrappers (SLPM_654.95 main 0x24A240-0x24A790): pl_local_init (empty), pl01_effect_move,
 * sound_call* (play a sound once when the current motion reaches a frame), wall_sd_req/ashi_sd_req/yoroi_sd_req
 * (footstep, wall and armor sounds: the sound id is base*2 + random bit), ashi_eft_req (foot effect by kind),
 * move_default. pl01_effect_move drives ef_move_sub (the big per-motion effect/sound script at 0x24A790). */
#include "pl.h"
#include "game.h"
#include "plf.h"
#include "flow.h"

int frame_check(f32, PLW *, int);
void Pl_se_req2_com(PLW *, int, int, f32 *, int, int);
void Pl_se_req2(PLW *, int, int, f32 *, int, int);
void se_req2(int, int, int, f32 *, int, int);
void armor_sd_req(PLW *, int);
void eft13_set(PLW *, int, int);
void parts_chg();
void ef_move_sub_0024A790(PLW *, u8 *);

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
        ef_move_sub_0024A790(pl, ex);
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
