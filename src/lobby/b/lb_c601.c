/* lb_c601 - agent C round 7 0x005BF210-0x005BF2D4: lbc_admin_message_01 (Get_sw2 is u16, stp/c locals). */
#include "lobby_a.h"
extern u16 Get_sw2();
typedef struct { u8 pad0[0x2F6E]; u8 x2F6E; u8 x2F6F; u8 pad2F70[4]; s16 x2F74; s8 x2F76; } CWS_am1;
#define CWX ((CWS_am1 *)cw)
void lbc_admin_message_01(void) {
    u16 sw;
    u8 st;
    u8 *stp;
    CWS_am1 *c;
    sw = Get_sw2(0);
    c = CWX;
    st = c->x2F6F;
    stp = &c->x2F6F;
    switch (st) {
    case 0:
        if (sw & 0x20) {
            *stp = st + 1;
            cnWrap_SoundRequest(0);
            CWX->x2F74 = 8;
            cnLbc_EraseDialog(0x4C);
        }
        Lb_put_inputMsgForHTML();
        return;
    case 1:
        CWX->x2F74--;
        if (CWX->x2F74 < 0) {
            CWX->x2F76 = 1;
            cnLBS_AnswerAdminMessage(1);
            CWX->x2F6E++;
            CWX->x2F6F = 0;
        }
    }
}
