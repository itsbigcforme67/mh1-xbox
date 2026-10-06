/* lb_by74 - agent B promoted near-match 0x005BCF70-0x005BD0B4: lbc_in_lobby_03_01 (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern char jtbl_2502[];
extern char CallBack_Result_Lobby_LobbyExit[];
typedef struct { u8 pad0[0x2C35]; u8 x2C35; u8 pad2C36[0xF]; s8 x2C45; u8 pad2C46[6]; s32 x2C4C; u8 pad2C50[0x985]; s8 x35D5; } CWS_l301;
#define CWX ((CWS_l301 *)cw)

void lbc_in_lobby_03_01(void) {
    u8 t;
    t = CWX->x2C35;
    switch (t) {
    case 0:
        CWX->x2C35 = t + 1;
        Lbc_init_network_work();
        CWX->x2C4C = 0x26;
        SetDialogData_HTML((u8 *)cw + 0x32D1);
        /* fallthrough */
    case 1:
        F(s8, pNet, 0xC) = 1;
        CWX->x2C4C--;
        if (CWX->x2C4C <= 0) {
            CWX->x2C35++;
        case 2:
            CWX->x2C35++;
            CallBackWaitInit();
            CWX->x2C45 = 0x16;
            cnLBS_LobbyExit(&CallBack_Result_Lobby_LobbyExit);
            return;
        }
        break;
    case 3:
        Check_CallBackWait();
        break;
    case 4:
        fade_set(1);
        CWX->x2C35++;
        break;
    case 5:
        if ((Fade_busy_ck() & 0xFF) != 1) {
            CWX->x35D5 = 0;
            Lbc_set_prim(0, 0, 0);
            To_EnterPlaza2Lobby();
        }
        break;
    }
}
