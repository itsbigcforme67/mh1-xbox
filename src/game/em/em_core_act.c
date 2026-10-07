/* em_core_act - game.bin 0x00536040-0x0053610C: em_act_search (weighted random pick from an {rate, act} list ended by rate 0xFFFF). Whole file in em_core_nm.c. */
#include "em_sys.h"
#include "game.h"

typedef struct EM_ACTRATE {
    u16 rate;           /* 0x0 weight, 0xFFFF ends the list */
    u16 act;            /* 0x2 */
} EM_ACTRATE;

u16 ran_suu(int);

u16 em_act_search(EM_ACTRATE *tbl) {
    EM_ACTRATE *p = tbl;
    u16 sum = 0;
    u16 r;
    u16 x;

    while ((x = p->rate) != 0xFFFF) {
        sum += x;
        p++;
    }
    r = ran_suu(0) % sum;
    sum = 0;
    while (tbl->rate != 0xFFFF) {
        sum += tbl->rate;
        if (r < sum) {
            return tbl->act;
        }
        tbl++;
    }
    return 0xFFFF;
}
