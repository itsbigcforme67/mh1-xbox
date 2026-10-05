/* set14 - game.bin 0x006229B0-0x006233F4 (set14_trans, the draw function,
 * is still assembly). A stage overlay with scrolling textures: on stages
 * 0 and 26 it follows the master player; on stages 51-53 two of five
 * masked layers fade in and out at random intervals (mode2/se0 pick the
 * layers, timer/cnt count up to se1/x1E). */
#include "set.h"
#include "game.h"
#include "pl.h"
#include "prim.h"

extern PLW player_work[];
extern u8 ot3[];

u32 ran_suu(int);
void set14_trans(PRIM *pr);

static void set14_move(SETW *sw);
static void set14_i(SETW *sw);
static void set14_m(SETW *sw);
static void set14_d(SETW *sw);
static void set14_e(SETW *sw);

void set14_set(void) {
    SETW *sw = pull_set_work(0);

    if (sw != 0) {
        sw->type = 0xE;
        sw->arg = 0;
        sw->move = set14_move;
    }
}

static void set14_move(SETW *sw) {
    switch (sw->mode) {
    case 0:
        set14_i(sw);
        break;
    case 1:
        set14_m(sw);
        break;
    case 2:
        set14_d(sw);
        break;
    case 3:
        set14_e(sw);
        break;
    }
}

static void set14_i(SETW *sw) {
    sw->mode++;
    sw->be_flag = 1;
    sw->cnt = 0;
    sw->timer = 0;
    switch (game_w.stage) {
    case 0:
    case 1:
    case 3:
    case 0x1A:
        sw->pos[0] = 0.0f;
        sw->pos[1] = 0.0f;
        sw->pos[2] = 0.0f;
        break;
    case 4:
        sw->pos[0] = 11060.0f;
        sw->pos[1] = 0.0f;
        sw->pos[2] = 1566.0f;
        break;
    case 0x2A:
        sw->pos[0] = 7227.0f;
        sw->pos[1] = -34.0f;
        sw->pos[2] = 5795.0f;
        break;
    case 0x33:
        sw->pos[0] = 9400.0f;
        sw->pos[1] = 18.0f;
        sw->pos[2] = 11400.0f;
        sw->mode2 = (u16)ran_suu(1) % 5;
        sw->timer = (u16)ran_suu(1) & 0xF;
        sw->se1 = (u16)ran_suu(1) % 10 + 0x37;
        sw->se0 = (u16)ran_suu(1) % 4 + 1;
        sw->se0 += sw->mode2;
        if (sw->se0 >= 5) {
            sw->se0 -= 5;
        }
        sw->cnt = (u16)ran_suu(1) & 0xF;
        sw->x1E = (u16)ran_suu(1) % 10 + 0x37;
        break;
    case 0x34:
        sw->pos[0] = 9800.0f;
        sw->pos[1] = 70.0f;
        sw->pos[2] = 12600.0f;
        sw->mode2 = (u16)ran_suu(1) % 5;
        sw->timer = (u16)ran_suu(1) & 0xF;
        sw->se1 = (u16)ran_suu(1) % 10 + 0x37;
        sw->se0 = (u16)ran_suu(1) % 4 + 1;
        sw->se0 += sw->mode2;
        if (sw->se0 >= 5) {
            sw->se0 -= 5;
        }
        sw->cnt = (u16)ran_suu(1) & 0xF;
        sw->x1E = (u16)ran_suu(1) % 10 + 0x37;
        break;
    case 0x35:
        sw->pos[0] = 8800.0f;
        sw->pos[1] = 13.0f;
        sw->pos[2] = 10400.0f;
        sw->mode2 = (u16)ran_suu(1) % 5;
        sw->timer = (u16)ran_suu(1) & 0xF;
        sw->se1 = (u16)ran_suu(1) % 10 + 0x37;
        sw->se0 = (u16)ran_suu(1) % 4 + 1;
        sw->se0 += sw->mode2;
        if (sw->se0 >= 5) {
            sw->se0 -= 5;
        }
        sw->cnt = (u16)ran_suu(1) & 0xF;
        sw->x1E = (u16)ran_suu(1) % 10 + 0x37;
        break;
    }
    sw->work14 = 0;
    sw->prim = get_prim_ptr(get_prim());
    if (sw->prim != 0) {
        sw->prim->owner = sw;
        sw->prim->trans = set14_trans;
    }
}

static void set14_m(SETW *sw) {
    PLW *pl = &player_work[game_w.master];

    switch (game_w.stage) {
    case 0:
    case 0x1A:
        sw->timer++;
        sw->pos[0] = pl->pos[0];
        sw->pos[1] = 0.0f;
        sw->pos[2] = pl->pos[2];
        break;
    case 0x33:
        if (++sw->timer > sw->se1) {
            sw->mode2 = (u16)ran_suu(1) % 4 + 1;
            sw->mode2 += sw->se0;
            if (sw->mode2 >= 5) {
                sw->mode2 -= 5;
            }
            sw->timer = 0;
            sw->se1 = (u16)ran_suu(1) % 10 + 0x37;
        }
        if (++sw->cnt > sw->x1E) {
            sw->se0 = (u16)ran_suu(1) % 4 + 1;
            sw->se0 += sw->mode2;
            if (sw->se0 >= 5) {
                sw->se0 -= 5;
            }
            sw->cnt = 0;
            sw->x1E = (u16)ran_suu(1) % 10 + 0x37;
        }
        break;
    case 0x34:
        if (++sw->timer > sw->se1) {
            sw->mode2 = (u16)ran_suu(1) % 4 + 1;
            sw->mode2 += sw->se0;
            if (sw->mode2 >= 5) {
                sw->mode2 -= 5;
            }
            sw->timer = 0;
            sw->se1 = (u16)ran_suu(1) % 10 + 0x37;
        }
        if (++sw->cnt > sw->x1E) {
            sw->se0 = (u16)ran_suu(1) % 4 + 1;
            sw->se0 += sw->mode2;
            if (sw->se0 >= 5) {
                sw->se0 -= 5;
            }
            sw->cnt = 0;
            sw->x1E = (u16)ran_suu(1) % 10 + 0x37;
        }
        break;
    case 0x35:
        if (++sw->timer > sw->se1) {
            sw->mode2 = (u16)ran_suu(1) % 4 + 1;
            sw->mode2 += sw->se0;
            if (sw->mode2 >= 5) {
                sw->mode2 -= 5;
            }
            sw->timer = 0;
            sw->se1 = (u16)ran_suu(1) % 10 + 0x37;
        }
        if (++sw->cnt > sw->x1E) {
            sw->se0 = (u16)ran_suu(1) % 4 + 1;
            sw->se0 += sw->mode2;
            if (sw->se0 >= 5) {
                sw->se0 -= 5;
            }
            sw->cnt = 0;
            sw->x1E = (u16)ran_suu(1) % 10 + 0x37;
        }
        break;
    default:
        sw->timer++;
        break;
    }
    if (sw->work14 == 0) {
        sw->prim->pos[0] = sw->pos[0];
        sw->prim->pos[1] = sw->pos[1];
        sw->prim->pos[2] = sw->pos[2];
        add_prim(ot3, sw->prim, 8, 1);
    }
}

static void set14_d(SETW *sw) {
    sw->mode++;
    sw->be_flag = 0;
}

static void set14_e(SETW *sw) {
    push_set_work(sw);
}
