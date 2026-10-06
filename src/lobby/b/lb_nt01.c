/* lb_nt01 - agent C 0x005C1BC0-0x005C1D48: net_time_str (online play-time HH display; sprintf(buf,fmt,h,m,s) with dead m,s kept as args). */
#include "lobby_a.h"
extern char lit_291_0065EC10[];
typedef struct { u8 pad00[0x8]; s32 x08; u8 padEND[0x24]; } CNW;
extern CNW CnetWork;
void cnWrap_SetFontSize(f32);
void cnWrap_FontDisp(f32, f32, f32, char *);
void net_time_str(void) {
    char buf[0x20];
    s32 h;
    s32 m;
    s32 s;
    s32 n;
    s32 r;

    h = CnetWork.x08 / 216000;
    m = 59;
    if (h > 99) {
        h = 99;
        s = m;
    } else {
        r = CnetWork.x08 % 216000;
        m = r / 3600;
        r = r % 3600;
        s = r / 60;
    }
    sprintf(buf, lit_291_0065EC10, h, m, s);
    n = 1;
    if (h / 10 > 0) {
        n++;
    }
    cnWrap_SetFontSize(20.0f);
    cnWrap_SetFontColor(0);
    cnWrap_FontDisp(510.0f - (11.0f * (f32) (8 - (n + 6))) / 2.0f, 416.0f, 1.0f, buf);
}
