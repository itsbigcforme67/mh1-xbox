/* cpinet03 - PPP initialise / option (SLPM_654.95 0x00235BF0-0x00235C4C). Whole file in cpinet_nm.c. */
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
s16 CpInetPppInitialize(void) {
    s16 r = Ave_PppInit();

    Initialized_flags[1] = 1;
    return r;
}
s16 CpInetPppSetOption(int v) {
    return Ave_PppSetOption(1, 4, v);
}
