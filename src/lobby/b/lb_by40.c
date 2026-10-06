/* lb_by40 - agent B promoted near-match 0x005C1F70-0x005C2030: id_select_00 (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern char transOtSelectHandleName[];
extern char D_3C6FC8[];
typedef struct { u8 pad0000[0x2]; u8 x0002; u8 pad0003[0x5]; s8 x0008; } ARG_id_select_00_arg0;

void id_select_00(ARG_id_select_00_arg0 *arg0) {
    s8 var_s2;
    s32 var_s1;

    Lbc_set_prim(0, 0, &transOtSelectHandleName);
    arg0->x0002 = (u8) (arg0->x0002 + 1);
    SetSceneTitle(1, 0);
    SetHelpLineMsg(1, 1);
    arg0->x0008 = 0;
    var_s2 = 0;
    var_s1 = 0;
    do {
        if (memcmp((s32)cw + var_s1 + 0x2B, &D_3C6FC8, 0x10) == 0) {
            arg0->x0008 = var_s2;
            return;
        }
        var_s2++;
        var_s1 += 0x11;
    } while (var_s2 < 3);
}
