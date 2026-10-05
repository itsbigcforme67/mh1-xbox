/* lb_z55 - auto-drafted 0x005F7B00-0x005F7B28: BsCsMove01_KbdInput, ExitToolMenu (first drafted by tools/lbauto.py). */
#include "lobby.h"
extern s8 BsSoftKbdReq;
extern s8 BsCursorReq;
extern s8 BsToolMenuReq;
extern u8 * bsCur;

void BsCsMove01_KbdInput(void) {
    BsSoftKbdReq = 1;
}

void ExitToolMenu(void) {
    BsToolMenuReq = 0;
    BsCursorReq = 1;
    F(s8, bsCur, 1) = 0;
}
