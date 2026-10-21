/* G online (PC): lbc_login_top_information, written from Monster Hunter G's code at lobby 0x57BDA0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 CallBack_Result_LoginTopInformation();
s32 Check_CallBackWait();
s32 Get_sw2();
s32 Lb_eat_to_bell();
s32 Lbc_set_prim();
s32 SetDialogData();
s32 SetSceneTitle();
s32 To_LogOut();
s32 cnWrap_SoundRequest();
s32 fade_set();
s32 text_lobby_trans_ot0();
extern u8 cw;
extern u8 jtbl_850_0065E910;
extern u8 pNet;
s32 Fade_busy_ck();    /* extern */
s32 check_top_information_level(); /* extern */
s32 cnLBS_Get_TopInformation(); /* extern */
s32 cnLBS_Read_TopInformation(); /* extern */

void lbc_login_top_information(void) {
    s32 temp_s0;
    s32 temp_v1;
    s32 temp_v1_2;
    u8 temp_a1;
    u8 *temp_a0;
    u8 *temp_a0_2;
    u8 *temp_a2;
    u8 *temp_a3;
    u8 *temp_v1_3;

    temp_a2 = (*(void **)&cw);
    temp_s0 = Get_sw2(0) & 0xFFFF;
    temp_a1 = (*(u8 *)((u8 *)(temp_a2) + 0xB9C));
    temp_a3 = temp_a2 + 0xB9C;
    switch (temp_a1) {
    case 0:
        (*(u8 *)((u8 *)(temp_a2) + 0xB9C)) = (u8) (temp_a1 + 1);
        Lbc_set_prim((s32) text_lobby_trans_ot0, 0, 0, 0);
        SetSceneTitle(1, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        (*(s32 *)((u8 *)((*(void **)&cw)) + 0xFA68)) = 0xE10;
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xFA65)) = 0;
        cnLBS_Read_TopInformation(CallBack_Result_LoginTopInformation);
        break;
    case 1:
        Check_CallBackWait((s32) &jtbl_850_0065E910, (s32) temp_a1, (s32) temp_a2, (s32) temp_a3);
        break;
    case 2:
        (*(u8 *)((u8 *)(temp_a2) + 0xB9C)) = (u8) (temp_a1 + 1);
        cnLBS_Get_TopInformation((*(void **)&cw) + 0x10A8E, temp_a1, temp_a2, temp_a3);
        break;
    case 3:
        if (check_top_information_level((*(u8 *)((u8 *)(temp_a2) + 0x10A8E)), temp_a1, temp_a2, temp_a3) != 0) {
            temp_a0 = (*(void **)&cw);
            (*(u8 *)((u8 *)(temp_a0) + 0xB9C)) = (u8) ((*(u8 *)((u8 *)(temp_a0) + 0xB9C)) + 1);
        } else {
            (*(u8 *)((u8 *)((*(void **)&cw)) + 0xB9C)) = 7U;
        }
        break;
    case 4:
        (*(u8 *)((u8 *)(temp_a2) + 0xB9C)) = (u8) (temp_a1 + 1);
        if (Fade_busy_ck(&jtbl_850_0065E910, temp_a1, temp_a2, temp_a3) & 0xFF) {
            fade_set(2);
        }
        Lb_eat_to_bell((s32) ((*(void **)&cw) + 0x10A92), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        (*(s32 *)((u8 *)((*(void **)&cw)) + 0xBB4)) = 0x1E;
        (*(s32 *)((u8 *)((*(void **)&cw)) + 0xBB8)) = 0x708;
        break;
    case 5:
        temp_v1 = (*(s32 *)((u8 *)(temp_a2) + 0xBB4));
        if (temp_v1 != 0) {
            (*(s32 *)((u8 *)(temp_a2) + 0xBB4)) = (s32) (temp_v1 - 1);
        } else if (((Fade_busy_ck(temp_a2 + 0xBB4, temp_a1, temp_a2, temp_a3) & 0xFF) != 1) && ((temp_s0 & 0xFFFF & 0x20) || (temp_a0_2 = (*(void **)&cw), temp_v1_2 = (*(s32 *)((u8 *)(temp_a0_2) + 0xBB8)) - 1, (*(s32 *)((u8 *)(temp_a0_2) + 0xBB8)) = temp_v1_2, (temp_v1_2 <= 0)))) {
            temp_v1_3 = (*(void **)&cw);
            (*(u8 *)((u8 *)(temp_v1_3) + 0xB9C)) = (u8) ((*(u8 *)((u8 *)(temp_v1_3) + 0xB9C)) + 1);
            cnWrap_SoundRequest(0, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            SetDialogData(0x19, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        }
        break;
    case 6:
        if ((Fade_busy_ck(&jtbl_850_0065E910, temp_a1, temp_a2, temp_a3) & 0xFF) != 1) {
        case 7:
            (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB9B)) = 9;
            (*(u8 *)((u8 *)((*(void **)&cw)) + 0xB9C)) = 0U;
        }
        break;
    default:
        To_LogOut(4, (s32) temp_a1, (s32) temp_a2, (s32) temp_a3);
        break;
    }
    (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
}

