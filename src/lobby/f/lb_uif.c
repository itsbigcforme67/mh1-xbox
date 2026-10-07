/* lb_uif - one translation unit 0x005931B0-0x00594260 (lbtu3). */
#define flfntLocate flfntLocate_hdr
#define DispDialogData DispDialogData_hdr
#include "lbui_proto.h"
#undef flfntLocate
#undef DispDialogData
typedef struct { s16 x; s16 y; s16 tx; s16 ty; } BHPOS;
extern BHPOS bh_pos[];
extern char lit_1085_0065DA48[];
extern char *strButtonHelp[];
typedef struct { u8 pad0000[0x2BFE]; s8 x2BFE[8]; u8 pad2C06[0x35D5 - 0x2C06]; u8 x35D5; } CWS_put_button_help;
#define BHCW ((CWS_put_button_help *)cw)
#pragma readonly_strings on
/* sprite template for Put_2TF / Put_sprite_rotate (helpLineTbl entries, stride 0x14) */
typedef struct { s16 x; s16 y; s16 w; s16 h; u8 pad08[4]; s16 u0; s16 v0; s16 u1; s16 v1; } DLGSPR;
typedef struct { f32 f[5]; } DLGF5;
void Put_sprite_rotate();
/* dialog frame: top/bottom edge strips of 0x28 high tiles then the two rotated side strips (near-match) */
void Paint_square();
/* menu frame: filled body (Paint_square or 5-high strips) + four 20-unit edge strips + four 7x6 corners (near-match) */
void put_button_help_a1(int a, int b, int c, u16 d);
extern char helpLineTbl_c2[];
typedef struct { s16 x; s16 y; char *s; } HLMSG;
extern struct { HLMSG *msg; int pos; int tick; } helpLineStr_c2;
#define font_print_double_a3 font_print_double_hdr
#define Draw_square_a3 Draw_square_hdr
#include "lbui.h"
#undef font_print_double_a3
#undef Draw_square_a3
void font_print_double_a3(int x, s16 y, int a, int b, char *s);
void Draw_square_a3(int x, s16 y, int w, int h, int c);
extern char lit_226_0065C500[];
extern char lit_227_0065C508[];
#pragma readonly_strings on
/* sprite template for Put_2TF / Put_sprite_rotate (helpLineTbl entries, stride 0x14) */
/* dialog frame: top/bottom edge strips of 0x28 high tiles then the two rotated side strips (near-match) */
/* menu frame: filled body (Paint_square or 5-high strips) + four 20-unit edge strips + four 7x6 corners (near-match) */
void put_button_help_a4(int a, int b, int c, u16 d);
void put_button_help();
void DispButtonHelp();
void DispHelpLine();
void DispNameAndIDonDialog(s16 y, char *name, char *id);
void LBDisp_NowLoading2();
int Lbs_plaza();
void tl_menu_cursor_up();
void tl_menu_cursor_down();
void plaza_selectMenu();
void put_button_help(id, txt, btn, on)
s8 id;
s8 txt;
int btn;
s32 on;
{
    char buf[0x40];
    int tx;
    int ty;
    s32 off;
    int x;
    int y;

    off = 0;
    if (on & 0xFFFF) {
        off = 2;
    }
    x = bh_pos[id].x;
    y = bh_pos[id].y;
    if (BHCW->x35D5 != 0 && ((s8 *)(*(u8 *)0x3F34C1 + (int)cw))[0x2BFE] != 0) {
        y = (s16)(y - 0x18);
    }
    if (btn == 9) {
        x = (s16)(x - 0x1E);
        y = (s16)(y - 2);
        flfntLocate((s16)(x + 3), (s16)(y + 6));
        font_print(lit_1085_0065DA48);
    }
    Lb_put_button(x, (s16)((s16)y + (s16)off), btn);
    tx = bh_pos[id].tx;
    ty = bh_pos[id].ty;
    if (BHCW->x35D5 != 0 && ((s8 *)(*(u8 *)0x3F34C1 + (int)cw))[0x2BFE] != 0) {
        ty = (s16)(ty - 0x18);
    }
    strcpy(buf, strButtonHelp[txt]);
    font_print_double(tx, ty, 1, 0, buf);
}

