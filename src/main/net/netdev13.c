/* netdev13 - DeviceLoadDriver2 (SLPM_654.95 0x00233420-0x00233540): unloads the old search module, then for an Ethernet device (type 1) loads the
   driver module for its adapter (sub type 1, 6 or 5) and the TCP/IP wrapper, and starts the RPC (names guessed from use). Written new in this pass from
   an m2c draft; the case order 2, 1, 3 reproduces the original compare ladder. */
#include "types.h"
typedef struct DEVREC { s32 type; s32 sub; } DEVREC;
extern DEVREC *CurDevice;
extern s32 INET_AN986_LOCATION;
extern s32 INET_AVE_WRAPE_LOCATION;
extern s32 INET_LANEGG_LOCATION;
extern s32 INET_SMAP_LOCATION;
extern char lit_317_0036CDD0[];
extern char lit_318_0036CDF0[];
extern char lit_319_0036CDF8[];
void module_loadhigh();
void module_unload_00232D00();
void rpc_initialize();
int sceSifSearchModuleByName();

int DeviceLoadDriver2(void) {
    int r;

    r = sceSifSearchModuleByName(lit_317_0036CDD0);
    if (0 <= r) {
        module_unload_00232D00(r);
    }
    switch (CurDevice->type) {
    case 2:
        break;
    case 1:
        switch (CurDevice->sub) {
        case 1:
            module_loadhigh(INET_AN986_LOCATION, 8, lit_318_0036CDF0, 0, 0);
            break;
        case 6:
            module_loadhigh(INET_LANEGG_LOCATION, 8, lit_318_0036CDF0, 0, 0);
            break;
        case 5:
            module_loadhigh(INET_SMAP_LOCATION, 9, lit_319_0036CDF8, 0, 0);
            break;
        }
        module_loadhigh(INET_AVE_WRAPE_LOCATION, 0, 0, 0, 0);
        rpc_initialize();
        break;
    case 3:
        break;
    }
    return 1;
}
