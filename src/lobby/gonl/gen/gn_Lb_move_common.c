/* G online (PC): Lb_move_common, written from Monster Hunter G's code at lobby 0x59B350 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 ItemCopy_Ud2Pl();
s32 Npc_se_req();
s32 add_prim();
s32 em_ride_sub();
s32 enemy_mk();
s32 enemy_mv();
s32 memcmp();
void * memcpy();
void * memset();
s32 old_pos_save();
extern u8 cw;
extern u8 game_w;
extern u8 lbShop;
s32 CameraMove();                               /* extern */
s32 Info_control();                             /* extern */
s32 Lb_check_newCommer();                       /* extern */
s32 Lb_check_pl_load();                          /* extern */
s32 Lb_check_target();                          /* extern */
s32 Lb_cockpit_move();                          /* extern */
s32 Lb_npc_mk();                       /* extern */
s32 Lb_npc_mv();                           /* extern */
s32 Lb_pl_move();                               /* extern */
s16 Lb_shop_sw();                            /* extern */
s32 Pit_mv_lb();                                /* extern */
s32 lb_check_status();                          /* extern */
s32 light_move();                               /* extern */
s32 move_eft();                                 /* extern */
s32 move_set();                                 /* extern */
s32 move_stage();                               /* extern */
s32 player_mk();                                /* extern */
s32 yure_move();                                /* extern */
extern u8 em_work;
extern s8 lbCommer;
extern s32 lb_sys;
extern s32 ot1;
extern u8 player_work;

void Lb_move_common(void) {
    u8 *var_s1;   /* (s32 *) */
    u8 *var_s1_2;   /* (s32 *) */
    u8 *var_s3;   /* (s32 *) */
    s32 temp_v0_4;
    s8 *var_s2;
    u32 var_a0;
    u32 var_s0;
    u32 var_s0_2;
    s32 var_s0_3;
    u8 temp_v0_2;
    u8 *temp_v0;
    u8 *temp_v0_3;

    var_a0 = 0;
    var_s3 = &em_work;
    do {
        temp_v0 = (*(void **)&cw) + var_a0;
        temp_v0_2 = (*(u8 *)((u8 *)(temp_v0) + 0xB5A));
        if (temp_v0_2 != 0) {
            (*(u8 *)((u8 *)(temp_v0) + 0xB5A)) = (u8) (temp_v0_2 - 1);
        }
        var_a0 += 1;
    } while (var_a0 < 0xAU);
    if ((*(s8 *)((u8 *)&lb_sys + 0x7D)) != 0) {
        (*(s8 *)((u8 *)&lb_sys + 0x7D)) -= 1;
    }
    if ((*(u8 *)((u8 *)&lb_sys + 0x88)) != 0) {
        (*(u8 *)((u8 *)&lb_sys + 0x88)) -= 1;
    }
    if ((*(u8 *)((u8 *)&lb_sys + 0x8F)) != 0) {
        (*(u8 *)((u8 *)&lb_sys + 0x8F)) -= 1;
    }
    (*(u8 *)((u8 *)&lb_sys + 0x90)) += 1;
    (*(s16 *)((u8 *)&lbShop + 0x8C)) = Lb_shop_sw(0);
    Lb_check_newCommer();
    memset((*(void **)&cw) + 3, 0, 8);
    (*(u8 *)((u8 *)&game_w + 0x3C)) = 0;
    var_s0 = 0;
    var_s2 = &lbCommer;
    var_s1 = &player_work;
    do {
        if ((*var_s2 != 0) && ((*(u8 *)((u8 *)(var_s1) + 0x736)) != 0) && ((temp_v0_3 = (*(void **)&cw), (*(u8 *)((u8 *)&game_w + 0x3C)) = (u8) ((*(u8 *)((u8 *)&game_w + 0x3C)) + 1), ((*(s8 *)((u8 *)(temp_v0_3) + 3)) == 0)) || (memcmp(temp_v0_3 + 3, var_s2, 8) < 0))) {
            memcpy((*(void **)&cw) + 3, var_s2, 8);
        }
        var_s0 += 1;
        var_s2 += 0x5C;
        var_s1 += 0xA00;
    } while (var_s0 < 0xAU);
    if ((*(s32 *)((u8 *)&lb_sys + 0x80)) != 0) {
        temp_v0_4 = (*(s32 *)((u8 *)&lb_sys + 0x80)) - 1;
        (*(s32 *)((u8 *)&lb_sys + 0x80)) = temp_v0_4;
        if ((temp_v0_4 == 0) && ((*(s32 *)((u8 *)&lb_sys + 0xC)) != 0)) {
            Npc_se_req((*(s32 *)((u8 *)&lb_sys + 0xC)), (s32) (*(s8 *)((u8 *)&lb_sys + 0x84)), (*(s32 *)((u8 *)&lb_sys + 0xC)) + 0xAC, 2);
        }
    }
    ItemCopy_Ud2Pl(&player_work + ((*(u8 *)((u8 *)&game_w + 0x3A)) * 0xA00));
    Info_control();
    Lb_pl_move();
    var_s0_2 = 0;
    do {
        if ((*(u8 *)((u8 *)(var_s3) + 0)) != 0) {
            old_pos_save(var_s3);
            if ((*(u8 *)((u8 *)(var_s3) + 0x1E)) == 0) {
                if (enemy_mv(var_s3) == 0) {
                    enemy_mk(var_s3);
                    em_ride_sub(var_s3);
                }
            } else if (Lb_npc_mv(var_s3) == 0) {
                Lb_npc_mk(var_s3);
            }
        }
        var_s0_2 += 1;
        var_s3 += 0xA00;
    } while (var_s0_2 < 0x14U);
    player_mk();
    yure_move();
    Lb_check_target();
    lb_check_status();
    CameraMove();
    light_move();
    move_eft();
    move_set();
    move_stage();
    if (lb_sys != 0x12) {
        Pit_mv_lb();
    }
    Lb_cockpit_move();
    var_s0_3 = 0;
    var_s1_2 = &player_work;
    do {
        if (((*(u8 *)((u8 *)(var_s1_2) + 0)) != 0) && ((*(u8 *)((u8 *)(var_s1_2) + 1)) != 0) && (Lb_check_pl_load(((s8) (var_s0_3))) != 0)) {
            (*(f32 *)((u8 *)((*(void **)((u8 *)(var_s1_2) + 0x564))) + 8)) = (f32) (*(f32 *)((u8 *)(var_s1_2) + 0xAC));
            (*(f32 *)((u8 *)((*(void **)((u8 *)(var_s1_2) + 0x564))) + 0xC)) = (f32) (*(f32 *)((u8 *)(var_s1_2) + 0xB0));
            (*(f32 *)((u8 *)((*(void **)((u8 *)(var_s1_2) + 0x564))) + 0x10)) = (f32) (*(f32 *)((u8 *)(var_s1_2) + 0xB4));
            add_prim(&ot1, (*(void **)((u8 *)(var_s1_2) + 0x564)), 0x20, 0);
        }
        var_s0_3 += 1;
        var_s1_2 += 0xA00;
    } while (var_s0_3 < 0xAU);
}

