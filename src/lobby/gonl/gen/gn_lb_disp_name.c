/* G online (PC): lb_disp_name, written from Monster Hunter G's code at lobby 0x5918B0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

#ifdef __MWERKS__
s32 Get_weapon_job();
#else
unsigned char Get_weapon_job();   /* the real return type (tools/check_kr_returns.py) */
#endif
s32 Lb_Pl_stg_ck();
s32 Lb_put_icon_free();
s32 Lb_put_job();
s32 Lb_put_status();
s32 SetFilterMode();
s32 SetTextureStage();
s32 flSetRenderState();
s32 flfntLocate();
s32 flfntSetSize();
s32 flmatInit();
s32 flvecrRotTransPers();
s32 font_print_double();
s32 font_set_stack_no();
s32 get_joint_pos();
s32 lobby_func_00595530();
s32 lobby_func_0059DA90();
s32 reload_tex();
extern u8 PitMenu;
extern u8 System_timer;
extern u8 User_data;
extern u8 cw;
extern u8 game_w;
s32 Lb_get_pl_stat2();                           /* extern */
s32 Online_ck();                                    /* extern */
#ifdef __MWERKS__
s32 SoftKeyboard_alive_check();                  /* extern */
#else
signed char SoftKeyboard_alive_check();                  /* extern */   /* the real return type (tools/check_kr_returns.py) */
#endif
extern s32 lbCommer;
extern s32 lb_quest_color_tbl;
extern s32 lb_sys;
extern u8 player_work;

