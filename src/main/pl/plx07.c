/* plx07 - Pl_vital_calc_item (SLPM_654.95 0x001534E0-0x0015355C): heal the player by dv, a quarter more when skill 0x1A (guess: recovery
   up) is on. Whole file in pl_nm.c. */
#include "pl.h"
#include "plf.h"
#include "game.h"
#include "plst.h"
void Pl_vital_calc_item(PLW *pl, int dv) {
    int n;
    if (Pl_Skill_ck(pl, 0x1A) == 1 && (n = (s16)dv, n > 0)) {
        n = (s16)(n + n / 4);
    } else {
        n = (s16)dv;
    }
    Pl_vital_calc(pl, n);
}


