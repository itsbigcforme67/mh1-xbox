/* set12 - SLPM_654.95 0x001567C0-0x001569D8.
 * A set object that plays a sound every 5 frames until its timer runs out. */
#include "set.h"

void set12_move(SETW *sw);

void set12_set(int se0, int se1, int arg, f32 *pos, s16 timer) {
    SETW *sw;

    se_req2(se0, se1, 0, pos, arg, 0);
    sw = pull_set_work(0);
    if (sw != 0) {
        sw->type = 12;
        sw->move = set12_move;
        sw->arg = arg;
        sw->se0 = se0;
        sw->se1 = se1;
        sw->pos[0] = pos[0];
        sw->pos[1] = pos[1];
        sw->pos[2] = pos[2];
        sw->timer = timer;
    }
}

void set12_i(SETW *sw);
void set12_m(SETW *sw);
void set12_d(SETW *sw);
void set12_e(SETW *sw);

void set12_move(SETW *sw) {
    switch (sw->mode) {
    case 0:
        set12_i(sw);
        break;
    case 1:
        set12_m(sw);
        break;
    case 2:
        set12_d(sw);
        break;
    case 3:
        set12_e(sw);
        break;
    }
}

void set12_i(SETW *sw) {
    sw->mode++;
    sw->be_flag = 1;
    sw->work14 = 0;
    sw->cnt = 0;
}

void set12_m(SETW *sw) {
    if ((u16)sw->timer != 0xFFFF) {
        if (--sw->timer <= 0) {
            sw->mode++;
        }
    }
    sw->cnt++;
    if (sw->cnt >= 5) {
        sw->cnt = 0;
        se_req2(sw->se0, sw->se1, 0, sw->pos, sw->arg, 1);
    }
}

void set12_d(SETW *sw) {
    sw->mode++;
    sw->be_flag = 0;
}

void set12_e(SETW *sw) {
    push_set_work(sw);
}
