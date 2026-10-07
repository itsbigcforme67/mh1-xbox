/* lb_pz08 - lobby.bin 0x00594410-0x00594840: plaza_enterLobby(), the lobby choice menu of a plaza (14 lobbies, 7 per column), member list request, enter-lobby download. Returns 3 = back, 2 = running (shared return at the end), 1 = error acknowledged, 0 = entered. Aliases D_3A1622 in config/lobby_aliases.txt. */
#pragma readonly_strings on
#include "lbui_proto.h"
#define X0A(p) (*(u8 *)&(p)->x0A)
extern u8 tl_member_buff[];
extern u8 D_3A1622[];
int Lbs_GetLobbyMemberList();
int Lbs_request_enter_lobby();
int Lbc_DownloadQuest();
void To_EnterLobby();

int plaza_enterLobby()
{
    u16 sw = Get_sw2(0);

    switch (pNet->step) {
    case 0:
        pNet->step++;
        break;
    case 1:
        pNet->x28 = Get_sw_on2(0);
        if (sw & 0x20) {
            pNet->step = 4;
            ClassInfo.lobby = X0A(pNet) + 1;
            cw[0x2C41] = X0A(pNet);
            cw[0x2C35] = 0;
            SetDialogData(0x15, 5);
            pNet->x0C = 1;
            cnWrap_SoundRequest(0);
            break;
        }
        if (sw & 0x40) {
            return 3;
        }
        if (sw & 0x2000) {
            if (X0A(pNet) % 7 != 0) {
                X0A(pNet) = X0A(pNet) - 1;
            } else {
                X0A(pNet) = X0A(pNet) / 7 * 7 + 6;
            }
            cnWrap_SoundRequest(1);
        } else if (sw & 0x1000) {
            X0A(pNet)++;
            if (X0A(pNet) >= 0xE || X0A(pNet) % 7 == 0) {
                X0A(pNet) -= 7;
            }
            cnWrap_SoundRequest(1);
        } else if (sw & 0xC00) {
            if (X0A(pNet) >= 7) {
                X0A(pNet) -= 7;
            } else {
                X0A(pNet) += 7;
            }
            cnWrap_SoundRequest(1);
        } else if (sw & 0x100) {
            pNet->x06 = *(u16 *)(D_3A1622 + X0A(pNet) * 0x15C);
            cnWrap_SoundRequest(6);
            memset(tl_member_buff, 0, 0x17E0);
            pNet->step++;
        }
        break;
    case 2:
        if (Lbs_GetLobbyMemberList(X0A(pNet)) == 1) {
            pNet->step++;
        }
        break;
    case 3:
        pNet->x28 = Get_sw_on2(0);
        if (sw & 0x40) {
            pNet->step = 1;
            cnWrap_SoundRequest(3);
        }
        break;
    case 4:
        pNet->x0C = 1;
        switch (Lbs_request_enter_lobby()) {
        case 0:
            pNet->step++;
            break;
        case 1:
            pNet->step = 6;
            ((LB_CW *)cw)->x30B6 = 0;
            ClassInfo.lobby = 0;
            SetDialogData_HTML(cw + 0x32D1);
            break;
        }
        break;
    case 5:
        pNet->x0C = 1;
        switch (Lbc_DownloadQuest()) {
        case 0:
            fade_set(0xA);
            To_EnterLobby();
            str_stop(0);
            str_stop(1);
            return 0;
        case 1:
            pNet->step = 6;
            SetDialogData(0xE, 5);
            break;
        }
        break;
    case 6:
        pNet->x0C = 1;
        if (sw & 0x20) {
            cw[0x2C32] = 0;
            cw[0x2C33] = 0;
            cw[0x2C34] = 0;
            return 1;
        }
        break;
    case 7:
        pNet->x0C = 1;
        if (sw & 0x20) {
            pNet->step = 0;
        }
        break;
    }
    return 2;
}
