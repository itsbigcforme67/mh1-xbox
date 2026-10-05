/* set13b - SLPM_654.95 0x00158130-0x00158188: set13_d / set13_e (see set13.c). */
#include "set.h"

void release_prim(s16);

void set13_d(SETW *sw) {
    sw->mode++;
    sw->be_flag = 0;
}

void set13_e(SETW *sw) {
    if (sw->prim != 0) {
        release_prim(sw->prim_no);
    }
    push_set_work(sw);
}
