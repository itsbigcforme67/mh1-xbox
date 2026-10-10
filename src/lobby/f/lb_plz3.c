/* lb_plz3 - one translation unit 0x00598DB0-0x0059DB40 (lbtu3). */
#define Lbs_MatchStart Lbs_MatchStart_hdr
#define Lb_on_dialog Lb_on_dialog_hdr
#include "lbui_proto.h"
#undef Lb_on_dialog
#undef Lbs_MatchStart
typedef struct CNET_W5D4 { s32 w[0x175]; } CNET_W5D4;
#pragma readonly_strings on
/* sprite template for Put_2TF / Put_sprite_rotate (helpLineTbl entries, stride 0x14) */
typedef struct { s16 x; s16 y; s16 w; s16 h; u8 pad08[4]; s16 u0; s16 v0; s16 u1; s16 v1; } DLGSPR;
typedef struct { f32 f[5]; } DLGF5;
void Put_sprite_rotate();
/* dialog frame: top/bottom edge strips of 0x28 high tiles then the two rotated side strips (near-match) */
void Paint_square();
/* menu frame: filled body (Paint_square or 5-high strips) + four 20-unit edge strips + four 7x6 corners (near-match) */
void put_button_help(int a, int b, int c, u16 d);
#pragma readonly_strings on
/* sprite template for Put_2TF / Put_sprite_rotate (helpLineTbl entries, stride 0x14) */
/* dialog frame: top/bottom edge strips of 0x28 high tiles then the two rotated side strips (near-match) */
/* menu frame: filled body (Paint_square or 5-high strips) + four 20-unit edge strips + four 7x6 corners (near-match) */
#pragma readonly_strings on
int Lb_put_icon();
extern u8 RecvMailInfo[][0x9A];
#pragma readonly_strings on
int DispFrameListA();
int Put_page_num_p3();
#pragma readonly_strings on
/* sprite template for Put_2TF / Put_sprite_rotate (helpLineTbl entries, stride 0x14) */
/* dialog frame: top/bottom edge strips of 0x28 high tiles then the two rotated side strips (near-match) */
/* menu frame: filled body (Paint_square or 5-high strips) + four 20-unit edge strips + four 7x6 corners (near-match) */
#pragma readonly_strings on
/* the header declares font_print_double_p5 without a prototype: rename that declaration away and give the function s16 parameters here */
#define font_print_double_p5 font_print_double_hdr
#undef font_print_double_p5
extern char lit_2316[];
extern char lit_2317[];
extern char lit_2354_0065DDD0[];
extern char lit_2355[];
int han2zen();
void flfntLocate(s16, s16);
int font_print_double_p5(s16, s16, int, int, char *);
int font_print();
void put_main_cursor_p5();
void Put_page_num_p5(s16, s16, int, int, int);
#pragma readonly_strings on
/* sprite template for Put_2TF / Put_sprite_rotate (helpLineTbl entries, stride 0x14) */
/* dialog frame: top/bottom edge strips of 0x28 high tiles then the two rotated side strips (near-match) */
/* menu frame: filled body (Paint_square or 5-high strips) + four 20-unit edge strips + four 7x6 corners (near-match) */
#define Lb_put_icon_free Lb_put_icon_free_hdr   /* lobby_a.h declares it K&R */
#undef Lb_put_icon_free
extern char lit_2418[];
extern char lit_2419[];
void Lb_put_icon_free(s16 x, s16 y, int z, int col, int n);
void Lb_put_icon_p7(s16 x, s16 y, int n, int col);
extern char tl_mail_tbl[];
extern char lit_2602[];
extern char lit_2603[];
extern char tl_job_tbl[];
extern char hunter_appellation[];
extern char PlazaInfo_c8[];
extern char LobbyInfo_c8[];
extern char D_3351D4[];
extern char D_3367BC[];
extern char D_336C00[];
extern char D_3371E0[];
extern char D_337800[];
extern char D_337E10[];
extern char D_338360[];
#pragma readonly_strings on
extern u8 lb_num_str[];
extern char lit_2632[];
extern char lit_2633[];
int font_print_uf();
#pragma readonly_strings on
/* sprite template for Put_2TF / Put_sprite_rotate (helpLineTbl entries, stride 0x14) */
/* dialog frame: top/bottom edge strips of 0x28 high tiles then the two rotated side strips (near-match) */
/* menu frame: filled body (Paint_square or 5-high strips) + four 20-unit edge strips + four 7x6 corners (near-match) */
#pragma readonly_strings on
void Put_comment();
#pragma readonly_strings on
void put_main_cursor_p12();
void put_main_cursor2_p12();
#pragma readonly_strings on
/* sprite template for Put_2TF / Put_sprite_rotate (helpLineTbl entries, stride 0x14) */
/* dialog frame: top/bottom edge strips of 0x28 high tiles then the two rotated side strips (near-match) */
/* menu frame: filled body (Paint_square or 5-high strips) + four 20-unit edge strips + four 7x6 corners (near-match) */
#define DispNameAndIDonDialog DispNameAndIDonDialog_hdr
#define Lb_on_dialog Lb_on_dialog_hdr
#undef DispNameAndIDonDialog
#undef Lb_on_dialog
void DispNameAndIDonDialog(s16 y, char *name, char *id);
#pragma readonly_strings on
/* sprite template for Put_2TF / Put_sprite_rotate (helpLineTbl entries, stride 0x14) */
/* dialog frame: top/bottom edge strips of 0x28 high tiles then the two rotated side strips (near-match) */
/* menu frame: filled body (Paint_square or 5-high strips) + four 20-unit edge strips + four 7x6 corners (near-match) */
typedef struct { s16 v[6]; } R12;
extern R12 lit_3380;
extern R12 lit_3382;
extern R12 lit_3397;
extern R12 lit_3399;
#pragma readonly_strings on
/* sprite template for Put_2TF / Put_sprite_rotate (helpLineTbl entries, stride 0x14) */
/* dialog frame: top/bottom edge strips of 0x28 high tiles then the two rotated side strips (near-match) */
/* menu frame: filled body (Paint_square or 5-high strips) + four 20-unit edge strips + four 7x6 corners (near-match) */
void Lb_clearChatID();
void Lb_clearChatList();
void plaza_ReibunEdit();
void plaza_checkChatLog();
void plaza_logOut();
void plaza_chatMain();
void tl_exit_sub_menu();
void Lb_put_new_mail();
void Lbs_plaza_trans();
void put_plaza_menu();
void put_titles();
void put_titles2();
void put_mainWindow();
void put_mainWindowTex();
void plaza_movePlazaTrans();
int Lb_get_cursor_col();
void Put_page_num(int x, int y, int page, s16 pages, int flag);
void put_member_info();
void plaza_checkMyStatusTrans();
void lb_put_comment();
void put_mail_input_square();
void plaza_chatTrans();
void Lb_on_dialog();
void plaza_trans_ot1();
void put_main_cursor(int n);
void put_main_cursor2(int x, int y, s32 n);
void Get_PlazaName();
void Get_LobbyName();
void Lbs_load();
void Lbc_release();
int Lb_put_icon_k();
int Put_page_num_k();
int disp_status_k();
int flfntLocate_k();
int font_print_double_k();
#ifdef __MWERKS__
asm int Lb_addChatMember()
{
#include "Lb_addChatMember.inc"
}
#endif

