#include "lobby_a.h"
extern s16 cnt_441;
extern s8 COM_R_No_4;
extern s8 SecCunt;
extern s8 TryCunt;
extern u16 PORT_NUMBER_439;
extern u8 COM_R_No_2;
extern char _fqdn_tmp_440[];
extern char _fqdn_tmp_440[];
extern char bsCsvWork[];
extern char ConnectLbsId[];
extern char _fqdn_tmp_440[];
extern char _fqdn_tmp_440[];
extern char _fqdn_tmp_440[];
extern char D_4E36F4[];
extern char D_4E3714[];
extern char D_4E36F4[];
s32 tcp_init(void) {
    s32 sp3C;
    int var_s0;
    s32 temp_a0;
    s32 temp_v0;
    s32 temp_v0_3;
    s32 var_s1;
    u16 temp_v0_2;
    int var_a1;
    int var_a1_2;
    u8 temp_v1;
    u8 temp_v1_2;
    u8 temp_v1_3;

    temp_v1 = COM_R_No_2;
    sp3C = 0;
    switch (temp_v1) {                              /* switch 1; irregular */
    case 0:                                         /* switch 1 */
        _fqdn_tmp_440[0] = 0;
        PORT_NUMBER_439 = 0U;
        cnt_441 = 0;
        COM_R_No_4 = 0;
        memset(&_fqdn_tmp_440, 0, 0x40);
        var_s1 = 0;
        var_s0 = (int)&bsCsvWork;
loop_6:
        if (strncmp(&ConnectLbsId, var_s0 + 0x603, 0xC) == 0) {
            strncpy(&_fqdn_tmp_440, (int)&bsCsvWork + (var_s1 * 0x62) + 0x22F, 0x40);
        } else {
            var_s1 += 1;
            var_s0 += 0x101;
            if (var_s1 < 0xA) {
                goto loop_6;
            }
        }
        var_a1 = (int)&_fqdn_tmp_440;
loop_11:
        temp_v1_2 = (*(u8 *)var_a1);
        if ((temp_v1_2 != 0x3A) && (temp_v1_2 != 0)) {
            var_a1 += 1;
            goto loop_11;
        }
        if (temp_v1_2 == 0x3A) {
            (*(u8 *)var_a1) = 0;
            var_a1_2 = var_a1 + 1;
loop_19:
            temp_v1_3 = (*(u8 *)var_a1_2);
            if (temp_v1_3 != 0x20) {
                if (temp_v1_3 == 0) {

                } else {
                    PORT_NUMBER_439 = (u16) (PORT_NUMBER_439 * 0xA);
                    temp_a0 = (*(u8 *)var_a1_2) - 0x30;
                    var_a1_2 += 1;
                    PORT_NUMBER_439 = (u16) (PORT_NUMBER_439 + (temp_a0 & 0xFFFF));
                    goto loop_19;
                }
            }
        }
        COM_R_No_2 = 1U;
        return -2;
    case 1:                                         /* switch 1 */
        temp_v0 = InetDnsGetIPAddress(&COM_R_No_4, &cnt_441, &sp3C, &_fqdn_tmp_440);
        switch (temp_v0) {                          /* switch 2; irregular */
        case 0:                                     /* switch 2 */
            return -2;
        case 1:                                     /* switch 2 */
            COM_R_No_4 = 0;
        default:                                    /* switch 2 */
            temp_v0_2 = PORT_NUMBER_439;
            temp_v0_3 = connect_ps2(sp3C, (((temp_v0_2 << 8) & 0xFF00) | ((temp_v0_2 >> 8) & 0xFF)) & 0xFFFF, 0);
            *(s32 *)0x4E36F4 = temp_v0_3;
            if (temp_v0_3 < 0) {
                return -1;
            }
            COM_R_No_2 = 2U;
            TryCunt = 0;
            SecCunt = 0x3C;
            return -2;
        case -1:                                    /* switch 2 */
            COM_R_No_4 = 0;
            return -1;
        }
        break;
    case 2:                                         /* switch 1 */
        SecCunt = (s8) (SecCunt - 1);
        if (SecCunt <= 0) {
            TryCunt = (s8) (TryCunt + 1);
            SecCunt = 0x3C;
            if (TryCunt >= 0x15) {
                CpInetTcpAbort(*(u8 *)0x4E36F4);
                CpInetTcpDelete(&D_4E36F4);
                SecCunt = 0;
                TryCunt = 0;
                return -1;
            }
        }
        if (CpInetTcpGetStatus(*(u8 *)0x4E36F4, &D_4E3714) < 0) {
            CpInetTcpAbort(*(u8 *)0x4E36F4);
            CpInetTcpDelete(&D_4E36F4);
            return -1;
        }
        if (*(s16 *)0x4E3714 != 4) {
            return -2;
        }
        SecCunt = 0;
        TryCunt = 0;
        COM_R_No_2 = 0U;
        return 0;
    default:                                        /* switch 1 */
        return -1;
    }
}
