/* cpinet25 - wait_http_static_sema / signal_http_static_sema (SLPM_654.95 0x002368C0-0x002368DC). Written new in this pass. */
#include "types.h"
extern s32 Inet_http_static_sema[4];
int SignalSema();
int WaitSema();

void wait_http_static_sema(void) {
    WaitSema(Inet_http_static_sema[0]);
}

void signal_http_static_sema(void) {
    SignalSema(Inet_http_static_sema[0]);
}
