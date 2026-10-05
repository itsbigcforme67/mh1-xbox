/* lb_c02 - lobby small helpers 0x005CB0E0-0x005CB0F4: Chk_lb_status. Whole file in lb_c.c. */
#include "lobby.h"













int Chk_lb_status(int n) {
    return lb_sys.x68 == n;
}