/* button help line of the plaza menus: which of the four buttons are shown for each menu / sub menu step (near-match) */

void DispButtonHelp(n)
LB_NETW *n;
{
    u16 pad;
    int p;

    pad = n->x28;
    if (n->x0C == 0) {
        flfntSetSize(0x12, 0x12);
        reload_tex(1, 0x157);
        SetTextureStage(0x157);
        switch (n->sel) {
        case 14:
            put_button_help_a1(0, 0, 2, (u16)Get_sw2(0) & 0x100);
            put_button_help_a1(1, 1, 3, (u16)Get_sw2(0) & 0x200);
            return;
        case 0:
            if (n->step != 3) {
                p = pad & 0xFFFF;
                put_button_help_a1(1, 2, 2, p & 0x100 & 0xFFFF);
                put_button_help_a1(3, 3, 0, p & 0x20 & 0xFFFF);
            }
            put_button_help_a1(2, 4, 1, pad & 0xFFFF & 0x40 & 0xFFFF);
            return;
        case 1:
            p = pad & 0xFFFF;
            put_button_help_a1(2, 4, 1, p & 0x40 & 0xFFFF);
            put_button_help_a1(3, 3, 0, p & 0x20 & 0xFFFF);
            return;
        case 3:
            switch (n->step) {
            case 0:
            case 1:
            case 2:
            case 3:
            case 9:
                p = pad & 0xFFFF;
                put_button_help_a1(0, 5, 6, p & 0x80 & 0xFFFF);
                put_button_help_a1(1, 0xC, 3, p & 0x200 & 0xFFFF);
                put_button_help_a1(2, 4, 1, p & 0x40 & 0xFFFF);
                put_button_help_a1(3, 6, 0, p & 0x20 & 0xFFFF);
                return;
            case 4:
                p = pad & 0xFFFF;
                put_button_help_a1(0, 5, 6, p & 0x80 & 0xFFFF);
                put_button_help_a1(1, 0xC, 3, p & 0x200 & 0xFFFF);
                put_button_help_a1(2, 4, 1, p & 0x40 & 0xFFFF);
                return;
            case 5:
                p = pad & 0xFFFF;
                put_button_help_a1(2, 4, 1, p & 0x40 & 0xFFFF);
                put_button_help_a1(3, 3, 0, p & 0x20 & 0xFFFF);
                return;
            }
            break;
        case 5:
            switch (n->step) {
            case 0:
            case 5:
            case 6:
            case 8:
            case 11:
                p = pad & 0xFFFF;
                put_button_help_a1(1, 8, 3, p & 0x200 & 0xFFFF);
                put_button_help_a1(2, 4, 1, p & 0x40 & 0xFFFF);
                put_button_help_a1(3, 6, 0, p & 0x20 & 0xFFFF);
                return;
            default:
                put_button_help_a1(3, 3, 0, pad & 0xFFFF & 0x20 & 0xFFFF);
            case 9:
            case 10:
                put_button_help_a1(2, 4, 1, pad & 0xFFFF & 0x40 & 0xFFFF);
                return;
            }
            break;
        case 6:
            switch (n->step) {
            case 6:
            case 8:
            case 9:
            case 11:
                p = pad & 0xFFFF;
                put_button_help_a1(1, 8, 3, p & 0x200 & 0xFFFF);
                put_button_help_a1(2, 4, 1, p & 0x40 & 0xFFFF);
                put_button_help_a1(3, 6, 0, p & 0x20 & 0xFFFF);
                return;
            default:
                put_button_help_a1(3, 3, 0, pad & 0xFFFF & 0x20 & 0xFFFF);
            case 10:
                put_button_help_a1(2, 4, 1, pad & 0xFFFF & 0x40 & 0xFFFF);
                return;
            }
            break;
        case 4:
            switch (n->step) {
            case 0:
            case 1:
                p = pad & 0xFFFF;
                put_button_help_a1(0, 0xD, 6, p & 0x80 & 0xFFFF);
                put_button_help_a1(2, 4, 1, p & 0x40 & 0xFFFF);
                put_button_help_a1(3, 0xB, 0, p & 0x20 & 0xFFFF);
                return;
            case 2:
                p = pad & 0xFFFF;
                put_button_help_a1(1, 0xA, 3, p & 0x200 & 0xFFFF);
                put_button_help_a1(2, 4, 1, p & 0x40 & 0xFFFF);
                return;
            case 3:
                p = pad & 0xFFFF;
                put_button_help_a1(2, 4, 1, p & 0x40 & 0xFFFF);
                put_button_help_a1(3, 3, 0, p & 0x20 & 0xFFFF);
                return;
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
            case 12:
                p = pad & 0xFFFF;
                put_button_help_a1(2, 4, 1, p & 0x40 & 0xFFFF);
                put_button_help_a1(3, 3, 0, p & 0x20 & 0xFFFF);
                return;
            }
            break;
        case 7:
            put_button_help_a1(2, 4, 1, pad & 0xFFFF & 0x40 & 0xFFFF);
            return;
        case 8:
        case 10:
            p = pad & 0xFFFF;
            put_button_help_a1(2, 4, 1, p & 0x40 & 0xFFFF);
            put_button_help_a1(3, 0xE, 0, p & 0x20 & 0xFFFF);
            return;
        case 9:
            if ((s32)n->step >= 3) {
                p = pad & 0xFFFF;
                put_button_help_a1(2, 4, 1, p & 0x40 & 0xFFFF);
                put_button_help_a1(3, 3, 0, p & 0x20 & 0xFFFF);
                return;
            }
            p = pad & 0xFFFF;
            put_button_help_a1(1, 0xF, 2, p & 0x100 & 0xFFFF);
            put_button_help_a1(2, 4, 1, p & 0x40 & 0xFFFF);
            put_button_help_a1(3, 3, 0, p & 0x20 & 0xFFFF);
            return;
        case 11:
            p = pad & 0xFFFF;
            put_button_help_a1(1, 1, 3, p & 0x200 & 0xFFFF);
            put_button_help_a1(2, 4, 1, p & 0x40 & 0xFFFF);
            break;
        }
    }
}

