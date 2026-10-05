/* lb_z42 - auto-drafted 0x005D9400-0x005D9408: BsParseCancel (first drafted by tools/lbauto.py). */
#include "lobby.h"
extern s8 ParseReq;

void BsParseCancel(void) {
    ParseReq = 0;
}
