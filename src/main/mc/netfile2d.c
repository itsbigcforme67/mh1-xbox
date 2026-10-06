/* netfile2d - SLPM_654.95 0x0028B4B0-0x0028B578 (encode_data_0028B4B0): see netfile2c.c. */
#include "types.h"

int ran_suu();

void encode_data_0028B4B0(buf)
u16 *buf;
{
    int r;
    int i;
    u16 key;
    u16 *sum;

    r = ran_suu(0);
    key = r & 0xFFFF;
    i = 0;
    *buf++ = 0x100;
    *buf++ = r;
    *buf = 0;
    sum = buf++;
    *buf++ = 0x5963;
    for (i = 0; i < 0x8A20; i++) {
        *sum = *sum + *buf;
        *buf ^= key;
        buf++;
        if ((key & 0xFFFF) == 0) {
            key = 1;
        }
        key = ((key & 0xFFFF) * 0xB0) % 65363 & 0xFFFF;
    }
}
