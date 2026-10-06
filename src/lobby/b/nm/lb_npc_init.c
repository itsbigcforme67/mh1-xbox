#include "lobby_a.h"
extern char npc_disp_parts_00647910[];
extern char npc_disp_parts_00647910[];
extern char lb_npc_jijii_tbl[];
void lb_npc_init(int arg0) {
    int var_a1;
    int var_a1_2;
    int var_a1_3;
    s16 temp_v1_2;
    s32 temp_a1;
    s32 temp_v1_3;
    s32 temp_v1_4;
    s32 var_a0;
    int var_v0_2;
    int var_a0_2;
    u16 var_v0;
    u8 temp_v0;
    int temp_s0;
    int temp_v0_2;
    int temp_v0_3;
    int temp_v1;

    temp_s0 = arg0 + 0x444;
    lb_set_npc();
    em_work_set();
    F(u8, arg0, 4) = (u8) (F(u8, arg0, 4) + 1);
    F(s8, arg0, 0x10) = 0;
    F(s8, arg0, 0x1E) = 1;
    F(s8, arg0, 0x412) = 0;
    F(s8, arg0, 1) = 1;
    F(s32, arg0, 0x1A0) = 0x40000000;
    F(s32, arg0, 0x1F0) = 0x40000000;
    F(s32, arg0, 0x240) = 0x40000000;
    F(s32, arg0, 0x290) = 0x40000000;
    F(s32, arg0, 0x39C) = 0;
    F(u8, arg0, 0x736) = (u8) game_w.stage;
    if (F(u8, arg0, 2) == 0) {
        F(s16, arg0, 0x300) = 2;
    } else {
        F(s8, arg0, 0x10) = 1;
        F(s16, arg0, 0x300) = 1;
    }
    F(s32, arg0, 0x798) = 0x3F800000;
    var_a0 = 0;
    F(s8, arg0, 0x7D6) = 0;
    F(s8, arg0, 0x4D4) = 1;
    F(s16, arg0, 0x2DC) = 0;
    F(s16, arg0, 0x2DE) = 0;
    F(s16, arg0, 0x2E0) = 0;
    F(s16, arg0, 0x2E2) = 0;
    do {
        temp_v1 = arg0 + var_a0;
        F(s8, temp_v1, 0x4E6) = 0;
        var_a0 += 8;
        F(s8, temp_v1, 0x4E7) = 0;
        F(s8, temp_v1, 0x4E8) = 0;
        F(s8, temp_v1, 0x4E9) = 0;
        F(s8, temp_v1, 0x4EA) = 0;
        F(s8, temp_v1, 0x4EB) = 0;
        F(s8, temp_v1, 0x4EC) = 0;
        F(s8, temp_v1, 0x4ED) = 0;
    } while (var_a0 < 0x20);
    temp_v0 = F(u8, arg0, 2);
    switch (temp_v0) {                              /* switch 1; irregular */
    case 0:                                         /* switch 1 */
        temp_a1 = F(u8, arg0, 0x1B) * 4;
        F(s8, ((*(int *)((u8 *)&npc_disp_parts_00647910 + temp_a1)) + arg0), 0x4E6) = 1;
        temp_v1_2 = (*(int *)((u8 *)&npc_disp_parts_00647910 + 2 + (F(u8, arg0, 0x1B) * 4)));
        if (temp_v1_2 != 0xFF) {
            F(s8, (temp_v1_2 + arg0), 0x4E6) = 1;
        }
        set_se_type(arg0, temp_a1, 1);
        break;
    case 1:                                         /* switch 1 */
        var_a0_2 = ((int *)&lb_npc_jijii_tbl)[F(u8, arg0, 0x1B)];
        var_v0 = (*(u16 *)var_a0_2);
        if (var_v0 != 0xFFFF) {
            do {
                var_a0_2 += 2;
                F(s8, (arg0 + (var_v0 & 0xFFFF)), 0x4E6) = 1;
                var_v0 = (*(u16 *)var_a0_2);
            } while (var_v0 != 0xFFFF);
        }
        break;
    }
    lb_npc_init_sub();
    if (F(u8, arg0, 2) == 0) {
        temp_v0_2 = F(int, temp_s0, 8);
        if (temp_v0_2 == 0) {
            if (F(u8, temp_s0, 0xE) != 4) {
                var_a1 = 1;
            } else {
                var_a1 = 0x2A8;
            }
            Lb_pl_chr_set(arg0, var_a1, 0, 0);
        } else {
            temp_v1_3 = F(s32, temp_v0_2, 0xC);
            switch (temp_v1_3) {                    /* switch 2; irregular */
            default:                                /* switch 2 */
                var_a1_2 = 1;
                break;
            case 0x77:                              /* switch 2 */
                var_a1_2 = 0x2A4;
                break;
            case 0x79:                              /* switch 2 */
                var_a1_2 = 0x291;
                break;
            case 0x6A:                              /* switch 2 */
                var_a1_2 = 0x29E;
                break;
            case 0x6B:                              /* switch 2 */
                var_a1_2 = 0x2A0;
                break;
            case 0x6D:                              /* switch 2 */
                var_a1_2 = 0x29C;
                break;
            case 0x7D:                              /* switch 2 */
            case 0x7C:                              /* switch 2 */
                var_a1_2 = 0x295;
                break;
            case 0x7A:                              /* switch 2 */
                var_a1_2 = 0x290;
                break;
            case 0x6F:                              /* switch 2 */
                var_a1_2 = 0x296;
                break;
            case 0x70:                              /* switch 2 */
                var_a1_2 = 0x298;
                break;
            case 0x73:                              /* switch 2 */
                var_a1_2 = 0x261;
                break;
            case 0x7E:                              /* switch 2 */
                var_a1_2 = 0x2A6;
                break;
            case 0x7F:                              /* switch 2 */
                var_a1_2 = 0x27E;
                break;
            case 0x80:                              /* switch 2 */
                var_a1_2 = 0x285;
                break;
            case 0x81:                              /* switch 2 */
                var_a1_2 = 0x284;
                break;
            }
            Lb_pl_chr_set(arg0, var_a1_2, 0, 0);
        }
        frame_init(arg0, F(u16, arg0, 0x2E4), F(s16, arg0, 0x2EC), 0);
        frame_init(arg0, F(u16, arg0, 0x2E6), F(s16, arg0, 0x2EE), 1);
    } else {
        temp_v0_3 = F(int, temp_s0, 8);
        if (temp_v0_3 == 0) {
            if (F(u8, temp_s0, 0xE) != 0x48) {
                var_a1_3 = 0x3E9;
            } else {
                var_a1_3 = 0x3FF;
            }
        } else {
            temp_v1_4 = F(s32, temp_v0_3, 0xC);
            switch (temp_v1_4) {                    /* switch 3; irregular */
            case 0x82:                              /* switch 3 */
                var_a1_3 = 0x426;
                break;
            case 0x86:                              /* switch 3 */
                var_a1_3 = 0x432;
                break;
            case 0x8B:                              /* switch 3 */
                var_v0_2 =  ((game_w.stage - 0x51) << 0x30) >> 0x30;
                if ((var_v0_2 < 0) || (var_v0_2 >= 0x56)) {
                    var_v0_2 = 0;
                }
                if ((*(s8 *)((u8 *)&lb_sys + 0x88 + ( (var_v0_2 << 0x30) >> 0x30))) == 2) {
                    Lb_act_set(arg0, 0, 0x8C);
                    var_a1_3 = 0x3E9;
                } else {
                    var_a1_3 = 0x3E9;
                }
                break;
            default:                                /* switch 3 */
                var_a1_3 = 0x3E9;
                break;
            }
        }
        Lb_pl_chr_set0(arg0, var_a1_3, 0, 0);
        frame_init(arg0, F(u16, arg0, 0x2E4), F(s16, arg0, 0x2EC), 0);
    }
    frame_move();
    lb_npc_chr_sub();
    lb_npc_chr_sub();
    Lb_World_calc();
}
