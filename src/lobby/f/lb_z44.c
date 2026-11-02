/* lb_z44 - auto-drafted 0x005E2A90-0x005E2AB4: To_DisplayDialog (first drafted by tools/lbauto.py). */
#include "lobby_f.h"
extern s8 BsCursorReq;
extern s8 BsDialogReq;
extern u8 * bsCur;

#ifdef VERSION_MH2
#include "mh2_lb.h"
#define BsDialogReq BW_DIALOGREQ
#define BsCursorReq BW_CURSORREQ
#endif
#ifdef VERSION_MH2
void To_DisplayDialog(s8 arg0, s8 arg1) {
    BsDialogReq = arg0;
    BsCursorReq = 2;
    F(s8, bsCur, 2) = arg1;
    F(s8, bsCur, 1) = 3;
    BW8(0x1C296) = 0;
}
#else
void To_DisplayDialog(s8 arg0, s8 arg1) {
    BsDialogReq = arg0;
    BsCursorReq = 2;
    F(s8, bsCur, 2) = arg1;
    F(s8, bsCur, 1) = 3;
}
#endif
