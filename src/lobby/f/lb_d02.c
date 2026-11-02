/* lb_d02 - lobby send functions 0x005D5F90-0x005D6140: Lb_send_myChair, Lb_send_trade_start, Lb_send_trade_startTU, Lb_send_item_request. Whole file in lb_d.c. */
#include "lobby_f.h"
















#if !defined(G_PART) || G_PART == 1
#ifdef VERSION_MH2
#ifdef VERSION_MH2
#include "mh2_lb.h"
#endif
void Lb_send_myChair(void) {
    lb_send_data(1, 0x1D, &lb_sys.x66);
    Lb_send_data_to_myself(1, 0x1D, &lb_sys.x66);
}
#else
void Lb_send_myChair(void) {
    lb_send_data(1, MH2V(0x11, 0x1D), &lb_sys.x66);
    Lb_send_data_to_myself(1, MH2V(0x11, 0x1D), &lb_sys.x66);
}
#endif
#endif

#if !defined(G_PART)
#ifdef VERSION_MH2
void Lb_send_trade_start(PLW *pl) {
    LBPKTRD t;
    memcpy(t.id, CWPLAYER(PL_W909(pl)) + 0xA16, 8);
    t.item = PL_W904(pl);
    t.num = PL_W906(pl);
    lb_send_data(0xE, 0xA, &t);
}
#else
void Lb_send_trade_start(PLW *pl) {
    LBPKTRD t;
    memcpy(t.id, CWPLAYER(pl->work909) + 0x132C, 8);
    t.item = pl->work904;
    t.num = pl->work906;
    lb_send_data(0xE, 0xC, &t);
}
#endif
#endif

#if !defined(G_PART)
#ifdef VERSION_MH2
void Lb_send_trade_startTU(PLW *pl, int a1) {
    LBPKTRD t;
    memcpy(t.id, CWPLAYER(PL_W909(pl)) + 0xA16, 8);
    t.item = PL_W904(pl);
    t.num = PL_W906(pl);
    lb_send_dataTU(0xE, 0xA, &t, a1);
}
#else
void Lb_send_trade_startTU(PLW *pl, int a1) {
    LBPKTRD t;
    memcpy(t.id, CWPLAYER(pl->work909) + 0x132C, 8);
    t.item = pl->work904;
    t.num = pl->work906;
    lb_send_dataTU(0xE, 0xC, &t, a1);
}
#endif
#endif

#if !defined(G_PART)
#ifdef VERSION_MH2
void Lb_send_item_request(int a0, PLW *pl) {
    LBPKTRD t;
    memcpy(t.id, CWPLAYER(PL_W909(pl)) + 0xA16, 8);
    t.item = PL_W904(pl);
    t.num = PL_W906(pl);
    lb_send_dataTU(0xE, 0xB, &t, a0);
}
#else
void Lb_send_item_request(int a0, PLW *pl) {
    LBPKTRD t;
    memcpy(t.id, CWPLAYER(pl->work909) + 0x132C, 8);
    t.item = pl->work904;
    t.num = pl->work906;
    lb_send_dataTU(0xE, 0xD, &t, a0);
}
#endif
#endif
