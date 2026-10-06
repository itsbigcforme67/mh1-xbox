/* pit05 - pit menu helpers 0x00275290-0x00275538: Item_valid_chk. Whole file in pit_nm.c. */
#include "types.h"
#include "game.h"
#include "pl.h"

extern u8 Item_data[327][16];
extern u8 PIT_TEX[];
extern u8 PitMenu[];

int load_texlist();
int mkTexture();
int mkmapTexture();
int se_req();
int act_ck();
int Pl_item_num_ck();
int pl_flag_ck();
s16 Pl_trap_use_ck();
int Nikuyaki_ck();
int Niku_ok_ck();
int Taru_ok_ck();
int Modori_dama_ck();
int St_pick_ck();
int ListSelect();











int Item_valid_chk(u16 id) {
    PLW *pl = &player_work[game_w.master];
    s16 a;
    int b[2];
    u8 *p;

    switch (id) {
    case 0x1E:
        if (Pl_trap_use_ck(pl) < 0) {
            return 0;
        }
        break;
    case 0x81:
    case 0x143:
        return Nikuyaki_ck(pl);
    case 0x69:
    case 0x9B:
    case 0x5E:
    case 0x6A:
        if (pl->kind == 1 || pl->kind == 5) {
            return 0;
        }
        break;
    case 0x20:
        return Taru_ok_ck();
    case 0x12:
    case 0x16:
    case 0x18:
    case 0x17:
        return Niku_ok_ck();
    case 0x83:
    case 0x84:
    case 0x85:
        if ((St_pick_ck(pl, &a, b) & 0xFFFF) == 0xFFFF) {
            return 0;
        }
        if (a != 3) {
            return 0;
        }
        break;
    case 0x86:
    case 0x87:
    case 0x88:
        if ((St_pick_ck(pl, &a, b) & 0xFFFF) == 0xFFFF) {
            return 0;
        }
        if (a != 4) {
            return 0;
        }
        break;
    case 0xA2:
        p = pl->fish878;
        if (p == 0) {
            return 0;
        }
        if (*(u16 *)(p + 2) != 0x11) {
            return 0;
        }
        break;
    case 0xA5:
        return Modori_dama_ck();
    default:
        if (Item_data[id][5] == 4 && pl_flag_ck(pl, 0x80000) == 0) {
            p = pl->fish878;
            if (p == 0) {
                return 0;
            }
            if (*(u16 *)(p + 2) != 2) {
                return 0;
            }
        }
        break;
    }
    return 1;
}
