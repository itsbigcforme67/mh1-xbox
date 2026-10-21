/* G online (PC): lbc_login_finish_after, written from Monster Hunter G's code at lobby 0x57C1A0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 CnetWork;
extern u8 cw;
extern u8 jtbl_0063E490;
extern u8 pNet;
s32 CallBackWaitInit(); /* extern */
s32 Check_CallBackWait(); /* extern */
s32 Fade_busy_ck();    /* extern */
s32 McCardOperation();    /* extern */
s32 McOperationSet(); /* extern */
s32 Q_camera_init();                            /* extern */
s32 SetDialogData();                     /* extern */
s32 To_LogOut();     /* extern */
s32 cnLBS_Send_LoginFinish(); /* extern */
s32 cnLBS_Send_UserMiniData(); /* extern */
s32 fade_set();                          /* extern */
s32 lobby_func_0057C0B0(); /* extern */
s32 lobby_func_0057CC60();                   /* extern */
s32 lobby_func_00580FA0(); /* extern */
s32 lobby_func_00581210(); /* extern */
s32 lobby_func_0058BAA0();             /* extern */
s32 str_stop_all();                             /* extern */
extern u8 CallBack_SendMiniData;
extern u8 my_user_mini_data;

void lbc_login_finish_after(void) {
    s32 temp_a0_4;
    s32 temp_v0;
    s32 temp_v0_2;
    s32 temp_v0_3;
    s32 temp_v1;
    u8 temp_a1;
    u8 *temp_a0;
    u8 *temp_a0_2;
    u8 *temp_a0_3;
    u8 *temp_a2;
    u8 *temp_a3;
    u8 *temp_v1_2;

    temp_a2 = (*(void **)&cw);
    temp_a1 = (*(u8 *)((u8 *)(temp_a2) + 0xB9C));
    temp_a3 = temp_a2 + 0xB9C;
    switch (temp_a1) {
    case 0:
        if ((*(u8 *)((u8 *)&CnetWork + 0x5)) == 3) {
            (*(u8 *)((u8 *)(temp_a2) + 0xB9C)) = (u8) (temp_a1 + 1);
            McOperationSet(7, temp_a1, temp_a2, temp_a3);
            if (Fade_busy_ck() & 0xFF) {
                fade_set(2);
            }
            (*(s32 *)((u8 *)((*(void **)&cw)) + 0xBB4)) = 3;
            str_stop_all();
            (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB70)) = 0;
            (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB97)) = 0;
        } else {
            (*(s8 *)((u8 *)(temp_a2) + 0xB97)) = 0;
            cnLBS_Send_LoginFinish(&jtbl_0063E490, temp_a1, temp_a2, temp_a3);
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
            if (McCardOperation(temp_a2 + 0xBB4, temp_a1, temp_a2, temp_a3) & 0xFF) {
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
        temp_v0 = lobby_func_00581210(&jtbl_0063E490, temp_a1, temp_a2, temp_a3);
        if ((temp_v0 != 0) && (temp_v0 != 1)) {
            goto block_39;
        }
        temp_a0 = (*(void **)&cw);
        (*(u8 *)((u8 *)(temp_a0) + 0xB9C)) = (u8) ((*(u8 *)((u8 *)(temp_a0) + 0xB9C)) + 1);
        return;
    case 3:
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
        temp_v0_2 = lobby_func_00580FA0(&jtbl_0063E490, temp_a1, temp_a2, temp_a3);
        if ((temp_v0_2 != 0) && (temp_v0_2 != 1)) {
            goto block_39;
        }
        temp_a0_2 = (*(void **)&cw);
        (*(u8 *)((u8 *)(temp_a0_2) + 0xB9C)) = (u8) ((*(u8 *)((u8 *)(temp_a0_2) + 0xB9C)) + 1);
        return;
    case 4:
        (*(u8 *)((u8 *)(temp_a2) + 0xB9C)) = (u8) (temp_a1 + 1);
        CallBackWaitInit(&jtbl_0063E490, temp_a1, temp_a2, temp_a3);
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xBA9)) = 2;
        lobby_func_0058BAA0(&my_user_mini_data);
        cnLBS_Send_UserMiniData(&my_user_mini_data, 0x40, &CallBack_SendMiniData);
        goto block_39;
    case 5:
        Check_CallBackWait(&jtbl_0063E490, temp_a1, temp_a2, temp_a3);
        goto block_39;
    case 6:
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
        temp_v0_3 = lobby_func_0057C0B0(&jtbl_0063E490, temp_a1, temp_a2, temp_a3);
        if ((temp_v0_3 != 0) && (temp_v0_3 != 1)) {

        } else {
            temp_a0_3 = (*(void **)&cw);
            (*(u8 *)((u8 *)(temp_a0_3) + 0xB9C)) = (u8) ((*(u8 *)((u8 *)(temp_a0_3) + 0xB9C)) + 1);
        }
        goto block_39;
    case 7:
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
        temp_a0_4 = Fade_busy_ck(&jtbl_0063E490, temp_a1, temp_a2, temp_a3) & 0xFF;
        if (temp_a0_4 != 1) {
            if ((*(u8 *)((u8 *)&CnetWork + 0x5)) == 3) {
                lobby_func_0057CC60(temp_a0_4);
                (*(s8 *)((u8 *)((*(void **)&cw)) + 0xFA5E)) = 1;
            } else {
                lobby_func_0057CC60(temp_a0_4);
                (*(s8 *)((u8 *)((*(void **)&cw)) + 0xFA5E)) = 0;
            }
            (*(u8 *)((u8 *)&CnetWork + 0x5)) = 2;
            Q_camera_init();
        }
        goto block_39;
    default:
        To_LogOut(4, temp_a1, temp_a2, temp_a3);
        goto block_39;
    }
}

