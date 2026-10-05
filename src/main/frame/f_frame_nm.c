/* Motion system, whole file incl. near-matches (SLPM_654.95 0x00125340-0x001267BC): building the motion-set
 * handles from the AAN banks (create_*_motion), the per-actor motion player
 * (frame_init / frame_init_b / frame_move), the frame tests the state code
 * uses (frame_check*), root-motion velocity (calc_*velocity, pl_velocity_sub)
 * and the main per-tick move().
 *
 * frame_init/frame_move work on players (PLW) and monsters (EMW) alike: both
 * keep up to four motion layers of 0x50 bytes at +0x194 (FRMT below) and the
 * current motion ids at +0x2DC. Field names are guesses from how this code
 * uses them.
 *
 * motion_set_handle_tbl (u32[0x1004]) holds one fl motion-set handle per
 * motion: common hunter motions at [0], player k's own at [500 + k * 300],
 * monster model k's at [1700 + k * 600]. *_mot_han_ofs[k][bank] is the index
 * of bank's first motion inside that block. A motion id decodes as
 * bank = id % 1000 / 100, slot = id % 100; ids >= 1000 are the actor's own
 * table, smaller ids the common one (players only; a monster asking for one
 * is a system_error). */
#include "types.h"
#include "game.h"
#include "pl.h"
#include "em.h"
#include "frame.h"

void create_plcom_motion(void) {
    u8 tmp[0x28];
    u8 frame[8];
    u8 *aan;
    u32 *han;
    int idx;
    s32 *ofs;
    u8 *p;
    u32 h;
    int base;
    int bank;
    int num;
    int i;

    idx = 0;
    han = motion_set_handle_tbl;
    ofs = com_mot_han_ofs;
    bank = 0;
    aan = pl_area_top;
    base = 0;
    do {
        *ofs = idx;
        num = aan_ctr_get(aan, bank);
        for (i = 0; i < num; i++) {
            p = aan_ofs_calc(aan, i + base);
            if (p != 0) {
                flGetFrame(frame);
                plCreateMotionSetFromAAN(tmp, data_load_ptr, p);
                h = flCreateMotionSetHandle(tmp);
                flReleaseFrame(frame);
                *han = h;
            } else {
                *han = 0;
            }
            han++;
            idx++;
        }
        bank++;
        ofs++;
        base += 100;
    } while (bank < 10);
}

void create_pl_motion(int pl) {
    u8 tmp[0x28];
    u8 frame[8];
    u8 *aan;
    u32 *han;
    int idx;
    s32 *ofs;
    u8 *p;
    u32 h;
    int base;
    int bank;
    int num;
    int i;

    aan = pl_area_top;
    idx = 0;
    bank = 0;
    han = &motion_set_handle_tbl[pl * 300 + 500];
    ofs = pl_mot_han_ofs[pl];
    base = 0;
    do {
        *ofs = idx;
        num = aan_ctr_get(aan, bank);
        for (i = 0; i < num; i++) {
            p = aan_ofs_calc(aan, i + base);
            if (p != 0) {
                flGetFrame(frame);
                plCreateMotionSetFromAAN(tmp, data_load_ptr, p);
                h = flCreateMotionSetHandle(tmp);
                flReleaseFrame(frame);
                *han = h;
            } else {
                *han = 0;
            }
            han++;
            idx++;
        }
        bank++;
        ofs++;
        base += 100;
    } while (bank < 6);
}

void create_em_motion(int no, s16 em) {
    u8 *aan;
    u8 tmp[0x28];
    u8 frame[8];
    u32 *han;
    s32 *ofs;
    int idx;
    int bank;
    int base;
    int i;
    int num;
    int banks;
    u8 *p;
    u32 h;

    han = &motion_set_handle_tbl[no * 600 + 1700];
    aan = pl_area_top;
    idx = 0;
    banks = Em_max_parts_get(em) * 2;
    for (bank = 0, base = 0, ofs = em_mot_han_ofs[no]; bank < banks; bank++, ofs++, base += 100) {
        *ofs = idx;
        num = aan_ctr_get(aan, bank);
        for (i = 0; i < num; i++) {
            p = aan_ofs_calc(aan, i + base);
            if (p != 0) {
                flGetFrame(frame);
                plCreateMotionSetFromAAN(tmp, data_load_ptr, p);
                h = flCreateMotionSetHandle(tmp);
                flReleaseFrame(frame);
                *han = h;
            } else {
                *han = 0;
            }
            han++;
            idx++;
        }
    }
}

/* AAN bank table: (count, word offset of the bank's offset list) per bank. */
u8 *aan_ofs_calc(u8 *aan, int no) {
    u8 *top = aan;
    s32 *bank = (s32 *)(aan + no / 100 * 8);
    int slot = no % 100;
    s32 ofs;

    if (slot >= bank[0]) {
        return 0;
    }
    ofs = ((s32 *)aan + bank[1] / 4)[slot];
    if (ofs == -1) {
        return 0;
    }
    return top + ofs;
}

