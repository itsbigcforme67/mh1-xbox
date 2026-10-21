/* G online (PC): lbc_login_warning_message, written from Monster Hunter G's code at lobby 0x57AB00 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Check_CallBackWait();
s32 Get_sw2();
s32 Lb_eat_to_bell();
s32 To_LogOut();
s32 cnWrap_SoundRequest();
s32 flfntSetSize();
void * memcpy();
extern u8 cw;
extern u8 lb_rule_member;
extern u8 pNet;
s32 check_warning_level(); /* extern */
s32 cnLBS_Answer_LoginWarningMessage(); /* extern */
s32 cnWrap_FontDisp(); /* extern */
s32 cnWrap_SetFontColor();               /* extern */

void lbc_login_warning_message(void) {
    s32 temp_a4;
    u8 temp_a1;
    u8 *temp_a0;
    u8 *temp_a0_2;
    u8 *temp_a1_2;
    u8 *temp_a2;
    u8 *temp_a3;
    u8 *temp_v1;
    u8 *temp_v1_2;
    u8 *temp_v1_3;

    temp_a2 = (*(void **)&cw);
    temp_a4 = Get_sw2(0) & 0xFFFF;
    temp_a1 = (*(u8 *)((u8 *)(temp_a2) + 0xB9C));
    temp_a3 = temp_a2 + 0xB9C;
    switch (temp_a1) {
    case 0:
        if (check_warning_level((*(u8 *)((u8 *)(temp_a2) + 0xFA8A)), temp_a1, temp_a2, temp_a3, temp_a4) == 0) {
            (*(u8 *)((u8 *)((*(void **)&cw)) + 0xB9C)) = 5U;
            (*(s32 *)((u8 *)((*(void **)&cw)) + 0xFA68)) = 0xE10;
            (*(s8 *)((u8 *)((*(void **)&cw)) + 0xFA65)) = 0;
            cnLBS_Answer_LoginWarningMessage(0);
            return;
        }
        temp_a0 = (*(void **)&cw);
        (*(u8 *)((u8 *)(temp_a0) + 0xB9C)) = (u8) ((*(u8 *)((u8 *)(temp_a0) + 0xB9C)) + 1);
        return;
    case 1:
        (*(u8 *)((u8 *)(temp_a2) + 0xB9C)) = (u8) (temp_a1 + 1);
        (*(s32 *)((u8 *)((*(void **)&cw)) + 0xBB4)) = 0x1E;
        return;
    case 2:
        (*(u8 *)((u8 *)(temp_a2) + 0xB9C)) = (u8) (temp_a1 + 1);
        temp_v1 = (*(void **)&cw);
        if ((*(u8 *)((u8 *)(temp_v1) + 0xFA8A)) != 0) {
            (*(s32 *)((u8 *)(temp_v1) + 0xBB8)) = 8;
            Lb_eat_to_bell((s32) ((*(void **)&cw) + 0xFA8E));
            return;
        }
        memcpy(temp_v1 + 0xF75D, temp_v1 + 0xFA8E, 0x300);
        To_LogOut(5);
        return;
    case 3:
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
        temp_a1_2 = (*(void **)&cw);
        if ((*(u16 *)((u8 *)(temp_a1_2) + 0xFA8C)) != 0) {
            (*(s32 *)((u8 *)(temp_a1_2) + 0xBB4)) = (s32) ((*(s32 *)((u8 *)(temp_a1_2) + 0xBB4)) - 1);
            temp_v1_2 = (*(void **)&cw);
            if ((s8) (*(s32 *)((u8 *)(temp_v1_2) + 0xBB4)) <= 0) {
                (*(s32 *)((u8 *)(temp_v1_2) + 0xBB4)) = 0x1E;
                temp_a0_2 = (*(void **)&cw);
                (*(u16 *)((u8 *)(temp_a0_2) + 0xFA8C)) = (u16) ((*(u16 *)((u8 *)(temp_a0_2) + 0xFA8C)) - 1);
                temp_v1_3 = (*(void **)&cw);
                if ((s16) (*(u16 *)((u8 *)(temp_v1_3) + 0xFA8C)) < 0) {
                    (*(u16 *)((u8 *)(temp_v1_3) + 0xFA8C)) = 0U;
                    return;
                }
            }
            return;
        }
        if (temp_a4 & 0xFFFF & 0x20) {
            (*(u8 *)((u8 *)(temp_a1_2) + 0xB9C)) = (u8) ((*(u8 *)((u8 *)(temp_a1_2) + 0xB9C)) + 1);
            cnWrap_SoundRequest(0);
        }
        cnWrap_SetFontColor(5);
        flfntSetSize(0x14, 0x14);
        cnWrap_FontDisp((*(s32 *)((u8 *)&lb_rule_member + 0x1C)), 0x43520000, 0x43970000, 0x40000000);
        return;
    case 4:
        (*(u8 *)((u8 *)(temp_a2) + 0xB9C)) = (u8) (temp_a1 + 1);
        (*(s32 *)((u8 *)((*(void **)&cw)) + 0xFA68)) = 0xE10;
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xFA65)) = 0;
        cnLBS_Answer_LoginWarningMessage(1, temp_a1, temp_a2, temp_a3, temp_a4);
        return;
    case 5:
        Check_CallBackWait();
        return;
    default:
        To_LogOut(4);
        break;
    }
}

