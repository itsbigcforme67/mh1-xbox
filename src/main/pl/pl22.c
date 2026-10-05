/* Player code (SLPM_654.95 0x0013F570-0x0013F968): pl_mv071 (pick up / pull out an item or egg) */
#include "pl.h"
#include "game.h"
#include "plf.h"

void pl_mv071(PLW *pl, u32 arg1) {
    u16 n;
    u16 r;
    s32 h;
    u8 s;

    ItemPickingDeclaration(pl, &pl->x8E6);
    pl->work40E = 2;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->x07 = 0;
        pl->work39C = 0;
        Pl_basic_flagset(pl, 0, 0, 0);
        switch (arg1) {
        case 0:
        case 1:
            pl_chr_set2(pl, 0x195, -0xE, 0);
            break;
        case 2:
            if (Pl_master_ck(pl) == 1) {
                set01_set(0, 0xA, 0);
            }
            pl_chr_set2(pl, 0x195, -0xE, 0);
            break;
        case 3:
            pl_chr_set2(pl, 0x1A3, -6, 0);
            break;
        case 4:
            pl_chr_set2(pl, 0x19F, 0, 0);
            break;
        case 5:
            pl_chr_set2(pl, 0x1A2, 0, 0);
            break;
        }
        break;
    case 1:
        if ((Pl_master_ck(pl) == 1) && (frame_check(120.0f, pl, 0) != 0) && (Game_clear_ck(1) == 0)) {
            n = 1;
            switch (arg1) {
            case 4:
            case 5:
                switch (pl->work88A) {
                case 0x83:
                    n = 3;
                    break;
                case 0x84:
                    n = 0xA;
                    break;
                case 0x85:
                    n = 0xF;
                    break;
                case 0x86:
                    n = 3;
                    break;
                case 0x87:
                    n = 0xA;
                    break;
                case 0x88:
                    n = 0xF;
                    break;
                }
                h = (ran_suu(1) & 0xFFFF) % n;
                if (h == 0) {
                    if ((Pl_Skill_ck(pl, 0x32) != 1 || ((r = ran_suu(1)) & 3)) && (Pl_Skill_ck(pl, 0x33) != 1 || ((r = ran_suu(1)) & 1))) {
                        set01_set(1, 0xE, (s16)pl->work88A);
                        Pl_item_stack(pl, pl->work88A, -1);
                    }
                }
            default:
                pick_set_sub(pl, 0);
                break;
            case 2:
                Item_regained(pl, 1);
                break;
            case 3:
                pick_set_sub(pl, 1);
                break;
            }
        } else if (pl->work194 == 0) {
            if (pl->x07 == 0) {
                switch (arg1) {
                case 0:
                case 1:
                case 2:
                    pl_to_normal(pl, 0, 0xA, 0);
                    break;
                case 3:
                case 5:
                    pl_to_normal(pl, 0, 6, 0);
                    break;
                default:
                    pl_to_normal(pl, 0, 4, 0);
                    break;
                }
            } else {
                egg_set(pl);
                Pl_act_set2(pl, 5, 9, 0);
            }
        }
        break;
    }
}
