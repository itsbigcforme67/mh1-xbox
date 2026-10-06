/* SLPM_654.95 0x00192DD0-0x00192E24: plBMPGetPixelAddressFromImage (BMP file header: pixel data offset at +0xA). */
#include "types.h"

int plReport(char *fmt, ...);
extern char lit_110_0035C150[];

u8 *plBMPGetPixelAddressFromImage(u8 *p) {
    if (*(u16 *)p != 0x4D42) {
        plReport(lit_110_0035C150);
        return 0;
    }
    return p + (*(u16 *)(p + 0xA) | (*(u16 *)(p + 0xC) << 16));
}
