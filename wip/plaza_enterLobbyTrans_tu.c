/* near-miss of plaza_enterLobbyTrans rewritten from the asm, 246/318 instructions differ (round 13). Meant to be pasted into the lb_plz3 TU in place of the asm stub (cut-file harness: TU prefix + this text). _p3 prototypes need alias lines (name_p3 = function address) in config/lobby_aliases.txt. Only register allocation / a few narrowings differ. */
extern char lit_2315[];
extern char lit_2317[];
extern char tl_msg_tbl[];
extern char tl_member_buff[];
void put_titles_p3(s16, s16, int);
void put_member_info_p3(s16, s16, void *, void *, void *, s8);
void plaza_enterLobbyTrans(int x, int y) {
    char b2[0x40];
    char b1[0x40];
    int i;
    int pg;
    int a;
    int c;
    LB_PINFO *q;
    int v;
    char *m;

    i = 0;
    m = tl_member_buff;
    put_mainWindow();
    flfntSetSize(0x12, 0x12);
    if (pNet->step != 2 && pNet->step != 3) {
        put_main_cursor2_p12(x, y, (u8)pNet->x0A % 7);
        put_titles_p3((s16)x + 10, (s16)y + 0x28, *(int *)tl_msg_tbl);
        y = (s16)(y + 0x3E);
        pg = (u8)pNet->x0A / 7 * 7;
        q = LobbyInfo + pg;
        v = (s16)x + 10;
        do {
            if (pg < *(u16 *)0x6DD7D2) {
                c = *(s16 *)((u8 *)q + 14);
                a = (s16)(*(s16 *)((u8 *)q + 2) - c);
                if (a < 0) a = 0;
                sprintf(b1, lit_193_0065DBE8, Get_ServerName(), q->name);
                sprintf(b2, lit_2315, ((char **)lb_num_str)[a], ((char **)lb_num_str)[c]);
                Lb_put_icon_p7((s16)v + 202, y, 7, -1);
                Lb_put_icon_p7((s16)v + 309, y, 8, -1);
                if (i == (u8)pNet->x0A % 7) {
                    font_print_double_p5(v, y, 1, 4, b1);
                    font_print_double_p5((s16)v + 232, y, 1, 4, b2);
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
                    font_print_double_p5(v, y, 1, 4, b2);
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
        Put_page_num_p5((s16)x + 316, y, (u8)pNet->x0A / 7, 2, 0);
    } else {
        put_titles_p3((s16)x + 10, (s16)y + 0x28, *(int *)(tl_msg_tbl + 4));
        y = (s16)(y + 0x3E);
        if (pNet->x06 == 0) {
            flfntSetSize(0x16, 0x12);
            font_set_palette(4);
            font_print_double_p5((s16)x + 10 + 70, y + 60, 1, 4, *(char **)(tl_msg_tbl + 8));
        } else {
            for (i = 0; i < 8; i++) {
                put_member_info_p3((s16)x + 10, y, m + 640, m + 648, m + 666, 1);
                y = (s16)(y + 22);
                m += 764;
            }
        }
    }
}
