/* G online (PC): lbc_login_users_personal_data, written from Monster Hunter G's code at lobby 0x57B2A0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 CallBackWaitInit();
s32 CallBack_Result_LoginPersonalDataRegist();
s32 Check_CallBackWait();
s32 Lb_eat_to_bell();
s32 To_LogOut();
s32 lobby_func_0057C7E0();
extern u8 cw;
extern u8 pNet;
s32 Get_sw2();                               /* extern */
s32 To_BootUpBrowser();                         /* extern */
s32 cnLBS_RegistPersonalData(); /* extern */
s32 memcmp();   /* extern */
s32 strcpy(); /* extern */
extern s128 BrPersonalData;
extern u8 first_url;
extern u8 lit_707_0065E8D0;
extern s128 tmpPersonalData;

void lbc_login_users_personal_data(void) {
    u8 *var_a0;   /* (s128 *) */
    u8 *var_a1;   /* (s128 *) */
    s32 temp_a0;
    s32 temp_a4;
    s32 var_v1;
    u8 temp_a1;
    u8 *temp_a0_2;
    u8 *temp_a1_2;
    u8 *temp_a2;
    u8 *temp_a2_2;
    u8 *temp_a2_3;
    u8 *temp_a3;

    temp_a2 = (*(void **)&cw);
    temp_a4 = Get_sw2(0) & 0xFFFF;
    temp_a1 = (*(u8 *)((u8 *)(temp_a2) + 0xB9C));
    temp_a3 = temp_a2 + 0xB9C;
    switch (temp_a1) {
    case 0:
        (*(u8 *)((u8 *)(temp_a2) + 0xB9C)) = (u8) (temp_a1 + 1);
        /* fallthrough */
    case 1:
        temp_a2_2 = (*(void **)&cw);
        var_a1 = &BrPersonalData;
        var_a0 = &tmpPersonalData;
        var_v1 = 0x1D;
        (*(u8 *)((u8 *)(temp_a2_2) + 0xB9C)) = (u8) ((*(u8 *)((u8 *)(temp_a2_2) + 0xB9C)) + 1);
        do {
            var_v1 -= 1;
            (*(s128 *)var_a0) = (*(s128 *)var_a1);
            var_a1 += 0x10;
            var_a0 += 0x10;
        } while (var_v1 > 0);
        strcpy(&first_url, &lit_707_0065E8D0, temp_a2_2, temp_a3, temp_a4);
        To_BootUpBrowser();
        return;
    case 2:
        if ((*(u8 *)((u8 *)(temp_a2) + 0xBA8)) == 2) {
            (*(u8 *)((u8 *)(temp_a2) + 0xB9C)) = (u8) (temp_a1 + 1);
            (*(u8 *)((u8 *)((*(void **)&cw)) + 0xBA8)) = 0U;
            return;
        }
        lobby_func_0057C7E0();
        return;
    case 3:
        if (memcmp(&tmpPersonalData, &BrPersonalData, 0x1D0, temp_a3, temp_a4) != 0) {
            CallBackWaitInit();
            temp_a2_3 = (*(void **)&cw);
            (*(u8 *)((u8 *)(temp_a2_3) + 0xB9C)) = (u8) ((*(u8 *)((u8 *)(temp_a2_3) + 0xB9C)) + 1);
            (*(s8 *)((u8 *)((*(void **)&cw)) + 0xBA9)) = 0x1D;
            cnLBS_RegistPersonalData(&BrPersonalData, CallBack_Result_LoginPersonalDataRegist, temp_a2_3);
            return;
        }
        (*(u8 *)((u8 *)((*(void **)&cw)) + 0xB9C)) = 8U;
        return;
    case 4:
        Check_CallBackWait();
        return;
    case 5:
        (*(u8 *)((u8 *)(temp_a2) + 0xB9C)) = (u8) (temp_a1 + 1);
        (*(s32 *)((u8 *)((*(void **)&cw)) + 0xBB4)) = 0;
        Lb_eat_to_bell((s32) ((*(void **)&cw) + 0xF75D));
        return;
    case 6:
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
        temp_a1_2 = (*(void **)&cw);
        temp_a0 = (*(s32 *)((u8 *)(temp_a1_2) + 0xBB4));
        (*(s32 *)((u8 *)(temp_a1_2) + 0xBB4)) = (s32) (temp_a0 + 1);
        if (temp_a0 < 0x12C) {
            if (((*(s32 *)((u8 *)((*(void **)&cw)) + 0xBB4)) >= 0x1E) && (temp_a4 & 0xFFFF & 0x60)) {
                goto block_19;
            }
            return;
        }
block_19:
        temp_a0_2 = (*(void **)&cw);
        (*(u8 *)((u8 *)(temp_a0_2) + 0xB9C)) = (u8) ((*(u8 *)((u8 *)(temp_a0_2) + 0xB9C)) + 1);
        return;
    case 7:
        (*(u8 *)((u8 *)(temp_a2) + 0xB9C)) = (u8) (temp_a1 + 1);
        /* fallthrough */
    case 8:
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB9B)) = 4;
        (*(u8 *)((u8 *)((*(void **)&cw)) + 0xB9C)) = 1U;
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB9D)) = 0;
        return;
    default:
        To_LogOut(4);
        break;
    }
}

