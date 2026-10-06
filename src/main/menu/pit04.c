/* pit04 - pit menu helpers 0x00274EC0-0x0027501C: ListSelect, PageSelect. Whole file in pit_nm.c. */
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
        se_req(7, 0x16, 0);
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
        se_req(7, 0x16, 0);
        return 1;
    }
    return 0;
}
