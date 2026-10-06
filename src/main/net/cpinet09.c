/* cpinet09 - TCP get / set option (SLPM_654.95 0x00235600-0x0023571C). Whole file in cpinet2_nm.c. */
/* cpinet2_nm - SLPM_654.95 0x002352E0-0x002359D0 (f_common.s): CpInet error mapping, interface status / problem tracking and
   the TCP socket wrappers (Ave_Tcp* RPC layer). Working file; see cpinet_nm.c for the first half. */
#include "types.h"
extern s32 Inet_interface_problem_disable[4];
extern s32 Inet_interface_status[4];
extern s32 Inet_interface_problem_status[4];
extern s8 Initialized_flags[16];
int Ave_SifCallRpcEnd();
int Ave_TcpInitialize();
int Ave_TcpOpen();
int Ave_GetOpt(s16, int, int, int);
int Ave_SetOpt(s16, int, int, int);
int Ave_TcpRecv();
int Ave_TcpSend();
int Ave_TcpClose();
int Ave_TcpAbort();
int Ave_TcpDelete();
int CpInetDevChanged();
int CpInetDhcpTimer();
int CpInetPppGetStatus();
void CpInetTcpDelayCloseReset(void);
int CpInetTcpNbCallEnd(void);
int common_error(int);
typedef struct TCPOPEN {
    s32 addr;                   /* 0x00 */
    u16 port;                   /* 0x04 */
    u16 lport;                  /* 0x06 */
} TCPOPEN;
int CpInetTcpGetOption(int sock, int buf) {
    int r;

    r = common_error((s16)Ave_GetOpt(sock, 0x2008, 4, buf));
    if (r >= 0) {
    } else {
        return r;
    }
    return common_error((s16)Ave_GetOpt(sock, 0x2001, 4, buf + 4));
}
int CpInetTcpSetOption(int sock, int buf) {
    int r;

    r = common_error((s16)Ave_SetOpt(sock, 0x2008, 4, buf));
    if (r >= 0) {
    } else {
        return r;
    }
    return common_error((s16)Ave_SetOpt(sock, 0x2001, 4, buf + 4));
}
