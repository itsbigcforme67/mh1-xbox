#pragma readonly_strings on
#define put_titles put_titles_hdr
#include "lbui_proto.h"
#undef put_titles
extern char lit_2316[];
extern char lit_2602[];
extern int tl_mail_tbl[];
extern int tl_job_tbl[];
extern u8 D_3C738C[];
extern u8 D_3C73B4[];
int han2zen();
int sprintf();
int font_print();
int Get_weapon_job();
void flfntLocate(s16, s16);
void put_titles(s16, s16, int);
void lb_put_comment(s16, s16, u8 *, int);

void plaza_setMyCommentTrans(arg0, arg1, arg2)
int arg0;
int arg1;
int arg2;
{
    char b70[0x20];
    char b50[0x20];
    int job;
    s16 y;

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
    y = arg1;
    y += 0x28;
    put_titles(arg2 + 0xA, y, tl_mail_tbl[0x2C / 4]);
    font_set_palette(0);
    y += 0x16;
    flfntLocate(arg2 + 0xA, y);
    font_print(lit_2316, tl_mail_tbl[2]);
    arg0 = (s16)(arg2 + 0xA) + 0x36;
    flfntLocate(arg0, y);
    font_print(lit_2316, (int)cw + 0x448);
    y += 0x16;
    flfntLocate(arg2 + 0xA, y);
    font_print(lit_2316, tl_mail_tbl[3]);
    flfntLocate(arg0, y);
    memcpy(b70, cw + 0x440, 8);
    han2zen(b70, b50);
    font_print(lit_2316, b50);
    job = Get_weapon_job(D_3C738C) & 0xFF;
    if (job == 5) {
        job = 1;
    }
    y += 0x16;
    flfntLocate(arg2 + 0xA, y);
    font_print(lit_2316, tl_mail_tbl[7]);
    arg0 = (s16)(arg2 + 0xA) + 0x36;
    flfntLocate(arg0, y);
    font_print(lit_2316, tl_job_tbl[job & 0xFF]);
    y += 0x16;
    flfntLocate(arg2 + 0xA, y);
    font_print(lit_2316, tl_mail_tbl[8]);
    flfntLocate(arg0, y);
    sprintf(b70, lit_2602, *(u8 *)0x3C733B);
    han2zen(b70, b50);
    font_print(lit_2316, b50);
    y += 0x16;
    flfntLocate(arg2 + 0xA, y);
    font_print(lit_2316, tl_mail_tbl[10]);
    lb_put_comment(arg2 + 0x68, y - 2, D_3C73B4, 1);
}
