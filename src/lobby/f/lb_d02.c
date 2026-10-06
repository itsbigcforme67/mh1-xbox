/* lb_d02 - lobby send functions 0x005D5F90-0x005D6140: Lb_send_myChair, Lb_send_trade_start, Lb_send_trade_startTU, Lb_send_item_request. Whole file in lb_d.c. */
#include "lobby_f.h"
















void Lb_send_myChair(void) {
    lb_send_data(1, 0x11, &lb_sys.x66);
    Lb_send_data_to_myself(1, 0x11, &lb_sys.x66);
}

void Lb_send_trade_start(PLW *pl) {
    LBPKTRD t;
    memcpy(t.id, CWPLAYER(pl->work909) + 0x132C, 8);
    t.item = pl->work904;
    t.num = pl->work906;
    lb_send_data(0xE, 0xC, &t);
}

void Lb_send_trade_startTU(PLW *pl, int a1) {
    LBPKTRD t;
    memcpy(t.id, CWPLAYER(pl->work909) + 0x132C, 8);
    t.item = pl->work904;
    t.num = pl->work906;
    lb_send_dataTU(0xE, 0xC, &t, a1);
}

void Lb_send_item_request(int a0, PLW *pl) {
    LBPKTRD t;
    memcpy(t.id, CWPLAYER(pl->work909) + 0x132C, 8);
    t.item = pl->work904;
    t.num = pl->work906;
    lb_send_dataTU(0xE, 0xD, &t, a0);
}
