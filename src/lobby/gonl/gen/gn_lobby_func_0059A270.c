/* G online (PC): lobby_func_0059A270, written from Monster Hunter G's code at lobby 0x59A270 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Lb_set_mini_data();
s32 Lbc_init_network_work();
s32 flMemset();
s32 init_set_work();
extern u8 cw;
extern u8 game_w;
extern u8 lb_gbyte_2EF2A4;
extern u8 pNet;
extern u8 client_work;
extern s32 lb_player;
extern u8 lb_sys;
extern u8 my_user_mini_data;
extern u8 network_work;
extern u8 player_work;

void lobby_func_0059A270(void) {
    u8 *var_a0;   /* (s32 *) */
    u8 *var_a1;   /* (s32 *) */
    u8 *var_a2;   /* (s32 *) */
    s32 var_a3;
    u8 temp_a1;
    u8 *temp_v1;

    flMemset(&lb_sys, 0, 0x98);
    (*(s8 *)((u8 *)&lb_sys + 0x95)) = 1;
    var_a3 = 0;
    var_a2 = &player_work;
    var_a1 = &lb_player;
    var_a0 = &client_work;
    (*(u8 *)((u8 *)&lb_sys + 0x93)) = (*(u8 *)&lb_gbyte_2EF2A4);
    do {
        (*(s32 **)((u8 *)(var_a1) + 0)) = var_a2;
        (*(s8 *)((u8 *)(var_a0) + 0xB64)) = 0;
        (*(void **)((u8 *)(var_a1) + 0x38)) = (void *) (var_a2 + 0xA00);
        (*(s8 *)((u8 *)(var_a0) + 0xB65)) = 0;
        (*(void **)((u8 *)(var_a1) + 0x70)) = (void *) (var_a2 + 0x1400);
        var_a3 += 5;
        (*(s8 *)((u8 *)(var_a0) + 0xB66)) = 0;
        temp_v1 = var_a2 + 0x2800;
        (*(void **)((u8 *)(var_a1) + 0xA8)) = (void *) (var_a2 + 0x1E00);
        var_a2 += 0x3200;
        (*(s8 *)((u8 *)(var_a0) + 0xB67)) = 0;
        (*(void **)((u8 *)(var_a1) + 0xE0)) = temp_v1;
        (*(s8 *)((u8 *)(var_a0) + 0xB68)) = 0;
        var_a1 += 0x118;
        var_a0 += 5;
    } while (var_a3 < 0xA);
    init_set_work();
    (*(u8 **)&cw) = &client_work;
    (*(u8 **)&pNet) = &network_work;
    Lbc_init_network_work(M2C_ERROR(/* Read from unset register $a0 */), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
    temp_a1 = (*(u8 *)((u8 *)&game_w + 0x3A));
    if (*(&player_work + (temp_a1 * 0xA00)) != 0) {
        Lb_set_mini_data(&my_user_mini_data, (s32) temp_a1, M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
    }
    (*(s8 *)((u8 *)&game_w + 0x25)) = 0;
}

