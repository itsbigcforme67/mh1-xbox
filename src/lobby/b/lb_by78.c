/* lb_by78 - agent B promoted near-match 0x00539370-0x00539458: Lb_make_mySrcEquip (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern char D_3C738C[];
typedef struct { s16 a, b, c; } S3;

void Lb_make_mySrcEquip(int arg0) {
    F(s8, &lbShop, 0x55) = (s8) arg0;
    switch ((s16)arg0) {
    case 6:
    case 7:
        *(S3 *)((u8 *)&lbShop + 0x54) = *(S3 *)&D_3C738C;
        break;
    case 2:
        F(s16, &lbShop, 0x56) = *(u8 *)0x3C7393;
        break;
    case 3:
        F(s16, &lbShop, 0x56) = *(u8 *)0x3C7394;
        break;
    case 4:
        F(s16, &lbShop, 0x56) = *(u8 *)0x3C7395;
        break;
    case 5:
        F(s16, &lbShop, 0x56) = *(u8 *)0x3C7396;
        break;
    case 0:
        F(s16, &lbShop, 0x56) = *(u8 *)0x3C7392;
        break;
    }
    F(s8, &lbShop, 0x54) = 1;
}
