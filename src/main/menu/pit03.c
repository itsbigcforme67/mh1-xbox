/* SLPM_654.95 0x00275540-0x002755C8: Item_ok_chk .. Pit_shot_ok_chk. See pit_nm.c. */
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











int Item_ok_chk(PLW *pl) {
    return *((u8 *)pl + 0x8F0) != 0;
}

int Pit_shot_ok_chk(PLW *pl) {
    if (*((u8 *)pl + 0x8ED) != 0) {
        return 1;
    }
    if ((s16)act_ck(pl, 0, 0x36) != 0 && (s16)Pl_item_num_ck(pl, 0xA2) != 0) {
        return 1;
    }
    return 0;
}
