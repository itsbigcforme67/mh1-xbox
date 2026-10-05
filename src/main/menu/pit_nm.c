/* pit_nm - f_load_pit (SLPM_654.95 0x00274E10-0x00275580, main.bin): in-quest pit/menu helpers: texture load,
 * list/page cursor movement, item usability checks. Near-match C, not built. */
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
        se_req(7, 0x16, 0, v);
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
        se_req(7, 0x16, 0, v);
        return 1;
    }
    return 0;
}

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

int UseItemChk(u8 *pl, int slot) {
    u8 *q = pl + (slot & 0xFFFF) * 4;
    s16 id;

    if (*(s16 *)(q + 0x82A) > 0) {
        id = *(s16 *)(q + 0x828);
        if (id != 0 && Item_data[id][1] == 1) {
            return 1;
        }
    }
    return 0;
}

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
        return 1;
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
        return 1;
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
        return 1;
    case 0x86:
    case 0x87:
    case 0x88:
        if ((St_pick_ck(pl, &a, b) & 0xFFFF) == 0xFFFF) {
            return 0;
        }
        if (a != 4) {
            return 0;
        }
        return 1;
    case 0xA2:
        p = pl->fish878;
        if (p == 0) {
            return 0;
        }
        if (*(u16 *)(p + 2) != 0x11) {
            return 0;
        }
        return 1;
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
            return 1;
        }
        return 1;
    }
}
