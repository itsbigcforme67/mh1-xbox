/* lb_z45 - auto-drafted 0x005E5D20-0x005E5D30: BS_DispDialog (first drafted by tools/lbauto.py). */
#include "lobby.h"
extern s8 BsDialogReq;
extern s8 BsMCStatus;

void BS_DispDialog(s8 arg0) {
    BsMCStatus = arg0;
    BsDialogReq = 4;
}
