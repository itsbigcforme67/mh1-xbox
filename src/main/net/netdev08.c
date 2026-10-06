/* netdev08 - DeviceSelectInitialize (SLPM_654.95 0x00233280-0x002332BC): with no current device, marks the first eight device slots empty and the
   device counts unknown. Written new in this pass. */
#include "types.h"
typedef struct DEVREC { s32 x00; s32 x04; s32 id; s32 sub; s32 x10; s32 x14; s32 x18; } DEVREC;
extern DEVREC *CurDevice;
extern DEVREC DeviceWork[];
extern s32 TotalDeviceNum;
extern s32 UsbDeviceNum;
void *memset();

void DeviceSelectInitialize(void) {
    if (CurDevice == 0) {
        memset(DeviceWork, 0xFF, 0x20);
        UsbDeviceNum = -1;
        TotalDeviceNum = -1;
    }
}