#ifdef __MWERKS__
asm void Lb_clearChatMember()
{
#include "Lb_clearChatMember.inc"
}
#endif

/* button help line of the plaza menus: which of the four buttons are shown for each menu / sub menu step (near-match) */

void Lb_clearChatID(id)
u8 *id;
{
    int i;
    u8 *p = (u8 *)chatIDList;

    for (i = 0; ; ) {
        if (memcmp(p, id, 8) == 0) {
            Lb_clearChatMember(i);
            return;
        }
        i = (s8)(i + 1);
        p += 8;
        if (i >= 7) {
            return;
        }
    }
}

void Lb_clearChatList() {
    s8 i = 0;
    u8 *a = (u8 *)chatIDList;
    u8 *b = (u8 *)chatHandleList;

    CW->chatmode = 0;
    do {
        memset(a, 0, 8);
        memset(b, 0, 0x10);
        i++;
        a += 8;
        b += 0x10;
    } while (i < 7);
}

void plaza_ReibunEdit() {
    int sw = Get_sw2(0) & 0xFFFF;

    switch (pNet->step) {
    case 0:
        Plaza_ReibunEdit_i();
        pNet->step++;
        break;
    case 1:
        pNet->x28 = Get_sw_on2(0);
        if ((u16)Plaza_ReibunEdit_mv(sw) & 0x40) {
            tl_exit_sub_menu(0);
        }
        break;
    }
}

void plaza_checkChatLog() {
    int sw = Get_sw2(0) & 0xFFFF;

    switch (pNet->step) {
    case 0:
        Plaza_chatlog_i();
        pNet->step++;
        break;
    case 1:
        pNet->x28 = Get_sw_on2(0);
        if ((u16)sw & 0x40) {
            Plaza_chatlog_i();
            tl_exit_sub_menu(0);
            break;
        }
        Plaza_chatlog_mv(sw);
        break;
    }
}

#ifdef __MWERKS__
asm int plaza_capcomPage()
{
#include "plaza_capcomPage.inc"
}
#endif

/* button help line of the plaza menus: which of the four buttons are shown for each menu / sub menu step (near-match) */

void plaza_logOut(a)
LB_NETW *a;
{
    int sw = Get_sw2(0) & 0xFFFF;
    int t;

    switch (a->step) {
    case 0:
        a->step++;
        SetDialogData(0x27, 2);
        SetDialogYesNo(1);
        return;
    case 1:
        t = sw & 0xFFFF;
        a->x0C = 1;
        if (t & 0x20) {
            a->step++;
            return;
        }
        if (t & 0x800) {
            if (a->yesno != 0) {
                SetDialogYesNo(0);
                cnWrap_SoundRequest(1);
                return;
            }
        } else if (t & 0x400) {
            if (a->yesno != 1) {
                SetDialogYesNo(1);
                cnWrap_SoundRequest(1);
                return;
            }
        } else {
            if (t & 0x40) {
                if (a->yesno != 1) {
                    SetDialogYesNo(1);
                    cnWrap_SoundRequest(1);
                    return;
                }
                a->step++;
                return;
            }
        }
        break;
    case 2:
        if (a->yesno == 0) {
            a->step++;
            cnWrap_SoundRequest(0);
            fade_set(1);
            return;
        }
        tl_exit_sub_menu(0);
        return;
    case 3:
        if ((Fade_busy_ck() & 0xFF) != 1) {
            To_LogOut(1);
        }
        break;
    }
}

void plaza_chatMain(a)
LB_NETW *a;
{
    int tbl = (int)plazaMenuTbl[a->menu];
    int off;

    a->x28 = Get_sw(0);
    switch (a->step) {
    case 0:
        a->step++;
        Plaza_chat_init();
        break;
    case 1:
        a->x28 = Get_sw_on2(0);
        if (Plaza_chat_move(*(u16 *)0x3F3714) == -1) {
            a->step++;
        }
        break;
    case 2:
        a->step++;
        break;
    case 3:
        tl_exit_sub_menu(1);
        off = a->cur * 0x24;
        SetHelpLineMsg(2, *(u16 *)(off + tbl + 2) + 2);
        break;
    }
}

