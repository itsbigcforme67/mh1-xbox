/* lb_z41 - auto-drafted 0x005D93C0-0x005D9400: BsParseStart (first drafted by tools/lbauto.py). */
#include "lobby.h"
extern s32 ParseArg;
extern s32 ParseRet;
extern u8 ParseReq;

void BsParseStart(s32 arg0) {
    if (ParseReq == 0) {
        parsetag_ready();
        ParseArg = arg0;
        ParseReq = 1U;
        ParseRet = 0;
    }
}
