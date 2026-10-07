/* lb_pz01 - lobby.bin 0x00594260-0x0059440C: plaza_moveMain, dispatches the plaza sub menu chosen in pNet->sel (void; each callee gets pNet as its argument). The one-case switches on the result (== 3 then tl_exit_sub_menu) are the original's. */
#pragma readonly_strings on
#include "lbui_proto.h"
void plaza_moveMain(void)
{
    LB_NETW *a = pNet;

    switch (a->sel) {
    case 0:
        switch (plaza_enterLobby(a)) {
        case 3:
            tl_exit_sub_menu(0);
            break;
        }
        break;
    case 1:
        plaza_movePlaza(a);
        break;
    case 2:
        plaza_backToServer(a);
        break;
    case 3:
        switch (plaza_checkFriend(a)) {
        case 3:
            tl_exit_sub_menu(0);
            break;
        }
        break;
    case 5:
        plaza_searchAll(a);
        break;
    case 6:
        plaza_searchMember(a);
        break;
    case 7:
        plaza_checkMyStatus(a);
        break;
    case 8:
        switch (plaza_setMyComment(a)) {
        case 3:
            tl_exit_sub_menu(0);
            break;
        }
        break;
    case 4:
        switch (plaza_mailBox(a)) {
        case 3:
            tl_exit_sub_menu(0);
            break;
        }
        break;
    case 9:
        plaza_setChatMode(a);
        break;
    case 10:
        plaza_ReibunEdit(a);
        break;
    case 11:
        plaza_checkChatLog(a);
        break;
    case 12:
        plaza_capcomPage(a);
        break;
    case 13:
        plaza_logOut(a);
        break;
    case 14:
    default:
        plaza_chatMain(a);
        break;
    }
}
