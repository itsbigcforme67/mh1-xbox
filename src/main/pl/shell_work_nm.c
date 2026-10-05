/* NOT BUILT: Taru_ok_ck 0x00159380 near-match (logic equal; original lays out the 'return 0' arm inline before the else load, 8/14 insns differ). */
#include "types.h"
#include "game.h"

int Taru_ok_ck(void) {
    u8 a = game_w.stage;
    u8 b = game_w.x2F;
    if (a == b) {
        return 0;
    }
    return game_w.shl10_num < 2;
}

