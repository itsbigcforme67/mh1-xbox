/* set13 - SLPM_654.95 0x001569E0-0x00157080 (first matching run of the
 * file 0x001569E0-0x00158F18; set13_m and set13_trans are still asm, the
 * rest is in set13b.c / set13c.c). Sun glare / lens flare: the alpha
 * (SETW+0x30) starts per stage, on some stages from where the master player
 * stands (inside or outside a building); arg picks the kind. Guessed from
 * sun_pos_tbl and the camera maths in set13_m. */
#include "set.h"
#include "game.h"
#include "pl.h"
#include "prim.h"

void flvecCopy(f32 *, f32 *);

void set13_move(SETW *sw);
void set13_i(SETW *sw);
void set13_m(SETW *sw);
void set13_d(SETW *sw);
void set13_e(SETW *sw);
void set13_trans(PRIM *pr);

void Set13_set(int arg) {
    SETW *sw = pull_set_work(0);

    if (sw != 0) {
        sw->type = 13;
        sw->arg = arg;
        sw->move = set13_move;
    }
}

void Set13_set2(int arg, f32 *pos, int n) {
    SETW *sw;

    if ((sw = pull_set_work(0)) != 0) {
        sw->type = 13;
        sw->arg = arg;
        flvecCopy(sw->pos, pos);
        sw->se1 = n;
        sw->move = set13_move;
    }
}

void set13_move(SETW *sw) {
    switch (sw->mode) {
    case 0:
        set13_i(sw);
        break;
    case 1:
        set13_m(sw);
        break;
    case 2:
        set13_d(sw);
        break;
    case 3:
        set13_e(sw);
        break;
    }
}

void set13_i(SETW *sw) {
    PLW *pl = &player_work[game_w.master];

    sw->mode++;
    sw->be_flag = 1;
    sw->timer = 0;
    sw->cnt = 0;
    switch (game_w.stage) {
    case 0x00:
    case 0x05:
    case 0x12:
    case 0x16:
    case 0x17:
    case 0x2F:
    case 0x30:
    case 0x31:
    case 0x46:
    case 0x49:
    case 0x4B:
        sw->speed = 1.0f;
        break;
    case 0x18:
    case 0x22:
        sw->speed = 0.1f;
        break;
    case 0x23:
        sw->speed = 0.3f;
        break;
    case 0x33:
        if (pl->pos[0] < 8750.0f) {
            sw->speed = 0.0f;
        } else {
            sw->speed = 1.0f;
        }
        break;
    case 0x34:
        if (pl->pos[0] > 3000.0f && pl->pos[0] < 15000.0f && pl->pos[2] > 3000.0f && pl->pos[2] < 8000.0f) {
            sw->speed = 0.0f;
        } else {
            sw->speed = 1.0f;
        }
        break;
    case 0x37:
        if (pl->pos[0] > 0.0f && pl->pos[0] < 6200.0f && pl->pos[2] > 0.0f && pl->pos[2] < 9300.0f) {
            sw->speed = 0.0f;
        } else {
            sw->speed = 1.0f;
        }
        break;
    case 0x39:
        if (pl->pos[2] > 100.0f && pl->pos[2] < 10400.0f) {
            sw->speed = 1.0f;
        } else {
            sw->speed = 0.0f;
        }
        break;
    case 0x2D:
        if (game_w.info_stop == 1) {
            sw->se0 = 2;
            sw->speed = 1.0f;
        } else if (pl->pos[0] > 3400.0f && pl->pos[0] < 17200.0f && pl->pos[2] > 3400.0f &&
                   pl->pos[2] < 16600.0f) {
            sw->se0 = 2;
            sw->speed = 1.0f;
        } else if (pl->pos[0] > 2800.0f && pl->pos[0] < 17800.0f && pl->pos[2] > 2800.0f &&
                   pl->pos[2] < 17800.0f) {
            sw->se0 = 3;
            sw->speed = 0.0f;
        } else {
            sw->se0 = 0;
            sw->speed = 1.0f;
        }
        break;
    case 0x38:
        if (game_w.info_stop == 1) {
            sw->se0 = 2;
            sw->speed = 1.0f;
        } else if (pl->pos[0] > 11800.0f && pl->pos[0] < 16600.0f && pl->pos[2] > 2800.0f &&
                   pl->pos[2] < 16600.0f) {
            sw->se0 = 2;
            sw->speed = 1.0f;
        } else if (pl->pos[0] > 100.0f && pl->pos[0] < 17200.0f && pl->pos[2] > 2200.0f &&
                   pl->pos[2] < 17200.0f) {
            sw->se0 = 3;
            sw->speed = 0.0f;
        } else {
            sw->se0 = 0;
            sw->speed = 1.0f;
        }
        break;
    default:
        sw->speed = 0.4f;
        break;
    }
    sw->work14 = 0;
    sw->prim_no = get_prim();
    if (sw->prim_no != -1) {
        sw->prim = get_prim_ptr(sw->prim_no);
        sw->prim->owner = sw;
        sw->prim->trans = set13_trans;
    } else {
        push_set_work(sw);
    }
}
