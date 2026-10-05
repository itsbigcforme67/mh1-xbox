/* em19_flyinit - game.bin 0x005EB3B0-0x005EB4A0: em19_fly_adjy2_init,
 * the setup for em19's flight curves in fly.c (it sits right before that
 * file; the boundary between them is a guess). */
#include "em.h"

typedef struct EM19W {
    u8 _pad00[0x2C];
    u8 adj_x;           /* 0x2C fly_adjy2 channels on/off */
    u8 adj_y;           /* 0x2D */
    u8 adj_z;           /* 0x2E */
    u8 adj_type;        /* 0x2F table row */
    s16 adj_tm;         /* 0x30 time into the table */
} EM19W;

void em_rate_clear(EMW *);

void em19_fly_adjy2_init(EMW *em, u8 type) {
    EM19W *w = (EM19W *)em->ex;

    w->adj_x = 0;
    w->adj_y = 0;
    w->adj_z = 0;
    w->adj_type = type;
    switch (w->adj_type) {
    case 0:
        w->adj_y = 1;
        w->adj_z = 1;
        w->adj_tm = 10;
        break;
    case 1:
        w->adj_y = 1;
        w->adj_z = 1;
        w->adj_tm = 10;
        break;
    case 2:
        w->adj_y = 1;
        w->adj_z = 1;
        w->adj_tm = 4;
        break;
    case 3:
        w->adj_y = 1;
        w->adj_z = 1;
        w->adj_tm = 6;
        break;
    case 4:
        w->adj_y = 1;
        w->adj_z = 1;
        w->adj_tm = 6;
        break;
    case 5:
        w->adj_y = 1;
        w->adj_z = 1;
        w->adj_tm = 0;
        break;
    case 6:
        w->adj_y = 1;
        w->adj_z = 1;
        w->adj_tm = 3;
        break;
    }
    em_rate_clear(em);
}
