/* Motion system, matching part 2 (SLPM_654.95 0x001257D0-0x001263EC):
 * aan_ctr_get .. frame_init_b, frame_move. See f_frame_nm.c for the whole file and
 * what the motion system does. */
#include "types.h"
#include "game.h"
#include "pl.h"
#include "em.h"
#include "frame.h"

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

/* Steps every motion layer one tick (loops/ends, blends motion b in, accumulates root velocity). */
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
    u16 new_var;
    f32 f;

    n = 0;
    mdl = w->mdl;
    skl = mdl->skl;
    for (; n < 4; n++) {
        do { /* permuter: matching scheduling */
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
                    new_var = n;
                    flPlayMotionExSI(w->mt[new_var].frame, mdl->mot0, n);
                    frame_init_b(w, n);
                    flPlayMotionExSI(w->mt[n].frame, mdl->mot1, n);
                    flBlendMotionEx(mdl->mot0, mdl->mot1, n, 1.0f - w->sub_rate[n] / 100.0f, w->sub_rate[n] / 100.0f);
                }
            } else {
                w->mt[n].stat = 1;
            }
        }
        } while (0);
    }
    return end;
}
