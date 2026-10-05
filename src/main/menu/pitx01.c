/* SLPM_654.95 0x00275230-0x00275290: UseItemChk .. UseItemChk. See pit_nm.c. */
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











int UseItemChk(u8 *pl, int slot) {
    u8 *q = (u8 *)((slot & 0xFFFF) * 4) + (int)pl;
    s16 id;

    if (*(s16 *)(q + 0x82A) > 0) {
        id = *(s16 *)(q + 0x828);
        if (id != 0 && Item_data[id][1] == 1) {
            return 1;
        }
    }
    return 0;
}
