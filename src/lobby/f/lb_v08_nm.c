/* lb_v08 - lobby player name tags 0x005CB980-0x005CBF30: lb_disp_name (name, job icon and status icon above each avatar). Rewritten from the asm. */
#include "lobby_f.h"
extern u8 lb_quest_color_tbl[];
extern u8 D_3C738C[];
extern u16 System_timer;
void font_set_stack_no();
void reload_tex();
void SetTextureStage();
void SetFilterMode();
void flSetRenderState();
void flmatInit();
void flvecrRotTransPers();
void flfntLocate();
void flfntSetSize();
void font_print_double();
int Lb_Pl_stg_ck();
int Lb_get_pl_stat2();
int Get_weapon_job();
void Lb_put_job();
void Lb_put_icon_free();
void Lb_put_status();
int strlen();
int Online_ck();
void lb_disp_name(u8 *arg0) {
    f32 sp100[3];
    f32 spF0[4];
    f32 spB0[16];
    s32 var_v0;
    u8 *var_s0;
    s32 var_s4;
    int var_s3;
    s32 var_fp;
    u8 *temp_v1;
    s32 temp_s1_2;
    s32 temp_s2;
    s32 temp_s2_2;
    s32 var_s1;
    s32 var_s0_2;
    u8 temp_v1_3;
    s32 var_s3_2;
    u8 temp_v1_2;
    u8 *spA0;
    u8 *var_s6;
    u8 *temp_s1;
    s32 temp_s5;

    spA0 = (u8 *)lb_player;
    switch (lb_sys.x68) {
    case 0:
    case 0xF:
    case 8:
        break;
    default:
        return;
    }
    font_set_stack_no(*(s32 *)(arg0 + 0x18));
    reload_tex(1, 0x157);
    SetTextureStage(0x157);
    SetFilterMode(1);
    flSetRenderState(0x60, 0);
    var_s4 = 0;
    var_s6 = (u8 *)lb_player;
    var_fp = 0;
    do {
        temp_s1 = *(u8 **)spA0;
        if (temp_s1[0] != 0 && temp_s1[1] != 0 && (Lb_Pl_stg_ck(temp_s1) & 0xFF) && Lb_get_pl_stat2((s8)var_s4) == 0 && (*(f32 *)(temp_s1 + 0xAC) != 0.0f || *(f32 *)(temp_s1 + 0xB4) != 0.0f)) {
            var_s0 = var_s6 + 4;
            if (Online_ck() == 1 && *(u8 *)0x39DAD4 != 0) {
                var_s0 = var_s6 + 0x24;
            }
            temp_s2 = (s16)strlen(var_s0);
            flmatInit(spB0);
            flSetRenderState(0x1A, spB0);
            sp100[0] = *(f32 *)(temp_s1 + 0xAC);
            sp100[1] = 190.0f + *(f32 *)(temp_s1 + 0xB0);
            sp100[2] = *(f32 *)(temp_s1 + 0xB4);
            flvecrRotTransPers(spF0, sp100);
            if (spF0[0] < 700.0f && !(spF0[0] <= -60.0f)) {
                if (spF0[1] < 500.0f && !(spF0[1] <= -20.0f) && !(spF0[3] <= 0.0f)) {
                    temp_s2_2 = (s16)temp_s2;
                    var_v0 = temp_s2_2 / 2;
                    spF0[0] -= (f32)(var_v0 * 8);
                    flfntLocate((s32)(1.25f * (f32)(s32)spF0[0]), (s32)spF0[1]);
                    temp_s5 = (s16)(s32)(1.25f * (f32)(s32)spF0[0]);
                    temp_s1_2 = (s16)(s32)spF0[1];
                    flfntSetSize(0x10, 0x10);
                    temp_v1 = cw + var_fp;
                    temp_v1_2 = temp_v1[0x1347];
                    if (temp_v1_2 == 0x14) {
                        var_s3 = 2;
                    } else if ((s32)temp_v1_2 >= 0xD) {
                        var_s3 = 6;
                    } else {
                        var_s3 = 5;
                    }
                    font_set_stack_no(0);
                    font_print_double((s32)(1.25f * (f32)(s32)spF0[0]), (s32)spF0[1], 1, var_s3, var_s0);
                    if ((s16)var_s4 == *(u8 *)0x3F34C1) {
                        s32 job = (s8)Get_weapon_job(D_3C738C);
                        if (job == 5) {
                            job = 1;
                        }
                        var_s3_2 = (s16)temp_s1_2;
                        var_s1 = (s16)temp_s5;
                        var_s0_2 = var_s3_2 - 4;
                        Lb_put_job((s16)(var_s1 - 0x16), (s16)var_s0_2, 0x16, -1, job, 0);
                    } else {
                        var_s3_2 = (s16)temp_s1_2;
                        var_s1 = (s16)temp_s5;
                        var_s0_2 = var_s3_2 - 4;
                        Lb_put_job((s16)(var_s1 - 0x16), (s16)var_s0_2, 0x16, -1, temp_v1[0x1346], 0);
                    }
                    temp_v1_3 = temp_v1[0x1346 + 0x15];
                    if (temp_v1_3 & 0xC0) {
                        if (!(temp_v1_3 & 0x40)) {
                            if (!(System_timer & 0x10)) {
                                goto next;
                            }
                        }
                        Lb_put_icon_free((s16)(var_s1 + temp_s2_2 * 4 - 0xC), (s16)(var_s3_2 - 0x1A), 0x16, *(s32 *)(lb_quest_color_tbl + (temp_v1_3 & 0xF) * 4), 0xF);
                    } else if (temp_v1_3 & 0x30) {
                        if ((temp_v1_3 & 0x10) || (System_timer & 0x10)) {
                            Lb_put_icon_free((s16)(var_s1 + temp_s2_2 * 4 - 0xC), (s16)(var_s3_2 - 0x1A), 0x16, *(s32 *)(lb_quest_color_tbl + (temp_v1_3 & 0xF) * 4), 0xE);
                        }
                    } else {
                        Lb_put_status((s16)(var_s1 + temp_s2_2 * 8 + 6), (s16)var_s0_2, 0x14, -1, temp_v1[0x1346 + 2]);
                    }
                }
            }
        }
next:
        var_s6 += 0x38;
        var_s4 = (s16)(var_s4 + 1);
        spA0 += 0x38;
        var_fp += 0x2FC;
    } while (var_s4 < 8);
}