void tl_exit_sub_menu(silent)
int silent;
{
    if (!(silent & 0xFF)) {
        cnWrap_SoundRequest(3);
    }
    SetHelpLineMsg(2, pNet->sel + 2);
    pNet->depth--;
    pNet->step = 0;
    pNet->x04 = 0;
    pNet->x05 = 0;
    pNet->x0A = 0;
    pNet->x24 = 0;
    pNet->x12 = 0;
    pNet->x28 = 0;
    pNet->x26 = 0;
    pNet->sel = 0xE;
    pNet->x0D = 1;
}

void Lb_put_new_mail(x, y)
int x;
int y;
{
    s8 i;
    f32 a;
    u8 *p;
    int ys;
    s16 f;

    reload_tex(1, 0x157);
    SetTextureStage(0x157);
    i = 0;
    p = (u8 *)RecvMailInfo;
    do {
        if (*p != 0) {
            if (++pNet->idx & 8) {
                a = 255.0f - 36.42857f * (f32)(pNet->idx & 7);
            } else {
                a = 36.42857f * (f32)(pNet->idx & 7);
            }
            ys = (s16)y;
            Lb_put_icon(x, (s16)(ys + 2), 9, -1);
            f = pNet->idx;
            if (f & 0x10) {
                if (f & 0x80) {
                    Lb_put_icon((s16)((s16)x - 0x10), (s16)(ys - 6), 0xC, ((u32)a << 24) | 0xFFFFFF);
                } else {
                    Lb_put_icon((s16)((s16)x - 0x10), (s16)(ys + 8), 0xC, ((u32)a << 24) | 0xFFFFFF);
                }
            } else {
                if (f & 0x80) {
                    Lb_put_icon((s16)((s16)x + 0xA), (s16)(ys + 0xA), 0xC, ((u32)a << 24) | 0xFFFFFF);
                } else {
                    Lb_put_icon((s16)((s16)x + 0xA), (s16)(ys - 8), 0xC, ((u32)a << 24) | 0xFFFFFF);
                }
            }
            break;
        }
        i++;
        p += 0x9A;
    } while (i < 8);
}

void Lbs_plaza_trans(a)
u8 *a;
{
    u8 *p = plazaMenuTbl[pNet->menu];
    struct { s16 x; s16 y; s16 w; s16 h; s32 col; } box;
    int i;

    font_set_stack_no(*(s32 *)(a + 0x18));
    flfntSetSize(0x12, 0x12);
    font_set_palette(0);
    put_plaza_menu(pNet);
    reload_tex(1, 0x154);
    SetTextureStage(0x154);
    SetFilterMode(1);
    flSetRenderState(0x60, 0);
    Put_2TF(textLobbyTbl);
    reload_tex(1, 0x157);
    SetTextureStage(0x157);
    Lb_put_new_mail(0x1AA, 0x1C);
    if (pNet->depth < 2) {
        DispFrameListA(pfl_menu_2138, 0, pNet->cur, 0xFF);
    } else {
        DispFrameListA(pfl_menu_2138, 0, -1, 0xFF);
    }
    Put_page_num_p3(0x82, 0x56, pNet->menu, 2, 0);
    box.col = 0x30FFFFFF;
    box.x = 0x16;
    i = 0;
    box.w = 0xC6;
    box.y = 0x57;
    do {
        box.h = box.y + 0x12;
        if (*p == 0) {
            Put_F(&box);
        }
        i++;
        p += 0x24;
        box.y += 0x16;
    } while (i < 0xB);
}

/* button help line of the plaza menus: which of the four buttons are shown for each menu / sub menu step (near-match) */

void put_plaza_menu(a)
LB_NETW *a;
{
    u8 *p;
    int y;

    y = 0x56;
    p = plazaMenuTbl[a->menu];

    flfntSetSize(0x12, 0x12);
    if (p[0] != 2) {
        do {
            if (a->depth == 2) {
                Lb_put_msg2(0x10, y, p + 4);
            } else {
                font_print_double_k(0x10, y, 1, 0, p + 4);
            }
            p += 0x24;
            y = (s16)(y + 0x16);
        } while (p[0] != 2);
    }
}

void put_titles(a, b, c)
int a;
int b;
int c;
{
    flfntSetSize(0x12, 0x12);
    font_print_double_k(a, b, 1, 5, c);
}

void put_titles2(a)
u8 *a;
{
    put_titles(*(s16 *)a, *(s16 *)(a + 2), *(int *)(a + 4));
}

void put_mainWindow(x, y)
int x;
int y;
{
    int xs = (s16)x;
    int ys;

    Draw_menu_square((s16)(xs - 6), y, 0x1A0, 0x110, 1, 0xFF2A0000);
    ys = (s16)y;
    Draw_square(x, (s16)(ys + 0x26), 0x192, 1, 0xFF602020);
    Draw_square(x, (s16)(ys + 0xEC), 0x192, 1, 0xFF602020);
}

void put_mainWindowTex(x, y)
int x;
int y;
{
    int xs = (s16)x;
    int ys;

    Draw_menu_square((s16)(xs - 6), y, 0x1A0, 0x110, 0, 0);
    ys = (s16)y;
    Draw_square(x, (s16)(ys + 0x26), 0x192, 1, 0xFF602020);
    Draw_square(x, (s16)(ys + 0xEC), 0x192, 1, 0xFF602020);
}

