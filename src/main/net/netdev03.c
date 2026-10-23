/* netdev03 - _device_check (SLPM_654.95 0x00237F40-0x00238004): for the current device of kind 1 asks whether the device changed, re-selects it
   and stores its status in CurDevice->x10; returns 0 ok, -1 failure. Written new in this pass; field names are guesses. */
#include "types.h"
#ifndef __MWERKS__
short CpInetDevGetStatus();   /* (declared before its first use) */
#endif
typedef struct DEVI { s32 kind; s32 x04; s32 x08; s32 x0C; s32 x10; } DEVI;
extern DEVI *CurDevice;
int CpInetDevChanged();
#ifdef __MWERKS__
int CpInetDevGetStatus();
#else
short CpInetDevGetStatus();   /* the real return type (tools/check_kr_returns.py) */
#endif
int CpInetInterfaceGetStatus();
int CpInetDevSelect();
int _device_check(void) {
    s32 chg;
    s32 st;

    if (CurDevice->kind != 1) {
        return 0;
    }
    switch (CpInetDevChanged(&chg)) {
    case 0:
        return 0;
    case 1:
        if (CpInetDevGetStatus(0, &st) > 0 && CpInetInterfaceGetStatus() == 0 && CpInetDevSelect(st) == 0) {
            CurDevice->x10 = st;
            return 0;
        }
        CurDevice->x10 = -1;
    default:
        return -1;
    }
}
