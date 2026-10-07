/* lb_c514 - agent C round 5 0x005B8570-0x005B8794: lbc_login_users_personal_data (login step: copy the personal data, boot the browser, register changes; the counter is post-incremented inside the condition). */
#include "lobby_a.h"
extern u16 Get_sw2();
typedef struct { u8 b[0x1D0]; } PD;
extern PD BrPersonalData;
extern PD tmpPersonalData;
extern char first_url[];
extern char lit_707_0065E8D0[];
extern char CallBack_Result_LoginPersonalDataRegist[];
typedef struct { u8 pad0[0x2C33]; u8 x2C33; u8 step; u8 pad2C35[0x9]; u8 x2C3E[6]; u8 x2C44; s8 x2C45; u8 pad2C46[6]; s32 x2C4C; u8 pad2C50[0x681]; u8 x32D1[1]; } CWS_pd;
#define CWX ((CWS_pd *)cw)
void To_BootUpBrowser();
void lbc_browser();
void lbc_login_users_personal_data(void) {
    u16 sw;
    u8 st;
    u8 *stp;
    CWS_pd *c;

    sw = Get_sw2(0);
    c = CWX;
    st = c->step;
    stp = &c->step;
    switch (st) {
    case 0:
        *stp = st + 1;
    case 1:
        CWX->step++;
        tmpPersonalData = BrPersonalData;
        strcpy(first_url, lit_707_0065E8D0);
        To_BootUpBrowser();
        return;
    case 2:
        if (c->x2C44 == 2) {
            *stp = st + 1;
            CWX->x2C44 = 0;
            return;
        }
        lbc_browser(c->x2C44);
        return;
    case 3:
        if (memcmp(&tmpPersonalData, &BrPersonalData, 0x1D0) != 0) {
            CallBackWaitInit();
            CWX->step++;
            CWX->x2C45 = 0xD;
            cnLBS_RegistPersonalData(&BrPersonalData, CallBack_Result_LoginPersonalDataRegist);
            return;
        }
        CWX->step = 8;
        return;
    case 4:
        Check_CallBackWait();
        return;
    case 5:
        *stp = st + 1;
        CWX->x2C4C = 0;
        SetDialogData_HTML(CWX->x32D1);
        return;
    case 6:
        F(s8, pNet, 0xC) = 1;
        if (CWX->x2C4C++ >= 0x258 || (CWX->x2C4C >= 0x3C && (sw & 0x60))) {
            CWX->step++;
        }
        break;
    case 7:
        *stp = st + 1;
    case 8:
        CWX->x2C33 = 4;
        CWX->step = 1;
        ((s8 *)cw)[0x2C35] = 0;
        break;
    }
}