#ifdef __MWERKS__
asm int plaza_enterLobbyTrans()
{
#include "plaza_enterLobbyTrans.inc"
}
#else
/* PC build (ONLINE=1): the plaza's lobby list (7 lobbies a page: name, users) or the member list of the chosen
 * lobby; agent C's reading of the asm (wip/plaza_enterLobbyTrans_tu.c), the lobby count is ClassInfo+6 (lhu at
 * 0x59A0A4, the draft had +2). Not compared with the PS2 picture. */
extern char tl_member_buff[];
extern char tl_msg_tbl[];
extern char lit_2315[];


void plaza_enterLobbyTrans(int x, int y)
{
    char b2[0x40];
    char b1[0x40];
    int i = 0, pg, a, c, v;
    LB_PINFO *q;
    char *m = tl_member_buff;

    put_mainWindow(x, y);          /* a0 / a1 = x, y left over */
    flfntSetSize(0x12, 0x12);
    if (pNet->step != 2 && pNet->step != 3) {
        put_main_cursor2_p12(x, y, (u8)pNet->x0A % 7);
        put_titles((s16)x + 10, (s16)y + 0x28, *(int *)tl_msg_tbl);
        y = (s16)(y + 0x3E);
        pg = (u8)pNet->x0A / 7 * 7;
        q = LobbyInfo + pg;
        v = (s16)x + 10;
        do {
            if (pg < *(u16 *)((u8 *)&ClassInfo + 6)) {
                c = *(s16 *)((u8 *)q + 14);
                a = (s16)(*(s16 *)((u8 *)q + 2) - c);
                if (a < 0)
                    a = 0;
                sprintf(b1, lit_193_0065DBE8, Get_ServerName(), q->name);
                sprintf(b2, lit_2315, ((char **)lb_num_str)[a % 10], ((char **)lb_num_str)[c % 10]);
                Lb_put_icon_k((s16)v + 202, y, 7, -1);
                Lb_put_icon_k((s16)v + 309, y, 8, -1);
                if (i == (u8)pNet->x0A % 7) {
                    font_print_double_k(v, y, 1, 4, b1);
                    font_print_double_k((s16)v + 232, y, 1, 4, b2);
                } else {
                    font_set_palette(0);
                    flfntLocate(v, y);
                    font_print(lit_2316, b1);
                    flfntLocate((s16)v + 232, y);
                    font_print(lit_2316, b2);
                }
            } else {
                sprintf(b2, lit_2317);
                if (i == (u8)pNet->x0A % 7) {
                    font_print_double_k(v, y, 1, 4, b2);
                } else {
                    font_set_palette(0);
                    flfntLocate(v, y);
                    font_print(lit_2316, b2);
                }
            }
            y = (s16)(y + 22);
            i++;
            q++;
            pg++;
        } while (i < 7);
        flfntLocate((s16)x + 316, y);
        Put_page_num_k((s16)x + 316, y, (u8)pNet->x0A / 7, 2, 0);
    } else {
        put_titles((s16)x + 10, (s16)y + 0x28, *(int *)(tl_msg_tbl + 4));
        y = (s16)(y + 0x3E);
        if (pNet->x06 == 0) {
            flfntSetSize(0x16, 0x12);
            font_set_palette(4);
            font_print_double_k((s16)x + 10 + 70, y + 60, 1, 4, *(char **)(tl_msg_tbl + 8));
        } else {
            for (i = 0; i < 8; i++) {
                put_member_info((s16)x + 10, y, m + 640, m + 648, m + 666, 1);
                y = (s16)(y + 22);
                m += 764;
            }
        }
    }
}
#endif

void plaza_movePlazaTrans(a)
LB_NETW *a;
{
    char b3[0x10];
    char b2[0x10];
    char b1[0x50];
    LB_TXT *t;
    s16 sx;
    s16 y;
    int i;
    int pg;
    u8 *pi;

    t = text_lobby_msg[2];
    put_main_cursor_p5(*(u8 *)&a->x0A % 7);
    flfntSetSize(0x12, 0x12);
    put_titles2(t + 32);
    sx = t[32].x;
    i = 0;
    y = t[32].y + 0x16;
    pg = (*(u8 *)&a->x0A / 7) * 7;
    pi = (u8 *)PlazaInfo + pg * 0x15C;
    do {
        sprintf(b3, lit_2354_0065DDD0, *(u16 *)(pi + 2));
        han2zen(b3, b2);
        if (pg < *(u16 *)0x6DD7D2) {
            sprintf(b1, lit_2355, Get_ServerName(), pi + 0x14);
            if (i == *(u8 *)&a->x0A % 7) {
                font_print_double_p5(sx, y, 1, 4, b1);
                font_print_double_p5(sx + 0xFC, y, 1, 4, b2);
            } else {
                font_set_palette(0);
                flfntLocate(sx, y);
                font_print(lit_2316, b1);
                flfntLocate(sx + 0xFC, y);
                font_print(lit_2316, b2);
            }
        } else if (pg < 10) {
            font_set_palette(0);
            flfntLocate(sx, y);
            font_print(lit_2317);
        }
        i++;
        pi += 0x15C;
        pg++;
        y += 0x16;
    } while (i < 7);
    Put_page_num_p5(sx + 0x12C, y, a->x24, 2, 0);
}

/* button help line of the plaza menus: which of the four buttons are shown for each menu / sub menu step (near-match) */

int Lb_get_cursor_col() {
    f32 a = 0.0000958738f * (f32)(u32)(u16)((System_timer & 0x3F) << 10);

    return (((s8)(int)(80.0f * flSin(a)) + 0x9F) << 24) | 0xFF00;
}

