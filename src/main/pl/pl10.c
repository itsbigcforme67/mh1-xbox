/* Player code (SLPM_654.95 0x00138BE0-0x001398F0): basic_kabe_ck (wall check: wall_act_ck result and three
   front_land_ck probes pick the wall action), item_action_set (use the selected pouch item: picks the
   action from the item id), trade_get_ck (accept an item another player is handing over). */
#include "pl.h"
#include "game.h"
#include "plf.h"

u8 basic_kabe_ck(PLW *pl) {
    int sp3C;
    u8 r;

    r = 0;
    switch ((s8)wall_act_ck(pl, 0)) {
    case 0:
        break;
    case 1:
        Pl_act_set(pl, 0, 0x2B, 0xC);
        r = 1;
        break;
    }
    if (front_land_ck(55.0f, 60.0f, 120.0f, pl, &sp3C) != 0) {
        pl->ang_y = pl->ang[1];
        Pl_act_set2(pl, 0, 0x1E, 0xC);
        r = 1;
    } else if (front_land_ck(55.0f, 180.0f, 30.0f, pl, &sp3C) != 0) {
        pl->ang_y = pl->ang[1];
        Pl_act_set2(pl, 0, 0x1B, 0xC);
        r = 1;
    } else if (front_land_ck(55.0f, 210.0f, 90.0f, pl, &sp3C) != 0) {
        pl->ang_y = pl->ang[1];
        Pl_act_set2(pl, 0, 0x15, 0xC);
        r = 1;
    }
    return r;
}

void item_action_set(PLW *pl, u8 mode) {
    u8 sp3F;
    u16 sp3C;
    f32 sp30[3];
    u16 temp_v1;
    u8 temp_a0;
    u8 temp_a0_2;

    if ((s16)Get_Active_itemnum() <= 0) {
        se_req(7, 0x15, 0);
        if (mode == 1) {
            pl_to_normal(pl, 0, 4, 0);
        }
        return;
    }
    temp_v1 = pl->item[pl->work888].id;
    switch (temp_v1) {
    case 0x1A:
    case 0x1C:
        pl->work88A = temp_v1;
        if (pl_flag_ck(pl, 0x02000600) == 0) {
            Pl_act_set2(pl, 0, 0xB, 0xC);
            return;
        }
        Pl_act_set2(pl, 0, 0xE, 0xC);
        return;
    case 0xA5:
        if (Modori_dama_ck() == 1) {
            pl->work88A = (u16) pl->item[pl->work888].id;
            Pl_act_set2(pl, 0, 0x72, 0xC);
            return;
        }
        se_req(7, 0x15, 0);
        if (mode == 1) {
            pl_to_normal(pl, 0, 4, 0);
        }
        break;
    case 0x1B:
    case 0x80:
    case 0x68:
    case 0x21:
    case 0x9F:
        pl->work88A = temp_v1;
        Pl_act_set2(pl, 0, 0xE, 0xC);
        return;
    case 0x1F:
    case 0x12:
    case 0x16:
    case 0x18:
    case 0x17:
        if ((Niku_ok_ck() == 1) || (pl->item[pl->work888].id == 0x1F)) {
            temp_a0 = pl->st;
            if (temp_a0 == 0) {
                pl->work88A = (u16) pl->item[pl->work888].id;
                Pl_act_set2(pl, 0, 0x19, 0xC);
                return;
            }
            if (temp_a0 == 1) {
                pl->work88A = (u16) pl->item[pl->work888].id;
                Pl_act_set2(pl, 0, 0x59, 0xC);
                return;
            }
        } else {
            se_req(7, 0x15, 0);
            if (mode == 1) {
                pl_to_normal(pl, 0, 4, 0);
                return;
            }
        }
        break;
    case 0x20:
        if (Taru_ok_ck() == 1) {
            pl->work88A = (u16) pl->item[pl->work888].id;
            Pl_act_set2(pl, 0, 0x6F, 0xC);
            return;
        }
        se_req(7, 0x15, 0);
        if (mode == 1) {
            pl_to_normal(pl, 0, 4, 0);
            return;
        }
        break;
    case 0x1E:
        if (Pl_trap_use_ck(pl) >= 0) {
            pl->work88A = (u16) pl->item[pl->work888].id;
            Pl_act_set2(pl, 0, 0x2E, 0xC);
            return;
        }
        se_req(7, 0x15, 0);
        if (mode == 1) {
            pl_to_normal(pl, 0, 4, 0);
            return;
        }
        break;
    case 0x7A:
    case 0x7B:
    case 0x7C:
    case 0x7D:
        if ((St_unique_ck(pl, sp30, &sp3C, &sp3F) & 0xFFFF) == 2) {
            pl->work88A = (u16) pl->item[pl->work888].id;
            pl->ang_y = sp3C;
            Pl_act_set2(pl, 0, 0x4F, 0);
            return;
        }
        se_req(7, 0x15, 0);
        if (mode == 1) {
            pl_to_normal(pl, 0, 4, 0);
            return;
        }
        break;
    case 0xA2:
        if ((St_unique_ck(pl, sp30, &sp3C, &sp3F) & 0xFFFF) == 0x11) {
            pl->work800 = sp30[0];
            pl->work804 = sp30[1];
            pl->work808 = sp30[2];
            pl->ang_y = sp3C;
            Pl_adj_calc(pl, 0x14);
            pl->cnt39A = (s16) sp3F;
            Pl_act_set2(pl, 0, 0x36, 0x20);
            return;
        }
        se_req(7, 0x15, 0);
        if (mode == 1) {
            pl_to_normal(pl, 0, 4, 0);
            return;
        }
        break;
    case 0x41:
    case 0x5:
    case 0x6:
    case 0x1:
    case 0x2:
    case 0x9C:
    case 0x3:
    case 0x4:
    case 0x7:
    case 0x8:
    case 0x9:
    case 0xA:
    case 0xD:
    case 0xE:
    case 0xF:
    case 0x10:
    case 0x9D:
    case 0x5F:
    case 0x5B:
    case 0x42:
    case 0x4B:
    case 0x53:
    case 0x54:
    case 0xA0:
    case 0xA1:
    case 0x9A:
    case 0xA4:
    case 0x99:
        pl->work88A = temp_v1;
        Pl_act_set2(pl, 0, 0x58, 0);
        return;
    case 0x81:
    case 0x143:
        if (Nikuyaki_ck(pl) == 1) {
            Pl_act_set(pl, 0, 0x4C, 0);
            return;
        }
        se_req(7, 0x15, 0);
        if (mode == 1) {
            pl_to_normal(pl, 0, 4, 0);
            return;
        }
        break;
    case 0x69:
    case 0x9B:
    case 0x5E:
    case 0x6A:
        temp_a0_2 = pl->kind;
        if ((temp_a0_2 != 1) && (temp_a0_2 != 5)) {
            pl->work88A = temp_v1;
            Pl_act_set2(pl, 0, 0x61, 0);
            return;
        }
        se_req(7, 0x15, 0);
        if (mode == 1) {
            pl_to_normal(pl, 0, 4, 0);
            return;
        }
        break;
    case 0x89:
        pl->work88A = temp_v1;
        Pl_act_set(pl, 0, 0x5E, 0xC);
        return;
    case 0x8A:
    case 0x8B:
    case 0x8C:
    case 0x8D:
        pl->work88A = temp_v1;
        Pl_act_set(pl, 0, 0x62, 0xC);
        return;
    case 0x13:
    case 0x14:
    case 0x15:
        pl->work88A = temp_v1;
        Pl_act_set2(pl, 0, 0x5A, 0);
        return;
    case 0x82:
        if (pl->st == 1) {
            Pl_act_set2(pl, 0, 0x66, 0);
            return;
        }
        Pl_act_set2(pl, 0, 0x65, 0);
        return;
    case 0x83:
    case 0x84:
    case 0x85:
    case 0x86:
    case 0x87:
    case 0x88:
        break;
    default:
        se_req(7, 0x15, 0);
        if (mode == 1) {
            pl_to_normal(pl, 0, 4, 0);
        }
        break;
    }
}

