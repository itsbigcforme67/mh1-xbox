/* cpinet02 - TCP terminate and DNS wrappers (SLPM_654.95 0x002359D0-0x00235B6C). Whole file in cpinet_nm.c. */
/* cpinet_nm - SLPM_654.95 0x00235260-0x002368D0 (f_cpinet / g_CpInetTcpTerminate.s): thin wrappers of the PS2 network stack
   (Ave_* RPC layer): PPP / network device / DHCP / DNS / route set-up, the status words Inet_interface_status[0] and
   Initialized_flags[] (byte per module: 0 tcp, 1 ppp, 2 ndg, 3 interface, 4 dns, 5 route, 6 dhcp). Results are s16 and
   mapped to -1 on an error. Working file. */
#include "types.h"
#ifndef __MWERKS__
short Ave_DgGetDeviceInfo();   /* (declared before its first use) */
short Ave_DgSelectDevice();   /* (declared before its first use) */
short Ave_DnsGetTicket();   /* (declared before its first use) */
short Ave_DnsInit();   /* (declared before its first use) */
short Ave_DnsReleaseTicket();   /* (declared before its first use) */
short Ave_Ifconfig();   /* (declared before its first use) */
short Ave_NdgStart();   /* (declared before its first use) */
short Ave_PppSetOption();   /* (declared before its first use) */
short Ave_SifBindRpc();   /* (declared before its first use) */
short Ave_SifUnBindRpc();   /* (declared before its first use) */
short Ave_TcpTerminate();   /* (declared before its first use) */
#endif
extern u8 Inet_device_change_flag[16];
/* far (non-gp) data: declared with a size > 8 so lui/addiu is used */
extern s32 Inet_device_change_num[4];
extern s32 Inet_interface_problem_disable[4];
extern s32 Inet_interface_status[4];
extern s32 Inet_interface_problem_status[4];
extern s8 Initialized_flags[16];
extern s8 tcp_delay_close_num[16];
#ifdef __MWERKS__
int Ave_SifBindRpc();
#else
short Ave_SifBindRpc();   /* the real return type (tools/check_kr_returns.py) */
#endif
#ifdef __MWERKS__
int Ave_SifUnBindRpc();
#else
short Ave_SifUnBindRpc();   /* the real return type (tools/check_kr_returns.py) */
#endif
#ifdef __MWERKS__
int Ave_TcpTerminate();
#else
short Ave_TcpTerminate();   /* the real return type (tools/check_kr_returns.py) */
#endif
#ifdef __MWERKS__
int Ave_DnsInit();
#else
short Ave_DnsInit();   /* the real return type (tools/check_kr_returns.py) */
#endif
int Ave_DnsDisp();
#ifdef __MWERKS__
int Ave_DnsGetTicket();
#else
short Ave_DnsGetTicket();   /* the real return type (tools/check_kr_returns.py) */
#endif
#ifdef __MWERKS__
int Ave_DnsReleaseTicket();
#else
short Ave_DnsReleaseTicket();   /* the real return type (tools/check_kr_returns.py) */
#endif
int Ave_DnsLookUp();
int Ave_PppInit();
#ifdef __MWERKS__
int Ave_PppSetOption();
#else
short Ave_PppSetOption();   /* the real return type (tools/check_kr_returns.py) */
#endif
int Ave_PppStatus();
int Ave_PppGetUsbDeviceId();
int Ave_PppFinish();
int Ave_PppDisp();
int Ave_NdgInit();
#ifdef __MWERKS__
int Ave_Ifconfig();
#else
short Ave_Ifconfig();   /* the real return type (tools/check_kr_returns.py) */
#endif
#ifdef __MWERKS__
int Ave_NdgStart();
#else
short Ave_NdgStart();   /* the real return type (tools/check_kr_returns.py) */
#endif
int Ave_NdgPolling();
int Ave_Ifdown();
int Ave_DgGetOption();
int Ave_NdgFinish();
int Ave_NdgDisp();
int Ave_RouteAdd();
int Ave_RouteDel();
int Ave_DhcpInit();
int Ave_DhcpDisp();
int Ave_DhcpRequestNb();
int Ave_DhcpTimer();
int Ave_DhcpGetIfInfo();
int Ave_DhcpGetDns();
int Ave_DhcpGetGateway();
int Ave_DhcpReleaseNb();
int Ave_DgGetDeviceInfoNum();
#ifdef __MWERKS__
int Ave_DgGetDeviceInfo();
#else
short Ave_DgGetDeviceInfo();   /* the real return type (tools/check_kr_returns.py) */
#endif
#ifdef __MWERKS__
int Ave_DgSelectDevice();
#else
short Ave_DgSelectDevice();   /* the real return type (tools/check_kr_returns.py) */
#endif
s16 CpInetTcpTerminate(void) {
    s16 r = Ave_TcpTerminate();

    Initialized_flags[0] = 0;
    Inet_interface_status[0] = 0;
    Inet_interface_problem_status[0] = 0;
    return r;
}
s16 CpInetDnsInitialize(void) {
    s16 r = Ave_DnsInit();

    switch (r) {
    case -33:
    case -1:
        r = -1;
        break;
    }
    Initialized_flags[4] = 1;
    return r;
}
s16 CpInetDnsDispose(void) {
    s16 r = Ave_DnsDisp();

    switch (r) {
    case -33:
    case -1:
        r = -1;
        break;
    }
    Initialized_flags[4] = 0;
    return r;
}
s16 CpInetDnsGetTicket(void) {
    s16 r = Ave_DnsGetTicket();

    switch (r) {
    case -33:
    case -1:
        r = -1;
        break;
    }
    return r;
}
s16 CpInetDnsReleaseTicket(void) {
    s16 r = Ave_DnsReleaseTicket();

    switch (r) {
    case -33:
    case -1:
        r = -1;
        break;
    }
    return r;
}
