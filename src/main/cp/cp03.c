/* cp03 - SLPM_654.95 0x00121280-0x00121380 (end of the cp part helpers): parts_chg selects which one of a group of hunter part switches
 * (bytes PLW+0x4E6+n) is visible; yure_init clears the four hair sway slots. */
#include "types.h"
void parts_chg(u8 *w, u8 kind, u8 which) {
    u8 first;
    u8 num;
    u8 i;

    switch (kind) {
    case 0x12:
        first = num = 2;
        break;
    case 0xE:
        num = 2;
        first = 4;
        break;
    default:
        return;
    }
    for (i = first; i < first + num; i++) {
        if (i == which + first) {
            w[0x4E6 + i] = 1;
        } else {
            w[0x4E6 + i] = 0;
        }
    }
}

void yure_init(u8 *w) {
    s16 i;

    i = 0;
    do {
        *(s32 *)(w + 0x628) = 0;
        *(s32 *)(w + 0x62C) = 0;
        *(s32 *)(w + 0x630) = 0;
        *(s32 *)(w + 0x634) = 0;
        *(s32 *)(w + 0x638) = 0;
        *(s32 *)(w + 0x63C) = 0;
        w[0x624] = 0;
        w[0x625] = 0;
        w[0x626] = 0;
        w[0x627] = 0;
        w += 0x20;
        i++;
    } while (i < 4);
}
