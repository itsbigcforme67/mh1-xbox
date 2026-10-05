/* lb_bz125 - lobby UI/client 0x005BE620-0x005BE6BC: Set_ErrorDialog (first drafted by tools/lbauto.py). */
#include "lobby_f.h"

void Set_ErrorDialog(int arg0) {
    int temp_v1;

    temp_v1 = (s8)arg0;
    switch (temp_v1) {
    case 1:
        SetDialogData(9, 5);
        return;
    case 3:
        SetDialogData(8, 3);
        return;
    case 4:
    case 6:
        SetDialogData(0xA, 3);
        return;
    case 5:
    case 7:
        SetDialogData_HTML((s32)cw + 0x32D1);
        /* fallthrough */
    default:
        return;
    }
}
