#define CFS(T, p, o) (*(T *)((u8 *)(p) + (o)))
extern char lit_2316[];
extern char lit_2602[];
extern u8 D_3C738C[];
void put_titles_s(s16, s16, int);
void lb_put_comment_s(s16, s16, u8 *, int);
void plaza_setMyCommentTrans(arg0, arg1, arg2)
int arg0;
int arg1;
int arg2;
{
    char b70[0x20];
    char b50[0x20];
    int job;
    int x2;

    flfntSetSize(0x12, 0x12);
    switch ((s8)arg2) {
    case 0:
        put_mainWindow(arg0, arg1);
        break;
    case 1:
        put_mainWindowTex(arg0, arg1);
        break;
    }
    arg2 = (s16)arg0;
    arg1 = (s16)(arg1 + 0x28);
    put_titles_s(arg2 + 0xA, arg1, ((int *)tl_mail_tbl)[0x2C / 4]);
    font_set_palette(0);
    arg1 = (s16)(arg1 + 0x16);
    flfntLocate(arg2 + 0xA, arg1);
    font_print(lit_2316, ((int *)tl_mail_tbl)[2]);
    x2 = (s16)(arg2 + 0xA) + 0x36;
    flfntLocate(x2, arg1);
    font_print(lit_2316, (int)cw + 0x448);
    arg1 = (s16)(arg1 + 0x16);
    flfntLocate(arg2 + 0xA, arg1);
    font_print(lit_2316, ((int *)tl_mail_tbl)[3]);
    flfntLocate(x2, arg1);
    memcpy(b70, cw + 0x440, 8);
    han2zen(b70, b50);
    font_print(lit_2316, b50);
    job = Get_weapon_job(D_3C738C) & 0xFF;
    if (job == 5) {
        job = 1;
    }
    arg1 = (s16)(arg1 + 0x16);
    flfntLocate(arg2 + 0xA, arg1);
    font_print(lit_2316, ((int *)tl_mail_tbl)[7]);
    x2 = (s16)(arg2 + 0xA) + 0x36;
    flfntLocate(x2, arg1);
    font_print(lit_2316, ((int *)tl_job_tbl)[job & 0xFF]);
    arg1 = (s16)(arg1 + 0x16);
    flfntLocate(arg2 + 0xA, arg1);
    font_print(lit_2316, ((int *)tl_mail_tbl)[8]);
    flfntLocate(x2, arg1);
    sprintf(b70, lit_2602, *(u8 *)0x3C733B);
    han2zen(b70, b50);
    font_print(lit_2316, b50);
    arg1 = (s16)(arg1 + 0x16);
    flfntLocate(arg2 + 0xA, arg1);
    font_print(lit_2316, ((int *)tl_mail_tbl)[10]);
    lb_put_comment_s(arg2 + 0x68, arg1 - 2, D_3C73B4, 1);
}
