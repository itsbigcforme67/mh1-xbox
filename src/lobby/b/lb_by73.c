/* lb_by73 - agent B promoted near-match 0x005BBD30-0x005BBE6C: lbc_in_lobby_00_05 (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
typedef struct { u16 x0; u8 pad2[0xE]; u8 x10; u8 pad11[0x14B]; } RINFO;
extern RINFO RoomInfo[];
extern char ClassInfo[];
typedef struct { u8 pad0[0x2C33]; s8 x2C33; s8 x2C34; s8 x2C35; s8 x2C36; u8 pad2C37[0x15]; s32 x2C4C; u8 pad2C50[0x673]; s8 x32C3; s8 x32C4; u8 pad32C5[0xD]; s8 x32D1_; } CWS_l005;
#define CWX ((CWS_l005 *)cw)

void lbc_in_lobby_00_05(void) {
    int idx;
    CWX->x2C4C--;
    if (CWX->x2C4C < 0) {
        idx = CWX->x32C3 + CWX->x32C4;
        switch (RoomInfo[idx].x10) {
        case 3:
            F(s8, &ClassInfo, 8) = (s8) RoomInfo[idx].x0;
            CWX->x2C33 = 2;
            CWX->x2C34 = 0;
            CWX->x2C35 = 0;
            CWX->x2C36 = 0;
            break;
        case 1:
            F(s8, &ClassInfo, 8) = (s8) RoomInfo[idx].x0;
            CWX->x2C33 = 1;
            CWX->x2C34 = 0;
            CWX->x2C35 = 0;
            CWX->x2C36 = 0;
            F(s8, (u8 *)cw, 0x32C2) = 0;
            break;
        default:
        case 0:
        case 2:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
            cnLBS_Get_ServerMessage((u8 *)cw + 0x32D1);
            SetDialogData_HTML((u8 *)cw + 0x32D1);
            *(s8 *)0x3F36AB = 0;
            F(s32, &lb_sys, 0x6C) = 1;
            F(s32, &lb_sys, 0x68) = 0x17;
            break;
        }
    }
}
