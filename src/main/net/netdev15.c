/* netdev15 - DeviceRollbackDriver (SLPM_654.95 0x002332C0-0x00233420): after a failed load, unloads the adapter driver modules again (type 1 with a
   sub type other than 1), reloads the USB search module, rebinds the search RPC and forgets the current device; returns -1 for the device types it
   cannot roll back (names guessed from use). Written new in this pass from an m2c draft. */
#include "types.h"
typedef struct DEVREC { s32 type; s32 sub; } DEVREC;
extern DEVREC *CurDevice;
extern s32 Device_driver_rollbacked;
extern s32 INET_USBSRCH_LOCATION;
extern u8 Search_sif[];
extern char lit_295_0036CD70[];
extern char lit_296_0036CD90[];
extern char lit_297_0036CDB0[];
void CpInetTerminate();
void DeviceSelectInitialize();
void DeviceUpdateStatus();
void bind_rpc_blocking();
void module_loadhigh();
void module_unload_00232D00();
int sceSifSearchModuleByName();

int DeviceRollbackDriver(void) {
    int r;
    int m;

    Device_driver_rollbacked = 1;
    switch (CurDevice->type) {
    case 2:
        return -1;
    case 1:
        switch (CurDevice->sub) {
        case 1:
            return -1;
        default:
            r = sceSifSearchModuleByName(lit_295_0036CD70);
            if (0 <= r) {
                module_unload_00232D00(r);
            }
            m = -1;
            CpInetTerminate();
            switch (CurDevice->sub) {
            case 6:
                m = sceSifSearchModuleByName(lit_296_0036CD90);
                break;
            case 5:
                m = sceSifSearchModuleByName(lit_297_0036CDB0);
                break;
            }
            if (0 <= m) {
                module_unload_00232D00(m);
            }
            module_loadhigh(INET_USBSRCH_LOCATION, 0, 0, 0, 0);
            bind_rpc_blocking(Search_sif, 0x01270010);
            CurDevice = 0;
            DeviceSelectInitialize();
            DeviceUpdateStatus();
            goto z;
        }
    case 3:
        return -1;
    }
z:
    return 0;
}
