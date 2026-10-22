/* SLPM_654.95 0x00275230-0x00275290: UseItemChk .. UseItemChk. See pit_nm.c. */
#include "types.h"
#include "game.h"
#include "pl.h"

#include "version.h"
#ifndef __MWERKS__
short Pl_item_num_ck();   /* (declared before its first use) */
short act_ck();   /* (declared before its first use) */
#endif
extern u8 Item_data[327][ITEM_ROW];
extern u8 PIT_TEX[];
extern u8 PitMenu[];

int load_texlist();
int mkTexture();
int mkmapTexture();
int se_req();
#ifdef __MWERKS__
int act_ck();
#else
short act_ck();   /* the real return type (tools/check_kr_returns.py) */
#endif
#ifdef __MWERKS__
int Pl_item_num_ck();
#else
short Pl_item_num_ck();   /* the real return type (tools/check_kr_returns.py) */
#endif
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
