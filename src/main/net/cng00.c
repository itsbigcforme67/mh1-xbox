/* cng00 - 0x0022E090-0x0022E1AC: CnInetNetworkAveTcpPoll, the connection state machine of the online stack
 * (ave_env[0]: 0 start InetConnectInitialize, 1 InetConnectStart until it reports an address (2) or
 * failure (4), 2 wait for CpInetGetStatus, 3 InetDisconnectAll then DeviceRollbackDriver (-> 5), 4/5 idle).
 * Returns the state. */
#include "types.h"

extern s8 ave_env[0x10];
extern int MyIPAddr;

void InetConnectInitialize(void);
int InetConnectStart(void);
void CpInetSetLocalIpAddr(int);
int CpInetGetStatus(void);
int InetDisconnectAll(s8 *, s8 *);
void DeviceRollbackDriver(void);

int CnInetNetworkAveTcpPoll(void) {
    int r;

    switch (ave_env[0]) {
    case 0:
        InetConnectInitialize();
        ave_env[0]++;
        break;
    case 1:
        r = InetConnectStart();
        switch (r) {
        case 2:
            CpInetSetLocalIpAddr(MyIPAddr);
            ave_env[0]++;
            break;
        case 4:
            ave_env[0] = 4;
            break;
        case 0:
            break;
        }
        break;
    case 2:
        if (CpInetGetStatus() != 0) {
            ave_env[0] = 4;
            return 4;
        }
        break;
    case 3:
        if (InetDisconnectAll(ave_env + 3, ave_env + 4) != 0) {
            DeviceRollbackDriver();
            ave_env[0] = 5;
        }
        break;
    case 4:
    case 5:
        break;
    }
    return ave_env[0];
}
