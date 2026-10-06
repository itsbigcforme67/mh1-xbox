/* lbui, run 2: event_eat_set_msg .. SetDialogData (lobby.bin 0x005919E0-0x00591D78): the matching functions of lbui_nm.c. */
#pragma readonly_strings on
#include "lbui_proto.h"


/* sprite template for Put_2TF / Put_sprite_rotate (helpLineTbl entries, stride 0x14) */
typedef struct { s16 x; s16 y; s16 w; s16 h; u8 pad08[4]; s16 u0; s16 v0; s16 u1; s16 v1; } DLGSPR;
typedef struct { f32 f[5]; } DLGF5;
void Put_sprite_rotate();

/* dialog frame: top/bottom edge strips of 0x28 high tiles then the two rotated side strips (near-match) */

void Paint_square();

/* menu frame: filled body (Paint_square or 5-high strips) + four 20-unit edge strips + four 7x6 corners (near-match) */

void put_button_help(int a, int b, int c, u16 d);

/* button help line of the plaza menus: which of the four buttons are shown for each menu / sub menu step (near-match) */

void event_eat_set_msg(void) {
    char sp10[0x100];
    s16 v;

    set01_set2(pRes->msg);
    v = Status_add_tbl[pRes->idx].s0;
    if (v != 0) {
        if (v > 0) {
            sprintf(sp10, lit_520_0065B990);
        } else if (v < 0) {
            sprintf(sp10, lit_521_0065B9B0);
        }
        set01_set2_use_mem(sp10);
    }
    v = Status_add_tbl[pRes->idx].s1;
    if (v != 0) {
        if (v > 0) {
            sprintf(sp10, lit_522_0065B9D0);
        } else if (v < 0) {
            sprintf(sp10, lit_523_0065B9F0);
        }
        set01_set2_use_mem(sp10);
    }
    v = Status_add_tbl[pRes->idx].s2;
    if (v != 0) {
        if (v > 0) {
            sprintf(sp10, lit_524_0065BA10);
        } else if (v < 0) {
            sprintf(sp10, lit_525_0065BA30);
        }
        set01_set2_use_mem(sp10);
    }
    v = Status_add_tbl[pRes->idx].s3;
    if (v != 0) {
        if (v > 0) {
            sprintf(sp10, lit_526_0065BA50);
        } else if (v < 0) {
            sprintf(sp10, lit_527_0065BA70);
        }
        set01_set2_use_mem(sp10);
    }
}

void event_eat_trans_ot1(a)
u8 *a;
{
    font_set_stack_no(*(int *)(a + 0x18));
}

void SetDialogData_HTML(arg0)
int arg0;
{
    htmlStr = arg0;
    dialogData.html = 1;
    set_dialog_square(0x1F4, 0x17C);
}

void SetDialogYesNo(v)
s8 v;
{
    dialogData.yesno = v;
    pNet->yesno = v;
}

void SetDialogData(id, kind)
int id;
s8 kind;
{
    int lines;
    int maxw = 0;
    char *p;
    char *line;
    int w;

    if (dialogData.id != id || dialogData.html != kind) {
        dialogData.id = id;
        lines = 0;
        dialogData.msg = netDialogMessage[id];
        dialogData.html = kind;
        dialogData.x04 = 0;
        dialogData.x06 = 0;
        p = dialogData.msg;
        line = p;
        if (*p != 0) {
            do {
                p = (char *)strchr(p, 0xA);
                lines++;
                if (p != 0) {
                    w = p - line;
                    line = p;
                } else {
                    w = strlen_sp(line);
                }
                if (w < 0) {
                    w = -w;
                }
                if (maxw < w) {
                    maxw = w;
                }
                if (p == 0) {
                    break;
                }
                p++;
            } while (*p != 0);
        }
        switch (kind) {
        case 2:
        case 4:
            dialogData.yesno = 0;
        case 3:
            lines += 2;
            break;
        }
        dialogData.lines = lines;
        set_dialog_square(maxw * 10 + 0x3C, lines * 20 + 0x3C);
    }
}
