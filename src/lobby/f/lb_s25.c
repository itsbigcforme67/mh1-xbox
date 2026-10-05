/* lb_s25 - small browser/http fixes 0x005FD160-0x005FD1E8: To_BodyMain_RcvSrc (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern s16 BsTimer0;
extern int bsCur;
extern int bsSys;
extern u8 bsGoHidePage;
extern int bsOW[];

void To_BodyMain_RcvSrc(s32 arg0) {
    int temp_a0;

    F(s8, bsSys, 0x2C) = 0;
    F(s8, bsSys, 0x2E) = 1;
    F(s32, bsCur, 0x20) = 0;
    BsTimer0 = 0;
    F(s8, bsSys, 1) = 1;
    F(s8, bsSys, 2) = 1;
    F(s8, bsSys, 3) = 0;
    if (bsGoHidePage == 0) {
        F(s8, bsSys, 0x3A) = -0x10;
    }
    if (bsOW[0] == 0) {
        F(s8, bsSys, 0x3A) = 0;
        temp_a0 = bsSys;
        if (F(s8, temp_a0, 0x36) != 0) {
            F(s32, temp_a0, 0x14) = 0xFF000001;
        }
    }
}