void Put_page_num(int x, int y, int page, s16 pages, int flag) {
    char buf[0x20];
    char buf2[0x20];
    int col;

    col = Lb_get_cursor_col();
    font_set_palette(0);
    if (pages < 10) {
        sprintf(buf, lit_2418, (s16)page + 1, ((char **)lb_num_str)[11], pages);
    } else {
        sprintf(buf, lit_2419, (s16)page + 1, ((char **)lb_num_str)[11], pages);
    }
    han2zen(buf, buf2);
    flfntSetSize(0x12, 0x12);
    flfntLocate_k(x, y);
    font_set_palette(0);
    if ((flag & 0xFF) == 0) {
        font_print(lit_2316, buf2);
    } else {
        font_print(lit_2316, buf);
    }
    if (pages > 1) {
        reload_tex(1, 0x157);
        SetTextureStage(0x157);
        if (flag != 0) {
            if (pages < 10) {
                Lb_put_icon_free((s16)x - 0x18, (s16)y - 1, 0x14, col, 0);
                Lb_put_icon_free((s16)x + 0x28, (s16)y - 1, 0x14, col, 1);
            } else {
                Lb_put_icon_free((s16)x - 0x18, (s16)y - 1, 0x14, col, 0);
                Lb_put_icon_free((s16)x + 0x3A, (s16)y - 1, 0x14, col, 1);
            }
        } else {
            if (pages < 10) {
                Lb_put_icon_p7((s16)x - 0x1A, (s16)y - 3, 0, col);
                Lb_put_icon_p7((s16)x + 0x36, (s16)y - 3, 1, col);
            } else {
                Lb_put_icon_p7((s16)x - 0x1A, (s16)y - 3, 0, col);
                Lb_put_icon_p7((s16)x + 0x5A, (s16)y - 3, 1, col);
            }
        }
    }
}

#ifdef __MWERKS__
asm int plaza_checkFriendTrans()
{
#include "plaza_checkFriendTrans.inc"
}
#endif

#ifdef __MWERKS__
asm int disp_status()
{
#include "disp_status.inc"
}
#else
/* PC: written from the asm of 0x59AE60 (not compared with check.py; agent B, 10 Oct 2026). A hunter's status card
 * (my status, a friend, a search result): title, page number, then for pages 0 / 1 the handle, the id, the weapon kind
 * and the hunter rank; page 0 the server / plaza / lobby, page 1 the comment, page 2 the equipment with icons.
 * Arguments as the asm takes them: a0 x, a1 y, a2 id, a3 handle, t0 the mini data (+0 weapon kind, +1 rank, +9 / +0xA
 * the weapon kind / id, +0xE and +0x10..0x13 the armour), t1 the page, t2 the page count, t3 the comment. */
void Lb_put_job();
int disp_status(int x0, int y0, int id, int handle, int mini_, int page_, int pages, int comment)
{
    u8 *mini = (u8 *)mini_;
    s16 x = (s16)x0, y = (s16)((s16)y0 + 0x28), xt = (s16)(x + 0xA), xv = (s16)(xt + 0x3C), xe, xi;
    s8 page = (s8)page_;
    char b1[0x50], b2[0x50];
    static char *const *tabs[5];
    static const u8 tab_field[5] = { 0x10, 0x11, 0x12, 0x13, 0xE };
    static const u8 tab_icon[5] = { 0x13, 0x14, 0x16, 0x15, 0x17 };
    int k;
    u16 n;
    put_titles(xt, y, *(int *)(tl_mail_tbl + 0x2C));
    reload_tex(1, 0x157);
    SetTextureStage(0x157);
    SetFilterMode(1);
    flSetRenderState(0x60, 0);
    Put_page_num((s16)(xt + 0x12C), y, page, (s8)pages, 0);
    if (page < 2) {
        y = (s16)(y + 0x16);
        flfntLocate(xt, y);
        font_print(lit_2316, *(int *)(tl_mail_tbl + 8));
        flfntSetSize(0x12, 0x12);
        flfntLocate(xv, y);
        font_print(lit_2316, handle);
        flfntSetSize(0x12, 0x12);
        y = (s16)(y + 0x16);
        flfntLocate(xt, y);
        font_print(lit_2316, *(int *)(tl_mail_tbl + 0xC));
        flfntLocate(xv, y);
        han2zen(id, b1);
        font_print(lit_2316, b1);
        y = (s16)(y + 0x16);
        flfntLocate(xt, y);
        font_print(lit_2316, *(int *)(tl_mail_tbl + 0x1C));
        flfntLocate(xv, y);
        font_print(lit_2316, ((int *)tl_job_tbl)[mini[0]]);
        y = (s16)(y + 0x16);
        flfntLocate(xt, y);
        font_print(lit_2316, *(int *)(tl_mail_tbl + 0x20));
        flfntLocate(xv, y);
        sprintf(b1, lit_2602, mini[1]);
        han2zen(b1, b2);
        font_print(lit_2316, b2);
        flfntLocate((s16)(xt + 0x64), y);
        font_print(lit_2603, ((int *)hunter_appellation)[mini[1]]);
    }
    switch (page) {
    case 0:     /* where: server, plaza, lobby */
        y = (s16)(y + 0x16);
        flfntLocate(xt, y);
        font_print(lit_2316, *(int *)(tl_mail_tbl + 0x24));
        flfntLocate((s16)(xt + 0x50), y);
        if ((n = *(u16 *)((u8 *)cw + 0x30B4)) != 0)
            font_print(lit_193_0065DBE8, Get_ServerName(), (u8 *)PlazaInfo + (n - 1) * 0x15C + 0x14);
        flfntLocate((s16)(xt + 0x50), (s16)(y + 0x16));
        if ((n = *(u16 *)((u8 *)cw + 0x30B6)) != 0) {
            sprintf(b1, lit_193_0065DBE8, Get_ServerName(), (u8 *)LobbyInfo + (n - 1) * 0x15C + 0x14);
            han2zen(b1, b2);
            font_print(lit_2316, b2);
        }
        break;
    case 1:     /* the comment */
        y = (s16)(y + 0x16);
        flfntLocate(xt, y);
        font_print(lit_2316, *(int *)(tl_mail_tbl + 0x28));
        lb_put_comment((s16)(xt + 0x5E), y, comment, 0);
        break;
    case 2:     /* the equipment: weapon, then the armour pieces, each with its icon */
        tabs[0] = (char *const *)D_336C00;
        tabs[1] = (char *const *)D_3371E0;
        tabs[2] = (char *const *)D_337800;
        tabs[3] = (char *const *)D_337E10;
        tabs[4] = (char *const *)D_338360;
        xe = (s16)(x + 0x2C);
        xi = (s16)(xe - 0x20);
        y = (s16)(y + 0x18);
        flfntLocate(xe, y);
        if (mini[9] == 6)
            font_print(lit_2316, *(int *)(D_3351D4 + *(u16 *)(mini + 0xA) * 0x18));
        else
            font_print(lit_2316, *(int *)(D_3367BC + *(u16 *)(mini + 0xA) * 0x14));
        Lb_put_job(xi, (s16)(y - 5), 0x1C, -1, mini[0], 0);
        reload_tex(1, 0x118);
        SetTextureStage(0x118);
        for (k = 0; k < 5; k++) {
            y = (s16)(y + 0x1E);
            flfntLocate(xe, y);
            font_print(lit_2316, *(int *)((u8 *)tabs[k] + mini[tab_field[k]] * 0x14));
            Lb_put_icon(xi, (s16)(y - 5), tab_icon[k], -1);
        }
        break;
    }
    return 0;
}
#endif


