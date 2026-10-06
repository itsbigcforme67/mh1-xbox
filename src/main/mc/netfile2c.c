/* netfile2c - SLPM_654.95 0x0028B3B0-0x0028B468 (decode_data_for_net): the net save image unscrambling, same code as the
 * memory card copy in mccomb.c. u16 version 0x100, key seed, checksum, 0x5963, then 0x8A20 words xored with a key stream
 * (key = key * 0xB0 % 65363); the sum is of the decoded words. encode is netfile2d.c. */
#include "types.h"


s32 decode_data_for_net(buf)
u16 *buf;
{
    int i;
    u16 key;
    u16 stored;
    int sum;

    if (*buf != 0x100) {
        return -1;
    }
    sum = 0;
    i = 0;
    buf++;
    key = *buf++;
    stored = *buf++;
    buf++;
    do {
        *buf ^= key;
        sum = (sum + *buf) & 0xFFFF;
        buf++;
        if ((key & 0xFFFF) == 0) {
            key = 1;
        }
        key = ((key & 0xFFFF) * 0xB0) % 65363 & 0xFFFF;
        i++;
    } while (i < 0x8A20);
    return -((stored & 0xFFFF) != (sum & 0xFFFF));
}
