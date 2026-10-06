/* lb_by75 - agent B promoted near-match 0x005BD440-0x005BD51C: lbc_in_lobby_03_02 (first drafted by tools/lbauto.py). */
#include "lobby_f.h"
extern char CallBack_Result_Lobby_RoomExit[];
typedef struct { u8 pad0[0x2C35]; u8 x2C35; u8 pad2C36[0xF]; s8 x2C45; u8 pad2C46[6]; s32 x2C4C; } CWS_l302;
#define CWX ((CWS_l302 *)cw)

void lbc_in_lobby_03_02(void) {
    switch (CWX->x2C35) {
    case 0:
        Lbc_init_network_work();
        CWX->x2C35++;
        CWX->x2C4C = 0x44;
        nwSetEff_FreeDialog(0x14, (u8 *)cw + 0x32D1);
        /* fallthrough */
    case 1:
        CWX->x2C4C--;
        if (CWX->x2C4C <= 0) {
        case 2:
            CWX->x2C35++;
            CallBackWaitInit();
            CWX->x2C45 = 0x18;
            cnLBS_RoomExit(&CallBack_Result_Lobby_RoomExit);
            return;
        }
        break;
    case 3:
        Check_CallBackWait();
    }
}
