/* cpinet06 - PPP finish .. device select (SLPM_654.95 0x00236370-0x002368BC). Whole file in cpinet_nm.c. */
/* cpinet_nm - SLPM_654.95 0x00235260-0x002368D0 (f_cpinet / g_CpInetTcpTerminate.s): thin wrappers of the PS2 network stack
   (Ave_* RPC layer): PPP / network device / DHCP / DNS / route set-up, the status words Inet_interface_status[0] and
   Initialized_flags[] (byte per module: 0 tcp, 1 ppp, 2 ndg, 3 interface, 4 dns, 5 route, 6 dhcp). Results are s16 and
   mapped to -1 on an error. Working file. */
#include "types.h"
extern u8 Inet_device_change_flag[16];
/* far (non-gp) data: declared with a size > 8 so lui/addiu is used */
extern s32 Inet_device_change_num[4];
extern s32 Inet_interface_problem_disable[4];
extern s32 Inet_interface_status[4];
extern s32 Inet_interface_problem_status[4];
extern s8 Initialized_flags[16];
extern s8 tcp_delay_close_num[16];
int Ave_SifBindRpc();
int Ave_SifUnBindRpc();
int Ave_TcpTerminate();
int Ave_DnsInit();
int Ave_DnsDisp();
int Ave_DnsGetTicket();
int Ave_DnsReleaseTicket();
int Ave_DnsLookUp();
int Ave_PppInit();
int Ave_PppSetOption();
int Ave_PppStatus();
int Ave_PppGetUsbDeviceId();
int Ave_PppFinish();
int Ave_PppDisp();
int Ave_NdgInit();
int Ave_Ifconfig();
int Ave_NdgStart();
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
int Ave_DgGetDeviceInfo();
int Ave_DgSelectDevice();
s16 CpInetPppFinish(void) {
    s16 r = Ave_PppFinish();

    Inet_interface_status[0] = 5;
    return r;
}
s16 CpInetPppCleanup(void) {
    s16 r = Ave_PppDisp();

    Initialized_flags[1] = 0;
    return r;
}
s16 CpInetNdgInitialize(void) {
    s16 r = Ave_NdgInit();

    Initialized_flags[2] = 1;
    return r;
}
int CpInetNdgSetConfig(int *cfg) {
    int b = cfg[1];
    int a = cfg[0];

    return -((s16)Ave_Ifconfig(a, b, a | ~(a & b)) < 0);
}
s16 CpInetNdgStart(s16 n) {
    s16 r = Ave_NdgStart(n);

    Inet_interface_status[0] = 2;
    return r;
}
s16 CpInetNdgPolling(void) {
    return Ave_NdgPolling();
}
int CpInetNdgIfUp(void) {
    Inet_interface_problem_status[0] = 0;
    Inet_interface_status[0] = 3;
    Initialized_flags[3] = 1;
    return 0;
}
int CpInetNdgIfDown(void) {
    Ave_Ifdown();
    Initialized_flags[3] = 0;
    Inet_interface_status[0] = 4;
    return 0;
}
int CpInetNdgGetStatus(int v) {
    return -((s16)Ave_DgGetOption(8, 4, v) != 4);
}
s16 CpInetNdgFinish(void) {
    s16 r = Ave_NdgFinish();

    Inet_interface_status[0] = 5;
    return r;
}
s16 CpInetNdgCleanup(void) {
    s16 r = Ave_NdgDisp();

    Initialized_flags[2] = 0;
    return r;
}
s16 CpInetRouteAdd(void) {
    s16 r = Ave_RouteAdd();

    if (r < 0) {
        return -1;
    }
    Initialized_flags[5] = 1;
    return r;
}
s16 CpInetRouteDel(void) {
    s16 r = Ave_RouteDel();

    if (r < 0) {
        return -1;
    }
    Initialized_flags[5] = 0;
    return r;
}
s16 CpInetDhcpInitialize(void) {
    s16 r = Ave_DhcpInit();

    Initialized_flags[6] = 1;
    return r;
}
int CpInetDhcpDispose(void) {
    Ave_DhcpDisp();
    Initialized_flags[6] = 0;
    return 0;
}
s16 CpInetDhcpRequestNb(void) {
    return Ave_DhcpRequestNb();
}
s16 CpInetDhcpTimer(void) {
    return Ave_DhcpTimer();
}
s16 CpInetDhcpGetIfInfo(int a) {
    int tmp;

    return Ave_DhcpGetIfInfo(a, a + 4, &tmp);
}
s16 CpInetDhcpGetDns(int a, int b) {
    return Ave_DhcpGetDns(a, b, b + 4);
}
s16 CpInetDhcpGetGateway(void) {
    return Ave_DhcpGetGateway();
}
s16 CpInetDhcpReleaseNb(int *a) {
    return Ave_DhcpReleaseNb(*a);
}
s16 CpInetDevChanged(int *n) {
    s16 r;

    if (Inet_device_change_flag[0] != 0) {
        *n = Inet_device_change_num[0];
        return 1;
    }
    r = Ave_DgGetDeviceInfoNum();
    if (r != 0) {
        Inet_device_change_flag[0] = 1;
        Inet_device_change_num[0] = *n;
    }
    return r;
}
s16 CpInetDevGetStatus(int x, int y) {
    int a;
    int b;

    Inet_device_change_flag[0] = 0;
    Inet_device_change_num[0] = 0;
    return Ave_DgGetDeviceInfo(x, y, &a, &b);
}
s16 CpInetDevSelect(void) {
    return Ave_DgSelectDevice();
}
void CpInetTcpDelayCloseReset(void) {
    tcp_delay_close_num[0] = 0;
}
