/* lb_ag01 - lobby 0x005CCBC0-0x005CCC90: Lb_put_job (job icon of a member; e is the s16 y position, the 5th argument of the icon calls). Whole file in lb_ag.c. */
#include "lobby_f.h"

void Lb_put_icon_free2(int, int, int, int, int);
void Lb_put_icon_free(int, int, int, int, int);

void Lb_put_job(int a, int b, int c, int d, s16 e, s8 f) {
    if (f != 0) {
        reload_tex(1, 0x118);
        SetTextureStage(0x118);
#ifdef VERSION_MHG     /* G 0x593100: its icon sheet has one more cell before the weapon icons (the shop lists showed the
                       * neighbouring icon until the detail view, which draws its own; owner, Xbox, 21 Oct 2026) */
        Lb_put_icon_free2(a, b, c, d, (s16)(e + 0x19));
#else
        Lb_put_icon_free2(a, b, c, d, (s16)(e + 0x18));
#endif
        reload_tex(1, 0x157);
        SetTextureStage(0x157);
        return;
    }
#ifdef VERSION_MHG     /* G: jobs 5 and 6 take the icons of 1 and 4 in the small sheet */
    if (e == 6)
        e = 4;
    else if (e == 5)
        e = 1;
#endif
    Lb_put_icon_free(a, b, c, d, (s16)(e + 2));
}
