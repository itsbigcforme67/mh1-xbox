#include "lobby_a.h"
extern s32 netr_ret;
extern int lpSKey;
extern u8 BsLbsErrNum;
extern char network_lobby_client_main_jmp_207[];
s32 internet_lobby_act(void) {
    s32 temp_s0;
    s32 temp_v0;
    s32 temp_v0_2;
    s32 var_s1;
    s8 temp_v1_4;
    s8 temp_v1_6;
    int temp_a0;
    int temp_a0_2;
    int temp_v1;
    int temp_v1_2;
    int temp_v1_3;
    int temp_v1_5;

    temp_s0 = Get_sw(0) & 0xFFFF;
    var_s1 = 0;
    if (*(s8 *)0x3F36CC == 0) {
loop_2:
        if (cnLBS_RecvData(*(s32 *)0x4E36F4) == 1) {
            var_s1 += 1;
            F(s32, (u8 *)cw, 0x35F4) = 0xE10;
            if (var_s1 < 0x64) {
                goto loop_2;
            }
        }
    }
    temp_a0 = (int)cw;
    F(s32, temp_a0, 0x35F4) = (F(s32, temp_a0, 0x35F4) - 1);
    if (F(u8, (u8 *)cw, 0x2C31) != 5) {
        if (cnWrap_IsAccessMemoryCard(temp_a0) == 0) {
            temp_v1 = (int)cw;
            if (F(s8, temp_v1, 0x2C08) != 0) {
                if ((temp_s0 == 0) && (F(u8, lpSKey, 0x658) == 0)) {
                    if (*(u16 *)0x3F3718 & 0x3C3C) {
                        goto block_11;
                    }
                } else {
block_11:
                    cnLbc_SetIspRestTime(temp_v1 + 0x35F8);
                }
                temp_v1_2 = (int)cw;
                temp_v0 = F(s32, temp_v1_2, 0x35F8) - 1;
                F(s32, temp_v1_2, 0x35F8) = temp_v0;
                if (temp_v0 < 0) {
                    To_LogOut(3);
                    return 1;
                }
                if (CpInetGetStatus() == 0) {
                    if (BsLbsErrNum != 0) {
                        goto block_18;
                    }
                    temp_v1_3 = (int)cw;
                    if (F(s32, temp_v1_3, 0x35F4) < 0) {
                        To_LogOut(6);
                        return 1;
                    }
                    temp_v1_4 = F(s8, temp_v1_3, 0x2C0E);
                    if (temp_v1_4 == 1) {
                        To_LogOut(5);
                        return 1;
                    }
                    if (temp_v1_4 == 2) {
                        To_LogOut(7);
                        return 1;
                    }
                    goto block_44;
                }
block_18:
                To_LogOut(6);
                return 1;
            }
        }
        temp_a0_2 = (int)cw;
        if ((F(u8, temp_a0_2, 0x2C44) == 1) && (BsLbsErrNum == 0)) {
            if ((temp_s0 != 0) || (F(u8, lpSKey, 0x658) != 0)) {
                cnLbc_SetIspRestTime(temp_a0_2 + 0x35F8);
            }
            temp_v1_5 = (int)cw;
            if (0 > F(s32, temp_v1_5, 0x35F4)) {
                goto block_43;
            }
            temp_v0_2 = F(s32, temp_v1_5, 0x35F8) - 1;
            F(s32, temp_v1_5, 0x35F8) = temp_v0_2;
            if (0 > temp_v0_2) {
                goto block_43;
            }
            temp_v1_6 = F(s8, (u8 *)cw, 0x2C0E);
            if (temp_v1_6 != 1) {
                if (temp_v1_6 == 2) {
                    goto block_39;
                }
                if (CpInetGetStatus() != 0) {
                    goto block_43;
                }
            } else {
block_39:
block_43:
                BsLbsErrNum = 1U;
            }
        }
        goto block_44;
    }
block_44:
    if (lobby_client_admin_message() != 0) {
        if (F(u8, pNet, 0x11) == 0) {
            lbc_text_lobby_trans(&network_work);
        }
        return netr_ret;
    }
    F(s8, pNet, 0xC) = 0;
    F(u8, pNet, 0x11) = 0U;
    font_set_stack_no(0);
    ((int (**)())&network_lobby_client_main_jmp_207)[F(u8, (u8 *)cw, 0x2C31)]();
    if (F(u8, pNet, 0x11) == 0) {
        lbc_text_lobby_trans(&network_work);
    }
    return netr_ret;
}
