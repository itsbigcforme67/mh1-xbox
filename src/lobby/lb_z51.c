/* lb_z51 - auto-drafted 0x005F32A0-0x005F32FC: BsBody02_PrsInit (first drafted by tools/lbauto.py). */
#include "lobby.h"
extern s32 srcbuf;
extern s8 BsKddiQueryEnable;
extern u8 * bsSys;

void BsBody02_PrsInit(void) {
    void *temp_a0;

    MoveAndTransSet();
    BsUrlBaseClear();
    BsParseInitialize();
    BsParseStart(srcbuf);
    BsKddiQueryEnable = 0;
    F(s8, bsSys, 0x37) = 0;
    F(s8, bsSys, 0x2E) = 3;
    temp_a0 = bsSys;
    F(u8, temp_a0, 2) = (u8) (F(u8, temp_a0, 2) + 1);
}
