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

int common_error(int e) {
    return e;
}

int CpInetTcpNbCallEnd(void) {
    return (s16)Ave_SifCallRpcEnd();
}

int CpInetInterfaceGetStatus(void) {
    return Inet_interface_status[0];
}

int CpInetTcpConnected(void) {
    return Inet_interface_status[0] == 3;
}

int CpInetInterfaceProblemEnable(int on) {
    return Inet_interface_problem_disable[0] = !on;
}

/* Polls for network trouble (device change, DHCP lease lost, PPP drop) and latches it in Inet_interface_problem_status
   (1 = lost, 2 = changed); returns the latched value. */
int CpInetInterfaceProblem(int kind, int dhcp) {
    int dn;
    u8 st[0x1C];
    int dev;

    if (Inet_interface_problem_status[0] != 0) {
        return Inet_interface_problem_status[0];
    }
    if (Inet_interface_problem_disable[0] != 0) {
        return Inet_interface_problem_status[0];
    }
    if (CpInetTcpNbCallEnd() == 0) {
        return Inet_interface_problem_status[0];
    }
    switch (kind) {
    case 1:
        if (dhcp != 0 && CpInetDhcpTimer() < 0) {
            Inet_interface_problem_status[0] = 1;
            break;
        }
        dev = CpInetDevChanged(&dn);
        switch (dev) {
        case 0:
            break;
        case 1:
            Inet_interface_problem_status[0] = 2;
            break;
        }
        break;
    case 2:
    case 3:
        if (CpInetPppGetStatus(st) < 0) {
            Inet_interface_problem_status[0] = 1;
            break;
        }
        if (*(s16 *)(st + 0xC) != 4) {
            Inet_interface_problem_status[0] = 2;
        }
        break;
    }
    return Inet_interface_problem_status[0];
}

int CpInetGetStatus(void) {
    return Inet_interface_problem_status[0];
}

/* Starts the TCP stack for the interface kind (1 ethernet, 2/3 modem / ppp) and provider; returns the Ave result. */
int CpInetTcpInitialize(int kind, int prov) {
    int r;

    switch (kind) {
    case 3:
    case 2:
        switch (prov) {
        case 5:
            r = (s16)Ave_TcpInitialize(0, 1);
            break;
        default:
            r = (s16)Ave_TcpInitialize(0, 4);
            break;
        }
        break;
    case 1:
        switch (prov) {
        case 1:
        case 6:
            r = (s16)Ave_TcpInitialize(1, 5);
            break;
        default:
            r = (s16)Ave_TcpInitialize(0, 2);
            break;
        }
        break;
    }
    Initialized_flags[0] = 1;
    Inet_interface_status[0] = 1;
    CpInetTcpDelayCloseReset();
    return r;
}

typedef struct TCPOPEN {
    s32 addr;                   /* 0x00 */
    u16 port;                   /* 0x04 */
    u16 lport;                  /* 0x06 */
} TCPOPEN;

int CpInetTcpOpen(TCPOPEN *o) {
    return common_error((s16)Ave_TcpOpen(o->addr, o->port, o->lport));
}

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

int CpInetTcpClose(int *sock) {
    int r = 0;
    int t;

    t = *sock;
    if (t >= 0) {
        r = (s16)Ave_TcpClose((s16)t);
        *sock = -1;
        return common_error(r);
    }
    *sock = -1;
    return r;
}

int CpInetTcpAbort(int sock) {
    return common_error((s16)Ave_TcpAbort((s16)sock));
}

int CpInetTcpDelete(int *sock) {
    int r = 0;
    int t;

    t = *sock;
    if (t >= 0) {
        r = (s16)Ave_TcpDelete((s16)t);
        *sock = -1;
        return common_error(r);
    }
    *sock = -1;
    return r;
}
