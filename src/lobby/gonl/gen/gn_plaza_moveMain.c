/* G online (PC): plaza_moveMain, written from Monster Hunter G's code at lobby 0x5699E0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

#ifdef __MWERKS__
s32 Get_sw2();
#else
unsigned short Get_sw2();   /* the real return type (tools/check_kr_returns.py) */
#endif
s32 lobby_func_00569D40();
s32 lobby_func_0056B0C0();
s32 lobby_func_0056B4E0();
s32 lobby_func_0056B790();
s32 lobby_func_0056C550();
s32 lobby_func_0056ED50();
s32 lobby_func_00570320();
s32 lobby_func_00570E80();
s32 plaza_ReibunEdit();
s32 plaza_backToServer();
s32 plaza_capcomPage();
s32 plaza_chatMain();
s32 plaza_checkChatLog();
s32 plaza_checkFriend();
s32 plaza_checkMyStatus();
s32 plaza_logOut();
s32 plaza_setMyComment();
s32 tl_exit_sub_menu();
extern u8 D_0035EFEB;
extern u8 pNet;
void plaza_moveMain(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    u8 temp_v0;

    temp_v0 = (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x28));
    switch (temp_v0) {
    case 0:
        if (lobby_func_00569D40(arg0, arg1, arg2, arg3) != 3) {
            return;
        }
        tl_exit_sub_menu(0, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        return;
    case 1:
        lobby_func_0056B0C0(arg0, arg1, arg2, arg3);
        return;
    case 2:
        plaza_backToServer(arg0, arg1, arg2, arg3);
        return;
    case 3:
        if (lobby_func_0056C550(Get_sw2(0), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */)) != 3) {
            return;
        }
        (*(s8 *)&D_0035EFEB) = 1;
        tl_exit_sub_menu(0, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        return;
    case 5:
        lobby_func_0056ED50(arg0, arg1, arg2, arg3);
        return;
    case 6:
        lobby_func_00570320(arg0, arg1, arg2, arg3);
        return;
    case 7:
        plaza_checkMyStatus(arg0, arg1, arg2, arg3);
        return;
    case 8:
        if (plaza_setMyComment(Get_sw2(0), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */)) != 3) {
            return;
        }
        tl_exit_sub_menu(0, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        return;
    case 4:
        if (plaza_checkFriend(Get_sw2(0), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */)) != 3) {
            return;
        }
        tl_exit_sub_menu(0, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        return;
    case 9:
        if (lobby_func_00570E80(arg0, arg1, arg2, arg3) != 3) {
            return;
        }
        tl_exit_sub_menu(0, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        return;
    case 10:
        plaza_ReibunEdit(arg0, arg1, arg2, arg3);
        return;
    case 11:
        plaza_checkChatLog(arg0, arg1, arg2, arg3);
        return;
    case 12:
        plaza_capcomPage(arg0, arg1, arg2, arg3);
        return;
    case 13:
        plaza_logOut(arg0, arg1, arg2, arg3);
        return;
    case 14:
        if (lobby_func_0056B790(arg0, arg1, arg2, arg3) != 3) {
            return;
        }
        tl_exit_sub_menu(0, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        return;
    case 15:
        lobby_func_0056B4E0(arg0, arg1, arg2, arg3);
        return;
    default:
        plaza_chatMain(arg0, arg1, arg2, arg3);
        return;
    }
}

