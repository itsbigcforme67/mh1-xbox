/* fl library (SLPM_654.95 0x0018D5F0-0x0018D78C): flPS2DrawPreparation fills the draw-start GIF packet (flPs2DrawStart: scissor, xyoffset,
 * test/frame/zbuf copies, background colour) from flPs2State, copies it into a system temp buffer and queues it. */
#include "types.h"

typedef long u64;

extern u8 flPs2State[];
#define PS2S(T, o) (*(T *)(flPs2State + (o)))
extern u8 flPs2DrawStart[];
#define PSD(o) (*(u64 *)(flPs2DrawStart + (o)))
extern int flPs2VIF1Control[];
extern int flWidth;
extern int flHeight;
extern u32 flPs2FBA;

u64 flPS2GetSystemTmpBuff();
void flPS2_Mem_move16_16A();
void flPS2DmaAddQueue2();

void flPS2DrawPreparation(void) {
    u64 buf;

    PSD(0x30) = ((u64)(flWidth - 1) << 16) | ((u64)(flHeight - 1) << 48);
    PSD(0x90) = PS2S(u64, 0x440);
    PSD(0xA0) = PS2S(u64, 0x458);
    PSD(0x70) = (u64)(PS2S(int, 0x3A4) * 16) | ((u64)(PS2S(int, 0x3A8) * 16) << 16);
    PSD(0x80) = (u64)((PS2S(int, 0x3A4) + flWidth) * 16) | ((u64)((PS2S(int, 0x3A8) + flHeight) * 16) << 16);
    PSD(0x60) = ((u64)((PS2S(u32, 0x3A0) >> 24) & 0xFF) << 24)
              | (((u64)(PS2S(u32, 0x3A0) & 0xFF) << 16)
              | (((u64)((PS2S(u32, 0x3A0) >> 16) & 0xFF))
              | ((u64)((PS2S(u32, 0x3A0) >> 8) & 0xFF) << 8)))
              | ((u64)1 << 32);
    PSD(0xB0) = PS2S(u64, 0x420);
    PSD(0xC0) = PS2S(u64, 0x428);
    PSD(0xD0) = PS2S(u64, 0x430);
    PSD(0xE0) = PS2S(u64, 0x438);
    PSD(0xF0) = flPs2FBA;
    PSD(0x100) = flPs2FBA;
    buf = flPS2GetSystemTmpBuff(0x140, 0x10);
    flPS2_Mem_move16_16A(flPs2DrawStart, buf, 0x14);
    flPS2DmaAddQueue2(0, (unsigned long)buf & 0xFFFFFFFUL, buf, flPs2VIF1Control);
}
