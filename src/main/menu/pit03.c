/* SLPM_654.95 0x00275540-0x002755C8: Item_ok_chk .. Pit_shot_ok_chk. See pit_nm.c. */
#include "g_alias.h"
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











#if !defined(G_PART) || G_PART == 1
int Item_ok_chk(PLW *pl) {
    return *((u8 *)pl + 0x8F0) != 0;
}
#endif

#if !defined(G_PART) || G_PART == 2
int Pit_shot_ok_chk(PLW *pl) {
    if (*((u8 *)pl + 0x8ED) != 0) {
        return 1;
    }
    if ((s16)act_ck(pl, 0, 0x36) != 0 && (s16)Pl_item_num_ck(pl, GV(0xA2, 0x200)) != 0) {
        return 1;
    }
    return 0;
}
#endif
