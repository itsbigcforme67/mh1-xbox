/* netdev01 - DeviceGetOptionalStatus (SLPM_654.95 0x00232C50-0x00232CF8): reports the current network device (CurDevice) fields, the USB
   keyboard bit and, for device kinds 2 and 3, the PPP status word / 100 (a guess: a connection speed or time). Written new in this pass. */
#include "types.h"
typedef struct DEVI { s32 kind; s32 x04; s32 x08; s32 x0C; } DEVI;
extern DEVI *CurDevice;
extern s16 is_usbkb;
int CpInetPppGetStatus();
void DeviceGetOptionalStatus(s16 *a, s16 *b, s16 *c, s16 *d) {
    s32 st[8];

    *a = CurDevice->x08;
    *b = CurDevice->x0C;
    *c = is_usbkb & 1;
    *d = 0;
    switch (CurDevice->kind) {
    case 2:
    case 3:
        CpInetPppGetStatus(st);
        *d = st[2] / 100;
        break;
    }
}
