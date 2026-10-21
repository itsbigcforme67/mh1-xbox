/* G online (PC): Lb_stage_load, written from Monster Hunter G's code at lobby 0x59C370 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Lb_act_set();
s32 Lb_pl_to_normal();
s32 Lb_player_load();
s32 Lb_send_pl_warp();
s32 Set06_set();
s32 lobby_func_0059A830();
s32 lobby_func_0059ACD0();
s32 ran_suu();
s32 set_viewproj();
s32 st_model_load();
s32 stage_fog_set();
s32 stage_set_set();
extern u8 cw;
extern u8 game_w;
extern u8 lb_start_pos;
extern u8 stage_work;
extern u8 system_w;
s32 CameraInit();                               /* extern */
s32 CameraMove();                               /* extern */
s32 Lb_load_player_all();                       /* extern */
s32 Lb_pl_chr_set(); /* extern */
s32 Lb_pl_init();                            /* extern */
s32 Lbc_connect();                              /* extern */
s32 Online_ck();                                  /* extern */
s32 fade_reset();                               /* extern */
s32 flCompact();                                /* extern */
s32 get_questLevelNum();                     /* extern */
s32 light_init();                             /* extern */
s32 player_mk();                     /* extern */
s32 stage_w_init();                             /* extern */
s32 view_reset();                               /* extern */
extern s32 St_unique_tbl;
extern u8 lb_return_pos;
extern s32 lb_sys;
extern u8 player_work;
extern s32 stage_start_pos;

