/* Lobby item box UI and plaza chat log (SLPM_654.95 lobby overlay 0x609770-0x60E330). Whole file; runs split into lb_ibNN.c */
#include "lobby_f.h"
extern u8 *ib;
void se_req();
s32 Lb_ItemBox_open(u16 arg0, s32 arg1) {
    arg0 = 0;
    F(s32, ib, 4) = 0;
    F(s16, ib, 2) = 0;
    F(s16, ib, 8) = 0;
    F(s8, ib, 0xB) = arg0;
    F(s8, ib, 0x1F) = 0;
    F(u8, ib, 0x20) = 0xFF;
    *(s8 *)0x39DAD1 = 5;
    *(s8 *)0x39DAD0 = 0;
    *(s16 *)0x39DAD2 = 0;
    se_req(7, 0x11, 0, 0xFF);
    return 1;
}
void ListSelect();
int itembox_stock();
int itembox_pickup();
int itembox_equipchange();
int itembox_sortup();
int itembox_sellout();
s32 Lb_ItemBox_mv(int arg0) {
    s32 s0;
    u8 *a;
    s0 = arg0 & 0xFFFF;
    F(s8, ib, 0) = 0;
    a = ib;
    switch (a[4]) {
    case 0:
        *(s8 *)0x39DAD0 = 0;
        if (!(s0 & 0x40)) {
            ListSelect(a + 2, arg0, 5);
            if (s0 & 0x20) {
                arg0 = 0;
                F(s8, ib, 3) = 0;
                F(s8, ib, 0xB) = 0;
                F(s16, ib, 8) = (u8)arg0;
                F(s8, ib, 0x1F) = 0;
                F(s8, ib, 0x1D) = -1;
                F(u8, ib, 0x1E) = 0xFF;
                F(u8, ib, 4) = F(u8, ib, 4) + 1;
                F(s8, ib, 5) = 0;
                se_req(7, 0x13, 0, -1);
    case 1:
                *(u8 *)0x39DAD0 = 1;
                switch (F(u8, ib, 2)) {
                case 0:
                    s0 = itembox_stock(arg0) & 0xFFFF;
                    break;
                case 1:
                    s0 = itembox_pickup(arg0) & 0xFFFF;
                    break;
                case 2:
                    s0 = itembox_equipchange(arg0) & 0xFFFF;
                    break;
                case 3:
                    s0 = itembox_sortup(arg0) & 0xFFFF;
                    break;
                case 4:
                    s0 = itembox_sellout(arg0) & 0xFFFF;
                    break;
                }
                if ((u16)s0 & 0x40) {
                    s0 = (u16)(s0 & 0xFFBF);
                    *(u8 *)0x39DAD0 = 0;
                    F(u8, ib, 4) = 0;
                    se_req(7, 0x14, 0);
                }
            }
        }
        break;
    }
    if ((u16)s0 & 0x40) {
        se_req(7, 0x14, 0);
        return 0;
    }
    F(s8, ib, 0) = 1;
    return 1;
}
