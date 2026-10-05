/* lb_z59 - auto-drafted 0x005FD130-0x005FD15C: To_BodyMain_ReqSrc (first drafted by tools/lbauto.py). */
#include "lobby_f.h"
extern s16 BsTimer0;
extern u8 * bsCur;
extern u8 * bsSys;

void To_BodyMain_ReqSrc(void) {
    F(s32, bsCur, 0x20) = 0;
    BsTimer0 = 0;
    F(s8, bsSys, 1) = 1;
    F(s8, bsSys, 2) = 0;
    F(s8, bsSys, 3) = 0;
}
