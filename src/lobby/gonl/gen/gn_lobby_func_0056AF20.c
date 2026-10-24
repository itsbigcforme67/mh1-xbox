/* G online (PC): lobby_func_0056AF20, written from Monster Hunter G's code at lobby 0x56AF20 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 SetDialogData();
extern u8 g_main_2EF390;
extern u8 g_main_2EF390;
extern u8 ClassInfo;
extern u8 my_user_mini_data;

s32 lobby_func_0056AF20(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    u8 *temp_a0;

    if ((arg0 != 0) && (arg0 == ClassInfo) && (arg1 == 0)) {
        SetDialogData(0x16, arg1, arg2, arg3);
        return 0;
    }
    temp_a0 = (*(s32 *)&g_main_2EF390) + 0x23F4 + 0x20 + ((((s8) (((((s8) ((s32) arg2))) - 0x31)))) * 2);
    if ((s32) (*(u8 *)((u8 *)(&my_user_mini_data) + 1)) < (*(s8 *)((u8 *)(temp_a0) + 0x27E))) {
        SetDialogData(0xD, arg1, arg2, arg3);
        return 0;
    }
    if ((*(s8 *)((u8 *)(temp_a0) + 0x27F)) < (s32) (*(u8 *)((u8 *)(&my_user_mini_data) + 1))) {
        SetDialogData(0xD, arg1, arg2, arg3);
        return 0;
    }
    return 1;
}

