/* lb_z56 - auto-drafted 0x005F7E50-0x005F7E70: ExitDialog (first drafted by tools/lbauto.py). */
#include "lobby.h"
extern s8 BsCursorReq;
extern s8 BsDialogReq;
extern u8 * bsCur;
extern u8 * bsSys;

void ExitDialog(void) {
    F(s8, bsSys, 0x33) = 0;
    BsDialogReq = 1;
    BsCursorReq = 1;
    F(s8, bsCur, 1) = 0;
}