void DispHelpLine() {
    u16 pad;
    s32 t;
    s32 p;
    s32 q;

    pad = Get_sw2(0);
    reload_tex(1, 0x157);
    SetTextureStage(0x157);
    SetFilterMode(1);
    flSetRenderState(0x60, 0);
    Put_2TF(helpLineTbl_c2 + 0x14);
    if (helpLineStr_c2.pos >= 0) {
        t = helpLineStr_c2.tick + 1;
        helpLineStr_c2.tick = t;
        if (t > 0) {
            p = helpLineStr_c2.pos + 1;
            helpLineStr_c2.tick = 0;
            helpLineStr_c2.pos = p;
            if (p > 0x100) {
                helpLineStr_c2.pos = -1;
            }
        }
    }
    if ((pad & 0xFFFF) & 0x100) {
        helpLineStr_c2.pos = -1;
    }
    flfntSetSize(0x12, 0x12);
    font_set_palette(0);
    if (helpLineStr_c2.msg != 0) {
        flfntLocate(helpLineStr_c2.msg->x, helpLineStr_c2.msg->y);
        font_print2((s16)helpLineStr_c2.pos, helpLineStr_c2.msg->s);
    }
}

void DispNameAndIDonDialog(s16 y, char *name, char *id) {
    int y1;
    int y2;

    if (*(u8 *)(cw + 0x2C5C) == 0 && *(u8 *)(cw + 0x2C31) != 5) {
        flfntSetSize(0x1C, 0x14);
        font_print_double_a3(0xEE, y, 1, 4, name);
        y1 = y;
        y2 = y1 + 0x1E;
        font_print_double_a3(0xEE, y2, 1, 4, id);
        flfntSetSize(0x14, 0x14);
        font_print_double_a3(0xB2, y, 1, 4, lit_226_0065C500);
        font_print_double_a3(0xB2, y2, 1, 4, lit_227_0065C508);
        Draw_square_a3(0xB2, y1 + 0x16, 0x11C, 1, -1);
        Draw_square_a3(0xB2, y1 + 0x34, 0x11C, 1, -1);
    }
}

/* button help line of the plaza menus: which of the four buttons are shown for each menu / sub menu step (near-match) */

