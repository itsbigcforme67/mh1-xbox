/* SLPM_654.95 0x00275020-0x00275230: Menu_select_mv .. Cockpit_chat_chk. See pit_nm.c. */
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











void Menu_select_mv(u8 *p, int pad, int n) {
    u8 half = ((n & 0xFF) >> 1);
    int sw;
    u8 v;

    if (*p < half) {
        ListSelect(p, pad, half);
    } else {
        *p = *p - half;
        ListSelect(p, pad, half);
        *p += half;
    }
    sw = pad & 0xFFFF;
    if (sw & 0xC00) {
        if (sw & 0x800) {
            if (*p < half) {
                *p = *p + n;
            }
            *p = *p - half;
        } else {
            *p += half;
            v = *p;
            if (v >= (n & 0xFF)) {
                *p = v - n;
            }
        }
        se_req(7, 0x11, 0);
    }
}

u8 Reibun_select_mv(int pad, u8 sel) {
    int sw;

    if (sel < 6) {
        ListSelect(&sel, pad, 6);
    } else {
        sel -= 6;
        ListSelect(&sel, pad, 6);
        sel += 6;
    }
    sw = pad & 0xFFFF;
    if (sw & 0xC00) {
        if (sw & 0x800) {
            if (sel < 6) {
                sel += 0xC;
            }
            sel = sel - 6;
        } else {
            sel += 6;
            if (sel >= 0xC) {
                sel = sel - 0xC;
            }
        }
        se_req(7, 0x11, 0);
    }
    return sel;
}

int Cockpit_chat_chk(void) {
    return PitMenu[0x1D] != 0;
}
