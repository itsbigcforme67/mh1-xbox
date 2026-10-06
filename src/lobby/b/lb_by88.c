/* lb_by88 - agent B promoted near-match 0x005B8820-0x005B8974: lbc_login_error (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern char text_lobby_trans_ot0[];
typedef struct { u8 pad0[0x2C33]; s8 x2C33; u8 x2C34; u8 pad2C35[0x17]; s32 x2C4C; } CWS_le;
#define CWX ((CWS_le *)cw)

void lbc_login_error(void) {
    u16 sw;

    sw = Get_sw2(0);
    switch (CWX->x2C34) {
    case 0:
        Lbc_init_network_work(1);
        Lbc_set_prim(&text_lobby_trans_ot0, 0, 0);
        CWX->x2C34++;
        fade_set(2);
        CWX->x2C4C = 0x258;
        SetDialogData_HTML((u8 *)cw + 0x32D1);
        break;
    case 1:
        F(s8, pNet, 0xC) = 1;
        CWX->x2C4C--;
        if (CWX->x2C4C > 0) {
            if (CWX->x2C4C < 0x1E0 && (sw & 0x20)) {
                CWX->x2C34++;
                cnWrap_SoundRequest(0);
                fade_set(1);
            }
        } else {
            CWX->x2C34++;
            fade_set(1);
        }
        break;
    case 2:
        if ((Fade_busy_ck() & 0xFF) != 1) {
            F(s8, pNet, 0x11) = 1;
            CWX->x2C33 = 4;
            CWX->x2C34 = 1;
        }
        break;
    }
}
