/* lb_by108 - agent B promoted near-match 0x005B9990-0x005B9A64: CallBack_ReadCurrentPlace (first drafted by tools/lbauto.py). */
#include "lobby_b.h"
typedef struct { u8 x0; u8 pad1[3]; u8 x4; u8 pad5[0x1B]; } CLSI;
extern CLSI ClassInfo;
typedef struct { u8 pad0[0x2C31]; u8 x2C31; u8 x2C32; u8 pad2C33[0x12]; u8 x2C45; u8 pad2C46[0x98C]; s8 x35D2; } CWS_cp;
#define CWX ((CWS_cp *)cw)

void CallBack_ReadCurrentPlace(CNET_RES res) {
    u16 sp18[3];

    if ((CWX->x2C31 != 5) && (CWX->x2C45 == 1)) {
        CWX->x2C45 = 0;
        if (res.val == 0) {
            CWX->x2C32++;
            cnLBS_Get_CurrentPlace(sp18);
            ClassInfo.x0 = sp18[0];
            ClassInfo.x4 = sp18[1];
            if (ClassInfo.x0 == 0 || ClassInfo.x4 == 0) {
                To_TopMenu();
                CWX->x35D2 = 0;
            } else {
                CWX->x35D2 = 1;
            }
        } else {
            To_TopMenu();
            CWX->x35D2 = 0;
        }
    }
}
