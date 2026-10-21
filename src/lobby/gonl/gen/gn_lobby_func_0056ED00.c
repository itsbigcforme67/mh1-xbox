/* G online (PC): lobby_func_0056ED00, written from Monster Hunter G's code at lobby 0x56ED00 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 pNet;
extern u8 tl_member_buff;
s32 SetDialogData();                     /* extern */
s32 SetDialogYesNo();                    /* extern */
s32 cnLBS_Get_ConditionSearchUser();   /* extern */
s32 memset();        /* extern */
extern s32 g_main_2EFBC4;

void lobby_func_0056ED00(void) {
    memset(&(*(u8 *)((u8 *)&tl_member_buff + 0x18)), 0, 0x1376);
    SetDialogData(0x48);
    SetDialogYesNo(0);
    (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
    cnLBS_Get_ConditionSearchUser(&g_main_2EFBC4);
}

