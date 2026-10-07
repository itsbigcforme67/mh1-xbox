/* disp_savesel 0x280EF0 (712 bytes): 26-35 of 178 instructions differ; same body as waku+moji merged in one loop. Uses SFRM etc. from src/main/mc/savesel.c. Only the scheduling of the `s = option_w + 0x10; y = 90;` setup differs. */
void disp_savesel(u8 *e, int a, u8 c) {
    SFRM fr;
    s16 i;
    s16 y;
    s16 col;
    u8 *s = option_w + 0x10;

    flfntSetSize(20, 20);
    fr.x = 50;
    fr.y = 88;
    fr.colw = 20;
    fr.h = 20;
    fr.cols = 12;
    fr.rows = 3;
    fr.pal = 5;
    fr.col = 0xC0402010;
    if (a == 2) {
        fr.mode = 2;
    } else {
        fr.mode = 4;
    }
    y = 90;
    for (i = 0; i < 3; i++) {
        if (i == e[1]) {
            col = 5;
            if (a == 0 || a == 2 || c >= 2) {
                Sel_csr_disp(fr.x + 115, fr.y - 3, 280, 70, 0xFF20C0C0);
            }
        } else {
            col = 0;
        }
        DispFrameMessageA(&fr, 0, 128);
        if (s[0] == 0) {
            flfntSetSize(24, 24);
            font_print_ex(122, y + 18, col, lit_159_00384EA8, i + 1);
        } else {
            u32 t;
            flfntSetSize(24, 24);
            font_print_ex(60, y, col, lit_160_00384EB0, i + 1, s + 8);
            flfntSetSize(18, 18);
            font_print_ex(60, y + 24, col, lit_161_00384EB8, sex_char_tbl[s[1] + 2]);
            t = *(u32 *)(s + 884);
            font_print_ex(60, y + 42, col, lit_162_00384ED0, t / 3600, t % 3600 / 60);
        }
        y += 88;
        s += 1152;
        fr.y += 88;
    }
}
