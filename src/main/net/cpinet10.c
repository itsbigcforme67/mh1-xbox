/* cpinet10 - TCP recv / send (SLPM_654.95 0x00235850-0x002358D8). Whole file in cpinet2_nm.c. */
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
int CpInetTcpRecv(int sock, int buf, int len) {
    return common_error((s16)Ave_TcpRecv((s16)sock, buf, len));
}
int CpInetTcpSend(int sock, int buf, int len) {
    int r;

    r = common_error((s16)Ave_TcpSend((s16)sock, buf, len));
    if (r >= 0) {
    } else {
        return r;
    }
    if (r == 0) {
        r = (s16)len;
    }
    return r;
}
