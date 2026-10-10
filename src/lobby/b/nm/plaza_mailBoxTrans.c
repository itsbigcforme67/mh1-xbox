/* plaza_mailBoxTrans (lobby.bin 0x0059BD50-0x0059C2E8, raw on the PS2: config/c_rawfuncs.txt): the short mail box
 * window. Rewritten for the PC from the asm (agent B, 10 Oct 2026; not compared with check.py): the earlier int-mode
 * draft computed every coordinate with 64-bit shifts ((x << 0x30) >> 0x30, 0 in 32-bit C) and passed no string to
 * font_print_double. RecvMailInfo: 8 x 0x9A (+0 unread, +1 id[8], +9 handle[0x11], +0x1A text[0x80]). pNet: +3 the
 * box's step, +0xA the cursor, +0xE the mail shown, +0x26 the number of mails (counted here). */
#include "lobby_a.h"
extern char RecvMailInfo[];
extern char tl_msg_tbl[];
extern char tl_mail_tbl[];
extern char lit_2316[];
void plaza_mailBoxTrans(int arg0, int arg1, int arg2) {
    char zen[0x20];
    u8 *m = (u8 *)RecvMailInfo, *use;
    char **num = (char **)lb_num_str;
    s16 x = (s16)arg0, y = (s16)arg1, xt, yt, i;

    F(s16, pNet, 0x26) = 0;
    for (i = 0; i < 8 && m[i * 0x9A + 1] != 0; i++)
        F(s16, pNet, 0x26) = (s16)(F(s16, pNet, 0x26) + 1);
    if (F(u8, pNet, 3) == 0)
        return;
    switch ((s8)arg2) {
    case 0:
        put_mainWindow(arg0, arg1);
        break;
    case 1:
        put_mainWindowTex(arg0, arg1);
        break;
    }
    font_set_palette(0);
    flfntSetSize(0x12, 0x12);
    xt = (s16)(x + 0xA);
    yt = (s16)(y + 0x28);
    if (F(u8, pNet, 3) < 2) {
        if (F(s16, pNet, 0x26) == 0) {     /* no mail */
            put_titles(xt, yt, F(s32, &tl_msg_tbl, 0xC));
            flfntSetSize(0x14, 0x14);
            font_print_double((s16)(xt + 0x46), (s16)(yt + 0x3C), 1, 4, F(s32, &tl_msg_tbl, 0x10));
            return;
        }
        put_main_cursor2(arg0, arg1, F(u8, pNet, 0xA));
        put_titles(xt, yt, F(s32, &tl_msg_tbl, 0xC));
        y = (s16)(y + 0x3E);
        for (i = 0, use = m; i < F(s16, pNet, 0x26) && use[1] != 0; i++, use += 0x9A, y = (s16)(y + 0x16)) {
            /* the line: number, handle, id (full width), the unread / read icon */
            if (F(u8, pNet, 0xA) == i) {
                font_print_double((s16)(xt + 9), y, 1, 4, (int)num[i + 1]);
                flfntSetSize(0x16, 0x12);
                font_print_double((s16)(xt + 0x78), y, 1, 4, (int)(use + 9));
                flfntSetSize(0x12, 0x12);
                han2zen(use + 1, zen);
                font_print_double((s16)(xt + 0x104), y, 1, 4, (int)zen);
            } else {
                font_set_palette(0);
                flfntLocate((s16)(xt + 9), y);
                font_print(lit_2316, num[i + 1]);
                flfntSetSize(0x16, 0x12);
                flfntLocate((s16)(xt + 0x78), y);
                font_print(lit_2316, use + 9);
                flfntSetSize(0x12, 0x12);
                flfntLocate((s16)(xt + 0x104), y);
                han2zen(use + 1, zen);
                font_print(lit_2316, zen);
            }
            Lb_put_icon((s16)(xt + 0x40), y, use[0] == 0 ? 0xA : 9, -1);
        }
        return;
    }
    if (F(u8, pNet, 3) == 2) {     /* reading a mail: title, "No.", its number, the mail */
        put_titles(xt, yt, F(s32, &tl_mail_tbl, 0));
        put_titles((s16)(xt + 0x12C), yt, F(s32, &tl_mail_tbl, 4));
        put_titles((s16)(xt + 0x168), yt, (int)num[F(u8, pNet, 0xE) + 1]);
        plaza_disp_mail(pNet, xt, (s16)(yt + 0x16));
        return;
    }
    /* writing a reply */
    put_titles(xt, yt, F(s32, &tl_mail_tbl, 0x14));
    plaza_disp_mail(pNet, xt, (s16)(yt + 0x16));
    put_mail_input_square(pNet, xt, yt);
    font_set_palette(F(s8, (u8 *)cw, 0x2F99) != 0 ? 0 : 0xA);
    flfntLocate(xt, (s16)(yt + 0xB0));
    font_print(lit_2316, F(int, &tl_mail_tbl, 0x18));
}
