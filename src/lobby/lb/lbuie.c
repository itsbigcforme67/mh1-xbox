/* lbui, run 5: LBDisp_NowLoading2 .. tl_menu_cursor_down (lobby.bin 0x00593CB0-0x00593F78): the matching functions of lbui_nm.c. */
#pragma readonly_strings on
#include "lbui_proto.h"

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
        sprintf(t->s, "%s%s", Get_ServerName(), PlazaInfo[ClassInfo.plaza - 1].name);
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
