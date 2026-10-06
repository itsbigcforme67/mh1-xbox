/* SLPM_654.95 0x00175120-0x001751CC: flPS2psAddQueue .. flPS2psAddQueue. See flps_misc_nm.c. */
#include "types.h"

typedef unsigned long u64;

extern u8 flPs2State[];
extern f32 flViewportCX, flViewportCY, flViewportLX, flViewportLY;
extern u8 flPs2VIF1Control[];

u64 flPS2GetSystemTmpBuff(int, int);
void flPS2_Mem_move16_16A(void *, u64, int);
void flPS2DmaAddQueue2(int, u64, u64, void *);


int flPS2psAddQueue(u32 *p) {
    int n;
    u64 buf;
    u64 t;

    n = (p[0] & 0x7FFF) + 1;
    buf = flPS2GetSystemTmpBuff(n * 16, 16);
    p[0] |= 0x80000000;
    p[2] = 0x13000000;
    p[3] = (n - 1) | 0x51000000;
    flPS2_Mem_move16_16A(p, buf, n);
    t = (buf & 0xFFFFFFFULL) | 0x40000000;
    flPS2DmaAddQueue2(0, t, buf, flPs2VIF1Control);
    return 1;
}
