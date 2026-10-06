#include "lobby_a.h"
extern char lit_547_0065E850[];
typedef struct { u8 pad0[0x2C34]; u8 step; u8 pad2C35[0x17]; s32 x2C4C; s32 x2C50; u8 pad2C54[0x9AA]; u8 x35FE; u8 pad35FF; u16 x3600; u8 x3602[4]; } CWS_lw;
#define CWX ((CWS_lw *)cw)
void cnWrap_FontDisp(f32, f32, f32, char *);
void lbc_login_warning_message(void) {
    u16 sw;
    CWS_lw *c;
    u8 st;
    u8 *stp;

    sw = Get_sw2(0);
    c = CWX;
    st = c->step;
    stp = &c->step;
    switch (st) {
    case 0:
        if (check_warning_level(c->x35FE) == 0) {
            CWX->step = 7;
            CallBackWaitInit();
            cnLBS_Answer_LoginWarningMessage(0);
            return;
        }
        CWX->step++;
        return;
    case 1:
        *stp = st + 1;
    case 2:
        CWX->step++;
        CWX->x2C4C = 0x3C;
        return;
    case 3:
        *stp = st + 1;
        if (CWX->x35FE != 0) {
            CWX->x2C50 = 8;
            SetDialogData_HTML(CWX->x3602);
            return;
        }
        To_LogOut(1);
        return;
    case 4:
        F(s8, pNet, 0xC) = 1;
        c = CWX;
        if (c->x3600 != 0) {
            c->x2C4C--;
            if ((s8) CWX->x2C4C > 0) {
                return;
            }
            CWX->x2C4C = 0x3C;
            CWX->x3600--;
            if ((s16) CWX->x3600 >= 0) {
                return;
            }
            CWX->x3600 = 0;
            return;
        }
        if (sw & 0xFFFF & 0x20) {
            c->step++;
            cnWrap_SoundRequest(0);
        }
        cnWrap_SetFontColor(5);
        flfntSetSize(0x14, 0x14);
        cnWrap_FontDisp(210.0f, 302.0f, 2.0f, lit_547_0065E850);
        return;
    case 5:
        *stp = st + 1;
        return;
    case 6:
        *stp = st + 1;
        CallBackWaitInit();
        cnLBS_Answer_LoginWarningMessage(1);
        return;
    case 7:
        Check_CallBackWait();
        break;
    }
}
