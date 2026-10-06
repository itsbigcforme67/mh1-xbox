/* lb_ila01 - agent C 0x005B70D0-0x005B73C4: internet_lobby_act (online lobby per-frame: socket pump, ISP rest-time/timeouts, state dispatch). */
#include "lobby_a.h"
extern s32 netr_ret;
extern int lpSKey;
extern u8 BsLbsErrNum;
extern char network_lobby_client_main_jmp_207[];
typedef struct { u8 pad0[0x2C08]; s8 x2C08; u8 pad2C09[5]; s8 x2C0E; u8 pad2C0F[0x22]; u8 x2C31; u8 pad2C32[0x12]; u8 x2C44; u8 pad2C45[0x9AF]; s32 x35F4; s32 x35F8; } CWS_ila;
#define CWX ((CWS_ila *)cw)
s32 internet_lobby_act(void) {
    s32 i;
    s32 sw;
    s8 t;
    s32 v;

    sw = Get_sw(0) & 0xFFFF;
    i = 0;
    if (*(s8 *)0x3F36CC == 0) {
        do {
            if (cnLBS_RecvData(*(s32 *)0x4E36F4) != 1) {
                break;
            }
            i++;
            CWX->x35F4 = 0xE10;
        } while (i < 0x64);
    }
    CWX->x35F4--;
    if (CWX->x2C31 != 5) {
        if (cnWrap_IsAccessMemoryCard() == 0 && CWX->x2C08 != 0) {
            if (sw != 0 || F(u8, lpSKey, 0x658) != 0 || (*(u16 *)0x3F3718 & 0x3C3C) != 0) {
                cnLbc_SetIspRestTime((u8 *)cw + 0x35F8);
            }
            v = CWX->x35F8 - 1;
            CWX->x35F8 = v;
            if (v < 0) {
                To_LogOut(3);
                return 1;
            }
            if (CpInetGetStatus() != 0 || BsLbsErrNum != 0) {
                To_LogOut(6);
                return 1;
            }
            if (CWX->x35F4 < 0) {
                To_LogOut(6);
                return 1;
            }
            t = CWX->x2C0E;
            if (t == 1) {
                To_LogOut(5);
                return 1;
            }
            if (t == 2) {
                To_LogOut(7);
                return 1;
            }
        } else if (CWX->x2C44 == 1 && BsLbsErrNum == 0) {
            if (sw != 0 || F(u8, lpSKey, 0x658) != 0) {
                cnLbc_SetIspRestTime((u8 *)cw + 0x35F8);
            }
            if (CWX->x35F4 < 0) {
                BsLbsErrNum = 1;
            } else {
                v = CWX->x35F8 - 1;
                CWX->x35F8 = v;
                if (v < 0) {
                    BsLbsErrNum = 1;
                } else {
                    t = CWX->x2C0E;
                    if (t == 1 || t == 2) {
                        BsLbsErrNum = 1;
                    } else if (CpInetGetStatus() != 0) {
                        BsLbsErrNum = 1;
                    }
                }
            }
        }
    }
    if (lobby_client_admin_message() != 0) {
        if (F(u8, pNet, 0x11) == 0) {
            lbc_text_lobby_trans(&network_work);
        }
        return netr_ret;
    }
    F(s8, pNet, 0xC) = 0;
    F(u8, pNet, 0x11) = 0;
    font_set_stack_no(0);
    ((int (**)())network_lobby_client_main_jmp_207)[CWX->x2C31]();
    if (F(u8, pNet, 0x11) == 0) {
        lbc_text_lobby_trans(&network_work);
    }
    return netr_ret;
}
