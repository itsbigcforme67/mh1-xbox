/* cpinet23 - InetDnsCacheInitialize (SLPM_654.95 0x00237B70-0x00237BA0): clears the DNS resolver cache. Written new in this pass. */
#include "types.h"
extern u8 Resolv_cache_004FAB70[0x820];
extern s32 Resolv_cache_cnt_0038A5F4;
void *memset();

int InetDnsCacheInitialize(void) {
    memset(Resolv_cache_004FAB70, 0, 0x820);
    Resolv_cache_cnt_0038A5F4 = 0;
    return 0;
}