void LBDisp_NowLoading2(a)
int a;
{
    switch (a) {
    case 0:
        break;
    case 1:
        SetDialogData(2, 5);
        break;
    case 2:
        SetDialogData(0, 5);
        break;
    case 3:
        SetDialogData(1, 5);
        break;
    }
    DispDialogData();
}

int Lbs_plaza(a)
LB_NETW *a;
{
    LB_TXT *t = text_lobby_msg[2];

    a->x28 = Get_sw2(0);
    switch (a->depth) {
    case 0:
        sprintf(t->s, lit_193_0065DBE8, Get_ServerName(), PlazaInfo[ClassInfo.plaza - 1].name);
        SetSceneTitle(2, 0);
        SetHelpLineMsg(2, 2);
        Lbc_set_prim(Lbs_plaza_trans, plaza_trans_ot0, plaza_trans_ot1);
        lobby_bgm_set2(0x48);
        a->cur = 2;
        a->depth++;
    case 1:
        plaza_selectMenu(a);
        break;
    case 2:
        plaza_moveMain();
        break;
    }
    return a->x10;
}

static void tl_menu_cursor_up(m)
LB_TLMENU *m;
{
    u8 *tbl = plazaMenuTbl[m->menu];
    int i;
    u8 *p;

    m->x28 = 0;
    do {
        if (m->cur == 0) {
            i = 0;
            p = tbl;
            do {
                if (p[0x24] == 2) {
                    break;
                }
                i++;
                p += 0x24;
            } while (i < 0x14);
            m->cur = i;
        } else {
            m->cur--;
        }
    } while (tbl[m->cur * 0x24] != 1);
}

static void tl_menu_cursor_down(m)
LB_TLMENU *m;
{
    u8 *tbl = plazaMenuTbl[m->menu];
    u8 t;

    for (;;) {
        if (tbl[m->cur * 0x24] == 2) {
            m->cur = 0;
        } else {
            m->cur++;
        }
        t = tbl[m->cur * 0x24];
        if (t == 1) {
            return;
        }
        if (t == 2) {
            m->cur = 0;
        }
    }
}

void plaza_selectMenu(a)
LB_NETW *a;
{
    u8 *tbl = plazaMenuTbl[a->menu];
    int sw;
    u8 *t2;
    int v;
    int off;

    v = (u16)Get_sw2(0);
    a->x28 = v;
    SetSceneSubTitle(2, 1, tl_etc[0]);
    sw = v & 0xFFFF;
    a->sel = 0xE;
    if (sw & 0x2000) {
        tl_menu_cursor_up(a);
        off = a->cur * 0x24;
        SetHelpLineMsg(2, *(u16 *)(off + (int)tbl + 2) + 2);
        cnWrap_SoundRequest(1);
        return;
    }
    if (sw & 0x1000) {
        tl_menu_cursor_down(a);
        off = a->cur * 0x24;
        SetHelpLineMsg(2, *(u16 *)(off + (int)tbl + 2) + 2);
        cnWrap_SoundRequest(1);
        return;
    }
    if (sw & 0xC00) {
        a->menu ^= 1;
        t2 = plazaMenuTbl[a->menu];
        if (t2[a->cur * 0x24] != 1) {
            tl_menu_cursor_down(a);
        }
        off = a->cur * 0x24;
        SetHelpLineMsg(2, *(u16 *)(off + (int)t2 + 2) + 2);
        cnWrap_SoundRequest(1);
        return;
    }
    if (sw & 0x20) {
        off = a->cur * 0x24;
        a->sel = *(u16 *)(off + (int)tbl + 2);
        a->depth++;
        if (a->sel != 0xC) {
            SetSceneSubTitle(2, 1, tbl + a->cur * 0x24 + 4);
        }
        SetHelpLineMsg(2, a->sel + 0x10);
        cnWrap_SoundRequest(0);
        switch (a->sel) {
        case 0:
        case 2:
        case 3:
        case 5:
        case 6:
        case 7:
        case 0xB:
            plaza_moveMain(a->sel);
            break;
        }
        a->x28 = 0;
        return;
    }
    if ((*(u16 *)0x3F3714 & 0x100) || kb_chat_in_chk() == 1) {
        a->sel = 0xE;
        cnWrap_SoundRequest(0xE);
        a->depth++;
        return;
    }
    if (sw & 0x200) {
        Name_ID_change();
    }
}

