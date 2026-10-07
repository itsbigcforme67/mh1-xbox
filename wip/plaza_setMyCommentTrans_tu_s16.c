#define CFS(T, p, o) (*(T *)((u8 *)(p) + (o)))
extern char lit_2316[];
extern char lit_2602[];
extern u8 D_3C738C[];
void put_titles_s(s16, s16, int);
void lb_put_comment_s(s16, s16, u8 *, int);
void put_mainWindow_s(s16, s16);
void put_mainWindowTex_s(s16, s16);
void plaza_setMyCommentTrans(s16 x, s16 y, s8 mode)
{
    char b70[0x20];
    char b50[0x20];
    int job;
    int x2;

    flfntSetSize(0x12, 0x12);
    switch (mode) {
    case 0:
        put_mainWindow_s(x, y);
        break;
    case 1:
        put_mainWindowTex_s(x, y);
        break;
    }
    y = (s16)(y + 0x28);
    put_titles_s(x + 10, y, ((int *)tl_mail_tbl)[0x2C / 4]);
    font_set_palette(0);
    y = (s16)(y + 0x16);
    flfntLocate(x + 10, y);
    font_print(lit_2316, ((int *)tl_mail_tbl)[2]);
    x2 = (s16)(x + 10) + 0x36;
    flfntLocate(x2, y);
    font_print(lit_2316, (int)cw + 0x448);
    y = (s16)(y + 0x16);
    flfntLocate(x + 10, y);
    font_print(lit_2316, ((int *)tl_mail_tbl)[3]);
    flfntLocate(x2, y);
    memcpy(b70, cw + 0x440, 8);
    han2zen(b70, b50);
    font_print(lit_2316, b50);
    job = Get_weapon_job(D_3C738C) & 0xFF;
    if (job == 5) {
        job = 1;
    }
    y = (s16)(y + 0x16);
    flfntLocate(x + 10, y);
    font_print(lit_2316, ((int *)tl_mail_tbl)[7]);
    x2 = (s16)(x + 10) + 0x36;
    flfntLocate(x2, y);
    font_print(lit_2316, ((int *)tl_job_tbl)[job & 0xFF]);
    y = (s16)(y + 0x16);
    flfntLocate(x + 10, y);
    font_print(lit_2316, ((int *)tl_mail_tbl)[8]);
    flfntLocate(x2, y);
    sprintf(b70, lit_2602, *(u8 *)0x3C733B);
    han2zen(b70, b50);
    font_print(lit_2316, b50);
    y = (s16)(y + 0x16);
    flfntLocate(x + 10, y);
    font_print(lit_2316, ((int *)tl_mail_tbl)[10]);
    lb_put_comment_s(x + 0x68, y - 2, D_3C73B4, 1);
}
