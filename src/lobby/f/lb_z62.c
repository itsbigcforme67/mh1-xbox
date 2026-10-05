/* lb_z62 - auto-drafted 0x005FD460-0x005FD4EC: To_RetVal, BsCloseCapDlg (first drafted by tools/lbauto.py). */
#include "lobby_f.h"
extern s8 bsMainRetVal;
extern u8 * bsSys;
extern s8 BsDialogReq;
extern u8 * bsCur;

void To_RetVal(s8 arg0) {
    bsMainRetVal = arg0;
    F(s8, bsSys, 1) = 3;
    F(s8, bsSys, 2) = 0;
    F(s8, bsSys, 3) = 0;
}

void BsCloseCapDlg(s32 arg0) {
    s32 temp_a0;

    temp_a0 = arg0 & 0xFF;
    switch (temp_a0) {                              /* irregular */
    case 0:
        cnWrap_SoundRequest(3);
        break;
    case 1:
        cnWrap_SoundRequest(0);
        break;
    }
    F(s8, bsCur, 1) = 0;
    BsDialogReq = 1;
}
