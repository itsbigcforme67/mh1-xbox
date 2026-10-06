/* fl ps2 gs clip check (SLPM_654.95 0x001751D0-0x00175288): flPS2CheckGSClip. */
#include "types.h"

typedef unsigned long u64;

extern u8 flPs2State[];
extern f32 flViewportCX, flViewportCY, flViewportLX, flViewportLY;
extern u8 flPs2VIF1Control[];

u64 flPS2GetSystemTmpBuff(int, int);
void flPS2_Mem_move16_16A(void *, u64, int);
void flPS2DmaAddQueue2(int, u64, u64, void *);

int flPS2CheckGSClip(f32 x, f32 y, f32 z) {
    if (flViewportCX + flViewportLX < x) {
        return 0;
    }
    if (!(flViewportCX - flViewportLX <= x)) {
        return 0;
    }
    if (flViewportCY + flViewportLY < y) {
        return 0;
    }
    if (!(flViewportCY - flViewportLY <= y)) {
        return 0;
    }
    if (z < 0.0f) {
        return 0;
    }
    if (*(f32 *)(flPs2State + 0x44) < z) {
        return 0;
    }
    return 1;
}
