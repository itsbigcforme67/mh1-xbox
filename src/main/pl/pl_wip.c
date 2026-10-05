/* work in progress; functions move to pl_nm.c once they compile */
#include "pl.h"
#include "game.h"
#include "plf.h"

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
