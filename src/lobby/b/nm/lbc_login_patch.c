#include "lobby_a.h"
extern char jtbl_575_0065E890[];
extern char jtbl_575_0065E890[];
extern char jtbl_575_0065E890[];
extern char jtbl_575_0065E890[];
extern char jtbl_575_0065E890[];
void lbc_login_patch(void) {
    s32 temp_v0;
    u8 temp_a1;
    int temp_a2;
    int temp_a3;
    int temp_v1;

    temp_a2 = (int)cw;
    temp_a1 = F(u8, temp_a2, 0x2C34);
    temp_a3 = temp_a2 + 0x2C34;
    switch (temp_a1) {
    case 0:
        F(u8, temp_a2, 0x2C34) = (u8) (temp_a1 + 1);
        F(s8, (u8 *)cw, 0x2C08) = 0;
        cnLBS_Get_PatchInformation((u8 *)cw + 0xBF20, temp_a1, temp_a2, temp_a3);
        ms_net_patch_set_init();
        all_reset();
        F(s8, &network_work, 0x11) = 1;
        return;
    case 1:
        F(u8, temp_a2, 0x2C34) = (u8) (temp_a1 + 1);
        F(s8, &network_work, 0x11) = 1;
        return;
    case 2:
        F(s8, &network_work, 0x11) = 1;
        temp_v0 = ms_net_patch_set(&jtbl_575_0065E890);
        if (temp_v0 == 1) {
            temp_v1 = (int)cw;
            F(u8, temp_v1, 0x2C34) = (u8) (F(u8, temp_v1, 0x2C34) + 1);
            cnWrap_InitWork();
        } else if (temp_v0 == -1) {
            F(u8, (u8 *)cw, 0x2C34) = 6U;
            cnWrap_InitWork();
        }
        Net_trans_set(0);
        return;
    case 3:
        F(u8, temp_a2, 0x2C34) = (u8) (temp_a1 + 1);
        Lbs_load(&jtbl_575_0065E890);
        F(s8, &network_work, 0x11) = 0;
        return;
    case 4:
        F(u8, temp_a2, 0x2C34) = (u8) (temp_a1 + 1);
        F(s8, (u8 *)cw, 0x2C08) = 1;
        CallBackWaitInit(&jtbl_575_0065E890);
        cnLBS_Answer_PatchFinish();
        return;
    case 5:
        Check_CallBackWait(&jtbl_575_0065E890);
        return;
    case 6:
        F(u8, temp_a2, 0x2C34) = (u8) (temp_a1 + 1);
        Lbs_load(&jtbl_575_0065E890);
        F(s8, &network_work, 0x11) = 0;
        return;
    case 7:
        F(s8, temp_a2, 0x2C08) = 1;
        To_LogOut(1);
        /* fallthrough */
    default:
        return;
    }
}
