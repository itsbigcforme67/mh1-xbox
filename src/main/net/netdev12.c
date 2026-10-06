/* netdev12 - DeviceModuleInitialize_blocking (SLPM_654.95 0x00233A00-0x00233B04): sets up the SRAM and device list, loads the TCP/IP and USB
   search IOP modules, copies the first device description into the device work area when the second module is present, then binds the
   search RPC and refreshes the device status (names guessed from use). Written new in this pass from an m2c draft. */
#include "types.h"
extern u8 DeviceWork[];
extern s32 Device_driver_rollbacked;
extern s32 Device_get_usb_id_r_no;
extern s32 INET_AVETCP_LOCATION;
extern s32 INET_USBSRCH_LOCATION;
extern s32 Sce_device_list[];
extern u8 Search_sif[];
extern char lit_347_0036CE08[];
extern char lit_456_0036CE10[];
void InetSramInitialize();
void DeviceSelectInitialize();
void DeviceUpdateStatus();
void bind_rpc_blocking();
void module_load_00232F60();
void module_loadhigh();
int sceSifSearchModuleByName();
#define DW(o) (*(s32 *)(DeviceWork + (o)))

int DeviceModuleInitialize_blocking(void) {
    InetSramInitialize();
    DeviceSelectInitialize();
    Device_driver_rollbacked = 0;
    Device_get_usb_id_r_no = 0;
    module_load_00232F60(INET_AVETCP_LOCATION, 1, lit_347_0036CE08, 0, 0);
    if (0 <= sceSifSearchModuleByName(lit_456_0036CE10)) {
        DW(0x3B8) = Sce_device_list[0];
        DW(0x3BC) = Sce_device_list[1];
        DW(0x3C0) = Sce_device_list[2];
        DW(0x3C4) = Sce_device_list[3];
        DW(0x3CC) = Sce_device_list[5];
        DW(0x3D0) = Sce_device_list[6];
        DW(0x3C8) = 0x21;
    }
    module_loadhigh(INET_USBSRCH_LOCATION, 0, 0, 0, 0);
    bind_rpc_blocking(Search_sif, 0x01270010);
    DeviceUpdateStatus();
    return 0;
}
