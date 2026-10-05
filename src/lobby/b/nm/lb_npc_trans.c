#include "lobby_a.h"
void lb_npc_trans(int arg0) {
    int sp160;
    int sp120;
    int spE0;
    int spA0;
    int temp_s3;
    s16 temp_s7;
    s32 temp_fp;
    s32 var_s5;
    int var_s6;
    u8 temp_v0;
    u8 temp_v1;
    int temp_s0;
    int temp_s1;
    int var_s1;
    int var_s4;

    temp_s0 = F(int, arg0, 0x18);
    temp_s1 = F(int, temp_s0, 0x50C);
    if (F(u8, temp_s0, 0) != 0) {
        if (F(u8, temp_s0, 1) == 0) {

        } else {
            pl_light_change(temp_s0, 1);
            Pl_light_set(temp_s0);
            cpAng2Rad_all(temp_s0 + 0xA0, &sp160);
            flmatMakeScale(F(f32, temp_s0, 0xB8), F(f32, temp_s0, 0xC0), &spA0);
            cpRotMatrixYXZ2(temp_s0 + 0xA0, &sp120);
            flmatSetTrans(F(f32, temp_s0, 0xAC), F(f32, temp_s0, 0xB4), &sp120);
            flmatMul33_2(&sp120, &spA0);
            flmatCopy(temp_s0 + 0x60, &sp120);
            SetFilterMode(1);
            flSetRenderState(0x60, 0);
            var_s4 = F(int, temp_s1, 0x30);
            temp_s7 = F(s16, temp_s1, 0x2C);
            temp_fp = F(s32, temp_s1, 0x10);
            flSetSkinTrans(F(s32, temp_s1, 0x24));
            flmatInit(&spE0);
            flSetRenderState(0x19, &spE0);
            reload_tex(0xA, (F(u8, temp_s0, 2) * 0x14) + 0x9A);
            var_s6 = 0;
            if (temp_s7 > 0) {
                do {
                    if ((F(u8, temp_s0, 2) != 0) || (F(s8, (temp_s0 + var_s6), 0x4E6) != 0)) {
                        flSetRenderState(0x67, (int *)-1);
                        if (F(s32, var_s4, 0) != -1) {
                            var_s5 = 0;
                            if (F(s32, var_s4, 4) > 0) {
                                var_s1 = var_s4;
                                do {
                                    temp_v1 = F(u8, temp_s0, 2);
                                    temp_s3 = temp_fp + (F(s32, var_s1, 8) * 0x4C);
                                    switch (temp_v1) { /* switch 1; irregular */
                                    case 1:         /* switch 1 */
                                        lb_normal_material(temp_s0, temp_s3, var_s5,  (var_s6 << 0x30) >> 0x30);
                                        if (F(s8, (temp_s0 + var_s5), 0x4E6) == 0) {
block_44:
                                            F(int, temp_s3, 0x10) = 0;
                                        } else {
                                            F(int, temp_s3, 0x10) = (int) F(int, temp_s0, 0x798);
                                        }
                                        break;
                                    case 2:         /* switch 1 */
                                        F(int, temp_s3, 0x10) = (int) F(int, temp_s0, 0x798);
                                        lb_cat_material(temp_s0, temp_s3, var_s5);
                                        break;
                                    case 0:         /* switch 1 */
                                        F(int, temp_s3, 0x10) = (int) F(int, temp_s0, 0x798);
                                        lb_normal_material(temp_s0, temp_s3, var_s5,  (var_s6 << 0x30) >> 0x30);
                                        temp_v0 = F(u8, (temp_s0 + 0x444), 0xE);
                                        switch (temp_v0) { /* switch 2; irregular */
                                        case 0x33:  /* switch 2 */
                                        case 0x2F:  /* switch 2 */
                                        case 0x2D:  /* switch 2 */
                                        case 0x8:   /* switch 2 */
                                        case 0xB:   /* switch 2 */
                                            if ((var_s6 == 1) && (var_s5 == 1)) {
                                                goto block_44;
                                            }
                                            break;
                                        case 0x3A:  /* switch 2 */
                                        case 0x36:  /* switch 2 */
                                        case 0x35:  /* switch 2 */
                                        case 0x34:  /* switch 2 */
                                        case 0x30:  /* switch 2 */
                                        case 0x2E:  /* switch 2 */
                                        case 0x2B:  /* switch 2 */
                                        case 0x1D:  /* switch 2 */
                                        case 0x16:  /* switch 2 */
                                        case 0x15:  /* switch 2 */
                                        case 0x14:  /* switch 2 */
                                        case 0x17:  /* switch 2 */
                                        case 0x13:  /* switch 2 */
                                            if ((var_s6 == 6) && (var_s5 == 3)) {
                                                goto block_44;
                                            }
                                            break;
                                        }
                                        break;
                                    }
                                    flSetRenderState((var_s5 + 0x3A) & 0xFF, temp_s3);
                                    var_s5 += 1;
                                    var_s1 += 4;
                                } while (var_s5 < F(s32, var_s4, 4));
                            }
                            clay_attr_set(F(s32, var_s4, 0x88));
                            flExecuteClay(F(s32, var_s4, 0), 0);
                        }
                    }
                    var_s6 += 1;
                    var_s4 += 0x8C;
                } while (var_s6 < temp_s7);
            }
            clay_attr_reset();
            lb_npc_item_trans(temp_s0);
            flSetRenderState(0x60, 0);
        }
    }
}
