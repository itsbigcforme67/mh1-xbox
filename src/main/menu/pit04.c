/* pit04 - pit menu helpers 0x00274EC0-0x0027501C: ListSelect, PageSelect. Whole file in pit_nm.c. */
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











int ListSelect(u8 *p, int pad, int n) {
    u8 v;
    u8 old = *p;
    int sw = pad & 0xFFFF;

    v = old;

    if (sw & 0x2000) {
        if (old == 0) {
            v = (n & 0xFF) - 1;
        } else {
            v = old - 1;
        }
    } else if (sw & 0x1000) {
        v = old + 1;
        if (v >= (n & 0xFF)) {
            v = 0;
        }
    }
    if (v != old) {
        *p = v;
        se_req(7, GV(0x16, 0x17), 0);
        return 1;
    }
    return 0;
}

int PageSelect(u8 *p, int pad, int n) {
    u8 v;
    u8 old = *p;
    int sw = pad & 0xFFFF;

    v = old;

    if (sw & 0x800) {
        if (old == 0) {
            v = (n & 0xFF) - 1;
        } else {
            v = old - 1;
        }
    } else if (sw & 0x400) {
        v = old + 1;
        if (v >= (n & 0xFF)) {
            v = 0;
        }
    }
    if (v != old) {
        *p = v;
        se_req(7, GV(0x16, 0x17), 0);
        return 1;
    }
    return 0;
}
