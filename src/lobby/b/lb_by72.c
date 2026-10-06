/* lb_by72 - agent B promoted near-match 0x005BAED0-0x005BAF44: lbc_in_plaza_04 (first drafted by tools/lbauto.py). */
#include "lobby_f.h"
typedef struct { u8 pad0[0x2C33]; s8 x2C33; s8 x2C34; u8 pad2C35[7]; s8 x2C3C; u8 pad2C3D[0xF]; s32 x2C4C; } CWS_p4;
#define CWX ((CWS_p4 *)cw)

void lbc_in_plaza_04(void) {
    u16 sw;
    sw = Get_sw2(0);
    if (CWX->x2C4C++ >= 0x258 || (CWX->x2C4C >= 0x3C && (sw & 0x60))) {
        CWX->x2C33 = 0;
        CWX->x2C34 = 0;
        CWX->x2C3C = 0;
    }
}
