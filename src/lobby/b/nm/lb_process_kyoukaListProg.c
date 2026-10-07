#include "lobby_s.h"
extern s32 armorIndex;
extern LB_SHOPITEM shopList2[];
extern u8 D_3C7005[];
extern s32 shop_process2_help[];
extern char User_data[];
int Gun_option_ck();
void cnWrap_SoundRequest();
void lb_process_tag_decide01();
s32 lb_process_kyoukaListProg(void) {
    LB_SHOPITEM *e;
    int n;

    e = &shopList2[lbShop.x70];
    if (lbShop.key & 0x20) {
        if (e->state == 0) {
            if (D_3C7005[armorIndex * 6] != 7) {
                lbShop.help = shop_process2_help[5];
            } else {
                switch (*(u16 *)((u8 *)e + 0x26)) {
                case 1:
                    if (Gun_option_ck(User_data, (s16)armorIndex, 0x20) == 1) {
                        lbShop.help = shop_process2_help[19];
                    } else {
                        lbShop.help = shop_process2_help[17];
                    }
                    break;
                case 3:
                    if (Gun_option_ck(User_data, (s16)armorIndex, 0x10) == 1) {
                        lbShop.help = shop_process2_help[18];
                    } else {
                        lbShop.help = shop_process2_help[17];
                    }
                    break;
                default:
                    lbShop.help = shop_process2_help[17];
                    break;
                }
            }
            cnWrap_SoundRequest(0);
            lbShop.x6E = 0;
            lbShop.x84 = 2;
            return 0;
        }
        cnWrap_SoundRequest(7);
    } else if (lbShop.key & 0x40) {
        cnWrap_SoundRequest(3);
        if (lbShop.x1C == 0) {
            lbShop.x70 = 0;
            lb_process_tag_decide01();
            return 3;
        }
        lbShop.x1C = 0;
    } else if (lbShop.key & 0x200) {
        if (e->state != 2) {
            cnWrap_SoundRequest(0xE);
            if (lbShop.x1C != 1) {
                lbShop.x1C = 1;
                lbShop.x6E = 0;
            } else {
                lbShop.x1C = 0;
            }
        } else {
            cnWrap_SoundRequest(7);
        }
    } else if (lbShop.key & 0x80) {
        if (D_3C7005[armorIndex * 6] != 7) {
            if (e->state != 2) {
                cnWrap_SoundRequest(0xF);
                if (lbShop.x1C != 2) {
                    lbShop.x1C = 2;
                } else {
                    lbShop.x1C = 0;
                }
            } else {
                cnWrap_SoundRequest(7);
            }
        }
    } else if (lbShop.key & 0x2000) {
        if (lbShop.x1C == 0) {
            cnWrap_SoundRequest(1);
            n = lbShop.x70 - 1;
            lbShop.x70 = n;
            if (n < 0) {
                lbShop.x70 = lbShop.count - 1;
            }
        }
    } else if (lbShop.key & 0x1000) {
        if (lbShop.x1C == 0) {
            cnWrap_SoundRequest(1);
            n = lbShop.x70 + 1;
            lbShop.x70 = n;
            if (n >= lbShop.count) {
                lbShop.x70 = 0;
            }
        }
    } else if (lbShop.key & 0x800) {
        if (lbShop.x1C == 1) {
            s8 lim;
            s8 v;
            cnWrap_SoundRequest(1);
            lim = lbShop.tbl[lbShop.cur * 2] == 7 ? 4 : 2;
            v = lbShop.x6E - 1;
            lbShop.x6E = v;
            if (v < 0) {
                lbShop.x6E = lim - 1;
            }
        }
    } else if (lbShop.key & 0x400) {
        if (lbShop.x1C == 1) {
            s8 lim;
            s8 v;
            cnWrap_SoundRequest(1);
            lim = lbShop.tbl[lbShop.cur * 2] == 7 ? 4 : 2;
            v = lbShop.x6E + 1;
            lbShop.x6E = v;
            if (v >= lim) {
                lbShop.x6E = 0;
            }
        }
    }
    if (lbShop.x1C == 0 || D_3C7005[armorIndex * 6] != 7) {
        lbShop.help = shop_process2_help[3];
    } else {
        switch (*(u16 *)((u8 *)e + 0x26)) {
        case 0:
            lbShop.help = shop_process2_help[10];
            break;
        case 1:
            lbShop.help = shop_process2_help[11];
            break;
        case 2:
            lbShop.help = shop_process2_help[12];
            break;
        case 3:
            lbShop.help = shop_process2_help[13];
            break;
        case 4:
            lbShop.help = shop_process2_help[14];
            break;
        case 5:
            lbShop.help = shop_process2_help[15];
            break;
        case 6:
            lbShop.help = shop_process2_help[16];
            break;
        }
    }
    lbShop.cur = lbShop.x70 + lbShop.x6C * 7;
    return 2;
}