s32 trade_get_ck_00139680(PLW *pl) {
    s16 i;
    PLW *p;
    u16 id;

    if (Online_ck() == 0) {
        return 0;
    }
    if (pl->work936 != 0) {
        return 0;
    }
    p = player_work;
    for (i = 0; i < game_w.pl_num; i++, p++) {
        {
            if (i != pl->id && p->be_flag != 0 && (s16)act_ck(p, 0, 0x67) != 0 && p->work909 == pl->id
                && flvecCalcDistance(pl->pos, p->pos) <= 300.0f) {
                id = p->work904;
                if (Item_data[id][2] < 3) {
                    if ((s16)Pl_item_num_ck(pl, id) == 0) {
                        if ((s16)Pl_item_search_space(pl) != 0) {
                            pl->work904 = p->work904;
                            pl->work906 = p->work906;
                            if (Pl_master_ck(pl) == 1) {
                                net_send_pl(pl, 7, p->id);
                            }
                            Pl_act_set2(pl, 0, 0x68, 0);
                            return 1;
                        }
                        set01_set2(lit_1830);
                        pl->work936 = 0x5A;
                        goto ret0;
                    }
                    if ((s16)Pl_item_num_ck2(pl, p->work904) >= p->work906) {
                        pl->work904 = p->work904;
                        pl->work906 = p->work906;
                        if (Pl_master_ck(pl) == 1) {
                            net_send_pl(pl, 7, p->id);
                        }
                        Pl_act_set2(pl, 0, 0x68, 0);
                        return 1;
                    }
                    set01_set(1, 0xF, (s16)p->work904);
                    pl->work936 = 0x5A;
ret0:
                    return 0;
                }
            }
        }
    }
    return 0;
}
