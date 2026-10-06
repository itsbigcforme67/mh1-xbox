/* lbui, run 16: plaza_checkMyStatusTrans .. plaza_checkMyStatusTrans (lobby.bin 0x0059C2F0-0x0059C324): the matching functions of lbui_nm.c. */
#pragma readonly_strings on
#include "lbui_proto.h"

void plaza_checkMyStatusTrans(void) {
    disp_status(0xD8, 0x50, CW->x440, CW->x448, my_user_mini_data, *(s8 *)((u8 *)pNet + 0x24), 3, D_3C73B4);
}
