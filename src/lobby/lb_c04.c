/* lb_c04 - lobby small helpers 0x005CB750-0x005CB778: GetAdrsMiniData. Whole file in lb_c.c. */
#include "lobby.h"













u8 *GetAdrsMiniData(int id) {
    return CWPLAYER(id & 0xFF) + 0x1346;
}
