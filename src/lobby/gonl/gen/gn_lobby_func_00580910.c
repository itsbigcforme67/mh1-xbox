/* G online (PC): lobby_func_00580910, written from Monster Hunter G's code at lobby 0x580910 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 CallBackWaitInit();
s32 CallBack_Result_SearchUserPlace();
s32 Check_CallBackWait();
extern u8 cw;
s32 Visual_main();                               /* extern */
s32 cnLBS_Get_ServerMessage();       /* extern */
s32 lobby_func_005E69B0();   /* extern */
s32 memcmp();               /* extern */
extern u8 my_user_id;

s32 lobby_func_00580910(s8 *arg0) {
    s32 temp_a1;
    u8 temp_v1_2;
    u8 *temp_v1;
    u8 *temp_v1_3;

    temp_v1 = (*(void **)&cw);
    temp_a1 = temp_v1 + 0xB9D;
    temp_v1_2 = (*(u8 *)((u8 *)(temp_v1) + 0xB9D));
    switch (temp_v1_2) {                            /* irregular */
    case 0x0:
        if ((*arg0 == 0) || (Visual_main(temp_a1) < 6)) {
            (*(u8 *)((u8 *)((*(void **)&cw)) + 0xB9D)) = 0x63U;
            goto block_17;
        }
        if (memcmp(&my_user_id, arg0, 8) == 0) {
            return 3;
        }
        temp_v1_3 = (*(void **)&cw);
        (*(u8 *)((u8 *)(temp_v1_3) + 0xB9D)) = (u8) ((*(u8 *)((u8 *)(temp_v1_3) + 0xB9D)) + 1);
        CallBackWaitInit();
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xBA9)) = 0xE;
        lobby_func_005E69B0(arg0, CallBack_Result_SearchUserPlace);
block_17:
    default:
        return 2;
    case 0x1:
        Check_CallBackWait();
        goto block_17;
    case 0x2:
        (*(u8 *)((u8 *)(temp_v1) + 0xB9D)) = 0U;
        return 0;
    case 0x63:
        (*(u8 *)((u8 *)(temp_v1) + 0xB9D)) = 0U;
        cnLBS_Get_ServerMessage((*(void **)&cw) + 0xF75D, temp_a1);
        return 1;
    }
}

