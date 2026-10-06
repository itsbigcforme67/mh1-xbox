/* lb_pz11 - lobby.bin 0x00597480-0x0059763C: plaza_setMyComment(), edits the own plaza comment (cw comment slot of the master), returns 3 = cancelled, 2 = running. u16 sw read first, pNet cached after the call, step through a pointer. Was a 14-instruction near-match in lbui_nm.c. */
#pragma readonly_strings on
#include "lbui_proto.h"
extern u8 D_3C73B4[];
int my_comment_input();

int plaza_setMyComment(void) {
    u16 sw = Get_sw2(0);
    LB_NETW *n = pNet;
    u8 *stp = &n->step;

    switch (*stp) {
    case 0:
        (*stp)++;
        memcpy(CW->comment[game_w.master], D_3C73B4, 0x62);
        break;
    case 1:
        if (sw & 0x20) {
            (*stp)++;
            cnWrap_SoundRequest(0);
            break;
        }
        if (sw & 0x40) {
            memcpy(D_3C73B4, CW->comment[game_w.master], 0x62);
            return 3;
        }
        break;
    case 2:
        if (my_comment_input(CW->comment[game_w.master]) == 1) {
            KinshiYogo_chk(CW->comment[game_w.master]);
            memcpy(D_3C73B4, CW->comment[game_w.master], 0x62);
            pNet->step--;
        }
        break;
    }
    return 2;
}
