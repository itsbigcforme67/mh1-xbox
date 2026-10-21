/* G online (PC): lbc_login_finish_after, written from Monster Hunter G's code at lobby 0x57C1A0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 CallBack_SendMiniData();
s32 Check_CallBackWait();
s32 Fade_busy_ck();
s32 McCardOperation();
s32 McOperationSet();
s32 Q_camera_init();
s32 SetDialogData();
s32 To_LogOut();
s32 fade_set();
s32 lobby_func_0057C0B0();
s32 lobby_func_0057CC60();
s32 lobby_func_00580FA0();
s32 lobby_func_00581210();
s32 lobby_func_0058BAA0();
s32 str_stop_all();
extern u8 CnetWork;
extern u8 cw;
extern u8 jtbl_0063E490;
extern u8 pNet;
s32 cnLBS_Send_LoginFinish(); /* extern */
s32 cnLBS_Send_UserMiniData(); /* extern */
extern u8 my_user_mini_data;

void lbc_login_finish_after(void) {
    s32 temp_v0;
    s32 temp_v0_2;
    s32 temp_v0_3;
    s32 temp_v1;
    u8 temp_a1;
    u8 *temp_a0;
    u8 *temp_a0_2;
    u8 *temp_a0_3;
    u8 *temp_a2;
    u8 *temp_v1_2;

    temp_a2 = (*(void **)&cw);
    temp_a1 = (*(u8 *)((u8 *)(temp_a2) + 0xB9C));
    switch (temp_a1) {
    case 0:
        if ((*(u8 *)((u8 *)&CnetWork + 0x5)) == 3) {
            (*(u8 *)((u8 *)(temp_a2) + 0xB9C)) = (u8) (temp_a1 + 1);
            McOperationSet(7);
            if (Fade_busy_ck() & 0xFF) {
                fade_set(2);
            }
            (*(s32 *)((u8 *)((*(void **)&cw)) + 0xBB4)) = 3;
            str_stop_all();
            (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB70)) = 0;
            (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB97)) = 0;
        } else {
            (*(s8 *)((u8 *)(temp_a2) + 0xB97)) = 0;
            cnLBS_Send_LoginFinish(&jtbl_0063E490, temp_a1, temp_a2, temp_a2 + 0xB9C);
            (*(u8 *)((u8 *)((*(void **)&cw)) + 0xB9C)) = 2U;
            (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB9D)) = 0;
            (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB70)) = 1;
        }
block_39:
        if ((s32) (*(u8 *)((u8 *)((*(void **)&cw)) + 0xB9C)) >= 2) {
            (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
        }
        return;
    case 1:
        temp_v1 = (*(s32 *)((u8 *)(temp_a2) + 0xBB4));
        if (temp_v1 == 0) {
            if (McCardOperation() & 0xFF) {
                temp_v1_2 = (*(void **)&cw);
                (*(u8 *)((u8 *)(temp_v1_2) + 0xB9C)) = (u8) ((*(u8 *)((u8 *)(temp_v1_2) + 0xB9C)) + 1);
                cnLBS_Send_LoginFinish();
                SetDialogData(0x19);
                (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 0;
                (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB70)) = 1;
                return;
            }
            goto block_39;
        }
        (*(s32 *)((u8 *)(temp_a2) + 0xBB4)) = (s32) (temp_v1 - 1);
        goto block_39;
    case 2:
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
        temp_v0 = lobby_func_00581210();
        if ((temp_v0 != 0) && (temp_v0 != 1)) {
            goto block_39;
        }
        temp_a0 = (*(void **)&cw);
        (*(u8 *)((u8 *)(temp_a0) + 0xB9C)) = (u8) ((*(u8 *)((u8 *)(temp_a0) + 0xB9C)) + 1);
        return;
    case 3:
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
        temp_v0_2 = lobby_func_00580FA0();
        if ((temp_v0_2 != 0) && (temp_v0_2 != 1)) {
            goto block_39;
        }
        temp_a0_2 = (*(void **)&cw);
        (*(u8 *)((u8 *)(temp_a0_2) + 0xB9C)) = (u8) ((*(u8 *)((u8 *)(temp_a0_2) + 0xB9C)) + 1);
        return;
    case 4:
        (*(u8 *)((u8 *)(temp_a2) + 0xB9C)) = (u8) (temp_a1 + 1);
        (*(s32 *)((u8 *)((*(void **)&cw)) + 0xFA68)) = 0xE10;
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xFA65)) = 0;
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xBA9)) = 2;
        lobby_func_0058BAA0(&my_user_mini_data);
        cnLBS_Send_UserMiniData(&my_user_mini_data, 0x40, CallBack_SendMiniData);
        goto block_39;
    case 5:
        Check_CallBackWait();
        goto block_39;
    case 6:
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
        temp_v0_3 = lobby_func_0057C0B0();
        if ((temp_v0_3 != 0) && (temp_v0_3 != 1)) {

        } else {
            temp_a0_3 = (*(void **)&cw);
            (*(u8 *)((u8 *)(temp_a0_3) + 0xB9C)) = (u8) ((*(u8 *)((u8 *)(temp_a0_3) + 0xB9C)) + 1);
        }
        goto block_39;
    case 7:
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
        if ((Fade_busy_ck() & 0xFF) != 1) {
            if ((*(u8 *)((u8 *)&CnetWork + 0x5)) == 3) {
                lobby_func_0057CC60();
                (*(s8 *)((u8 *)((*(void **)&cw)) + 0xFA5E)) = 1;
            } else {
                lobby_func_0057CC60();
                (*(s8 *)((u8 *)((*(void **)&cw)) + 0xFA5E)) = 0;
            }
            (*(u8 *)((u8 *)&CnetWork + 0x5)) = 2;
            Q_camera_init();
        }
        goto block_39;
    default:
        To_LogOut(4);
        goto block_39;
    }
}

