/* Motion system, matching part 3 (SLPM_654.95 0x001263F0-0x001267BC):
 * frame_check .. em_frame_check3 and move(). See f_frame_nm.c. */
#include "types.h"
#include "game.h"
#include "pl.h"
#include "em.h"
#include "frame.h"

/* 1 if frame f is reached during this tick's step of layer n. */
int frame_check(FRW *w, int n, f32 f) {
    int loop = 0;
    f32 cur;
    f32 next;
    f32 end;

    if (w->mt[0].b_dir != 0) {
        return 0;
    }
    cur = w->mt[n].frame;
    next = cur + w->mt[n].spd;
    end = w->mt[n].end;
    if (next > end) {
        if (w->mt[n].loop != 0) {
            loop = 1;
            next = w->mt[n].loopfr + (next - end);
        } else {
            next = end;
        }
    }
    if (loop == 0) {
        if (f >= cur && f < next) {
            return 1;
        }
    } else if (f >= cur && next < end) {
        return 1;
    }
    return 0;
}

/* Monsters: frame 2 is scaled by the animation speed (act_spd at +0x930). */
int em_frame_check(FRW *w, int n, f32 f) {
    f32 spd = *(f32 *)((u8 *)w + 0x930);

    if (spd != 1.0f && f == 2.0f) {
        f *= spd;
    }
    return frame_check(w, n, f);
}

/* 1 once layer n is at or past frame f. */
int frame_check2(FRW *w, int n, f32 f) {
    if (w->mt[0].b_dir != 0) {
        return 0;
    }
    if (f <= w->mt[n].frame) {
        return 1;
    }
    return 0;
}

int em_frame_check2(FRW *w, int n, f32 f) {
    return frame_check2(w, n, f);
}

/* 1 while layer n is between frames a and b. */
int frame_check3(FRW *w, int n, f32 a, f32 b) {
    if (frame_check2(w, n, a) != 0 && frame_check2(w, n, b) == 0) {
        return 1;
    }
    return 0;
}

int em_frame_check3(FRW *w, int n, f32 a, f32 b) {
    return frame_check3(w, n, a, b);
}

void player_mv(void);
void old_pos_save(EMW *);
int enemy_mv(EMW *);
void enemy_mk(EMW *);
void em_ride_sub(EMW *);
int npc_mv(EMW *);
void npc_mk(EMW *);
void item_check(void);
void body_hit(void);
void bgm_server(void);
void HitWallPlayer(void *, int);
void player_mk(void);
void yure_move(void);
void CameraMove(void);
void light_move(void);
void move_eft(void);
void move_shell(void);
void move_set(void);
void move_item(void);
void move_senko(void);
void move_smoke(void);
void move_stage(void);
void Pit_mv(void);

/* One game tick: players, monsters/NPCs (20 slots), hits, then the
 * camera, effects, shells, set objects, items and the cockpit. */
void move(void) {
    int i;
    EMW *em;
    PLW *pl;

    em = em_work;
    if (game_w.info_stop == 0) {
        player_mv();
    }
    for (i = 0; i < 20; i++, em++) {
        if (em->be_flag != 0) {
            old_pos_save(em);
            if (((u8 *)em)[0x1E] == 0) {
                if (enemy_mv(em) == 0) {
                    enemy_mk(em);
                    em_ride_sub(em);
                }
            } else if (npc_mv(em) == 0) {
                npc_mk(em);
            }
        }
    }
    if (game_w.info_stop == 0) {
        item_check();
        body_hit();
    }
    bgm_server();
    for (i = 0, pl = player_work; i < 8; i++, pl++) {
        if (pl->be_flag != 0 && (game_w.x2E == 1 || ((u8 *)pl)[0x7EC] != 0)) {
            HitWallPlayer(pl, 1);
        }
    }
    for (i = 0, em = em_work; i < 20; i++, em++) {
        if (em->be_flag != 0 && ((u8 *)em)[0x7EC] != 0) {
            HitWallPlayer(em, 1);
        }
    }
    player_mk();
    yure_move();
    CameraMove();
    light_move();
    move_eft();
    move_shell();
    move_set();
    move_item();
    move_senko();
    move_smoke();
    move_stage();
    Pit_mv();
}
