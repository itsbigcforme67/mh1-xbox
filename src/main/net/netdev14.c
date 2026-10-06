/* netdev14 - DeviceLoadDriver (SLPM_654.95 0x00233540-0x002337E4): once per connection, loads the IOP modules for the current device type (1 Ethernet,
   2 and 3 modem / adapter variants: device glue, DHCP or PPP, the modem or adapter driver, the TCP/IP wrapper) and records LoadedDeviceType
   (names guessed from use). Written new in this pass from an m2c draft; switch (sub) { case 4: .. default: .. } gives the original layout. */
#include "types.h"
typedef struct DEVREC { s32 type; s32 sub; } DEVREC;
extern DEVREC *CurDevice;
extern s32 Device_driver_rollbacked;
extern s32 INET_ATERMAS_LOCATION;
extern s32 INET_ATERMCS_LOCATION;
extern s32 INET_AVEDHCP_LOCATION;
extern s32 INET_AVEPPP_LOCATION;
extern s32 INET_AVE_WRAPM_LOCATION;
extern s32 INET_CXTMDM_LOCATION;
extern s32 INET_DEVGLUE_LOCATION;
extern s32 INET_MINIJPS2_LOCATION;
extern s32 INET_OSTMDM_A_LOCATION;
extern s8 LoadedDeviceType;
extern char lit_317_0036CDD0[];
extern char lit_347_0036CE08[];
void module_load_00232F60();
void module_unload_00232D00();
void rpc_initialize();
int sceSifSearchModuleByName();

int DeviceLoadDriver(void) {
    int r;

    if (Device_driver_rollbacked != 0) {
        return 1;
    }
    r = sceSifSearchModuleByName(lit_317_0036CDD0);
    if (0 <= r) {
        module_unload_00232D00(r);
    }
    switch (CurDevice->type) {
    case 2:
        switch (CurDevice->sub) {
        case 2:
            module_load_00232F60(INET_CXTMDM_LOCATION, 0, 0, 0, 0);
            break;
        case 4:
            module_load_00232F60(INET_OSTMDM_A_LOCATION, 0, 0, 0, 0);
            break;
        }
        module_load_00232F60(INET_AVEPPP_LOCATION, 1, lit_347_0036CE08, 0, 0);
        module_load_00232F60(INET_AVE_WRAPM_LOCATION, 0, 0, 0, 0);
        rpc_initialize();
        LoadedDeviceType = 2;
        break;
    case 1:
        module_load_00232F60(INET_DEVGLUE_LOCATION, 1, lit_347_0036CE08, 0, 0);
        module_load_00232F60(INET_AVEDHCP_LOCATION, 0, 0, 0, 0);
        module_load_00232F60(INET_AVEPPP_LOCATION, 1, lit_347_0036CE08, 0, 0);
        LoadedDeviceType = 1;
        break;
    case 3:
        switch (CurDevice->sub) {
        case 4:
            module_load_00232F60(INET_OSTMDM_A_LOCATION, 0, 0, 0, 0);
            break;
        default:
            module_load_00232F60(INET_DEVGLUE_LOCATION, 1, lit_347_0036CE08, 0, 0);
            switch (CurDevice->sub) {
            case 7:
                module_load_00232F60(INET_ATERMAS_LOCATION, 0, 0, 0, 0);
                break;
            case 8:
                module_load_00232F60(INET_ATERMCS_LOCATION, 0, 0, 0, 0);
                break;
            case 9:
                module_load_00232F60(INET_MINIJPS2_LOCATION, 0, 0, 0, 0);
                break;
            }
            break;
        }
        module_load_00232F60(INET_AVEPPP_LOCATION, 1, lit_347_0036CE08, 0, 0);
        module_load_00232F60(INET_AVE_WRAPM_LOCATION, 0, 0, 0, 0);
        rpc_initialize();
        LoadedDeviceType = 2;
        break;
    }
    return 1;
}