void put_member_info(x, y, name, col, info, flag)
int x;
int y;
s8 *name;
int col;
u8 *info;
s8 flag;
{
    char zen[0x10];
    char buf[0x60];
    int c;

    if (*name != 0) {
        han2zen(name, zen);
        if (info != 0) {
            sprintf(buf, lit_2632, zen, *(char **)(lb_num_str + (info[1] / 10) * 4), *(char **)(lb_num_str + (info[1] % 10) * 4));
        } else {
            sprintf(buf, lit_2633, zen);
        }
        flfntSetSize(0x12, 0x12);
        if (flag == 0 && pNet->x0C == 0) {
            font_print_double_k(x, y, 1, 4, col);
            font_print_double_k((s16)((s16)x + 0xA2), y, 1, 4, buf);
            c = 0xFF8080FF;
        } else {
            font_set_palette(0);
            flfntLocate_k(x, y);
            font_print(lit_2316, col);
            flfntLocate_k((s16)((s16)x + 0xA2), y);
            font_print_uf(buf);
            c = -1;
        }
        if (info != 0) {
            Lb_put_icon(0x1FE, y, info[0] + 2, c);
        }
    }
}

#ifdef __MWERKS__
asm int plaza_searchMemberTrans()
{
#include "plaza_searchMemberTrans.inc"
}
#endif

#ifdef __MWERKS__
asm int plaza_mailBoxTrans()
{
#include "plaza_mailBoxTrans.inc"
}
#endif

/* button help line of the plaza menus: which of the four buttons are shown for each menu / sub menu step (near-match) */

void plaza_checkMyStatusTrans() {
    disp_status_k(0xD8, 0x50, CW->x440, CW->x448, my_user_mini_data, *(s8 *)((u8 *)pNet + 0x24), 3, D_3C73B4);
}

#ifdef __MWERKS__
asm int plaza_setMyCommentTrans()
{
#include "plaza_setMyCommentTrans.inc"
}
#endif

void lb_put_comment(x, y, c, f)
int x;
int y;
int c;
s8 f;
{
    if (f != 0) {
        Draw_square((s16)((s16)x - 6), (s16)((s16)y - 2), 0x12C, 0x42, 0xFFAA8820);
        Draw_square((s16)((s16)x - 7), (s16)((s16)y - 3), 0x12E, 0x44, 0xFFAA8820);
    }
    KinshiYogo_chk(c);
    Put_comment(x, (s16)((s16)y - 0x16), 0x16, c);
}

#ifdef __MWERKS__
asm int plaza_setChatModeTrans()
{
#include "plaza_setChatModeTrans.inc"
}
#endif

