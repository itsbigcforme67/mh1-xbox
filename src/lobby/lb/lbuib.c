/* lbui, run 2: event_eat_trans_ot1 .. SetDialogData (lobby.bin 0x00591B90-0x00591D78): the matching functions of lbui_nm.c. */
#pragma readonly_strings on
#include "lbui_proto.h"

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
