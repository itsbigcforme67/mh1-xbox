/* light05 - flash_move (SLPM_654.95 0x0011DE60-0x0011E278): lightning-flash effect on one light set (p = light_work + 0x10, state byte 0:
 * 0 start (both counters = 30), 1 blend the two light rows toward the flash colours over p[2] frames, 2 stop). Field meanings are guesses. */
#include "types.h"
extern f32 *light_tbl[];

void flash_move(u8 *p) {
    f32 *a = light_tbl[2];
    f32 *b = light_tbl[3];
    f32 *d = (f32 *)(p + 8);
    int c;

    switch (p[0]) {
    case 0:
        p[0] = p[0] + 1;
        p[2] = p[3] = 0x1E;
        break;
    case 1:
        d[1] = a[0];
        d[2] = a[1];
        a++; a++;
        d[3] = a[0];
        d[9] = b[0];
        d[10] = b[1];
        b++; b++;
        d[11] = b[0];
        c = p[3] - 1;
        p[3] = c;
        if (c & 0xFF) {
            d[1] += (1.0f / (u32)p[2]) * (u32)p[3];
            d[2] += (1.0f / (u32)p[2]) * (u32)p[3];
            d[3] += (1.0f / (u32)p[2]) * (u32)p[3];
            d[9] += (1.0f / (u32)p[2]) * (u32)p[3];
            d[10] += (1.0f / (u32)p[2]) * (u32)p[3];
            d[11] += (1.0f / (u32)p[2]) * (u32)p[3];
        } else {
            p[0] = p[0] + 1;
        }
        break;
    case 2:
        p[1] = 0;
        break;
    }
}
