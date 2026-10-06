#include "lobby_a.h"
extern s16 cnt_441;
extern s8 COM_R_No_4;
extern s8 SecCunt;
extern s8 TryCunt;
extern u16 PORT_NUMBER_439;
extern u8 COM_R_No_2;
extern char _fqdn_tmp_440[];
extern char bsCsvWork[];
extern char ConnectLbsId[];
extern char D_4E36F4[];
extern char D_4E3714[];
s32 tcp_init(void) {
    s32 sp3C;
    int i;
    char *p;
    u8 *q;
    s32 h;
    u16 port;
    u8 c;

    sp3C = 0;
    switch (COM_R_No_2) {
    case 0:
        _fqdn_tmp_440[0] = 0;
        PORT_NUMBER_439 = 0;
        cnt_441 = 0;
        COM_R_No_4 = 0;
        memset(_fqdn_tmp_440, 0, 0x40);
        i = 0;
        p = bsCsvWork;
        do {
            if (strncmp(ConnectLbsId, p + 0x603, 0xC) == 0) {
                strncpy(_fqdn_tmp_440, bsCsvWork + i * 0x62 + 0x22F, 0x40);
                break;
            }
            i++;
            p += 0x101;
        } while (i < 0xA);
        q = (u8 *)_fqdn_tmp_440;
        for (;;) {
            u8 c = *q;
            if (c == 0x3A || c == 0) break;
            q++;
        }
        if (*q == 0x3A) {
            *q = 0;
            q++;
            while (*q != 0x20 && *q != 0) {
                PORT_NUMBER_439 = (u16) (PORT_NUMBER_439 * 0xA);
                PORT_NUMBER_439 = (u16) (PORT_NUMBER_439 + ((*q - 0x30) & 0xFFFF));
                q++;
            }
        }
        COM_R_No_2 = 1;
        return -2;
    case 1:
        switch (InetDnsGetIPAddress(&COM_R_No_4, &cnt_441, &sp3C, _fqdn_tmp_440)) {
        case 0:
            return -2;
        case 1:
            COM_R_No_4 = 0;
            break;
        case -1:
            COM_R_No_4 = 0;
            return -1;
        }
        port = PORT_NUMBER_439;
        h = connect_ps2(sp3C, (((port << 8) & 0xFF00) | ((port >> 8) & 0xFF)) & 0xFFFF, 0);
        *(s32 *)0x4E36F4 = h;
        if (h < 0) {
            return -1;
        }
        COM_R_No_2 = 2;
        TryCunt = 0;
        SecCunt = 0x3C;
        return -2;
    case 2:
        SecCunt = (s8) (SecCunt - 1);
        if (SecCunt <= 0) {
            TryCunt = (s8) (TryCunt + 1);
            SecCunt = 0x3C;
            if (TryCunt >= 0x15) {
                CpInetTcpAbort(*(s32 *)0x4E36F4);
                CpInetTcpDelete(&D_4E36F4);
                SecCunt = 0;
                TryCunt = 0;
                return -1;
            }
        }
        if (CpInetTcpGetStatus(*(s32 *)0x4E36F4, &D_4E3714) < 0) {
            CpInetTcpAbort(*(s32 *)0x4E36F4);
            CpInetTcpDelete(&D_4E36F4);
            return -1;
        }
        if (*(s16 *)0x4E3714 != 4) {
            return -2;
        }
        SecCunt = 0;
        TryCunt = 0;
        COM_R_No_2 = 0;
        return 0;
    default:
        return -1;
    }
}
