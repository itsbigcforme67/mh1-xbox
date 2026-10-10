/* lb_bz155 - lobby UI/client 0x005B2920-0x005B2968: cnWrap_FontDisp (first drafted by tools/lbauto.py). */
#include "lobby_b.h"
extern char lit_342_0065E510[];
void flfntLocate(int x, int y);
void flfntPrintf();
void flfntSetSize(int w, int h);

#ifdef __MWERKS__
void cnWrap_FontDisp(char *s, f32 x, f32 y) {
    flfntLocate((int)x, (int)y);
    flfntPrintf(lit_342_0065E510, s);
}
#else   /* the callers pass (x, y, size, string) (lb_cli.c, lb_c506.c, lb_nt01.c): on the EE the floats go to f12-f14 and
         * the string to a0, so this matching spelling works there; on the PC the arguments are on the stack in the
         * callers' order (the admin message and the logout clock crashed) */
void cnWrap_FontDisp(f32 x, f32 y, f32 z, char *s) {
    (void)z;
    flfntLocate((int)x, (int)y);
    flfntPrintf(lit_342_0065E510, s);
}
#endif
