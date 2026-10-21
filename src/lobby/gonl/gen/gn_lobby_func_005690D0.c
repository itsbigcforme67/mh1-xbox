/* G online (PC): lobby_func_005690D0, written from Monster Hunter G's code at lobby 0x5690D0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 g_main_2EFA74;
extern u8 g_main_2EFA74;
s16 Get_sw2();                               /* extern */
s32 Lbc_set_prim(); /* extern */
s32 SetHelpLineMsg();                    /* extern */
s32 SetSceneSubTitleColor();              /* extern */
s32 lobby_bgm_set2();                    /* extern */
s32 lobby_func_00569610();                      /* extern */
s32 plaza_moveMain();                           /* extern */
extern u8 ClassInfo;
extern u8 Lbs_plaza_trans;
extern u8 PlazaInfo;
extern u8 plaza_trans_ot0;
extern u8 plaza_trans_ot1;

s32 lobby_func_005690D0(void *arg0) {
    u16 temp_v0;
    u8 temp_v1;

    (*(s16 *)((u8 *)(arg0) + 0x20)) = Get_sw2(0);
    (*(s32 *)((u8 *)(arg0) + 0x14)) = 2;
    temp_v1 = (*(u8 *)((u8 *)(arg0) + 0x22));
    switch (temp_v1) {                              /* irregular */
    case 0:
        SetSceneSubTitleColor((u8 *)&PlazaInfo + ((ClassInfo - 1) * 0x15C) + 0x1A);
        SetHelpLineMsg(2);
        Lbc_set_prim(&Lbs_plaza_trans, &plaza_trans_ot0, &plaza_trans_ot1, 0);
        lobby_bgm_set2(0x4B);
        (*(s8 *)((u8 *)(arg0) + 0x2A)) = 2;
        (*(u8 *)((u8 *)(arg0) + 0x22)) = (u8) ((*(u8 *)((u8 *)(arg0) + 0x22)) + 1);
        /* fallthrough */
    case 1:
        lobby_func_00569610();
        break;
    case 2:
        plaza_moveMain();
        break;
    default:
        (*(u8 *)((u8 *)(arg0) + 0x22)) = 0U;
        break;
    }
    temp_v0 = (*(u16 *)&g_main_2EFA74);
    if (temp_v0 != 0) {
        (*(u16 *)&g_main_2EFA74) = (u16) (temp_v0 - 1);
    }
    return (*(s32 *)((u8 *)(arg0) + 0x14));
}

