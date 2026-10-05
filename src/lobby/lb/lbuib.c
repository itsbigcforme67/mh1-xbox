/* lbui, run 2: event_eat_set_msg .. SetDialogData (lobby.bin 0x005919E0-0x00591D78): the matching functions of lbui_nm.c. */
#pragma readonly_strings on
#include "lbui_proto.h"

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
