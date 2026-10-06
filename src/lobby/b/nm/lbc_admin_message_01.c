#include "lobby_a.h"
typedef struct { u8 pad0[0x2F6E]; u8 x2F6E; u8 x2F6F; u8 pad2F70[4]; s16 x2F74; s8 x2F76; } CWS_am1;
#define CWX ((CWS_am1 *)cw)
void lbc_admin_message_01(void) {
    u16 sw;
    u8 st;
    sw = Get_sw2(0);
    st = CWX->x2F6F;
    switch (st) {
    case 0:
        if (sw & 0x20) {
            CWX->x2F6F = st + 1;
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
