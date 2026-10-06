/* taru01 - barrel bomb allowed check (SLPM_654.95 0x00159380-0x001593B8): Taru_ok_ck. Whole file in shell_work_nm.c. */
/* NOT BUILT: Taru_ok_ck 0x00159380 near-match (logic equal; original lays out the 'return 0' arm inline before the else load, 8/14 insns differ). */
#include "types.h"
#include "game.h"
int Taru_ok_ck(void) {
    u8 a = game_w.stage;
    u8 b = game_w.x2F;
    unsigned int new_var; /* permuter: matching scheduling */
    if (a == b) {
        return 0;
    }
    new_var = 1;
    if (new_var) {
        return game_w.shl10_num < 2;
    }
}
