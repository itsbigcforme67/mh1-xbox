/* lb_by93 - agent B promoted near-match 0x005B2980-0x005B2A08: cnWrap_SetFontSize (first drafted by tools/lbauto.py). */
#include "lobby_b.h"
extern char lit_342_0065E510[];
void flfntLocate(int x, int y);
void flfntPrintf();
void flfntSetSize(int w, int h);

void cnWrap_SetFontSize(f32 size) {
    u32 w;
    f32 s;

    s = size;
    w = (u32)s;
    flfntSetSize(w, (u32)s);
}
