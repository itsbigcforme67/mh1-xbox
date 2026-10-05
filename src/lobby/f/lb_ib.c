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

extern char frame_itembox_cmd[];
extern u32 D_3C733C[];
extern char *yes_or_no[2];
int DispFrameList();
int Disp_help_mess();
int Put_shousai();
int font_print_sp();
void flfntSetSize();
void flfntLocate();
void font_set_palette();
void font_print_uf();
void SetFilterMode();
void reload_tex();
void SetTextureStage();
void PutArrow();

int disp_itembox_cmd(int a) {
    if (!(a & 0xFF)) {
        *(s32 *)(frame_itembox_cmd + 0x10) = 0xA9182;
    } else {
        *(s32 *)(frame_itembox_cmd + 0x10) = 0x808080;
    }
    return DispFrameList(frame_itembox_cmd, 0, F(u8, ib, 2));
}

void item_explanation(int a, int b, int c) {
    if ((a & 0xFF) == 1) {
        Disp_help_mess(1, (u16)(*(u16 *)&D_3C733C[b & 0xFF] + 0x18));
    } else {
        Put_shousai();
    }
}

int yes_no_disp_sub(void) {
    flfntSetSize(0x12, 0x12);
    flfntLocate(0x1B0, 0x18E);
    return font_print_sp(yes_or_no[F(u8, ib, 0x21)]);
}

/* amount selector: two-digit number with up/down arrows */
void kosuu_disp_sub(void) {
    s8 buf[8];
    u8 *w;
    if (F(s16, ib, 0x1A) != 0x270F) {
        flfntSetSize(0x12, 0x12);
        font_set_palette(0);
        flfntLocate(0x22E, 0x17A);
        w = ib;
        buf[0] = 0x82;
        buf[1] = F(s16, w, 0x1A) / 10 + 0x4F;
        buf[2] = 0x82;
        buf[3] = F(s16, w, 0x1A) % 10 + 0x4F;
        buf[4] = 0;
        font_print_uf(buf, 0xA, w, -0x7E);
        SetFilterMode(1);
        reload_tex(1, 0x11A);
        SetTextureStage(0x11A);
        if (F(u8, ib, 0x1C) == 0) {
            PutArrow(0x234, 0x168, 0x18, 0x12, 0xFF20FF28, 2);
        }
        if (F(s16, ib, 0x1A) != 1) {
            PutArrow(0x234, 0x18E, 0x18, 0x12, 0xFF20FF28, 3);
        }
    }
}
