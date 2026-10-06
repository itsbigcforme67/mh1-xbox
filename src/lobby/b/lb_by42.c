/* lb_by42 - agent B promoted near-match 0x00593A70-0x00593B78: DispHelpLine (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern char helpLineTbl[];
typedef struct { s16 x; s16 y; char *s; } HLMSG;
extern struct { HLMSG *msg; int pos; int tick; } helpLineStr;

void DispHelpLine(void) {
    u16 pad;
    s32 t;
    s32 p;
    s32 q;

    pad = Get_sw2(0);
    reload_tex(1, 0x157);
    SetTextureStage(0x157);
    SetFilterMode(1);
    flSetRenderState(0x60, 0);
    Put_2TF(helpLineTbl + 0x14);
    if (helpLineStr.pos >= 0) {
        t = helpLineStr.tick + 1;
        helpLineStr.tick = t;
        if (t > 0) {
            p = helpLineStr.pos + 1;
            helpLineStr.tick = 0;
            helpLineStr.pos = p;
            if (p > 0x100) {
                helpLineStr.pos = -1;
            }
        }
    }
    if ((pad & 0xFFFF) & 0x100) {
        helpLineStr.pos = -1;
    }
    flfntSetSize(0x12, 0x12);
    font_set_palette(0);
    if (helpLineStr.msg != 0) {
        flfntLocate(helpLineStr.msg->x, helpLineStr.msg->y);
        font_print2((s16)helpLineStr.pos, helpLineStr.msg->s);
    }
}
