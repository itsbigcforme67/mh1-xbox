/* Player code (SLPM_654.95 0x00151A50-0x00151A58): atck_data_set_shl */
#include "pl.h"
#include "game.h"
#include "plf.h"
#include "hit.h"
u8 Get_hit_id(void);

void atck_data_set_shl(HSHL *sh, int idx, int tbl) {
    *(int *)((u8 *)sh + 0x90) = tbl;
    atck_data_set_shl2(sh, idx);
}
