/* lb_ag01 - lobby 0x005CCBC0-0x005CCC90: Lb_put_job (job icon of a member; e is the s16 y position, the 5th argument of the icon calls). Whole file in lb_ag.c. */
#include "lobby_f.h"

void Lb_put_icon_free2(int, int, int, int, int);
void Lb_put_icon_free(int, int, int, int, int);

void Lb_put_job(int a, int b, int c, int d, s16 e, s8 f) {
    if (f != 0) {
        reload_tex(1, 0x118);
        SetTextureStage(0x118);
        Lb_put_icon_free2(a, b, c, d, (s16)(e + 0x18));
        reload_tex(1, 0x157);
        SetTextureStage(0x157);
        return;
    }
    Lb_put_icon_free(a, b, c, d, (s16)(e + 2));
}
