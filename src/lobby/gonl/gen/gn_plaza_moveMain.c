/* G online (PC): plaza_moveMain, written from Monster Hunter G's code at lobby 0x5699E0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 D_0035EFEB;
extern u8 pNet;
s32 Get_sw2();                               /* extern */
s32 lobby_func_00569D40();                          /* extern */
s32 lobby_func_0056B0C0();                      /* extern */
s32 lobby_func_0056B4E0();                      /* extern */
s32 lobby_func_0056B790();                          /* extern */
s32 lobby_func_0056C550();                       /* extern */
s32 lobby_func_0056ED50();                      /* extern */
s32 lobby_func_00570320();                      /* extern */
s32 lobby_func_00570E80();                          /* extern */
s32 plaza_ReibunEdit();                         /* extern */
s32 plaza_backToServer();                       /* extern */
s32 plaza_capcomPage();                         /* extern */
s32 plaza_chatMain();                           /* extern */
s32 plaza_checkChatLog();                       /* extern */
s32 plaza_checkFriend();                         /* extern */
s32 plaza_checkMyStatus();                      /* extern */
s32 plaza_logOut();                             /* extern */
s32 plaza_setMyComment();                        /* extern */
s32 tl_exit_sub_menu();                  /* extern */

void plaza_moveMain(void) {
    u8 temp_v0;

    temp_v0 = (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x28));
    switch (temp_v0) {
    case 0:
        if (lobby_func_00569D40() != 3) {
            return;
        }
        tl_exit_sub_menu(0);
        return;
    case 1:
        lobby_func_0056B0C0();
        return;
    case 2:
        plaza_backToServer();
        return;
    case 3:
        if (lobby_func_0056C550(Get_sw2(0)) != 3) {
            return;
        }
        (*(s8 *)&D_0035EFEB) = 1;
        tl_exit_sub_menu(0);
        return;
    case 5:
        lobby_func_0056ED50();
        return;
    case 6:
        lobby_func_00570320();
        return;
    case 7:
        plaza_checkMyStatus();
        return;
    case 8:
        if (plaza_setMyComment(Get_sw2(0)) != 3) {
            return;
        }
        tl_exit_sub_menu(0);
        return;
    case 4:
        if (plaza_checkFriend(Get_sw2(0)) != 3) {
            return;
        }
        tl_exit_sub_menu(0);
        return;
    case 9:
        if (lobby_func_00570E80() != 3) {
            return;
        }
        tl_exit_sub_menu(0);
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
    case 14:
        if (lobby_func_0056B790() != 3) {
            return;
        }
        tl_exit_sub_menu(0);
        return;
    case 15:
        lobby_func_0056B4E0();
        return;
    default:
        plaza_chatMain();
        return;
    }
}

