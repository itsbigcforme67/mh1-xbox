/* Player code (SLPM_654.95 0x001365F0-0x00136D38): item_sel_sub, item_blank_ck, pl_item_sel, shell_chg_ck, pl_shell_sel, pl_status_ck, hit_stop_calc, stick_dir_set. */
#include "pl.h"
#include "game.h"
#include "plf.h"

u16 item_sel_sub(PLW *pl, u16 sel, int dir) {
    s16 n;
    s16 i;
    s16 found;
    found = 0;
    if (!(dir & 0xFF)) {
        n = 0;
        i = sel;
        for (;;) {
            i++;
            if (i >= 20) i = 0;
            if (pl->item[i].id != 0 && Item_data[pl->item[i].id][1] == 1) {
                sel = i;
                found = 1;
                break;
            }
            n++;
            if (n >= 20) break;
        }
    } else {
        n = 0;
        i = sel;
        for (;;) {
            if (i == 0) i = 19;
            else i--;
            if (pl->item[i].id != 0 && Item_data[pl->item[i].id][1] == 1) {
                sel = i;
                found = 1;
                break;
            }
            n++;
            if (n >= 20) break;
        }
    }
    if (found == 0) sel = 0;
    return sel;
}

int item_blank_ck(PLW *pl) {
    s16 i;
    for (i = 0; i < 20; i++) {
        if (pl->item[i].id != 0 && pl->item[i].num > 0 && Item_data[pl->item[i].id][1] == 1) return 1;
    }
    return 0;
}

void pl_item_sel(PLW *pl) {
    u16 t;
    if (Pl_master_ck(pl) == 0) return;
    if (act_ck(pl, 2, 0x13) == 0 && (u32)(pl->flag14 - 3) > 1 && Game_clear_ck(1) != 1) {
        if (pl->sw.pad08[0] & 4) {
            if (pl->work88C == 0) se_req(7, 0x11, 0);
            pl->work88C = 1;
            if (item_blank_ck(pl) != 0) {
                t = pl->sw.pad08[2];
                if (t & 0x20) {
                    se_req(7, 0x12, 0);
                    pl->work888 = item_sel_sub(pl, pl->work888, 0);
                    pl->work8F2 |= 2;
                } else if (t & 0x200) {
                    se_req(7, 0x12, 0);
                    pl->work888 = item_sel_sub(pl, pl->work888, 1);
                    pl->work8F2 |= 4;
                }
            }
        } else if (pl->work88C != 0) {
            se_req(7, 0x14, 0);
            pl->work88C = 0;
        }
        return;
    }
    pl->work88C = 0;
}

int shell_chg_ck(PLW *pl) {
    u8 k = pl->kind;
    if (k != 1 && k != 5 && pl->work88E == 0xFF) return 0;
    if (pl->sw.pad08[0] & 4) return 0;
    if (pl->x56E == 0 && pl->work56D != pl->ammo_type && pl->flag12 != 0) return 1;
    return 0;
}

int pl_shell_sel(PLW *pl) {
    u16 t;
    u8 k;
    if (Pl_master_ck(pl) == 0) return 0;
    if (act_ck(pl, 2, 0x13) != 0) return 0;
    if (pl->flag14 == 3 || pl->flag14 == 4) return 0;
    if (Game_clear_ck(1) == 1) return 0;
    if (pl->work8BE != 0) return 0;
    k = pl->kind;
    if (k != 1 && k != 5) return 0;
    if (pl->x56E == 0) {
        pl->x56E = 1;
        pl->work56D = pl->ammo_type;
        pl->work8CE = pl->work8BC;
        pl->work8D2 = pl->work01D;
        pl->work8D1 = pl->work01C;
    }
    if (pl->sw.pad08[0] & 4) {
        t = pl->sw.pad08[2];
        if (t & 0x40) {
            if (pl->work88E == 0xFF) pl->work88E = Pl_shell_set(pl, 0, 3);
            else pl->work88E = Pl_shell_set(pl, pl->work88E, 1);
            Shell_type_set(pl, 0);
            if (pl->work88E != 0xFF) {
                pl->work8F2 |= 8;
                se_req(7, 0x12, 0);
            }
        } else if (t & 0x100) {
            if (pl->work88E == 0xFF) pl->work88E = Pl_shell_set(pl, 0, 2);
            else pl->work88E = Pl_shell_set(pl, pl->work88E, 0);
            Shell_type_set(pl, 0);
            if (pl->work88E != 0xFF) {
                pl->work8F2 |= 0x10;
                se_req(7, 0x12, 0);
            }
        }
    } else if (pl->x56E != 0) {
        pl->x56E = 0;
        if (pl->work56D != pl->ammo_type) {
            Shell_type_set(pl, 0);
        } else {
            pl->work8BC = pl->work8CE;
            pl->work01D = pl->work8D2;
            pl->work01C = pl->work8D1;
        }
    }
    if (pl->work56D == pl->ammo_type) {
        pl->work8BC = pl->work8CE;
        pl->work01D = pl->work8D2;
        pl->work01C = pl->work8D1;
    }
    return 0;
}

int pl_status_ck(PLW *pl) {
    if (pl->x7B2 > 0) return 1;
    if (pl->x7C4 > 0) return 2;
    return 0;
}

void hit_stop_calc(PLW *pl) {
    u8 a = pl->work409;
    if (a != 0) {
        pl->x40A += a;
        pl->work409 = 0;
        return;
    }
    if (pl->x40A != 0) pl->x40A--;
}

int stick_dir_set(PLW *pl, int no) {
    return (pl->sw.ang[no] + (Get_view_dir() & 0xFFFF)) & 0xFFFF;
}
