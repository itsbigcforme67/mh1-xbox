/* G online (PC): lbc_top_menu_00, written from Monster Hunter G's code at lobby 0x57DAC0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Lbc_init_network_work();
s32 Lbc_set_prim();
s32 SetSceneTitle();
s32 lobby_func_0057D5D0();
void * memset();
s32 str_play_vol();
extern u8 cw;
extern u8 game_w;
extern u8 pNet;
extern s8 ClassInfo;
extern u8 PlazaInfo;

void lbc_top_menu_00(void) {
    u8 *var_v1;   /* (s32 *) */
    s32 var_a0;
    u8 *temp_v0;

    ClassInfo = 0;
    (*(s8 *)((u8 *)&ClassInfo + 0x4)) = 0;
    (*(s8 *)((u8 *)&ClassInfo + 0x8)) = 0;
    memset(&PlazaInfo, 0, 0x2358);
    (*(s8 *)((u8 *)&game_w + 0x25)) = 0x4A;
    str_play_vol(0, 0x4A, 0x1E);
    var_a0 = 0;
    var_v1 = &PlazaInfo;
    do {
        (*(s8 *)((u8 *)(var_v1) + 0x16)) = 0;
        (*(s8 *)((u8 *)(var_v1) + 0x172)) = 0;
        var_a0 += 8;
        (*(s8 *)((u8 *)(var_v1) + 0x2CE)) = 0;
        (*(s8 *)((u8 *)(var_v1) + 0x42A)) = 0;
        (*(s8 *)((u8 *)(var_v1) + 0x586)) = 0;
        (*(s8 *)((u8 *)(var_v1) + 0x6E2)) = 0;
        (*(s8 *)((u8 *)(var_v1) + 0x83E)) = 0;
        (*(s8 *)((u8 *)(var_v1) + 0x99A)) = 0;
        var_v1 += 0xAE0;
    } while (var_a0 < 0x12);
    temp_v0 = &(*(u8 *)((u8 *)&PlazaInfo + 0x16)) + (var_a0 * 0x15C);
    (*(s8 *)((u8 *)(temp_v0) + 0)) = 0;
    (*(s8 *)((u8 *)(temp_v0) + 0x15C)) = 0;
    Lbc_init_network_work(var_a0, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
    (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB70)) = 1;
    (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x27)) = 0;
    Lbc_set_prim((s32) lobby_func_0057D5D0, 0, 0, 0);
    SetSceneTitle(3, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
    (*(s8 *)((u8 *)((*(void **)&cw)) + 0xBA1)) = 0;
}

