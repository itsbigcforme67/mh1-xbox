/* imebz: chk_entry2 and set_entry2 (SLPM_654.95 0x00242850-0x00242990): test and mark a kana entry (one or two bytes, 0xA1..) in the entry bitmap entry2code. IME engine (name entry), see imead.c for the context. */
#include "types.h"

extern u8 *entry2code;
extern int entry2upd;

int chk_entry2(u8 *key, int len)
{
    int b;
    int row;
    u8 *q;

    if (key[0] < 0xA1) {
        return 0;
    }
    row = key[0] - 0xA1;
    if ((s16)len == 1) {
        b = 0;
    } else {
        if (key[1] < 0xA1) {
            return 0;
        }
        b = key[1] - 0xA0;
    }
    q = &entry2code[(b >> 3) + row * 0xB];
    if ((1 << (b & 7)) & *q) {
        return 0;
    }
    return 1;
}

void set_entry2(u8 *key, int len)
{
    int b;
    int row;
    u8 *q;

    if (key[0] >= 0xA1) {
        row = key[0] - 0xA1;
        if ((s16)len == 1) {
            b = 0;
        } else {
            if (key[1] < 0xA1) {
                return;
            }
            b = key[1] - 0xA0;
        }
        q = &entry2code[(b >> 3) + row * 0xB];
        *q |= (1 << (b & 7)) & 0xFF;
        entry2upd = 1;
    }
}
