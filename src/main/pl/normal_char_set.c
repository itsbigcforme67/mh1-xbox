/* normal_char_set - SLPM_654.95 0x0014F0D0-0x0014F1DC.
 * Built with common-subexpression elimination off for this function: the
 * original reloads the constant 1 in the switch's default case where our
 * compiler build otherwise reuses the register left by the `case 1`
 * comparison. This pragma is how we reproduce that; it is not known to be
 * what Capcom wrote (to_normal, next in the binary, needs CSE on for its
 * last call and stays parked in pl_normal_nm.c). */
#include "pl.h"
#include "game.h"


s16 Stage_env_ck(u8);
void pl_chr_set();   /* called without a prototype: args are promoted */
int Pl_master_ck(PLW *);
void net_send_pl(PLW *, int, int);

#pragma opt_common_subs off
void normal_char_set(PLW *pl, int a, int b) {
    u16 c0, c1;

    if (pl->flag604 != 0) {
        c0 = 0x18;
        c1 = 0x7C;
    } else if (pl->flag12 != 0) {
        c0 = 0x3E9;
        c1 = 0x44D;
    } else if (pl->work882 < 0x4C) {
        c0 = 0x193;
        c1 = 0x1F7;
    } else {
        switch (Stage_env_ck(pl->stg)) {
        default:
            c0 = 1;
            c1 = 0x65;
            break;
        case 1:
            c0 = 0x15;
            c1 = 0x79;
            break;
        case 2:
            c0 = 0x1AF;
            c1 = 0x213;
            break;
        }
    }
    pl_chr_set(pl, c0, a, b, 0);
    pl_chr_set(pl, c1, a, b, 1);
}
#pragma opt_common_subs reset
