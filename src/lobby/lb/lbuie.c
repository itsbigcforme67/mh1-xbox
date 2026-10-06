/* lbui, run 5: LBDisp_NowLoading2 .. plaza_selectMenu (lobby.bin 0x00593CB0-0x00594260): the matching functions of lbui_nm.c. */
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
