/* near-miss of plaza_checkFriendTrans rewritten from the asm, 176/259 instructions differ (round 13). Meant to be pasted into the lb_plz3 TU in place of the asm stub (cut-file harness: TU prefix + this text). _p3 prototypes need alias lines (name_p3 = function address) in config/lobby_aliases.txt. Only register allocation / a few narrowings differ. */
extern char lit_2528[];
extern char tl_msg_tbl[];
extern char tl_member_buff[];
void plaza_disp_mail(int, s16, s16);
void put_titles_p3(s16, s16, int);
void put_member_info_p3(s16, s16, void *, void *, void *, s8);
void put_mail_input_square_p3(LB_NETW *, s16, s16);
void plaza_checkFriendTrans(int x, int y, s8 mode) {
    LB_NETW *n;
    char *q;
    int i;
    int bx;
    s16 by;
    char *m;

    i = 0;
    q = (char *)Friend_data + pNet->x24 * 0x150;
    switch (mode) {
    case 0:
        put_mainWindow();
        break;
    case 1:
        put_mainWindowTex();
        break;
    }
    if (net_Check_FriendSuu(Friend_data, 0x32) == 0) {
        flfntSetSize(0x14, 0x14);
        font_print_double_p5((s16)x + 0x82, (s16)y + 0x64, 1, 4, lit_2528);
        return;
    }
    n = pNet;
    if (n->step < 4 || n->step == 13 || (u32)(n->step - 9) < 3 || n->step == 14) {
        put_main_cursor2_p12(x, y, (u8)n->x0A);
        bx = (s16)x + 10;
        put_titles_p3(bx, (s16)y + 0x28, *(int *)(tl_msg_tbl + 4));
        y = (s16)(y + 0x3E);
        m = tl_member_buff;
        do {
            int f = i != (u8)pNet->x0A;
            int idx = i + pNet->x24 * 7;
            if (idx < 50 && *(s8 *)(Friend_data + idx * 0x30) != 0) {
                if (m[0x280] != 0) {
                    put_member_info_p3((s16)x + 10, y, q, q + 8, m + 0x29A, f);
                } else {
                    put_member_info_p3((s16)x + 10, y, q, q + 8, 0, f);
                }
            }
            i++;
            y = (s16)(y + 0x16);
            m += 0x2FC;
            q += 0x30;
        } while (i < 7);
        Put_page_num_p5((s16)bx + 0x12C, y, pNet->x24, pNet->x26, 0);
    } else if (n->step == 4) {
        char *r = (char *)Friend_data + (n->x06 + n->x24 * 7) * 0x30;
        disp_status_k(x, y, r, r + 8, tl_member_buff + n->x06 * 0x2FC + 0x29A, n->x12, 3, (char *)cw + 0x2B9C);
    } else {
        bx = (s16)x + 10;
        by = (s16)y + 0x3E;
        put_titles_p3(bx, by - 0x16, *(int *)(tl_mail_tbl + 0x14));
        plaza_disp_mail((int)pNet, bx, by);
        put_mail_input_square_p3(pNet, bx, by - 0x16);
        if (*(s8 *)((u8 *)cw + 0x2F99) != 0) font_set_palette(0); else font_set_palette(10);
        flfntLocate(bx, by + 0x9A);
        font_print(lit_2316, *(int *)(tl_mail_tbl + 0x18));
    }
}
