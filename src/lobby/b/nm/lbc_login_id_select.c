#include "lobby_a.h"
extern char jtbl_650_0065E8B0[];
extern char jtbl_650_0065E8B0[];
extern char CnetWork[];
extern char jtbl_650_0065E8B0[];
extern char D_3C6FC8[];
extern char jtbl_650_0065E8B0[];
void lbc_login_id_select(void) {
    s32 temp_s0;
    s32 temp_v0;
    s32 var_s0;
    s32 var_s1;
    s32 var_s2;
    s32 var_s3;
    u8 temp_a1;
    u8 temp_s1;
    int temp_a0;
    int temp_a0_2;
    int temp_a0_3;
    int temp_a0_4;
    int temp_a1_2;
    int temp_a2;
    int temp_v1;
    int temp_v1_2;
    int temp_v1_3;

    temp_v1 = (int)cw;
    temp_a1 = F(u8, temp_v1, 0x2C34);
    temp_a2 = temp_v1 + 0x2C34;
    switch (temp_a1) {                              /* switch 1 */
    case 0:                                         /* switch 1 */
        F(u8, temp_v1, 0x2C34) = (u8) (temp_a1 + 1);
        F(u8, (u8 *)cw, 2) = 0U;
        F(u8, (u8 *)cw, 1) = 0U;
        memset((u8 *)cw + 0xB, 0, 0x20);
        memset((u8 *)cw + 0x2B, 0, 0x44);
        memset((u8 *)cw + 0x6F, 0, 0x100);
        F(u8, (u8 *)cw, 0) = cnetGet_Login_NoOfUserAccount();
        var_s3 = 0;
        if (F(u8, (u8 *)cw, 0) > 0) {
            var_s2 = 0;
            var_s1 = 0;
            var_s0 = 0;
            do {
                cnetGet_Login_UserID(var_s3 & 0xFF, (u8 *)cw + var_s2 + 0xB);
                cnetGet_Login_UserHandle(var_s3 & 0xFF, (u8 *)cw + var_s1 + 0x2B);
                cnetGet_Login_UserMiniData(var_s3 & 0xFF, (u8 *)cw + var_s0 + 0x6F);
                var_s3 += 1;
                var_s2 += 8;
                var_s1 += 0x11;
                var_s0 += 0x40;
            } while (var_s3 < F(u8, (u8 *)cw, 0));
        }
        if (cnWrap_IsBBConnect() != 0) {
            temp_a0 = (int)cw;
            if (F(u8, temp_a0, 0) == 0) {
                F(s8, temp_a0, 0x2C33) = 5;
                F(u8, (u8 *)cw, 0x2C34) = 0U;
                return;
            }
        }
    default:                                        /* switch 1 */
    case -2:                                        /* switch 2 */
    case -1:                                        /* switch 2 */
        return;
    case 1:                                         /* switch 1 */
        F(u8, temp_v1, 0x2C34) = (u8) (temp_a1 + 1);
        Lbc_init_network_work(&jtbl_650_0065E8B0, temp_a1, temp_a2);
        F(u8, pNet, 6) = (u8) F(u8, (u8 *)cw, 2);
        F(u8, pNet, 8) = (u8) F(u8, (u8 *)cw, 1);
        return;
    case 2:                                         /* switch 1 */
        if ((Fade_busy_ck(&jtbl_650_0065E8B0, temp_a1, temp_a2) & 0xFF) != 1) {
            temp_a0_2 = (int)cw;
            F(u8, temp_a0_2, 0x2C34) = (u8) (F(u8, temp_a0_2, 0x2C34) + 1);
            if (F(u8, &CnetWork, 5) == 0) {
                fade_set(2);
                return;
            }
        }
        break;
    case 3:                                         /* switch 1 */
        temp_v0 = net_SetMenu_SelectHandleName(&network_work, temp_a1, temp_a2);
        switch (temp_v0) {                          /* switch 2; irregular */
        case 0:                                     /* switch 2 */
            temp_a0_3 = (int)cw;
            F(u8, temp_a0_3, 0x2C34) = (u8) (F(u8, temp_a0_3, 0x2C34) + 1);
            return;
        }
        break;
    case 4:                                         /* switch 1 */
        if ((Fade_busy_ck(&jtbl_650_0065E8B0, temp_a1, temp_a2) & 0xFF) != 1) {
            temp_v1_2 = (int)cw;
            F(u8, temp_v1_2, 0x2C34) = (u8) (F(u8, temp_v1_2, 0x2C34) + 1);
            Lbc_set_prim(0, 0, 0);
        }
        /* fallthrough */
    case 5:                                         /* switch 1 */
        temp_v1_3 = (int)cw;
        F(u8, temp_v1_3, 0x2C34) = (u8) (F(u8, temp_v1_3, 0x2C34) + 1);
        temp_s1 = F(u8, pNet, 8);
        F(u8, (u8 *)cw, 1) = temp_s1;
        CallBackWaitInit();
        Set_userdata((int)&player_work + (game_w.master * 0xA00));
        temp_s0 = temp_s1 * 0x11;
        memcpy((u8 *)cw + temp_s0 + 0x2B, &D_3C6FC8, 0x10);
        temp_a1_2 = (int)cw;
        temp_a0_4 = (temp_s1 * 8) + temp_a1_2;
        if (F(s8, temp_a0_4, 0xB) == 0) {
            cnLBS_Send_LoginUserAccount(0, temp_a1_2 + temp_s0 + 0x2B, temp_a1_2 + (temp_s1 << 6) + 0x6F);
        } else {
            cnLBS_Send_LoginUserAccount(temp_a0_4 + 0xB, temp_a1_2 + temp_s0 + 0x2B, temp_a1_2 + (temp_s1 << 6) + 0x6F);
        }
        cnetGet_Login_DecideUserID((u8 *)cw + 0x440);
        cnetGet_Login_DecideUserHandle((u8 *)cw + 0x448);
        return;
    case 6:                                         /* switch 1 */
        Check_CallBackWait(&jtbl_650_0065E8B0, temp_a1, temp_a2);
        break;
    }
}
