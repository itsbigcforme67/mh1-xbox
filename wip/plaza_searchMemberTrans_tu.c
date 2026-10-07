/* near-miss of plaza_searchMemberTrans rewritten from the asm, 40/330 instructions differ (round 13). Meant to be pasted into the lb_plz3 TU in place of the asm stub (cut-file harness: TU prefix + this text). _p3 prototypes need alias lines (name_p3 = function address) in config/lobby_aliases.txt. Only register allocation / a few narrowings differ. */
extern char seekStr[];
extern char tl_hr_tbl[];
extern char tl_msg_tbl[];
void put_member_info_p3(s16, s16, void *, void *, void *, s8);
void plaza_searchMemberTrans(a)
LB_NETW *a;
{
    char sp60[0x40];
    LB_TXT *t;
    u8 *r;

    t = text_lobby_msg[2];
    if (a->step == 10) {
        r = SearchResult + ((u8)a->x0A + a->x24 * 7) * 0x5C + 4;
        disp_status_k(0xD8, 0x50, r, r + 8, r + 28, a->x12, 3, (char *)cw + 0x2B9C);
        return;
    }
    switch (a->step) {
    case 6:
    case 8:
    case 9:
    case 11: {
        s16 x;
        s16 y;
        int i;
        put_titles2(t + 31);
        y = t[31].y + 22;
        x = t[31].x;
        if (SearchResult != 0) {
            put_main_cursor((u8)a->x0A);
            i = 0;
            do {
                int idx = i + a->x24 * 7;
                r = SearchResult + idx * 0x5C + 4;
                if (idx < *SearchResult) {
                    put_member_info_p3(x, y, r, r + 8, r + 28, i != (u8)a->x0A);
                }
                y += 22;
                i++;
            } while (i < 7);
            Put_page_num_p5((s16)x + 300, y, a->x24, a->x26, 0);
        } else {
            flfntSetSize(20, 20);
            font_print_double_p5((s16)x + 70, y + 60, 1, 4, *(char **)(tl_msg_tbl + 0x14));
        }
        break;
    }
    default: {
        s16 y;
        int i;
        int c;
        int len;
        if (a->sel != 5) {
            if (a->x0C == 0) {
                put_main_cursor((u8)a->x0A + 1);
            }
            t += a->x04 + 33;
            put_titles2(t);
            y = t->y + 44;
            font_set_palette(0);
            Lb_put_msg_type2(t + 4);
            switch (a->x04) {
            case 0:
                if (a->step == 3) {
                    len = strlen(seekStr);
                    flfntLocate(432 - (u32)(len * 9) / 2, y);
                    font_print_uf(seekStr);
                }
                break;
            case 1:
                if (a->step == 3) {
                    flfntLocate(379, y);
                    han2zen(seekStr, sp60);
                    font_print(lit_2316, sp60);
                }
                break;
            case 2:
                i = 0;
                do {
                    if (a->step == 3) {
                        if ((u8)a->x06 == i) {
                            font_set_palette(4);
                            c = 0xFF8080FF;
                        } else {
                            font_set_palette(10);
                            c = 0xFF707070;
                        }
                    } else {
                        font_set_palette(0);
                        c = -1;
                    }
                    flfntLocate(324, y);
                    font_print(lit_2316, ((char **)tl_job_tbl)[i]);
                    Lb_put_icon_p7(300, y - 2, i + 2, c);
                    y += 22;
                    i++;
                } while (i < 5);
                break;
            case 3:
                i = 0;
                do {
                    if (a->step == 3) {
                        if ((u8)a->x06 == i) {
                            font_set_palette(4);
                        } else {
                            font_set_palette(10);
                        }
                    } else {
                        font_set_palette(0);
                    }
                    flfntLocate(324, y);
                    font_print(lit_2316, ((char **)tl_hr_tbl)[i]);
                    y += 22;
                    i++;
                } while (i < 5);
                break;
            }
            Put_page_num_p5(500, 120, a->x04, 4, 0);
        }
        break;
    }
    }
    switch (a->step) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
        if (a->step < 3) {
            font_set_palette(10);
        } else {
            font_set_palette(0);
        }
        Lb_put_msg_type2(text_lobby_msg[2] + 41);
        break;
    }
}
