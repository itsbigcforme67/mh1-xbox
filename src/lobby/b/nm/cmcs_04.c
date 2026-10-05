#include "lobby_a.h"
extern s16 Vs_Cnt_0;
extern u8 COM_R_No_1;
extern char recv_header[];
extern char recv_header[];
extern char D_3A3B71[];
extern char send_work[];
extern char send_work[];
extern char send_work[];
extern char send_work[];
extern char send_work[];
extern char send_work[];
extern char send_work[];
extern char send_work[];
extern char send_work[];
extern char recv_work[];
void cmcs_04(void) {
    int sp3C;
    int sp30;
    int sp20;
    s16 temp_a0;
    s32 temp_v0;
    int temp_s0;

    temp_a0 = Vs_Cnt_0;
    Vs_Cnt_0 = (s16) (temp_a0 - 1);
    if (temp_a0 < 0) {
        COM_R_No_1 = 7U;
        *(s8 *)0x4E4723 = 5;
        return;
    }
    temp_v0 = select_ps2(*(s32 *)0x4E36F4, &recv_header, &recv_work, 0x600);
    switch (temp_v0) {                              /* irregular */
    case 0:
        return;
    case -1:
        COM_R_No_1 = 7U;
        *(u8 *)0x4E4723 = 5;
        return;
    default:
    case 1:
        if (((((F(u8, &recv_header, 2) << 8) & 0xFFFF) | F(u8, &recv_header, 3)) & 0xFFFF) != 0x1031) {
            return;
        }
        mmbbc_encode(&sp20, &D_3A3B71, (((F(u8, &recv_header, 6) << 8) & 0xFFFF) + F(u8, &recv_header, 7)) & 0xFFFF);
        Mcs_SetSendCommand(&send_work, 0x1031);
        SetSendCategory(&send_work, 2);
        F(u8, &send_work, 0xA) = (u8) F(u8, &recv_header, 6);
        F(u8, &send_work, 0xB) = (u8) F(u8, &recv_header, 7);
        SetSendStringData(&send_work, &sp20, 0xA);
        SetSendCommandLen(&send_work);
        memcpy(&sp30, (int)&send_work + 4, 0xCU);
        memcpy(&sp3C, (int)&send_work + 0x10, F(u16, &send_work, 0));
        temp_s0 = F(u16, &send_work, 0) + 0xC;
        if (temp_s0 != CpInetTcpSend(*(u8 *)0x4E36F4, &sp30,  (temp_s0 << 0x30) >> 0x30)) {
            COM_R_No_1 = 7U;
            *(u8 *)0x4E4723 = 5;
            return;
        }
        COM_R_No_1 = (u8) (COM_R_No_1 + 1);
        return;
    }
}