int aan_ctr_get(u8 *aan, int bank) {
    return *(s32 *)(aan + bank * 8);
}

void calc_ofs_velocity(f32 start, f32 end, f32 *vel, s32 mot) {
    if (start < 0.0f) {
        flCalcTransVelocity(0.0f, end - start, vel, mot);
    } else {
        flCalcTransVelocity(start, end, vel, mot);
    }
}

void calc_velocity(f32 *out, f32 *a, f32 *b, f32 ra, f32 rb) {
    out[0] = a[0] * ra + b[0] * rb;
    out[1] = a[1] * ra + b[1] * rb;
    out[2] = a[2] * ra + b[2] * rb;
}

/* Turns a root-motion step into world space and moves the actor by it. */
void pl_velocity_sub(FRW *w, f32 *vel) {
    f32 v[4];
    FLMAT m;
    FLMAT s;

    cpRotMatrix(w->ang, &m);
    flmatMakeScale(&s, w->scl[0], w->scl[1], w->scl[2]);
    flmatMul(&m, &m, &s);
    cpApplyMatrix(&m, vel, v);
    if (pl_flag_ck(w, 0x20000) == 0) {
        w->pos[0] += v[0];
        w->pos[1] += v[1];
        w->pos[2] += v[2];
    }
}

/* Starts motion chr[n] on layer n at `frame`; blend != 0 cross-fades into it
 * over |blend| + 1 ticks (negative: without root motion during the blend). */
void frame_init(FRW *w, int frame, int blend, int n) {
    u16 id = w->chr[n];
    FRMDL *mdl = w->mdl;
    u32 *han;
    int no = id % 1000;

    if (w->x10 == 0 || (w->x1E != 0 && w->x1F != 0)) {
        if (id >= 1000) {
            han = &motion_set_handle_tbl[w->id * 300 + 500];
            han += pl_mot_han_ofs[w->id][no / 100];
        } else {
            han = motion_set_handle_tbl;
            han += com_mot_han_ofs[no / 100];
        }
    } else if (id >= 1000) {
        han = &motion_set_handle_tbl[w->mdl_no * 600 + 1700];
        han += em_mot_han_ofs[w->mdl_no][no / 100];
    } else {
        system_error(lit_277_003584C0, (s16)id, 0, 0);
    }
    if (w->x10 == 0 || w->x1E != 0) {
        han += no % 100;
    } else {
        han = (u32 *)((u8 *)han + (no % 100) * 4);
    }
    if (blend != 0) {
        if (blend < 0) {
            blend = -blend;
            w->mt[n].b_dir = -1;
        } else {
            w->mt[n].b_dir = 1;
        }
        flSetMotionEx(mdl->mot1, *han, n);
        w->mt[n].b_end = flGetMotionSetTime(*han);
        w->mt[n].b_loop = flGetMotionSetLoopInfo(*han, &w->mt[n].b_loopfr);
        w->mt[n].b_frame = frame;
        w->mt[n].b_cnt = blend + 1;
        w->mt[n].b_rate = w->mt[n].b_step = 1.0f / (blend + 1);
        w->mt[n].b_han = han;
    } else {
        flSetMotionEx(mdl->mot0, *han, n);
        w->mt[n].end = flGetMotionSetTime(*han);
        w->mt[n].loop = flGetMotionSetLoopInfo(*han, &w->mt[n].loopfr);
        w->mt[n].frame = frame;
        w->mt[n].b_cnt = 0;
        w->mt[n].b_dir = 0;
        w->mt[n].b_han = han;
    }
    w->mt[n].stat = 1;
    if (n == 0) {
        w->mt[0].ofs[0] = 0.0f;
        w->mt[0].ofs[1] = 0.0f;
        w->mt[0].ofs[2] = 0.0f;
    }
}

/* Loads the second motion sub_chr[n] of layer n into the blend player. */
void frame_init_b(FRW *w, int n) {
    u16 id = w->sub_chr[n];
    FRMDL *mdl = w->mdl;
    u32 *han;
    int no = id % 1000;

    if (w->x10 == 0 || (w->x1E != 0 && w->x1F != 0)) {
        if (id >= 1000) {
            han = &motion_set_handle_tbl[w->id * 300 + 500];
            han += pl_mot_han_ofs[w->id][no / 100];
        } else {
            han = motion_set_handle_tbl;
            han += com_mot_han_ofs[no / 100];
        }
    } else if (id >= 1000) {
        han = &motion_set_handle_tbl[w->mdl_no * 600 + 1700];
        han += em_mot_han_ofs[w->mdl_no][no / 100];
    } else {
        system_error(lit_277_003584C0, (s16)id, 0, 0);
    }
    if (w->x10 == 0 || w->x1E != 0) {
        han += no % 100;
    } else {
        han = (u32 *)((u8 *)han + (no % 100) * 4);
    }
    flSetMotionEx(mdl->mot1, *han, n);
}

