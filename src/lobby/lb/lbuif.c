/* lbui, run 6: getUserInfo .. Lb_get_comment (lobby.bin 0x00594FD0-0x005950B8): the matching functions of lbui_nm.c. */
#pragma readonly_strings on
#include "lbui_proto.h"

int getUserInfo(void) {
    switch (Lbs_SeekId()) {
    case 0:
        Lbc_RequestNetComment(CW->x2F80);
        return 0;
    case 1:
        return 1;
    default:
        return 2;
    }
}

int Lb_get_comment(a)
int a;
{
    int id = Lb_get_plID() & 0xFF;

    if (id != 0xFF) {
        memset(CW->comment[id], 0, 0x62);
        Lbc_RequestNetComment(a);
        return 1;
    }
    return 0;
}
