/* lb_n10 - lobby 0x005D8370-0x005D8408: Lb_get_pl_stat2 (1 = member present and not busy, 2 = no commer data, 0 = offline). Whole file in lb_n.c. */
#include "lobby_f.h"

int Lb_get_pl_stat2(int a0) {
    s8 i;
    if (Online_ck() == 0) {
        return 0;
    }
    i = a0;
    if (lbCommer[i].mac[0] != 0) {
        return PLU8(&player_work[i], 0x736) == 0 ? 1 : 0;
    }
    return 2;
}
