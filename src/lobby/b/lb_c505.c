/* lb_c505 - agent C round 5 0x005B8C70-0x005B8E78: lbc_login_top_information (u8 Fade_busy_ck result, counter via pointer, break after the nested ifs). */
#include "lobby_a.h"
extern u16 Get_sw2();
extern char text_lobby_trans_ot0[];
extern char CallBack_Result_LoginTopInformation[];
typedef struct { u8 pad0[0x2C33]; s8 x2C33; u8 step; u8 pad2C35[0x17]; s32 x2C4C; s32 x2C50; u8 pad2C54[0x19AE]; u8 x4602; u8 pad4603[3]; u8 x4606[4]; } CWS_lt;
#define CWX ((CWS_lt *)cw)
void lbc_login_top_information(void) {
    u16 sw;
    u8 st;
    u8 *stp;
    CWS_lt *c;
    s32 *p;
    s32 v;

    sw = Get_sw2(0);
    c = CWX;
    st = c->step;
    stp = &c->step;
    switch (st) {
    case 0:
        *stp = st + 1;
        Lbc_set_prim(text_lobby_trans_ot0, 0, 0);
        SetSceneTitle(0, 1);
        CallBackWaitInit();
        cnLBS_Read_TopInformation(CallBack_Result_LoginTopInformation);
        return;
    case 1:
        Check_CallBackWait();
        return;
    case 2:
        *stp = st + 1;
        cnLBS_Get_TopInformation(&CWX->x4602);
        return;
    case 3:
        if (check_top_information_level(c->x4602) != 0) {
            CWX->step++;
            return;
        }
        CWX->step = 7;
        return;
    case 4:
        *stp = st + 1;
        fade_set(2);
        SetDialogData_HTML(CWX->x4606);
        CWX->x2C4C = 0x1E;
        CWX->x2C50 = 0x708;
        return;
    case 5:
        F(s8, pNet, 0xC) = 1;
        c = CWX;
        p = &c->x2C4C;
        v = *p;
        if (v != 0) {
            *p = v - 1;
            return;
        }
        if ((u8)Fade_busy_ck() != 1) {
            if ((sw & 0x20) || (v = CWX->x2C50 - 1, CWX->x2C50 = v, v <= 0)) {
                CWX->step++;
                cnWrap_SoundRequest(0);
                fade_set(1);
                return;
            }
        }
        break;
    case 6:
        F(s8, pNet, 0xC) = 1;
        if ((u8)Fade_busy_ck() == 1) {
            break;
        }
    case 7:
        CWX->x2C33 = 9;
        CWX->step = 0;
        break;
    }
}
