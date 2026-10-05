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
