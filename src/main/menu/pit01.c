/* SLPM_654.95 0x00274E10-0x00274EBC: load_pit .. load_pit. See pit_nm.c. */
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











void load_pit(void) {
    load_texlist(*(s32 *)(PIT_TEX + 4), 0x118, 0);
    if (game_w.x1DC == 0) {
        mkmapTexture(game_w.x2E, game_w.quest, 0x119, 0);
        mkTexture(2, 0x11A, 0);
    } else {
        mkTexture(7, 0x119, 0);
        mkTexture(5, 0x11A, 0);
    }
    mkTexture(8, 0x11B, 0);
    mkTexture(4, 0x11C, 0);
}
