/* G online (PC): Lb_put_job, written from Monster Hunter G's code at lobby 0x593100 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Lb_put_icon_free();
s32 Lb_put_icon_free2();
s32 SetTextureStage();
s32 reload_tex();
void Lb_put_job(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4, s32 arg5) {
    s32 temp_v1;
    s32 var_s0;

    var_s0 = arg4;
    if ((((s8) (arg5))) != 0) {
        reload_tex(1, 0x13F);
        SetTextureStage(0x13F);
        Lb_put_icon_free2(arg0, arg1, arg2, arg3, (s32) (((s16) (((((s16) (var_s0))) + 0x19)))));
        reload_tex(1, 0x180);
        SetTextureStage(0x180);
        return;
    }
    temp_v1 = ((s16) (var_s0));
    switch (temp_v1) {                              /* irregular */
    case 5:
        var_s0 = 1;
        break;
    case 6:
        var_s0 = 4;
        break;
    }
    Lb_put_icon_free(arg0, arg1, arg2, arg3, (s32) (((s16) (((((s16) (var_s0))) + 2)))));
}

