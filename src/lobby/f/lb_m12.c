/* lb_m12 - browser small helpers 0x005E8890-0x005E8898: BsUrlCompare_SS. Whole file in lb_m.c. */
#include "lobby_f.h"
void BsWorkTrans();
void To_RetVal();
void flSetRenderState();
int strncmp();















int BsUrlCompare_SS(char *a, char *b) {
    return strncmp(a, b, 0x100);
}
