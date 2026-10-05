/* lb_bz155 - lobby UI/client 0x005B2920-0x005B2968: cnWrap_FontDisp (first drafted by tools/lbauto.py). */
#include "lobby_b.h"
extern char lit_342_0065E510[];
void flfntLocate(int x, int y);
void flfntPrintf();
void flfntSetSize(int w, int h);

void cnWrap_FontDisp(char *s, f32 x, f32 y) {
    flfntLocate((int)x, (int)y);
    flfntPrintf(lit_342_0065E510, s);
}