void put_mail_input_square(a, x, y)
LB_NETW *a;
int x;
int y;
{
    int f;
    s16 *p2;
    s16 *p3;
    u32 *pc;
    int sx;
    int sy;
    s16 *p1;
    s16 q[6];

    f = 0;
    if (a->x0C == 0) {
        u8 t = a->step;
        if (t < 7) {
            if ((*(u8 *)&a->x0A) == 0) {
                f = 1;
            }
        } else {
            switch ((*(u8 *)&a->x0A)) {
            case 0:
                f = 2;
                break;
            case 1:
                f = 1;
                break;
            }
        }
        if (t == 5 || t == 6) {
            if (a->sel == 4) {
                f = 0;
            }
        }
        sx = (s16)x + 0x33;
        sy = (s16)y;
        q[0] = sx;
        p2 = &q[2];
        *p2 = q[0] + 0x151;
        p1 = &q[1];
        *p1 = sy + 0x2A;
        p3 = &q[3];
        *p3 = *p1 + 0x16;
        if (f == 2) {
            pc = (u32 *)&q[4];
            *pc = 0xFFAA8820;
        } else {
            pc = (u32 *)&q[4];
            *pc = 0xFF501515;
        }
        if (a->step > 7) {
            Put_F(q);
        }
        q[0] += 3;
        *p2 -= 3;
        *p1 += 2;
        *p3 -= 2;
        if (f == 2) {
            *pc = 0xFF4A2020;
        } else {
            *pc = 0xFF2A0000;
        }
        if (a->step > 7) {
            Put_F(q);
        }
        q[0] = sx;
        *p2 = q[0] + 0x151;
        *p1 = sy + 0x40;
        *p3 = *p1 + 0x58;
        if (f == 1) {
            *pc = 0xFFAA8820;
        } else {
            *pc = 0xFF501515;
        }
        Put_F(q);
        q[0] += 3;
        *p2 -= 3;
        *p1 += 2;
        *p3 -= 2;
        if (f == 1) {
            *pc = 0xFF4A2020;
        } else {
            *pc = 0xFF2A0000;
        }
        Put_F(q);
        if (((LB_CW *)cw)->x35D5 != 0) {
            if (a->step < 6) {
                if ((*(u8 *)&a->x0A) == 1) {
                    put_main_cursor2_p12(0xD8, 0x38, 7);
                }
            } else if ((*(u8 *)&a->x0A) == 2) {
                put_main_cursor2_p12(0xD8, 0x38, 7);
            }
        } else if (a->step < 6) {
            if ((*(u8 *)&a->x0A) == 1) {
                put_main_cursor_p12(7);
            }
        } else if ((*(u8 *)&a->x0A) == 2) {
            put_main_cursor_p12(7);
        }
    }
}

void plaza_disp_mail(int a, s16 x, s16 y)
{
    char buf[0x80];
    char *p;
    int len;
    int i;

    font_set_palette(0);
    flfntLocate(x, y);
    font_print(lit_2316, *(int *)(tl_mail_tbl + 8));
    flfntLocate(x + 0x3C, y);
    flfntSetSize(0x16, 0x12);
    font_print(lit_2316, (char *)cw + 0x2F88);
    flfntSetSize(0x12, 0x12);
    y += 0x16;
    flfntLocate(x, y);
    font_print(lit_2316, *(int *)(tl_mail_tbl + 0xC));
    flfntLocate(x + 0x3C, y);
    han2zen((char *)cw + 0x2F80, buf);
    font_print(lit_2316, buf);
    flfntLocate(x, y + 0x16);
    font_print(lit_2316, *(int *)(tl_mail_tbl + 0x10));
    font_set_palette(0);
    p = (char *)cw + 0x2F99;
    KinshiYogo_chk(p);
    for (i = 0; i < 4; i++) {
        if (p == 0) break;
        len = strlen(p);
        strcpy(buf, p);
        if (len > 0x23) {
            if (Ck_hankaku(buf, 0x23) == 0) {
                buf[0x24] = 0;
                p += 0x24;
            } else {
                buf[0x23] = 0;
                p += 0x23;
            }
            y += 0x16;
            flfntLocate(x + 0x3C, y);
            font_print(lit_2316, buf);
        } else {
            flfntLocate(x + 0x3C, y + 0x16);
            font_print(lit_2316, buf);
            break;
        }
    }
    font_set_palette(0);
}

/* button help line of the plaza menus: which of the four buttons are shown for each menu / sub menu step (near-match) */

void plaza_chatTrans() {
    s16 idx;

    switch (CW->chatmode) {
    case 0:
        Put_megaphone(0x1F6, 0x32, 3);
        idx = 0;
        break;
    case 1:
        Put_megaphone(0x1F6, 0x32, 1);
        idx = 1;
        break;
    default:
        Put_megaphone(0x1F6, 0x32, 2);
        idx = 2;
        break;
    }
    flfntSetSize(0x16, 0x16);
    font_print_double_k(0x22E, 0x33, 1, 0, tl_etc[3 + idx]);
}

