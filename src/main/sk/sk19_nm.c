/* Near-match (not linked): sk_get_key_code (0x0025F9C0-0x0025FA84), 3 of 49 instructions differ: the two lbu loads of the cursor row/column use v1/a0 where the original uses a0/v1. */
#include "types.h"

extern u8 *lpSKey;
#define SKB(o) (*(u8 *)(lpSKey + (o)))
#define SKS8(o) (*(s8 *)(lpSKey + (o)))
#define SKP(o) (*(u8 **)(lpSKey + (o)))

s8 sk_get_key_code(void) {
    u8 *l;

    SKS8(0x2E) = *(s8 *)(*(u8 **)(SKP(0) + 8) + (SKB(0x24) * 4 + SKB(0x25)));
    SKP(4) = *(u8 **)(SKP(0) + 4) + SKS8(0x2E) * 8;
    SKP(8) = *(u8 **)(SKP(0) + 0) + SKS8(0x2E) * 4;
    if (*(u8 **)(SKP(0) + 0xC) != 0) {
        SKP(0xC) = *(u8 **)(SKP(0) + 0xC) + (SKS8(0x2E) - 12) * 16;
    } else {
        SKP(0xC) = 0;
    }
    if (!(*(SKP(0) + 0x10) & 0x80)) {
        SKB(0x34) = *(SKP(0) + 0x10) & 0x7F;
    }
    return SKS8(0x2E);
}

