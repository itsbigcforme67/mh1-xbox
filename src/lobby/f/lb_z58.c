/* lb_z58 - auto-drafted 0x005F9520-0x005F95A4: refreshPage (first drafted by tools/lbauto.py). */
#include "lobby_f.h"
extern s8 BsToolMenuReq;
extern u8 * bsCur;
extern u8 * bsSys;

#ifdef VERSION_MH2
#include "mh2_lb.h"
#define BsToolMenuReq BW_TOOLMENUREQ
#endif
void refreshPage(void) {
    s32 temp_v0;

#ifdef VERSION_MH2
    if (BsRouteReloadCheck() != 0 && F(u8, bsSys, 0x38) == 0 && F(s8, bsSys, 0x3A) == 0) {
        goto ok;
    }
    return;
ok:
#else
    if ((BsRouteReloadCheck() == 0) || (F(u8, bsSys, 0x38) != 0)) {
        return;
    }
#endif
    temp_v0 = BsRouteReload();
    if (temp_v0 == 0) {
        To_BodyMain_ActDsp();
        return;
    }
    BsToolMenuReq = 0;
    F(s8, bsCur, 1) = 0;
    F(s8, bsCur, 4) = 0;
    SetNextURL(temp_v0);
    To_BodyMain_RcvSrc();
}
