/* cpinet09 - TCP get / set option (SLPM_654.95 0x00235600-0x0023571C). Whole file in cpinet2_nm.c. */
/* cpinet2_nm - SLPM_654.95 0x002352E0-0x002359D0 (f_common.s): CpInet error mapping, interface status / problem tracking and
   the TCP socket wrappers (Ave_Tcp* RPC layer). Working file; see cpinet_nm.c for the first half. */
#include "types.h"
#ifndef __MWERKS__
short Ave_SetOpt(s16, int, int, int);   /* (declared before its first use) */
short Ave_TcpAbort();   /* (declared before its first use) */
short Ave_TcpClose();   /* (declared before its first use) */
short Ave_TcpDelete();   /* (declared before its first use) */
short Ave_TcpInitialize();   /* (declared before its first use) */
short Ave_TcpOpen();   /* (declared before its first use) */
short CpInetDhcpTimer();   /* (declared before its first use) */
#endif
extern s32 Inet_interface_problem_disable[4];
extern s32 Inet_interface_status[4];
extern s32 Inet_interface_problem_status[4];
extern s8 Initialized_flags[16];
int Ave_SifCallRpcEnd();
#ifdef __MWERKS__
int Ave_TcpInitialize();
#else
short Ave_TcpInitialize();   /* the real return type (tools/check_kr_returns.py) */
#endif
#ifdef __MWERKS__
int Ave_TcpOpen();
#else
short Ave_TcpOpen();   /* the real return type (tools/check_kr_returns.py) */
#endif
int Ave_GetOpt(s16, int, int, int);
#ifdef __MWERKS__
int Ave_SetOpt(s16, int, int, int);
#else
short Ave_SetOpt(s16, int, int, int);   /* the real return type (tools/check_kr_returns.py) */
#endif
int Ave_TcpRecv();
int Ave_TcpSend();
#ifdef __MWERKS__
int Ave_TcpClose();
#else
short Ave_TcpClose();   /* the real return type (tools/check_kr_returns.py) */
#endif
#ifdef __MWERKS__
int Ave_TcpAbort();
#else
short Ave_TcpAbort();   /* the real return type (tools/check_kr_returns.py) */
#endif
#ifdef __MWERKS__
int Ave_TcpDelete();
#else
short Ave_TcpDelete();   /* the real return type (tools/check_kr_returns.py) */
#endif
int CpInetDevChanged();
#ifdef __MWERKS__
int CpInetDhcpTimer();
#else
short CpInetDhcpTimer();   /* the real return type (tools/check_kr_returns.py) */
#endif
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
