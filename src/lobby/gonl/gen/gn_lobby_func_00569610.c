/* G online (PC): lobby_func_00569610, the plaza menu's move (G lobby 0x569610; HAND-FIXED: written by hand from G's
 * code, m2c lost a register across a call). Two pages of entries (pNet+0x29 = page, +0x2A = cursor; G's table of the
 * two pages' entry lists at gp-15616, 4 bytes an entry: kind (1 = a heading, skipped), help message, s16 text);
 * up / down move, left / right turn the page, circle starts the entry (pNet+0x28 = its action, a jump table), triangle
 * (or the keyboard) opens the chat, R2 swaps names / ids. */
#include "../gonl.h"

extern u8 *pNet;
extern u8 g_main_2EEE70[];      /* G gp-15616: the two pages' entry lists (pointers) */
extern u8 *tl_etc;              /* G 0x636E70: a text the menu shows (the help line's first) */
extern u8 lb_ibox_str[];        /* G 0x6364C0: string pointers, by the entry's s16 */
extern u8 g_lb_5FBCC0[];        /* G 0x5FBCC0: s16 help message per action (-1 none) */
extern u8 ClassInfo;
extern u8 PlazaInfo[];
extern u8 g_main_2EF390[];      /* G gp-14304: the network info area (the info file at +0x2414) */
extern u8 Psw[];
s32 Get_sw2();
s32 Name_ID_change();
s32 Plaza_chat_init();
s32 SetHelpLineMsg();
s32 cnWrap_SoundRequest();
s32 func_0012D080();
s32 lobby_func_00569D10();
s32 lobby_func_0056AFF0();
s32 lobby_func_0056B4B0();
s32 lobby_func_0056B730();
s32 lobby_func_0056C480();
s32 lobby_func_0056ED00();
s32 lobby_func_0056FF80();
s32 lobby_func_00571AF0();
s32 lobby_func_0059F6C0();
s32 lobby_func_0059F740();
s32 plaza_moveMain();
s32 plaza_setMyComment();
s32 tl_menu_cursor_down();
s32 tl_menu_cursor_up();

void lobby_func_00569610(void)
{
    u8 *list = ((u8 **)g_main_2EEE70)[pNet[0x29]];
    u16 sw = (u16)Get_sw2(0);
    u8 *info;
    *(s16 *)(pNet + 0x20) = sw;
    lobby_func_0059F6C0(tl_etc);
    pNet[0x28] = 16;
    if (sw & 0x2000) {
        tl_menu_cursor_up();
        SetHelpLineMsg(list[pNet[0x2A] * 4 + 1] + 2);
        cnWrap_SoundRequest(1);
        return;
    }
    if (sw & 0x1000) {
        tl_menu_cursor_down();
        SetHelpLineMsg(list[pNet[0x2A] * 4 + 1] + 2);
        cnWrap_SoundRequest(1);
        return;
    }
    if (sw & 0xC00) {
        u8 *l2;
        pNet[0x29] ^= 1;
        l2 = ((u8 **)g_main_2EEE70)[pNet[0x29]];
        if (l2[pNet[0x2A] * 4] != 1)
            tl_menu_cursor_up();
        SetHelpLineMsg(l2[pNet[0x2A] * 4 + 1] + 2);
        cnWrap_SoundRequest(1);
        return;
    }
    if (sw & 0x20) {
        s16 h;
        pNet[0x28] = list[pNet[0x2A] * 4 + 1];
        pNet[0x22]++;
        if (pNet[0x28] != 12 && pNet[0x28] != 15)
            lobby_func_0059F6C0(((u8 **)lb_ibox_str)[*(s16 *)(list + pNet[0x2A] * 4 + 2)]);
        cnWrap_SoundRequest(0);
        h = ((s16 *)g_lb_5FBCC0)[pNet[0x28]];
        if (h != -1)
            SetHelpLineMsg(h);
        switch (pNet[0x28]) {
        case 0:
            lobby_func_00569D10();
            info = *(u8 **)g_main_2EF390 + 0x23F4 + 0x20;
            if ((s8)PlazaInfo[(ClassInfo - 1) * 348 + 91] == '1')
                lobby_func_0059F740(info + 1928);
            else
                lobby_func_0059F740(info + 2184);
            break;
        case 1:
            lobby_func_0056AFF0();
            break;
        case 2: case 7: case 11:
            plaza_moveMain();
            break;
        case 3: case 4:
            lobby_func_00571AF0();
            lobby_func_0056C480();
            break;
        case 5:
            lobby_func_0056ED00();
            lobby_func_0056C480();
            break;
        case 6:
            lobby_func_0056FF80();
            break;
        case 8:
            plaza_setMyComment(0);
            break;
        case 14:
            lobby_func_0056B730();
            break;
        case 15:
            lobby_func_0056B4B0();
            break;
        default:
            break;
        }
        *(s16 *)(pNet + 0x20) = 0;
        return;
    }
    if ((*(u16 *)(Psw + 4) & 0x100) || func_0012D080() == 1) {
        pNet[0x28] = 16;
        cnWrap_SoundRequest(14);
        pNet[0x22]++;
        Plaza_chat_init();
        return;
    }
    if (sw & 0x200)
        Name_ID_change();
}