/* Steps every motion layer one tick; returns 1 when a layer passed its end. */
int frame_move(FRW *w) {
    FRMDL *mdl;
    int end = 0;
    FRSKL *skl;
    u16 n;
    f32 vel[4];
    f32 va[4];
    f32 vb[4];
    f32 ra;
    f32 rb;
    f32 f;

    n = 0;
    mdl = w->mdl;
    skl = mdl->skl;
    for (; n < 4; n++) {
        if (n >= w->layers) {
            break;
        }
        if (w->mt[n].b_dir != 0) {
            flPlayMotionExSI(w->mt[n].frame, mdl->mot0, n);
            flPlayMotionExSI(w->mt[n].b_frame, mdl->mot1, n);
            calc_ofs_velocity(w->mt[0].frame, w->mt[0].frame + w->mt[0].spd, va, mdl->mot0->xD0);
            rb = plFCVFcurveInterpolateHermite(w->mt[n].b_rate, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f);
            ra = 1.0f - rb;
            flBlendMotionEx(mdl->mot0, mdl->mot1, n, ra, rb);
            if (n == 0) {
                calc_ofs_velocity(w->mt[0].b_frame, w->mt[0].b_frame + w->mt[0].spd, vb, mdl->mot1->xD0);
                calc_velocity(vel, va, vb, ra, rb);
                if (w->mt[n].b_dir > 0) {
                    pl_velocity_sub(w, vel);
                }
                skl->vel[0] = 0.0f;
                skl->vel[1] = 0.0f;
                skl->vel[2] = 0.0f;
            }
            w->mt[n].b_rate += w->mt[n].b_step;
            if (--w->mt[n].b_cnt <= 0.0f) {
                w->mt[n].b_dir = 0;
                flSetMotionEx(mdl->mot0, *w->mt[n].b_han, n);
                w->mt[n].end = w->mt[n].b_end;
                w->mt[n].loop = w->mt[n].b_loop;
                w->mt[n].loopfr = w->mt[n].b_loopfr;
                w->mt[n].old = w->mt[n].b_frame;
                w->mt[n].frame = w->mt[n].b_frame + w->mt[n].spd;
                w->mt[n].b_cnt = 0;
            }
        } else {
            flPlayMotionExSI(w->mt[n].frame, mdl->mot0, n);
            if (w->sub_on[n] != 0) {
                frame_init_b(w, n);
                flPlayMotionExSI(w->mt[n].frame, mdl->mot1, n);
                flBlendMotionEx(mdl->mot0, mdl->mot1, n, 1.0f - w->sub_rate[n] / 100.0f, w->sub_rate[n] / 100.0f);
            }
            if (n == 0) {
                calc_ofs_velocity(w->mt[n].frame - w->mt[n].spd, w->mt[n].frame, vel, mdl->mot0->xD0);
                pl_velocity_sub(w, vel);
                vel[0] = skl->vel[0] - w->mt[0].ofs[0];
                vel[1] = skl->vel[1] - w->mt[0].ofs[1];
                vel[2] = skl->vel[2] - w->mt[0].ofs[2];
                w->mt[0].ofs[0] = skl->vel[0];
                w->mt[0].ofs[1] = skl->vel[1];
                w->mt[0].ofs[2] = skl->vel[2];
                skl->vel[0] = 0.0f;
                skl->vel[1] = 0.0f;
                skl->vel[2] = 0.0f;
            }
            w->mt[n].old = w->mt[n].frame;
            w->mt[n].frame += w->mt[n].spd;
            if (w->mt[n].frame > w->mt[n].end) {
                if (w->mt[n].loop != 0) {
                    f = w->mt[n].loopfr;
                    if (!(f < 1.0f)) {
                        f -= 1.0f;
                    }
                    flPlayMotionExSI(f, mdl->mot0, n);
                    if (n == 0) {
                        w->mt[n].ofs[0] = skl->vel[0];
                        w->mt[n].ofs[1] = skl->vel[1];
                        w->mt[n].ofs[2] = skl->vel[2];
                    }
                    flPlayMotionExSI(w->mt[n].end, mdl->mot0, n);
                    w->mt[n].frame = w->mt[n].loopfr;
                    if (n == 0) {
                        skl->vel[0] = 0.0f;
                        skl->vel[1] = 0.0f;
                        skl->vel[2] = 0.0f;
                    }
                    w->mt[n].stat = 0;
                    end = 1;
                } else {
                    end = 1;
                    w->mt[n].frame = w->mt[n].end;
                    w->mt[n].stat = 0;
                }
                if (w->sub_on[n] != 0) {
                    flPlayMotionExSI(w->mt[n].frame, mdl->mot0, n);
                    frame_init_b(w, n);
                    flPlayMotionExSI(w->mt[n].frame, mdl->mot1, n);
                    flBlendMotionEx(mdl->mot0, mdl->mot1, n, 1.0f - w->sub_rate[n] / 100.0f, w->sub_rate[n] / 100.0f);
                }
            } else {
                w->mt[n].stat = 1;
            }
        }
    }
    return end;
}

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
