/* cpinet20 - CpInetHttpResolvCacheInitialize (SLPM_654.95 0x002377B0-0x002377EC): clears the HTTP resolver cache under the HTTP semaphore.
   Written new in this pass. */
#include "types.h"
extern u8 Resolv_cache_004FA1A0[0x9A0];
extern s32 Resolv_cache_cnt_004FA190[4];
void *memset();
void signal_http_static_sema(void);
void wait_http_static_sema(void);

void CpInetHttpResolvCacheInitialize(void) {
    wait_http_static_sema();
    memset(Resolv_cache_004FA1A0, 0, 0x9A0);
    Resolv_cache_cnt_004FA190[0] = 0;
    signal_http_static_sema();
}
