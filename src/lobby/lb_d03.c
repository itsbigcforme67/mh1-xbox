/* lb_d03 - lobby send functions 0x005D61F0-0x005D6204: Lb_send_statusReq. Whole file in lb_d.c. */
#include "lobby.h"
















void Lb_send_statusReq(void) {
    lb_send_data(2, 0xF, D_3F3404);
}
