#include "lobby_b.h"
extern char lit_342_0065E510[];
void flfntLocate(int x, int y);
void flfntPrintf();
void flfntSetSize(int w, int h);

void cnWrap_SetFontSize(f32 size) {
    flfntSetSize((u32)size, (u32)size);
}
