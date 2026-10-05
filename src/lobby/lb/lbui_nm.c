/* lbui - lobby.bin 0x00590D40-0x0059DB40: plaza / lobby UI (eat scene, dialogs, plaza menus, chat, mail, friends).
 * Near-match file in address order (tools/lbmerge.py ... include/lbui_proto.h lbui.h). */
#pragma readonly_strings on
#include "lbui_proto.h"

void Lb_eat_to_bell(void) {
    LBS8(6) = 1;
}

void Lb_eat_to_rcpt(void) {
    LBS8(6) = 3;
}

void Lb_eat_to_eat(void) {
    LBS8(6) = 4;
}

void Lb_eat_to_end(void) {
    LBS8(6) = 5;
}

void event_eat_trans_ot1(a)
u8 *a;
{
    font_set_stack_no(*(int *)(a + 0x18));
}

void SetDialogData_HTML(arg0)
int arg0;
{
    htmlStr = arg0;
    dialogData.html = 1;
    set_dialog_square(0x1F4, 0x17C);
}

void SetDialogYesNo(v)
s8 v;
{
    dialogData.yesno = v;
    pNet->yesno = v;
}

void SetSceneTitle(a, b)
int a;
int b;
{
    pSceneTitle = (int)text_lobby_msg[a];
    pSceneTitle = pSceneTitle + b * 8;
}

void SetSceneSubTitle(a, b, c)
int a;
int b;
char *c;
{
    subTitleCol = 0xFF2A0000;
    pSceneSubTitle = text_lobby_msg[a];
    pSceneSubTitle = pSceneSubTitle + b * 8;
    strcpy(*(char **)(pSceneSubTitle + 4), c);
}

void SetSceneSubTitleColor(c)
int c;
{
    subTitleCol = c;
}

void SetHelpLineMsg(a, b)
int a;
int b;
{
    *(u8 **)helpLineStr = text_lobby_msg[a];
    *(int *)(helpLineStr + 4) = 0;
    *(int *)(helpLineStr + 8) = 0;
    *(u8 **)helpLineStr = *(u8 **)helpLineStr + b * 8;
}

void tl_menu_cursor_up(m)
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

void tl_menu_cursor_down(m)
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

int getUserInfo(void) {
    switch (Lbs_SeekId()) {
    case 0:
        Lbc_RequestNetComment(cw + 0x2F80);
        return 0;
    case 1:
        return 1;
    default:
        return 2;
    }
}

int Lb_get_comment(a)
int a;
{
    int id = Lb_get_plID() & 0xFF;

    if (id != 0xFF) {
        memset(cw + id * 0x62 + 0x288C, 0, 0x62);
        Lbc_RequestNetComment(a);
        return 1;
    }
    return 0;
}

int Lb_checkChatID(id)
u8 *id;
{
    s8 i;
    u8 *p = (u8 *)chatIDList;

    for (i = 0; ; ) {
        if (memcmp(p, id, 8) == 0) {
            return 1;
        }
        i++;
        p += 8;
        if (i >= 7) {
            return 0;
        }
    }
}

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

void Lb_clearChatList(void) {
    s8 i = 0;
    u8 *a = (u8 *)chatIDList;
    u8 *b = (u8 *)chatHandleList;

    cw[0x32BE] = 0;
    do {
        memset(a, 0, 8);
        memset(b, 0, 0x10);
        i++;
        a += 8;
        b += 0x10;
    } while (i < 7);
}

void plaza_ReibunEdit(void) {
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

void plaza_checkChatLog(void) {
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
                font_print_double(0x10, y, 1, 0, p + 4);
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
    font_print_double(a, b, 1, 5, c);
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

int Lb_get_cursor_col(void) {
    f32 a = 0.0000958738f * (f32)(u32)(u16)((System_timer & 0x3F) << 10);

    return (((s8)(int)(80.0f * flSin(a)) + 0x9F) << 24) | 0xFF00;
}

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

void Get_PlazaName(dst)
char *dst;
{
    sprintf(dst, "%s%s", Get_ServerName(), PlazaInfo[ClassInfo.plaza - 1].name);
}

void Get_LobbyName(dst)
char *dst;
{
    sprintf(dst, "%s%s", Get_ServerName(), LobbyInfo[ClassInfo.lobby - 1].name);
}

void Lbs_load(void) {
    load_pit();
    load_texlist(*(int *)0x3876A8, 0x14D, 0);
}

void Lbc_release(void) {
    release_texture(0x118, 0x15);
}
