/* lb_by99 - agent B promoted near-match 0x005B8ED0-0x005B9024: lbc_login_finish_after (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
typedef struct { u8 pad0[5]; u8 x05; u8 pad06[0x26]; } CNW5;
extern CNW5 CnetWork;
typedef struct { u8 pad0[0x2C34]; u8 x2C34; u8 pad2C35[0x17]; s32 x2C4C; u8 pad2C50[0x982]; s8 x35D2; } CWS_lfa;
#define CWX ((CWS_lfa *)cw)

void lbc_login_finish_after(void) {
    switch (CWX->x2C34) {
    case 0:
        if (CnetWork.x05 == 3) {
            CWX->x2C34++;
            McOperationSet(7, 2);
            fade_set(2);
            CWX->x2C4C = 3;
            str_stop_all();
            break;
        }
        CWX->x2C34 = 2;
        break;
    case 1:
        if (CWX->x2C4C == 0) {
            if (McCardOperation(CWX, &CWX->x2C4C) & 0xFF) {
                CWX->x2C34++;
                fade_set(1);
            }
        } else {
            CWX->x2C4C--;
        }
        break;
    case 2:
        if ((Fade_busy_ck() & 0xFF) != 1) {
            cnLBS_Send_LoginFinish();
            if (CnetWork.x05 == 3) {
                To_MyLobby();
                CWX->x35D2 = 1;
            } else {
                To_MyLobby();
                CWX->x35D2 = 0;
            }
            CnetWork.x05 = 2;
            Q_camera_init();
        }
        break;
    }
}
