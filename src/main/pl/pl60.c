/* Player code (SLPM_654.95 0x00151960-0x00151970): pl_atck_data_set_shl */
#include "pl.h"
#include "game.h"
#include "plf.h"
#include "hit.h"
u8 Get_hit_id(void);

void pl_atck_data_set_shl(HSHL *sh, u16 *hd, int idx, int tbl) {
    *(int *)((u8 *)sh + 0x90) = tbl;
    sh->ailment_val = 0;
    sh->ailment = 0;
    pl_atck_data_set_shl2(sh, hd, idx);
}
