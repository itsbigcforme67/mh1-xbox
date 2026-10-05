/* lbui, run 13: plaza_chatTrans .. plaza_chatTrans (lobby.bin 0x0059D250-0x0059D314): the matching functions of lbui_nm.c. */
#pragma readonly_strings on
#include "lbui_proto.h"

void plaza_chatTrans(void) {
    s16 idx;

    switch (CW->chatmode) {
    case 0:
        Put_megaphone(0x1F6, 0x32, 3);
        idx = 0;
        break;
    case 1:
        Put_megaphone(0x1F6, 0x32, 1);
        idx = 1;
        break;
    default:
        Put_megaphone(0x1F6, 0x32, 2);
        idx = 2;
        break;
    }
    flfntSetSize(0x16, 0x16);
    font_print_double(0x22E, 0x33, 1, 0, tl_etc[3 + idx]);
}