void lb_disp_name(void *arg0) {
    u8 g_frame[0x110] __attribute__((aligned(16)));   /* G's stack frame */
#define spA0 (*(s32 *)(g_frame + 0xA0))
#define spB0 (*(s32 *)(g_frame + 0xB0))
#define spF0 (*(f32 *)(g_frame + 0xF0))
#define spF4 (*(f32 *)(g_frame + 0xF4))
#define spFC (*(f32 *)(g_frame + 0xFC))
#define sp100 (*(f32 *)(g_frame + 0x100))
#define sp104 (*(f32 *)(g_frame + 0x104))
#define sp108 (*(f32 *)(g_frame + 0x108))
    u8 *var_s0;   /* (s32 *) */
    u8 *var_s3;   /* (s32 *) */
    u8 *var_s7;   /* (s32 *) */
    u8 *var_v0;   /* (f32 *) */
    f32 temp_f0;
    f32 temp_f1;
    f32 var_f0;
    s32 var_s2;
    s32 var_v0_2;
    s32 temp_s1;
    s32 temp_s1_2;
    s32 temp_s5;
    s32 temp_s6;
    s32 var_s0_2;
    s32 var_s2_2;
    s32 var_s4;
    s32 var_s5;
    u8 temp_v1;
    u8 temp_v1_2;
    u8 *temp_fp;
    u8 *temp_v0;

    switch (lb_sys) {                               /* irregular */
    case 16:
        if (SoftKeyboard_alive_check(lb_sys) != 0) {
            return;
        }
    case 26:
    case 9:
    case 0:
        font_set_stack_no((*(s32 *)((u8 *)(arg0) + 0x18)));
        reload_tex(1, 0x180);
        SetTextureStage(0x180);
        SetFilterMode(1);
        flSetRenderState(0x60, 0, M2C_ERROR(/* Read from unset register $a2 */));
        var_s4 = 0;
        var_s3 = &player_work;
        var_s7 = &lbCommer;
        spA0 = 0;
        do {
            if (((*(u8 *)((u8 *)(var_s3) + 0)) != 0) && ((*(u8 *)((u8 *)(var_s3) + 1)) != 0) && (Lb_Pl_stg_ck(var_s3, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */)) & 0xFF) && (Lb_get_pl_stat2(((s8) (var_s4))) == 0)) {
                if ((*(f32 *)((u8 *)(var_s3) + 0xAC)) == 0.0f) {
                    if ((*(f32 *)((u8 *)(var_s3) + 0xB4)) != 0.0f) {
                        goto block_17;
                    }
                } else {
block_17:
                    if (((*(u8 *)((u8 *)(var_s3) + 0x14)) != 0) || ((u32) ((*(u8 *)((u8 *)(var_s3) + 0x15)) - 0x4C) >= 2U)) {
                        var_s0 = var_s7 + 8;
                        if ((Online_ck() == 1) && ((*(u8 *)((u8 *)&PitMenu + 0x14)) != 0)) {
                            var_s0 = var_s7;
                        }
                        temp_s1 = (s32) (lobby_func_0059DA90(var_s0, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */)) << 0x30) >> 0x30;
                        flmatInit(&spB0);
                        flSetRenderState(0x1A, (s32) &spB0, M2C_ERROR(/* Read from unset register $a2 */));
                        if (lobby_func_00595530((s32) (*(u8 *)((u8 *)(var_s3) + 0x14)), (s32) (*(u8 *)((u8 *)(var_s3) + 0x15)), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */)) == 0) {
                            sp100 = (*(f32 *)((u8 *)(var_s3) + 0xAC));
                            sp104 = 190.0f + (*(f32 *)((u8 *)(var_s3) + 0xB0));
                            var_f0 = (*(f32 *)((u8 *)(var_s3) + 0xB4));
                            var_v0 = &sp108;
                        } else {
                            get_joint_pos(var_s3, 0x14, &sp100);
                            var_f0 = sp104 + 40.0f;
                            var_v0 = &sp104;
                        }
                        (*(f32 *)var_v0) = var_f0;
                        flvecrRotTransPers(&spF0, &sp100);
                        if ((spF0 < 700.0f) && !(spF0 <= -60.0f)) {
                            temp_f1 = spF4;
                            if ((temp_f1 < 500.0f) && !(temp_f1 <= -20.0f) && !(spFC <= 0.0f)) {
                                temp_s1_2 = ((s16) (temp_s1));
                                var_v0_2 = temp_s1_2 >> 1;
                                if (temp_s1_2 < 0) {
                                    var_v0_2 = (s32) (temp_s1_2 + 1) >> 1;
                                }
                                temp_f0 = spF0 - (f32) (var_v0_2 * 8);
                                spF0 = temp_f0;
                                flfntLocate((s32) (1.25f * (f32) (s32) temp_f0), (s32) spF4);
                                temp_s6 = ((s16) ((s32) (s32) (1.25f * spF0)));
                                temp_s5 = ((s16) ((s32) (s32) spF4));
                                flfntSetSize(0x10, 0x10);
                                temp_v0 = (*(s32 *)&cw) + spA0;
                                temp_v1 = (*(u8 *)((u8 *)(temp_v0) + 0x3A9));
                                temp_fp = temp_v0 + 0x3A8;
                                var_s2 = 2;
                                if ((s32) temp_v1 >= 0x1D) {

                                } else {
                                    var_s2 = 7;
                                    if ((s32) temp_v1 >= 0x15) {

                                    } else {
                                        var_s2 = 6;
                                        if ((s32) temp_v1 >= 0xD) {

                                        } else {
                                            var_s2 = 5;
                                        }
                                    }
                                }
                                font_set_stack_no(0);
                                font_print_double((s32) (1.25f * (f32) (s32) spF0), (s32) spF4, 1, var_s2, var_s0);
                                if ((((s16) (var_s4))) == (*(u8 *)((u8 *)&game_w + 0x3A))) {
                                    var_s2_2 = ((s16) (temp_s5));
                                    var_s0_2 = ((s16) (temp_s6));
                                    var_s5 = var_s2_2 - 4;
                                    Lb_put_job((s32) (((s16) ((var_s0_2 - 0x16)))), (s32) (((s16) (var_s5))), 0x16, -1, (s32) (((s8) (Get_weapon_job(&(*(s32 *)((u8 *)&User_data + 0xC74)))))), 0);
                                } else {
                                    var_s2_2 = ((s16) (temp_s5));
                                    var_s0_2 = ((s16) (temp_s6));
                                    var_s5 = var_s2_2 - 4;
                                    Lb_put_job((s32) (((s16) ((var_s0_2 - 0x16)))), (s32) (((s16) (var_s5))), 0x16, -1, (s32) (*(u8 *)((u8 *)(temp_v0) + 0x3A8)), 0);
                                }
                                temp_v1_2 = (*(u8 *)((u8 *)(temp_fp) + 0x10));
                                if (temp_v1_2 & 0xC0) {
                                    if ((temp_v1_2 & 0x40) || ((*(u16 *)&System_timer) & 0x10)) {
                                        Lb_put_icon_free((s32) (((s16) (((var_s0_2 + (temp_s1_2 * 4)) - 0xC)))), (s32) (((s16) ((var_s2_2 - 0x1A)))), 0x16, (*(s32 *)((u8 *)&lb_quest_color_tbl + ((temp_v1_2 & 0xF) * 4))), 0xF);
                                    }
                                } else if (temp_v1_2 & 0x30) {
                                    if (!(temp_v1_2 & 0x10)) {
                                        if ((*(u16 *)&System_timer) & 0x10) {
                                            goto block_55;
                                        }
                                    } else {
block_55:
                                        Lb_put_icon_free((s32) (((s16) (((var_s0_2 + (temp_s1_2 * 4)) - 0xC)))), (s32) (((s16) ((var_s2_2 - 0x1A)))), 0x16, (*(s32 *)((u8 *)&lb_quest_color_tbl + ((temp_v1_2 & 0xF) * 4))), 0xE);
                                    }
                                }
                                if (Online_ck() == 1) {
                                    Lb_put_status((s32) (((s16) ((var_s0_2 + (temp_s1_2 * 8) + 6)))), (s32) (((s16) (var_s5))), 0x14, -1, (s32) (*(u8 *)((u8 *)(temp_fp) + 2)));
                                }
                            }
                        }
                    }
                }
            }
            var_s3 += 0xA00;
            spA0 += 0x5C;
            var_s4 = ((s16) ((var_s4 + 1)));
            var_s7 += 0x5C;
        } while (var_s4 < 0xA);
        return;
    }
}