#ifdef __MWERKS__
asm void plaza_trans_ot0()
{
#include "plaza_trans_ot0.inc"
}
#else
/* PC build (ONLINE=1): agent C's near-match (wip/plaza_trans_ot0_tu_2diff.c: one register differs from 0x598... asm) */
extern u8 D_3A27D6[];
extern char lit_3241[];
typedef struct { u8 p[0xF0]; s16 x; s16 y; } TXF;
void plaza_trans_ot0(a)
LB_NETW *a;
{
    int sp60[3] = {0, 0x01C00280, 0xB0101010};
    u32 col = 0xFF602020;
    char sp40[0x20];
    char sp30[0x10];
    int sp20[4] = {0x005600D8, 0x0D1B120F, 0x80000000, 0xFF2A0000};
    TXF *t;
    u8 *p;

    font_set_stack_no(*(int *)((u8 *)a + 0x18));
    if (pNet->depth > 1) {
        Put_F(sp60);
    }
    reload_tex(1, 0x157);
    SetTextureStage(0x157);
    SetFilterMode(1);
    flSetRenderState(0x60, 0);
    Put_2TF(textLobbyTbl + 0x14);
    DispSceneTitle();
    DispHelpLine();
    p = &pNet->sel;
    if (pNet->sel == 0xE || pNet->sel == 0xC) {
        col = 0xFF606025;
        sp20[3] = 0xCC151200;
    }
    switch (*p) {
    case 0:
    case 8:
    case 3:
    case 4:
        break;
    default:
        DispFrameMessage(sp20, 0);
        Draw_square(0xD8, 0x76, 0x192, 1, col);
        Draw_square(0xD8, 0x13C, 0x192, 1, col);
        break;
    }
    switch (pNet->sel) {
    case 12:
    case 14:
    default:
        SetSceneSubTitleColor(0xCC151200);
        plaza_chatTrans(pNet);
        Plaza_disp_chat();
        break;
    case 0:
        plaza_enterLobbyTrans(0xD8, 0x50);
        break;
    case 1:
        plaza_movePlazaTrans();
        break;
    case 2:
        break;
    case 3:
        plaza_checkFriendTrans(0xD8, 0x50, 0);
        break;
    case 5:
    case 6:
        plaza_searchMemberTrans();
        break;
    case 4:
        plaza_mailBoxTrans(0xD8, 0x50, 0);
        break;
    case 7:
        plaza_checkMyStatusTrans();
        break;
    case 8:
        plaza_setMyCommentTrans(0xD8, 0x50, 0);
        break;
    case 9:
        DispFrameMessage(sp20, 0);
        Draw_square(0xD8, 0x76, 0x192, 1, col);
        Draw_square(0xD8, 0x13C, 0x192, 1, col);
        plaza_setChatModeTrans();
        plaza_chatTrans(pNet);
        break;
    case 10:
        Plaza_disp_ReibunEdit();
        break;
    case 11:
        Plaza_disp_chatlog();
        break;
    case 13:
        break;
    }
    DispSceneSubTitle();
    DispButtonHelp(pNet);
    flfntSetSize(0x12, 0x12);
    t = (TXF *)text_lobby_msg[2];
    Lb_put_msg_type2(&t->x);
    sprintf(sp40, lit_3241, *(u16 *)(D_3A27D6 + ClassInfo.plaza * 0x15C));
    han2zen(sp40, sp30);
    font_set_palette(0);
    flfntLocate(t->x + 0x48, t->y);
    font_print(lit_2316, sp30);
}
#endif

void Lb_on_dialog() {
    LB_NETW *n;
    u8 *r;

    if (CW->x35D5 == 0 || lb_sys.x68 == 0xF) {
        n = pNet;
        switch (n->sel) {
        case 5:
            if (n->x05 == 1) {
                r = SearchResult + ((u8)n->x0A + n->x24 * 7) * 0x5C;
                DispNameAndIDonDialog(0x98, (char *)(r + 0xC), (char *)(r + 4));
                return;
            }
            break;
        case 6:
            if (n->step == 8 && n->x05 == 1) {
                r = SearchResult + ((u8)n->x0A + n->x24 * 7) * 0x5C;
                DispNameAndIDonDialog(0x98, (char *)(r + 0xC), (char *)(r + 4));
                return;
            }
            break;
        case 3:
            if (n->step == 10) {
                r = Friend_data + ((u8)n->x0A + n->x24 * 7) * 0x30;
                DispNameAndIDonDialog(0x98, (char *)(r + 8), (char *)r);
                return;
            }
            if (n->step == 13) {
                DispNameAndIDonDialog(0x98, (char *)(cw + 0x2F88), (char *)(cw + 0x2F80));
            }
            break;
        }
    }
}

/* button help line of the plaza menus: which of the four buttons are shown for each menu / sub menu step (near-match) */

void plaza_trans_ot1(a)
u8 *a;
{
    font_set_stack_no(*(int *)(a + 0x18));
    if (SoftKeyboard_alive_check() != 0) {
        DispSoftkeyboard(1);
    }
    if (pNet->x0C == 1) {
        DispDialogData(pNet->x0C);
        Lb_on_dialog();
        pNet->x0C = 0;
    }
}

void put_main_cursor(int n) {
    R12 a = lit_3380;
    R12 b = lit_3382;

    if (((u8 *)pNet)[0xC] == 0) {
        a.v[1] = n * 0x16 + 0x8C;
        a.v[3] = a.v[1] + 0x16;
        b.v[1] = a.v[1] - 2;
        b.v[3] = a.v[3] + 2;
        Put_F(&b, &b, &a.v[1], &a);
        Put_F(&a);
    }
}

void put_main_cursor2(int x, int y, s32 n) {
    R12 a = lit_3397;
    s16 t = x;
    R12 b;

    a.v[0] = t + 4;
    a.v[2] = t + 0x190;
    b = lit_3399;
    b.v[0] = t + 1;
    b.v[2] = t + 0x193;
    if (((u8 *)pNet)[0xC] == 0) {
        a.v[1] = (s16)y + 0x3C + n * 0x16;
        a.v[3] = a.v[1] + 0x16;
        b.v[1] = a.v[1] - 2;
        b.v[3] = a.v[3] + 2;
        Put_F(&b, &a.v[3], n, &a.v[1]);
        Put_F(&a);
    }
}

/* button help line of the plaza menus: which of the four buttons are shown for each menu / sub menu step (near-match) */

void Get_PlazaName(dst)
char *dst;
{
    sprintf(dst, lit_193_0065DBE8, Get_ServerName(), PlazaInfo[ClassInfo.plaza - 1].name);
}

void Get_LobbyName(dst)
char *dst;
{
    sprintf(dst, lit_193_0065DBE8, Get_ServerName(), LobbyInfo[ClassInfo.lobby - 1].name);
}

void Lbs_load() {
    load_pit();
    load_texlist(*(int *)0x3876A8, 0x14D, 0);
}

void Lbc_release() {
    release_texture(0x118, 0x15);
}

