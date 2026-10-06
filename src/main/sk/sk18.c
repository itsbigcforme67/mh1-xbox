/* sk18 - f_sk 0x002640C0-0x0026417C: kbdExecServer (reads the PS2 USB keyboard state into the soft keyboard work, keeps previous key / new press bytes). */
#include "types.h"

extern u8 *lpSKey;
#define SKB(o) (*(u8 *)(lpSKey + (o)))
#define SKS8(o) (*(s8 *)(lpSKey + (o)))
#define SKP(o) (*(u8 **)(lpSKey + (o)))

u8 *getPS2KbData(void);

void kbdExecServer(void) {
    s8 *f = (s8 *)(lpSKey + 0x37);
    u8 *k;

    if (*f == 0) {
        *f = 1;
        SKB(0x659) = SKB(0x658);
        SKB(0x65C) = SKB(0x65B);
        SKB(0x65B) = 0;
        SKB(0x658) = 0;
        SKB(0x65A) = 0;
        k = getPS2KbData();
        if (k != 0) {
            SKB(0x65B) = k[0];
            SKB(0x658) = k[2];
            if (SKB(0x658) != 0 && SKB(0x658) != SKB(0x659)) {
                SKB(0x65A) = SKB(0x658);
            }
            SKB(0x65D) = SKB(0x65B) & ~SKB(0x65C);
        }
    }
}