void Lb_stage_load(u8 arg0, s32 arg1) {
    s32 temp_v1;
    s32 temp_v1_4;
    s32 var_v0;
    s32 var_v1;
    s32 var_s1;
    u16 temp_v0_2;
    u16 temp_v0_4;
    u8 *var_s0;
    u8 temp_a0;
    u8 temp_a0_2;
    u8 temp_v1_7;
    u8 *temp_s0;
    u8 *temp_v0;
    u8 *temp_v0_3;
    u8 *temp_v0_5;
    u8 *temp_v1_2;
    u8 *temp_v1_3;
    u8 *temp_v1_5;
    u8 *temp_v1_6;

    temp_a0 = (*(u8 *)((u8 *)&game_w + 0x3A));
    temp_s0 = &player_work + (temp_a0 * 0xA00);
    if ((Online_ck(temp_a0) == 1) || ((*(s8 *)((u8 *)&lb_sys + 0x16)) == 0)) {
        if ((*(u8 *)((u8 *)&lb_sys + 0x7B)) == 0) {
            Lb_load_player_all();
        }
        flCompact();
    }
    (*(u8 *)((u8 *)(temp_s0) + 0x736)) = arg0;
    (*(s32 *)((u8 *)&lb_sys + 0x8)) = 0;
    if ((Online_ck() == 1) || ((*(s8 *)((u8 *)&lb_sys + 0x16)) == 0)) {
        stage_w_init();
        (*(s8 *)((u8 *)&stage_work + 0x1)) = 1;
    }
    set_viewproj(0);
    view_reset();
    fade_reset();
    if ((Online_ck() == 1) || ((*(s8 *)((u8 *)&lb_sys + 0x16)) == 0)) {
        st_model_load((s32) arg0);
        Lbc_connect();
    }
    if ((Online_ck() == 1) || ((*(s8 *)((u8 *)&lb_sys + 0x16)) == 0)) {
        Lb_pl_init(arg1);
    }
    light_init(arg0);
    (*(s8 *)((u8 *)&system_w + 0x2F)) = 0xFF;
    stage_fog_set(arg0 & 0xFF);
    stage_set_set(arg0 & 0xFF);
    if (lb_sys != 0x23) {
        lb_sys = 0;
    }
    (*(s8 *)((u8 *)(temp_s0) + 0x8EC)) = 0;
    (*(s8 *)((u8 *)(temp_s0) + 0x90F)) = 0;
    (*(s32 *)((u8 *)(temp_s0) + 0x878)) = 0;
    (*(s8 *)((u8 *)(temp_s0) + 0x936)) = 0;
    (*(s8 *)((u8 *)(temp_s0) + 0x7ED)) = 0;
    if (arg0 == 0x4E) {
        temp_v1 = (*(s32 *)((u8 *)((*(void **)&cw)) + 0x11AB0)) & 3;
        switch (temp_v1) {                          /* switch 1; irregular */
        case 1:                                     /* switch 1 */
            Set06_set(2);
            break;
        case 2:                                     /* switch 1 */
            Set06_set(0);
            break;
        case 3:                                     /* switch 1 */
            Set06_set(1);
            break;
        }
    } else if ((arg0 == 0x57) && (get_questLevelNum(0x2E) == 0)) {
        Set06_set(0);
    }
    if ((*(s8 *)((u8 *)&lb_sys + 0x7C)) == -1) {
        switch (arg0) {                             /* switch 2; irregular */
        case 0x57:                                  /* switch 2 */
            (*(f32 *)((u8 *)(temp_s0) + 0xAC)) = (f32) (*(f32 *)((u8 *)(&(*(u8 *)((u8 *)&lb_start_pos + 0xD0))) + 0));
            (*(f32 *)((u8 *)(temp_s0) + 0xB0)) = (f32) (*(f32 *)((u8 *)(&(*(u8 *)((u8 *)&lb_start_pos + 0xD0))) + 4));
            (*(f32 *)((u8 *)(temp_s0) + 0xB4)) = (f32) (*(f32 *)((u8 *)(&(*(u8 *)((u8 *)&lb_start_pos + 0xD0))) + 8));
            (*(u16 *)((u8 *)(temp_s0) + 0xE)) = (u16) (*(u16 *)((u8 *)(&(*(u8 *)((u8 *)&lb_start_pos + 0xD0))) + 0xC));
            (*(s32 *)((u8 *)(temp_s0) + 0xA4)) = (s32) (*(u16 *)((u8 *)(&(*(u8 *)((u8 *)&lb_start_pos + 0xD0))) + 0xC));
            break;
        case 0x56:                                  /* switch 2 */
            if (lb_sys == 0x23) {
                (*(f32 *)((u8 *)(temp_s0) + 0xAC)) = (f32) (*(f32 *)((u8 *)(&(*(u8 *)((u8 *)&lb_start_pos + 0xE0))) + 0));
                (*(f32 *)((u8 *)(temp_s0) + 0xB0)) = (f32) (*(f32 *)((u8 *)(&(*(u8 *)((u8 *)&lb_start_pos + 0xE0))) + 4));
                (*(f32 *)((u8 *)(temp_s0) + 0xB4)) = (f32) (*(f32 *)((u8 *)(&(*(u8 *)((u8 *)&lb_start_pos + 0xE0))) + 8));
                (*(u16 *)((u8 *)(temp_s0) + 0xE)) = (u16) (*(u16 *)((u8 *)(&(*(u8 *)((u8 *)&lb_start_pos + 0xE0))) + 0xC));
                (*(s32 *)((u8 *)(temp_s0) + 0xA4)) = (s32) (*(u16 *)((u8 *)(&(*(u8 *)((u8 *)&lb_start_pos + 0xE0))) + 0xC));
                Lb_act_set(temp_s0, 0, 0x4F, M2C_ERROR(/* Read from unset register $a3 */));
                Lb_pl_chr_set(temp_s0, 0x1AB, 0, 0);
            } else {
                temp_v1_2 = (u8 *)&stage_start_pos + (arg0 * 0xC);
                (*(f32 *)((u8 *)(temp_s0) + 0xAC)) = (f32) (*(f32 *)((u8 *)(temp_v1_2) + 0));
                (*(f32 *)((u8 *)(temp_s0) + 0xB0)) = (f32) (*(f32 *)((u8 *)(temp_v1_2) + 4));
                (*(f32 *)((u8 *)(temp_s0) + 0xB4)) = (f32) (*(f32 *)((u8 *)(temp_v1_2) + 8));
                (*(u16 *)((u8 *)(temp_s0) + 0xE)) = 0U;
                (*(s32 *)((u8 *)(temp_s0) + 0xA4)) = (s32) (0 & 0xFFFF);
            }
            break;
        case 0x4C:                                  /* switch 2 */
            temp_v0 = &(*(u8 *)((u8 *)&lb_start_pos + 0x30)) + (((s32) (ran_suu(1) & 0xFFFF) % 5) * 0x10);
            (*(f32 *)((u8 *)(temp_s0) + 0xAC)) = (f32) (*(f32 *)((u8 *)(temp_v0) + 0));
            (*(f32 *)((u8 *)(temp_s0) + 0xB0)) = (f32) (*(f32 *)((u8 *)(temp_v0) + 4));
            (*(f32 *)((u8 *)(temp_s0) + 0xB4)) = (f32) (*(f32 *)((u8 *)(temp_v0) + 8));
            temp_v0_2 = (*(u16 *)((u8 *)(temp_v0) + 0xC));
            (*(u16 *)((u8 *)(temp_s0) + 0xE)) = temp_v0_2;
            (*(s32 *)((u8 *)(temp_s0) + 0xA4)) = (s32) temp_v0_2;
            break;
        default:                                    /* switch 2 */
            temp_v1_3 = (u8 *)&stage_start_pos + (arg0 * 0xC);
            (*(f32 *)((u8 *)(temp_s0) + 0xAC)) = (f32) (*(f32 *)((u8 *)(temp_v1_3) + 0));
            (*(f32 *)((u8 *)(temp_s0) + 0xB0)) = (f32) (*(f32 *)((u8 *)(temp_v1_3) + 4));
            (*(f32 *)((u8 *)(temp_s0) + 0xB4)) = (f32) (*(f32 *)((u8 *)(temp_v1_3) + 8));
            (*(u16 *)((u8 *)(temp_s0) + 0xE)) = 0U;
            (*(s32 *)((u8 *)(temp_s0) + 0xA4)) = (s32) (0 & 0xFFFF);
            break;
        }
    } else if ((*(s8 *)((u8 *)&lb_sys + 0x7C)) == 0) {
        switch (arg0) {                             /* switch 3; irregular */
        case 0x4D:                                  /* switch 3 */
            if ((*(u8 *)((u8 *)((*(void **)&cw)) + 0xFA5E)) == 0) {
                (*(f32 *)((u8 *)(temp_s0) + 0xAC)) = (f32) (*(f32 *)((u8 *)(&(*(u8 *)((u8 *)&lb_start_pos + 0x80))) + 0));
                (*(f32 *)((u8 *)(temp_s0) + 0xB0)) = (f32) (*(f32 *)((u8 *)(&(*(u8 *)((u8 *)&lb_start_pos + 0x80))) + 4));
                (*(f32 *)((u8 *)(temp_s0) + 0xB4)) = (f32) (*(f32 *)((u8 *)(&(*(u8 *)((u8 *)&lb_start_pos + 0x80))) + 8));
                (*(u16 *)((u8 *)(temp_s0) + 0xE)) = (u16) (*(u16 *)((u8 *)(&(*(u8 *)((u8 *)&lb_start_pos + 0x80))) + 0xC));
                (*(s32 *)((u8 *)(temp_s0) + 0xA4)) = (s32) (*(u16 *)((u8 *)(&(*(u8 *)((u8 *)&lb_start_pos + 0x80))) + 0xC));
            } else {
                temp_v1_4 = ran_suu(1) & 0xFFFF;
                var_v0 = temp_v1_4 & 3;
                if (temp_v1_4 < 0) {
                    var_v1 = var_v0 * 0x10;
                    if (var_v0 != 0) {
                        var_v0 -= 4;
                        goto block_55;
                    }
                } else {
block_55:
                    var_v1 = var_v0 * 0x10;
                }
                temp_v0_3 = &lb_return_pos + var_v1;
                (*(f32 *)((u8 *)(temp_s0) + 0xAC)) = (f32) (*(f32 *)((u8 *)(temp_v0_3) + 0));
                (*(f32 *)((u8 *)(temp_s0) + 0xB0)) = (f32) (*(f32 *)((u8 *)(temp_v0_3) + 4));
                (*(f32 *)((u8 *)(temp_s0) + 0xB4)) = (f32) (*(f32 *)((u8 *)(temp_v0_3) + 8));
                temp_v0_4 = (*(u16 *)((u8 *)(temp_v0_3) + 0xC));
                (*(u16 *)((u8 *)(temp_s0) + 0xE)) = temp_v0_4;
                (*(s32 *)((u8 *)(temp_s0) + 0xA4)) = (s32) temp_v0_4;
            }
            break;
        case 0x50:                                  /* switch 3 */
            (*(f32 *)((u8 *)(temp_s0) + 0xAC)) = (f32) (*(f32 *)((u8 *)(&(*(u8 *)((u8 *)&lb_start_pos + 0x90))) + 0));
            (*(f32 *)((u8 *)(temp_s0) + 0xB0)) = (f32) (*(f32 *)((u8 *)(&(*(u8 *)((u8 *)&lb_start_pos + 0x90))) + 4));
            (*(f32 *)((u8 *)(temp_s0) + 0xB4)) = (f32) (*(f32 *)((u8 *)(&(*(u8 *)((u8 *)&lb_start_pos + 0x90))) + 8));
            (*(u16 *)((u8 *)(temp_s0) + 0xE)) = (u16) (*(u16 *)((u8 *)(&(*(u8 *)((u8 *)&lb_start_pos + 0x90))) + 0xC));
            (*(s32 *)((u8 *)(temp_s0) + 0xA4)) = (s32) (*(u16 *)((u8 *)(&(*(u8 *)((u8 *)&lb_start_pos + 0x90))) + 0xC));
            break;
        case 0x52:                                  /* switch 3 */
            (*(f32 *)((u8 *)(temp_s0) + 0xAC)) = (f32) (*(f32 *)((u8 *)(&(*(u8 *)((u8 *)&lb_start_pos + 0xA0))) + 0));
            (*(f32 *)((u8 *)(temp_s0) + 0xB0)) = (f32) (*(f32 *)((u8 *)(&(*(u8 *)((u8 *)&lb_start_pos + 0xA0))) + 4));
            (*(f32 *)((u8 *)(temp_s0) + 0xB4)) = (f32) (*(f32 *)((u8 *)(&(*(u8 *)((u8 *)&lb_start_pos + 0xA0))) + 8));
            (*(u16 *)((u8 *)(temp_s0) + 0xE)) = (u16) (*(u16 *)((u8 *)(&(*(u8 *)((u8 *)&lb_start_pos + 0xA0))) + 0xC));
            (*(s32 *)((u8 *)(temp_s0) + 0xA4)) = (s32) (*(u16 *)((u8 *)(&(*(u8 *)((u8 *)&lb_start_pos + 0xA0))) + 0xC));
            break;
        case 0x53:                                  /* switch 3 */
            (*(f32 *)((u8 *)(temp_s0) + 0xAC)) = (f32) (*(f32 *)((u8 *)(&(*(u8 *)((u8 *)&lb_start_pos + 0xB0))) + 0));
            (*(f32 *)((u8 *)(temp_s0) + 0xB0)) = (f32) (*(f32 *)((u8 *)(&(*(u8 *)((u8 *)&lb_start_pos + 0xB0))) + 4));
            (*(f32 *)((u8 *)(temp_s0) + 0xB4)) = (f32) (*(f32 *)((u8 *)(&(*(u8 *)((u8 *)&lb_start_pos + 0xB0))) + 8));
            (*(u16 *)((u8 *)(temp_s0) + 0xE)) = (u16) (*(u16 *)((u8 *)(&(*(u8 *)((u8 *)&lb_start_pos + 0xB0))) + 0xC));
            (*(s32 *)((u8 *)(temp_s0) + 0xA4)) = (s32) (*(u16 *)((u8 *)(&(*(u8 *)((u8 *)&lb_start_pos + 0xB0))) + 0xC));
            break;
        case 0x55:                                  /* switch 3 */
            (*(f32 *)((u8 *)(temp_s0) + 0xAC)) = (f32) (*(f32 *)((u8 *)(&(*(u8 *)((u8 *)&lb_start_pos + 0xC0))) + 0));
            (*(f32 *)((u8 *)(temp_s0) + 0xB0)) = (f32) (*(f32 *)((u8 *)(&(*(u8 *)((u8 *)&lb_start_pos + 0xC0))) + 4));
            (*(f32 *)((u8 *)(temp_s0) + 0xB4)) = (f32) (*(f32 *)((u8 *)(&(*(u8 *)((u8 *)&lb_start_pos + 0xC0))) + 8));
            (*(u16 *)((u8 *)(temp_s0) + 0xE)) = (u16) (*(u16 *)((u8 *)(&(*(u8 *)((u8 *)&lb_start_pos + 0xC0))) + 0xC));
            (*(s32 *)((u8 *)(temp_s0) + 0xA4)) = (s32) (*(u16 *)((u8 *)(&(*(u8 *)((u8 *)&lb_start_pos + 0xC0))) + 0xC));
            break;
        case 0x56:                                  /* switch 3 */
            (*(f32 *)((u8 *)(temp_s0) + 0xAC)) = (f32) (*(f32 *)((u8 *)(&(*(u8 *)((u8 *)&lb_start_pos + 0xA0))) + 0));
            (*(f32 *)((u8 *)(temp_s0) + 0xB0)) = (f32) (*(f32 *)((u8 *)(&(*(u8 *)((u8 *)&lb_start_pos + 0xA0))) + 4));
            (*(f32 *)((u8 *)(temp_s0) + 0xB4)) = (f32) (*(f32 *)((u8 *)(&(*(u8 *)((u8 *)&lb_start_pos + 0xA0))) + 8));
            (*(u16 *)((u8 *)(temp_s0) + 0xE)) = (u16) (*(u16 *)((u8 *)(&(*(u8 *)((u8 *)&lb_start_pos + 0xA0))) + 0xC));
            (*(s32 *)((u8 *)(temp_s0) + 0xA4)) = (s32) (*(u16 *)((u8 *)(&(*(u8 *)((u8 *)&lb_start_pos + 0xA0))) + 0xC));
            break;
        case 0x57:                                  /* switch 3 */
            (*(f32 *)((u8 *)(temp_s0) + 0xAC)) = (f32) (*(f32 *)((u8 *)(&(*(u8 *)((u8 *)&lb_start_pos + 0xF0))) + 0));
            (*(f32 *)((u8 *)(temp_s0) + 0xB0)) = (f32) (*(f32 *)((u8 *)(&(*(u8 *)((u8 *)&lb_start_pos + 0xF0))) + 4));
            (*(f32 *)((u8 *)(temp_s0) + 0xB4)) = (f32) (*(f32 *)((u8 *)(&(*(u8 *)((u8 *)&lb_start_pos + 0xF0))) + 8));
            (*(u16 *)((u8 *)(temp_s0) + 0xE)) = (u16) (*(u16 *)((u8 *)(&(*(u8 *)((u8 *)&lb_start_pos + 0xF0))) + 0xC));
            (*(s32 *)((u8 *)(temp_s0) + 0xA4)) = (s32) (*(u16 *)((u8 *)(&(*(u8 *)((u8 *)&lb_start_pos + 0xF0))) + 0xC));
            break;
        default:                                    /* switch 3 */
            temp_v1_5 = (*(s32 *)((u8 *)&St_unique_tbl + (arg0 * 4))) + ((*(s8 *)((u8 *)&lb_sys + 0x7C)) * 0x18);
            (*(f32 *)((u8 *)(temp_s0) + 0xAC)) = (f32) (*(f32 *)((u8 *)(temp_v1_5) + 4));
            (*(f32 *)((u8 *)(temp_s0) + 0xB0)) = (f32) (*(f32 *)((u8 *)(temp_v1_5) + 8));
            (*(f32 *)((u8 *)(temp_s0) + 0xB4)) = (f32) (*(f32 *)((u8 *)(temp_v1_5) + 0xC));
            (*(s32 *)((u8 *)(temp_s0) + 0xA4)) = (s32) (*(u16 *)((u8 *)(temp_v1_5) + 0x14));
            (*(u16 *)((u8 *)(temp_s0) + 0xE)) = (u16) (*(u16 *)((u8 *)(temp_v1_5) + 0x14));
            break;
        }
    } else {
        temp_v1_6 = (*(s32 *)((u8 *)&St_unique_tbl + (arg0 * 4))) + ((*(s8 *)((u8 *)&lb_sys + 0x7C)) * 0x18);
        (*(f32 *)((u8 *)(temp_s0) + 0xAC)) = (f32) (*(f32 *)((u8 *)(temp_v1_6) + 4));
        (*(f32 *)((u8 *)(temp_s0) + 0xB0)) = (f32) (*(f32 *)((u8 *)(temp_v1_6) + 8));
        (*(f32 *)((u8 *)(temp_s0) + 0xB4)) = (f32) (*(f32 *)((u8 *)(temp_v1_6) + 0xC));
        (*(s32 *)((u8 *)(temp_s0) + 0xA4)) = (s32) (*(u16 *)((u8 *)(temp_v1_6) + 0x14));
        (*(u16 *)((u8 *)(temp_s0) + 0xE)) = (u16) (*(u16 *)((u8 *)(temp_v1_6) + 0x14));
    }
    (*(s32 *)((u8 *)(temp_s0) + 0x3B0)) = 0;
    if (lb_sys != 0x23) {
        if ((*(u8 *)((u8 *)(temp_s0) + 0x11)) == 1) {
            Lb_pl_to_normal(temp_s0, 0, 0, 0);
            Lb_pl_chr_set(temp_s0, 0x3F, 0, 0x6A);
        } else {
            Lb_pl_to_normal(temp_s0, 0, 0, 0);
        }
    }
    (*(u8 *)((u8 *)&game_w + 0x14)) = arg0;
    Lb_send_pl_warp(temp_s0, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
    lobby_func_0059A830((s32) temp_s0, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
    if (Online_ck() == 1) {
        lobby_func_0059ACD0((s32) (*(u8 *)((u8 *)&game_w + 0x14)), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
    }
    temp_v1_7 = (*(u8 *)((u8 *)&game_w + 0x14));
    if (temp_v1_7 != 0x4C) {
        if (temp_v1_7 == 0x4D) {
            goto block_75;
        }
    } else {
block_75:
        var_s1 = 0;
        var_s0 = &player_work;
        do {
            if (*var_s0 != 0) {
                temp_v0_5 = (*(void **)&cw) + (((s8) (var_s1)));
                if ((*(s8 *)((u8 *)(temp_v0_5) + 0xB64)) == 0) {
                    (*(s8 *)((u8 *)(temp_v0_5) + 0xB64)) = 1;
                    Lb_player_load(var_s0, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
                    Lbc_connect();
                }
            }
            var_s1 = ((s8) ((var_s1 + 1)));
            var_s0 += 0xA00;
        } while (var_s1 < 0xA);
    }
    CameraInit();
    temp_a0_2 = (*(u8 *)((u8 *)&game_w + 0x3A));
    *(&(*(u8 *)((u8 *)&player_work + 0x1)) + (temp_a0_2 * 0xA00)) = 1;
    player_mk(temp_a0_2, 1);
    CameraMove();
}

