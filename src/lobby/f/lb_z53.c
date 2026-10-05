/* lb_z53 - auto-drafted 0x005F72D0-0x005F738C: BsQuit03_OtherInit (first drafted by tools/lbauto.py). */
#include "lobby_f.h"
extern s8 bsGameMenuNum;
extern s8 bsNetErrOccur;
extern u8 * bsSys;
extern u8 BS_MODE_R_NO;
extern u8 BsLbsErrNum;
extern u8 bsPleaseReboot;

void BsQuit03_OtherInit(void) {
    s8 temp_a0;
    void *temp_v1;

    if (BS_MODE_R_NO == 0) {
        BsTextureFreeAll();
    }
    BsWorkInitAll();
    flfntSetHalftype(1);
    if (bsNetErrOccur != 0) {
        To_RetVal(-1);
        return;
    }
    temp_v1 = bsSys;
    F(u8, temp_v1, 0) = (u8) F(u8, temp_v1, 0x2E);
    temp_a0 = bsGameMenuNum;
    if (temp_a0 != 0) {
        To_RetVal(temp_a0);
        return;
    }
    if (BsLbsErrNum != 0) {
        BsLbsErrNum = 3U;
    }
    if (bsPleaseReboot != 0) {
        To_RetVal(2);
        return;
    }
    To_RetVal(1);
}
