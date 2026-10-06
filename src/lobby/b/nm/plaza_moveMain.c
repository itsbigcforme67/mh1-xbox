#include "lobby_f.h"
extern u8 * pNet;
void plaza_moveMain(void) {
    u8 temp_v0;
    void *temp_a0;

    temp_a0 = pNet;
    temp_v0 = F(u8, temp_a0, 7);
    switch (temp_v0) {
    case 0:
        if (plaza_enterLobby(temp_a0) != 3) {
            return;
        }
        tl_exit_sub_menu(0);
        return;
    case 1:
        plaza_movePlaza();
        return;
    case 2:
        plaza_backToServer();
        return;
    case 3:
        if (plaza_checkFriend(temp_a0) != 3) {
            return;
        }
        tl_exit_sub_menu(0);
        return;
    case 5:
        plaza_searchAll();
        return;
    case 6:
        plaza_searchMember();
        return;
    case 7:
        plaza_checkMyStatus();
        return;
    case 8:
        if (plaza_setMyComment(temp_a0) != 3) {
            return;
        }
        tl_exit_sub_menu(0);
        return;
    case 4:
        if (plaza_mailBox(temp_a0) != 3) {
            return;
        }
        tl_exit_sub_menu(0);
        return;
    case 9:
        plaza_setChatMode();
        return;
    case 10:
        plaza_ReibunEdit();
        return;
    case 11:
        plaza_checkChatLog();
        return;
    case 12:
        plaza_capcomPage();
        return;
    case 13:
        plaza_logOut();
        return;
    default:
        plaza_chatMain();
        return;
    }
}
