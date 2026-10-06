/* cpinet24 - InetConnectAll (SLPM_654.95 0x00237EF0-0x00237F34): starts the connection state machine with pointers into InetSys. Written new in this pass. */
#include "types.h"
extern u8 InetSys[0x40];
int InetConnectAllCore();

void InetConnectAll(void) {
    InetConnectAllCore(&InetSys[1], &InetSys[2], &InetSys[3], &InetSys[4], &InetSys[6], &InetSys[8], &InetSys[10], &InetSys[15]);
}
