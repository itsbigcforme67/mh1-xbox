/* plx03 - pl_work_clr (SLPM_654.95 0x00134E70-0x00134F30): sets up one player slot (id, joined flag, program table, init calls).
   Whole file in pl_nm.c. K&R definition with an unnarrowed `no` (the original keeps the raw a1 in s1); plf.h declares
   it with a u8 prototype, so the header name is renamed around the include (no header edit). */
#define pl_work_clr pl_work_clr_proto_unused
#include "pl.h"
#include "plf.h"
#include "game.h"
#include "plst.h"
#undef pl_work_clr
void pl_work_clr();

void pl_work_clr(pl, no, prog)
PLW *pl;
int no;
PLPROG *prog;
{
    pl->id = (u8)no;
    if (game_w.pl_state[pl->id] == 1 || Pl_master_ck(pl) == 1) {
        pl->x01 = 1;
    } else {
        pl->x01 = 0;
    }
    pl->prog = prog;
    pl_init_sub(pl);
    pl->chr_no0 = (u8)no + 1;
    pl->prog->init(pl);
    pl->prog->init2(pl);
}
