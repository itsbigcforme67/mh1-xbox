/* emmat_nm - SLPM_654.95 0x0010CEA0-0x0010EC10: em_material_sub, per monster kind the material alphas (and a few
 * colours) of one model part (clay) each frame: EMW+0x798 as the base alpha, 0 for the materials hidden by the monster's
 * state (blinking eyes, broken or cut parts, ...). Called per clay by enemy_trans.
 * NEAR-MATCH, not compiled for the PS2 (m2c output of the asm, cleaned only as far as gcc needs). Behaviour read from the
 * draft; the byte-exact version is still to do (7500 bytes, 25 monster kinds).
 * arg0 = the monster (EMW), arg1 = clay index, arg2 = the clay descriptor table (0x8C bytes each: +4 material count,
 * +8.. material indices into the model's material array at mdl+0x10, 0x4C bytes each; +0x10 of a material is its alpha). */
#include "types.h"

typedef long long s64;
typedef double f64;
typedef s32 M2C_UNK;
typedef s32 M2C_UNK32;
#define M2C_FIELD(expr, type_ptr, offset) (*(type_ptr)((s8 *)(expr) + (offset)))
#define M2C_BITWISE(type, expr) ((type)(expr))

s32 em_frame_check2(void *, int, f32);
#ifdef __MWERKS__
M2C_UNK flSetRenderState(s32, void *);
#else
#define flSetRenderState(a, b) ((void)0)   /* PC: the host draws; the caller reads the alphas this writes */
#endif
extern u16 System_timer;
extern M2C_UNK game_w;
extern M2C_UNK mem_tex;

void em_material_sub(u8 *arg0, s32 arg1, u8 *arg2) {
    f32 temp_f20;
    f32 temp_f21;
    s16 temp_v0;
    s32 *temp_s0;
    s32 *temp_s1;
    s32 *temp_s1_10;
    s32 *temp_s1_11;
    s32 *temp_s1_13;
    s32 *temp_s1_2;
    s32 *temp_s1_3;
    s32 *temp_s1_4;
    s32 *temp_s1_5;
    s32 *temp_s1_6;
    s32 *temp_s1_8;
    s32 *temp_s1_9;
    s32 *temp_s3;
    s32 *temp_s6;
    s32 *temp_s7;
    s32 *temp_s7_2;
    u8 *temp_s2;
    s32 var_a0;
    s32 var_s0;
    s32 var_s0_10;
    s32 var_s0_11;
    s32 var_s0_12;
    s32 var_s0_13;
    s32 var_s0_2;
    s32 var_s0_3;
    s32 var_s0_4;
    s32 var_s0_5;
    s32 var_s0_6;
    s32 var_s0_7;
    s32 var_s0_8;
    s32 var_s0_9;
    s32 var_s1_2;
    s32 var_s6;
    s32 var_s6_4;
    s32 var_s6_5;
    s64 temp_s3_2;
    s64 temp_s3_3;
    s64 temp_s3_4;
    s64 var_v1_2;
    s8 temp_a2_2;
    u16 temp_a0_2;
    u16 temp_v1_2;
    u32 var_s1_3;
    u8 temp_a2;
    u8 temp_v0_2;
    u8 temp_v0_3;
    u8 temp_v0_4;
    u8 temp_v0_5;
    u8 temp_v1;
    u8 temp_v1_3;
    u8 temp_v1_4;
    u8 var_v1;
    u8 *temp_a0;
    u8 *temp_a1;
    u8 *temp_a1_10;
    u8 *temp_a1_11;
    u8 *temp_a1_12;
    u8 *temp_a1_13;
    u8 *temp_a1_14;
    u8 *temp_a1_15;
    u8 *temp_a1_2;
    u8 *temp_a1_3;
    u8 *temp_a1_4;
    u8 *temp_a1_5;
    u8 *temp_a1_6;
    u8 *temp_a1_7;
    u8 *temp_a1_8;
    u8 *temp_a1_9;
    u8 *temp_s0_2;
    u8 *temp_s0_3;
    u8 *temp_s1_12;
    u8 *temp_s1_7;
    u8 *var_s1;
    u8 *var_s3;
    u8 *var_s3_10;
    u8 *var_s3_11;
    u8 *var_s3_2;
    u8 *var_s3_3;
    u8 *var_s3_4;
    u8 *var_s3_5;
    u8 *var_s3_6;
    u8 *var_s3_7;
    u8 *var_s3_8;
    u8 *var_s3_9;
    u8 *var_s4;
    u8 *var_s4_2;
    u8 *var_s6_2;
    u8 *var_s6_3;

    temp_a2 = M2C_FIELD(arg0, u8 *, 2);
    temp_s2 = (u8 *)M2C_FIELD(M2C_FIELD(arg0, void **, 0x50C), s32 *, 0x10);
    switch (temp_a2) {                              /* switch 1 */
    case 1:                                         /* switch 1 */
        var_s1 = (arg1 * 0x8C) + arg2;
        var_s6 = 0;
        if (M2C_FIELD(var_s1, s32 *, 4) > 0) {
            temp_s0 = (s32 *)(var_s1 + 4);
            do {
                temp_a1 = temp_s2 + (M2C_FIELD(var_s1, s32 *, 8) * 0x4C);
                M2C_FIELD(temp_a1, M2C_UNK32 *, 0x10) = (M2C_UNK32) M2C_FIELD(arg0, M2C_UNK32 *, 0x798);
                switch (arg1) {                     /* switch 2; irregular */
                case 0:                             /* switch 2 */
                    switch (var_s6) {               /* switch 3; irregular */
                    case 0:                         /* switch 3 */
                        if ((s32) M2C_FIELD(arg0, u8 *, 0x312) > 0) {
                            M2C_FIELD(temp_a1, M2C_UNK32 *, 0x10) = 0;
                        }
                        break;
                    case 1:                         /* switch 3 */
                        if ((s32) M2C_FIELD(arg0, u8 *, 0x31A) > 0) {
                            M2C_FIELD(temp_a1, M2C_UNK32 *, 0x10) = 0;
                        }
                        break;
                    case 5:                         /* switch 3 */
                        M2C_FIELD(temp_a1, M2C_UNK32 *, 0x10) = 0;
                        break;
                    case 6:                         /* switch 3 */
                        if (!(M2C_FIELD(arg0, u8 *, 0x948) & 1)) {
                            M2C_FIELD(temp_a1, M2C_UNK32 *, 0x10) = 0;
                        }
                        break;
                    }
                    break;
                case 1:                             /* switch 2 */
                    if (var_s6 != 1) {

                    } else if (!(M2C_FIELD(arg0, u8 *, 0x948) & 1)) {
                        M2C_FIELD(temp_a1, M2C_UNK32 *, 0x10) = 0;
                    }
                    break;
                case 2:                             /* switch 2 */
                    switch (var_s6) {               /* switch 4; irregular */
                    case 2:                         /* switch 4 */
                    case 1:                         /* switch 4 */
                    case 0:                         /* switch 4 */
                        break;
                    case 3:                         /* switch 4 */
                        M2C_FIELD(temp_a1, M2C_UNK32 *, 0x10) = 0;
                        break;
                    }
                    break;
                case 3:                             /* switch 2 */
                    switch (var_s6) {               /* switch 5; irregular */
                    case 1:                         /* switch 5 */
                    case 0:                         /* switch 5 */
                        if ((s32) M2C_FIELD(arg0, u8 *, 0x33A) < 2) {
                            M2C_FIELD(temp_a1, M2C_UNK32 *, 0x10) = 0;
                        }
                        break;
                    case 2:                         /* switch 5 */
                        if (M2C_FIELD(arg0, u8 *, 0x31A) == 0) {
                            M2C_FIELD(temp_a1, M2C_UNK32 *, 0x10) = 0;
                        }
                        break;
                    case 3:                         /* switch 5 */
                        if (M2C_FIELD(arg0, u8 *, 0x312) == 0) {
                            M2C_FIELD(temp_a1, M2C_UNK32 *, 0x10) = 0;
                        }
                        break;
                    }
                    break;
                case 4:                             /* switch 2 */
                    switch (var_s6) {               /* switch 6; irregular */
                    case 1:                         /* switch 6 */
                        break;
                    case 0:                         /* switch 6 */
                        if ((s32) M2C_FIELD(arg0, u8 *, 0x33A) >= 2) {
                            M2C_FIELD(temp_a1, M2C_UNK32 *, 0x10) = 0;
                        }
                        break;
                    }
                    break;
                }
                flSetRenderState((var_s6 + 0x3A) & 0xFF, temp_a1);
                var_s6 += 1;
                var_s1 += 4;
            } while (var_s6 < *temp_s0);
            return;
        }
        return;
    case 2:                                         /* switch 1 */
        temp_s0_2 = arg0 + 0x444;
        var_s6_2 = (arg1 * 0x8C) + arg2;
        var_s1_2 = 0;
        if (M2C_FIELD(var_s6_2, s32 *, 4) > 0) {
            temp_s3 = (s32 *)(var_s6_2 + 4);
            do {
                temp_a1_2 = temp_s2 + (M2C_FIELD(var_s6_2, s32 *, 8) * 0x4C);
                M2C_FIELD(temp_a1_2, M2C_UNK32 *, 0x10) = (M2C_UNK32) M2C_FIELD(arg0, M2C_UNK32 *, 0x798);
                switch (arg1) {                     /* switch 7; irregular */
                case 1:                             /* switch 7 */
                    switch (var_s1_2) {             /* switch 8; irregular */
                    case 1:                         /* switch 8 */
                    case 0:                         /* switch 8 */
                        if (M2C_FIELD(temp_s0_2, s16 *, 0x52) < 0x6401) {
                            M2C_FIELD(temp_a1_2, M2C_UNK32 *, 0x10) = 0;
                        }
                        break;
                    case 2:                         /* switch 8 */
                        if (M2C_FIELD(temp_s0_2, s16 *, 0x52) < 0x4B01) {
                            M2C_FIELD(temp_a1_2, M2C_UNK32 *, 0x10) = 0;
                        }
                        break;
                    case 3:                         /* switch 8 */
                        if (M2C_FIELD(temp_s0_2, s16 *, 0x52) < 0x3201) {
                            M2C_FIELD(temp_a1_2, M2C_UNK32 *, 0x10) = 0;
                        }
                        break;
                    }
                    break;
                case 3:                             /* switch 7 */
                    switch (var_s1_2) {             /* switch 9; irregular */
                    case 5:                         /* switch 9 */
                    case 1:                         /* switch 9 */
                        var_a0 = 1;
                        if (M2C_FIELD(temp_s0_2, s16 *, 0x52) < 0x1901) {
                            var_a0 = 2;
                        }
                        M2C_FIELD(temp_a1_2, s32 *, 0x44) = (s32) *(&mem_tex + ((M2C_FIELD(arg0, u8 *, 0x34F) + 0x9A + var_a0) * 4));
                        break;
                    case 8:                         /* switch 9 */
                    case 3:                         /* switch 9 */
                        if (M2C_FIELD(temp_s0_2, s16 *, 0x52) < 0x1901) {
                            M2C_FIELD(temp_a1_2, M2C_UNK32 *, 0x10) = 0;
                        }
                        break;
                    }
                    break;
                case 4:                             /* switch 7 */
                    switch (var_s1_2) {             /* switch 10; irregular */
                    case 1:                         /* switch 10 */
                        if (M2C_FIELD(temp_s0_2, s16 *, 0x52) >= 0x6401) {
                            M2C_FIELD(temp_a1_2, M2C_UNK32 *, 0x10) = 0;
                        }
                        break;
                    case 2:                         /* switch 10 */
                        temp_v0 = M2C_FIELD(temp_s0_2, s16 *, 0x52);
                        if (temp_v0 < 0x4B01) {
                            if (temp_v0 < 0x3201) {
                                goto block_100;
                            }
                        } else {
block_100:
                            M2C_FIELD(temp_a1_2, M2C_UNK32 *, 0x10) = 0;
                        }
                        break;
                    case 3:                         /* switch 10 */
                        if (M2C_FIELD(temp_s0_2, s16 *, 0x52) >= 0x3201) {
                            M2C_FIELD(temp_a1_2, M2C_UNK32 *, 0x10) = 0;
                        }
                        break;
                    case 0:                         /* switch 10 */
                        if (M2C_FIELD(temp_s0_2, s16 *, 0x52) >= 0x1901) {
                            M2C_FIELD(temp_a1_2, M2C_UNK32 *, 0x10) = 0;
                        }
                        break;
                    }
                    break;
                }
                flSetRenderState((var_s1_2 + 0x3A) & 0xFF, temp_a1_2);
                var_s1_2 += 1;
                var_s6_2 += 4;
            } while (var_s1_2 < *temp_s3);
            return;
        }
        break;
    case 6:                                         /* switch 1 */
        var_s3 = (arg1 * 0x8C) + arg2;
        var_s0 = 0;
        if (M2C_FIELD(var_s3, s32 *, 4) > 0) {
            temp_s1 = (s32 *)(var_s3 + 4);
            do {
                temp_a1_3 = temp_s2 + (M2C_FIELD(var_s3, s32 *, 8) * 0x4C);
                M2C_FIELD(temp_a1_3, M2C_UNK32 *, 0x10) = (M2C_UNK32) M2C_FIELD(arg0, M2C_UNK32 *, 0x798);
                switch (arg1) {                     /* switch 11; irregular */
                case 0:                             /* switch 11 */
                    switch (var_s0) {               /* switch 12; irregular */
                    case 5:                         /* switch 12 */
                        if (!(M2C_FIELD(arg0, u8 *, 0x948) & 1)) {
                            M2C_FIELD(temp_a1_3, M2C_UNK32 *, 0x10) = 0;
                        }
                        break;
                    case 6:                         /* switch 12 */
                        M2C_FIELD(temp_a1_3, M2C_UNK32 *, 0x10) = 0;
                        break;
                    }
                    break;
                case 1:                             /* switch 11 */
                    switch (var_s0) {               /* switch 13; irregular */
                    case 0:                         /* switch 13 */
                        break;
                    case 1:                         /* switch 13 */
                        M2C_FIELD(temp_a1_3, M2C_UNK32 *, 0x10) = 0;
                        break;
                    case 3:                         /* switch 13 */
                    case 2:                         /* switch 13 */
                        if (!(M2C_FIELD(arg0, u8 *, 0x948) & 1)) {
                            M2C_FIELD(temp_a1_3, M2C_UNK32 *, 0x10) = 0;
                        }
                        break;
                    }
                    break;
                case 2:                             /* switch 11 */
                    if (var_s0 != 0) {

                    } else {
                        M2C_FIELD(temp_a1_3, M2C_UNK32 *, 0x10) = 0;
                    }
                    break;
                }
                flSetRenderState((var_s0 + 0x3A) & 0xFF, temp_a1_3);
                var_s0 += 1;
                var_s3 += 4;
            } while (var_s0 < *temp_s1);
            return;
        }
        break;
    case 7:                                         /* switch 1 */
        var_s3_2 = (arg1 * 0x8C) + arg2;
        var_s0_2 = 0;
        if (M2C_FIELD(var_s3_2, s32 *, 4) > 0) {
            temp_s1_2 = (s32 *)(var_s3_2 + 4);
            do {
                temp_a1_4 = temp_s2 + (M2C_FIELD(var_s3_2, s32 *, 8) * 0x4C);
                M2C_FIELD(temp_a1_4, M2C_UNK32 *, 0x10) = (M2C_UNK32) M2C_FIELD(arg0, M2C_UNK32 *, 0x798);
                switch (arg1) {                     /* switch 14; irregular */
                case 2:                             /* switch 14 */
                    switch (var_s0_2) {             /* switch 15; irregular */
                    case 0:                         /* switch 15 */
                        if ((s32) M2C_FIELD(arg0, u8 *, 0x33A) >= 2) {
                            M2C_FIELD(temp_a1_4, M2C_UNK32 *, 0x10) = 0;
                        }
                        break;
                    case 1:                         /* switch 15 */
                        if ((s32) M2C_FIELD(arg0, u8 *, 0x332) >= 2) {
                            M2C_FIELD(temp_a1_4, M2C_UNK32 *, 0x10) = 0;
                        }
                        break;
                    case 2:                         /* switch 15 */
                        if ((s32) M2C_FIELD(arg0, u8 *, 0x30A) >= 2) {
                            M2C_FIELD(temp_a1_4, M2C_UNK32 *, 0x10) = 0;
                        }
                        break;
                    case 3:                         /* switch 15 */
                        if ((s32) M2C_FIELD(arg0, u8 *, 0x30A) > 0) {
                            M2C_FIELD(temp_a1_4, M2C_UNK32 *, 0x10) = 0;
                        }
                        break;
                    }
                    break;
                case 4:                             /* switch 14 */
                    if (var_s0_2 != 0) {

                    } else if ((s32) M2C_FIELD(arg0, u8 *, 0x32A) >= 3) {
                        M2C_FIELD(temp_a1_4, M2C_UNK32 *, 0x10) = 0;
                    }
                    break;
                case 5:                             /* switch 14 */
                    switch (var_s0_2) {             /* switch 16; irregular */
                    case 0:                         /* switch 16 */
                        if ((s32) M2C_FIELD(arg0, u8 *, 0x33A) < 2) {
                            M2C_FIELD(temp_a1_4, M2C_UNK32 *, 0x10) = 0;
                        }
                        break;
                    case 1:                         /* switch 16 */
                        if ((s32) M2C_FIELD(arg0, u8 *, 0x332) < 2) {
                            M2C_FIELD(temp_a1_4, M2C_UNK32 *, 0x10) = 0;
                        }
                        break;
                    case 2:                         /* switch 16 */
                        if ((s32) M2C_FIELD(arg0, u8 *, 0x30A) < 2) {
                            M2C_FIELD(temp_a1_4, M2C_UNK32 *, 0x10) = 0;
                        }
                        break;
                    case 3:                         /* switch 16 */
                        if ((s32) M2C_FIELD(arg0, u8 *, 0x30A) <= 0) {
                            M2C_FIELD(temp_a1_4, M2C_UNK32 *, 0x10) = 0;
                        }
                        break;
                    case 4:                         /* switch 16 */
                        if ((s32) M2C_FIELD(arg0, u8 *, 0x32A) < 3) {
                            M2C_FIELD(temp_a1_4, M2C_UNK32 *, 0x10) = 0;
                        }
                        break;
                    }
                    break;
                }
                flSetRenderState((var_s0_2 + 0x3A) & 0xFF, temp_a1_4);
                var_s0_2 += 1;
                var_s3_2 += 4;
            } while (var_s0_2 < *temp_s1_2);
            return;
        }
        break;
    case 8:                                         /* switch 1 */
    case 34:                                        /* switch 1 */
        var_s3_3 = (arg1 * 0x8C) + arg2;
        var_s0_3 = 0;
        if (M2C_FIELD(var_s3_3, s32 *, 4) > 0) {
            temp_s1_3 = (s32 *)(var_s3_3 + 4);
            do {
                temp_a1_5 = temp_s2 + (M2C_FIELD(var_s3_3, s32 *, 8) * 0x4C);
                if (M2C_FIELD(arg0, u8 *, 2) == 8) {
                    M2C_FIELD(temp_a1_5, s32 *, 4) = 0x3ECACACB;
                    M2C_FIELD(temp_a1_5, s32 *, 8) = 0x3EC0C0C1;
                    M2C_FIELD(temp_a1_5, s32 *, 0xC) = 0x3E828283;
                }
                M2C_FIELD(temp_a1_5, M2C_UNK32 *, 0x10) = (M2C_UNK32) M2C_FIELD(arg0, M2C_UNK32 *, 0x798);
                switch (arg1) {                     /* switch 17; irregular */
                case 0:                             /* switch 17 */
                    switch (var_s0_3) {             /* switch 18; irregular */
                    case 2:                         /* switch 18 */
                        M2C_FIELD(temp_a1_5, M2C_UNK32 *, 0x10) = 0;
                        break;
                    case 3:                         /* switch 18 */
                        if (!(M2C_FIELD(arg0, u8 *, 0x948) & 1)) {
                            M2C_FIELD(temp_a1_5, M2C_UNK32 *, 0x10) = 0;
                        }
                        break;
                    }
                    break;
                case 1:                             /* switch 17 */
                    if (var_s0_3 != 2) {

                    } else if (!(M2C_FIELD(arg0, u8 *, 0x948) & 1)) {
                        M2C_FIELD(temp_a1_5, M2C_UNK32 *, 0x10) = 0;
                    }
                    break;
                case 2:                             /* switch 17 */
                    if (var_s0_3 != 2) {

                    } else {
                        M2C_FIELD(temp_a1_5, M2C_UNK32 *, 0x10) = 0;
                    }
                    break;
                }
                flSetRenderState((var_s0_3 + 0x3A) & 0xFF, temp_a1_5);
                var_s0_3 += 1;
                var_s3_3 += 4;
            } while (var_s0_3 < *temp_s1_3);
            return;
        }
        break;
    case 9:                                         /* switch 1 */
    case 18:                                        /* switch 1 */
    case 23:                                        /* switch 1 */
        var_s0_4 = 0;
        var_s6_3 = (arg1 * 0x8C) + arg2;
        if (M2C_FIELD(var_s6_3, s32 *, 4) > 0) {
            temp_s1_4 = (s32 *)(var_s6_3 + 4);
            temp_s3_2 = (s64) (((s64) ((s64) ((s32) ((s32) System_timer >> 1) % 10) << 0x30) >> 0x30) << 0x30) >> 0x30;
            do {
                temp_a1_6 = temp_s2 + (M2C_FIELD(var_s6_3, s32 *, 8) * 0x4C);
                M2C_FIELD(temp_a1_6, M2C_UNK32 *, 0x10) = (M2C_UNK32) M2C_FIELD(arg0, M2C_UNK32 *, 0x798);
                if ((arg1 != 1) && (arg1 != 0)) {

                } else if (temp_s3_2 != 9) {
                    if (temp_s3_2 != 8) {
                        if ((var_s0_4 != 4) && (var_s0_4 != 3) && (var_s0_4 != 2)) {

                        } else {
                            M2C_FIELD(temp_a1_6, M2C_UNK32 *, 0x10) = 0;
                        }
                    } else if ((var_s0_4 != 4) && (var_s0_4 != 3) && (var_s0_4 != 1)) {

                    } else {
                        M2C_FIELD(temp_a1_6, M2C_UNK32 *, 0x10) = 0;
                    }
                } else if ((var_s0_4 != 4) && (var_s0_4 != 2) && (var_s0_4 != 1)) {

                } else {
                    M2C_FIELD(temp_a1_6, M2C_UNK32 *, 0x10) = 0;
                }
                flSetRenderState((var_s0_4 + 0x3A) & 0xFF, temp_a1_6);
                var_s0_4 += 1;
                var_s6_3 += 4;
            } while (var_s0_4 < *temp_s1_4);
            return;
        }
        break;
    case 11:                                        /* switch 1 */
        var_s3_4 = (arg1 * 0x8C) + arg2;
        var_s0_5 = 0;
        if (M2C_FIELD(var_s3_4, s32 *, 4) > 0) {
            temp_s1_5 = (s32 *)(var_s3_4 + 4);
            do {
                temp_a1_7 = temp_s2 + (M2C_FIELD(var_s3_4, s32 *, 8) * 0x4C);
                M2C_FIELD(temp_a1_7, M2C_UNK32 *, 0x10) = (M2C_UNK32) M2C_FIELD(arg0, M2C_UNK32 *, 0x798);
                switch (arg1) {                     /* switch 19; irregular */
                case 0:                             /* switch 19 */
                    switch (var_s0_5) {             /* switch 20; irregular */
                    case 3:                         /* switch 20 */
                        if ((s32) M2C_FIELD(arg0, u8 *, 0x312) > 0) {
                            M2C_FIELD(temp_a1_7, M2C_UNK32 *, 0x10) = 0;
                        }
                        break;
                    case 4:                         /* switch 20 */
                        if ((s32) M2C_FIELD(arg0, u8 *, 0x31A) > 0) {
                            M2C_FIELD(temp_a1_7, M2C_UNK32 *, 0x10) = 0;
                        }
                        break;
                    }
                    break;
                case 1:                             /* switch 19 */
                    if (var_s0_5 != 1) {

                    } else if (!(M2C_FIELD(arg0, u8 *, 0x948) & 1)) {
                        M2C_FIELD(temp_a1_7, M2C_UNK32 *, 0x10) = 0;
                    }
                    break;
                case 2:                             /* switch 19 */
                    switch (var_s0_5) {             /* switch 21; irregular */
                    case 4:                         /* switch 21 */
                    case 0:                         /* switch 21 */
                        if ((s32) M2C_FIELD(arg0, u8 *, 0x33A) < 2) {
                            M2C_FIELD(temp_a1_7, M2C_UNK32 *, 0x10) = 0;
                        }
                        break;
                    case 1:                         /* switch 21 */
                        M2C_FIELD(temp_a1_7, M2C_UNK32 *, 0x10) = 0;
                        break;
                    case 5:                         /* switch 21 */
                        if (M2C_FIELD(arg0, u8 *, 0x312) == 0) {
                            M2C_FIELD(temp_a1_7, M2C_UNK32 *, 0x10) = 0;
                        }
                        break;
                    case 6:                         /* switch 21 */
                        if (M2C_FIELD(arg0, u8 *, 0x31A) == 0) {
                            M2C_FIELD(temp_a1_7, M2C_UNK32 *, 0x10) = 0;
                        }
                        break;
                    }
                    break;
                case 3:                             /* switch 19 */
                    switch (var_s0_5) {             /* switch 22; irregular */
                    case 4:                         /* switch 22 */
                        M2C_FIELD(temp_a1_7, M2C_UNK32 *, 0x10) = 0;
                        break;
                    case 5:                         /* switch 22 */
                        if (!(M2C_FIELD(arg0, u8 *, 0x948) & 1)) {
                            M2C_FIELD(temp_a1_7, M2C_UNK32 *, 0x10) = 0;
                        }
                        break;
                    }
                    break;
                case 4:                             /* switch 19 */
                    switch (var_s0_5) {             /* switch 23; irregular */
                    case 1:                         /* switch 23 */
                        break;
                    case 0:                         /* switch 23 */
                        if ((s32) M2C_FIELD(arg0, u8 *, 0x33A) >= 2) {
                            M2C_FIELD(temp_a1_7, M2C_UNK32 *, 0x10) = 0;
                        }
                        break;
                    }
                    break;
                }
                flSetRenderState((var_s0_5 + 0x3A) & 0xFF, temp_a1_7);
                var_s0_5 += 1;
                var_s3_4 += 4;
            } while (var_s0_5 < *temp_s1_5);
            return;
        }
        break;
    case 13:                                        /* switch 1 */
    case 16:                                        /* switch 1 */
    case 27:                                        /* switch 1 */
    case 28:                                        /* switch 1 */
    case 30:                                        /* switch 1 */
    case 31:                                        /* switch 1 */
        temp_a0 = arg0 + 0x444;
        if ((temp_a2 != 0x1F) && (temp_a2 != 0x1C) && (temp_a2 != 0x1B)) {
            var_v1 = M2C_FIELD(temp_a0, u8 *, 0x60);
        } else {
            var_v1 = M2C_FIELD(temp_a0, u8 *, 0x50);
        }
        if (var_v1 != 0) {
            var_v1_2 = 0;
        } else {
            var_v1_2 = (s64) ((s64) ((s32) M2C_FIELD(&game_w, u16 *, 0x1E) % 98) << 0x30) >> 0x30;
        }
        var_s0_6 = 0;
        var_s4 = (arg1 * 0x8C) + arg2;
        if (M2C_FIELD(var_s4, s32 *, 4) > 0) {
            temp_s1_6 = (s32 *)(var_s4 + 4);
            temp_s3_3 = (s64) (((s64) (var_v1_2 << 0x30) >> 0x30) << 0x30) >> 0x30;
            do {
                temp_a1_8 = temp_s2 + (M2C_FIELD(var_s4, s32 *, 8) * 0x4C);
                M2C_FIELD(temp_a1_8, M2C_UNK32 *, 0x10) = (M2C_UNK32) M2C_FIELD(arg0, M2C_UNK32 *, 0x798);
                temp_v1 = M2C_FIELD(arg0, u8 *, 2);
                switch (temp_v1) {                  /* switch 24; irregular */
                case 30:                            /* switch 24 */
                    /* fallthrough */
                case 16:                            /* switch 24 */
                case 13:                            /* switch 24 */
                    if (var_s0_6 != 5) {

                    } else {
                        M2C_FIELD(temp_a1_8, M2C_UNK32 *, 0x10) = 0;
                    }
                    break;
                default:                            /* switch 24 */
                    if (var_s0_6 != 4) {

                    } else {
                        M2C_FIELD(temp_a1_8, M2C_UNK32 *, 0x10) = 0;
                    }
                    break;
                }
                switch (temp_s3_3) {                /* switch 25 */
                case 0:                             /* switch 25 */
                case 1:                             /* switch 25 */
                case 2:                             /* switch 25 */
                case 3:                             /* switch 25 */
                    if ((var_s0_6 != 3) && (var_s0_6 != 1) && (var_s0_6 != 0)) {

                    } else {
                        M2C_FIELD(temp_a1_8, M2C_UNK32 *, 0x10) = 0;
                    }
                    break;
                case 4:                             /* switch 25 */
                case 5:                             /* switch 25 */
                    if ((var_s0_6 != 3) && (var_s0_6 != 2) && (var_s0_6 != 0)) {

                    } else {
                        M2C_FIELD(temp_a1_8, M2C_UNK32 *, 0x10) = 0;
                    }
                    break;
                case 6:                             /* switch 25 */
                case 7:                             /* switch 25 */
                    if ((var_s0_6 != 2) && (var_s0_6 != 1) && (var_s0_6 != 0)) {

                    } else {
                        M2C_FIELD(temp_a1_8, M2C_UNK32 *, 0x10) = 0;
                    }
                    break;
                default:                            /* switch 25 */
                    if ((var_s0_6 != 3) && (var_s0_6 != 2) && (var_s0_6 != 1)) {

                    } else {
                        M2C_FIELD(temp_a1_8, M2C_UNK32 *, 0x10) = 0;
                    }
                    break;
                }
                if (var_s0_6 != 9) {

                } else {
                    temp_v1_2 = M2C_FIELD(arg0, u16 *, 0x2DC);
                    if ((temp_v1_2 != 0x410) && (temp_v1_2 != 0x415)) {
                        M2C_FIELD(temp_a1_8, M2C_UNK32 *, 0x10) = 0;
                    }
                }
                flSetRenderState((var_s0_6 + 0x3A) & 0xFF, temp_a1_8);
                var_s0_6 += 1;
                var_s4 += 4;
            } while (var_s0_6 < *temp_s1_6);
            return;
        }
        break;
    case 14:                                        /* switch 1 */
        temp_s1_7 = arg0 + 0x444;
        if (M2C_FIELD(arg0, u16 *, 0x2DC) == 0x459) {
            if (em_frame_check2(arg0, 0, 10.0f) != 0) {
                var_s0_7 = 0;
            } else {
                var_s0_7 = 1;
            }
        } else {
            var_s0_7 = 0;
        }
        var_s3_5 = (arg1 * 0x8C) + arg2;
        var_s6_4 = 0;
        if (M2C_FIELD(var_s3_5, s32 *, 4) > 0) {
            temp_s7 = (s32 *)(var_s3_5 + 4);
            do {
                temp_a1_9 = temp_s2 + (M2C_FIELD(var_s3_5, s32 *, 8) * 0x4C);
                M2C_FIELD(temp_a1_9, M2C_UNK32 *, 0x10) = (M2C_UNK32) M2C_FIELD(arg0, M2C_UNK32 *, 0x798);
                switch (arg1) {                     /* switch 26; irregular */
                case 0:                             /* switch 26 */
                    switch (var_s6_4) {             /* switch 27; irregular */
                    case 3:                         /* switch 27 */
                        temp_v0_2 = M2C_FIELD(temp_s1_7, u8 *, 0x1A);
                        if (((s32) temp_v0_2 >= 2) && ((var_s0_7 == 0) || (temp_v0_2 != 2))) {
                            M2C_FIELD(temp_a1_9, M2C_UNK32 *, 0x10) = 0;
                        }
                        break;
                    case 4:                         /* switch 27 */
                        temp_v0_3 = M2C_FIELD(temp_s1_7, u8 *, 0x1A);
                        if ((s32) temp_v0_3 > 0) {
                            if (var_s0_7 != 0) {
                                if (temp_v0_3 != 1) {
                                    goto block_361;
                                }
                            } else {
block_361:
                                M2C_FIELD(temp_a1_9, M2C_UNK32 *, 0x10) = 0;
                            }
                        }
                        break;
                    case 6:                         /* switch 27 */
                        M2C_FIELD(temp_a1_9, M2C_UNK32 *, 0x10) = 0;
                        break;
                    }
                    break;
                case 1:                             /* switch 26 */
                    if (var_s6_4 != 2) {

                    } else if (!(M2C_FIELD(arg0, u8 *, 0x948) & 1)) {
                        M2C_FIELD(temp_a1_9, M2C_UNK32 *, 0x10) = 0;
                    }
                    break;
                case 2:                             /* switch 26 */
                    switch (var_s6_4) {             /* switch 28; irregular */
                    case 4:                         /* switch 28 */
                        M2C_FIELD(temp_a1_9, M2C_UNK32 *, 0x10) = 0;
                        break;
                    case 5:                         /* switch 28 */
                        if (!(M2C_FIELD(arg0, u8 *, 0x948) & 1)) {
                            M2C_FIELD(temp_a1_9, M2C_UNK32 *, 0x10) = 0;
                        }
                        break;
                    case 7:                         /* switch 28 */
                        temp_v1_3 = M2C_FIELD(temp_s1_7, u8 *, 0x1A);
                        if ((temp_v1_3 == 0) || ((var_s0_7 != 0) && (temp_v1_3 == 1))) {
                            M2C_FIELD(temp_a1_9, M2C_UNK32 *, 0x10) = 0;
                        }
                        break;
                    case 6:                         /* switch 28 */
                        temp_v0_4 = M2C_FIELD(temp_s1_7, u8 *, 0x1A);
                        if (((s32) temp_v0_4 < 2) || ((var_s0_7 != 0) && (temp_v0_4 == 2))) {
                            M2C_FIELD(temp_a1_9, M2C_UNK32 *, 0x10) = 0;
                        }
                        break;
                    }
                    break;
                }
                flSetRenderState((var_s6_4 + 0x3A) & 0xFF, temp_a1_9);
                var_s6_4 += 1;
                var_s3_5 += 4;
            } while (var_s6_4 < *temp_s7);
            return;
        }
        break;
    case 15:                                        /* switch 1 */
        var_s3_6 = (arg1 * 0x8C) + arg2;
        var_s0_8 = 0;
        if (M2C_FIELD(var_s3_6, s32 *, 4) > 0) {
            temp_s1_8 = (s32 *)(var_s3_6 + 4);
            do {
                temp_a1_10 = temp_s2 + (M2C_FIELD(var_s3_6, s32 *, 8) * 0x4C);
                M2C_FIELD(temp_a1_10, M2C_UNK32 *, 0x10) = (M2C_UNK32) M2C_FIELD(arg0, M2C_UNK32 *, 0x798);
                switch (arg1) {                     /* switch 29; irregular */
                case 1:                             /* switch 29 */
                    if (var_s0_8 != 1) {

                    } else if (!(M2C_FIELD(arg0, u8 *, 0x948) & 1)) {
                        M2C_FIELD(temp_a1_10, M2C_UNK32 *, 0x10) = 0;
                    }
                    break;
                case 3:                             /* switch 29 */
                    if (var_s0_8 != 1) {

                    } else {
                        M2C_FIELD(temp_a1_10, M2C_UNK32 *, 0x10) = 0;
                    }
                    break;
                case 4:                             /* switch 29 */
                    switch (var_s0_8) {             /* switch 30; irregular */
                    case 3:                         /* switch 30 */
                        M2C_FIELD(temp_a1_10, M2C_UNK32 *, 0x10) = 0;
                        break;
                    case 4:                         /* switch 30 */
                        if (!(M2C_FIELD(arg0, u8 *, 0x948) & 1)) {
                            M2C_FIELD(temp_a1_10, M2C_UNK32 *, 0x10) = 0;
                        }
                        break;
                    }
                    break;
                }
                flSetRenderState((var_s0_8 + 0x3A) & 0xFF, temp_a1_10);
                var_s0_8 += 1;
                var_s3_6 += 4;
            } while (var_s0_8 < *temp_s1_8);
            return;
        }
        break;
    case 17:                                        /* switch 1 */
        var_s3_7 = (arg1 * 0x8C) + arg2;
        var_s0_9 = 0;
        if (M2C_FIELD(var_s3_7, s32 *, 4) > 0) {
            temp_s1_9 = (s32 *)(var_s3_7 + 4);
            do {
                temp_a1_11 = temp_s2 + (M2C_FIELD(var_s3_7, s32 *, 8) * 0x4C);
                M2C_FIELD(temp_a1_11, M2C_UNK32 *, 0x10) = (M2C_UNK32) M2C_FIELD(arg0, M2C_UNK32 *, 0x798);
                switch (arg1) {                     /* switch 31; irregular */
                case 0:                             /* switch 31 */
                    if (var_s0_9 != 3) {

                    } else {
block_444:
                        M2C_FIELD(temp_a1_11, M2C_UNK32 *, 0x10) = 0;
                    }
                    break;
                case 1:                             /* switch 31 */
                    if (var_s0_9 != 2) {

                    } else if (!(M2C_FIELD(arg0, u8 *, 0x948) & 1)) {
                        goto block_444;
                    }
                    break;
                case 2:                             /* switch 31 */
                    switch (var_s0_9) {             /* switch 32; irregular */
                    case 4:                         /* switch 32 */
                        if ((s32) M2C_FIELD(arg0, u8 *, 0x33A) >= 2) {
                            goto block_444;
                        }
                        break;
                    case 5:                         /* switch 32 */
                        if ((s32) M2C_FIELD(arg0, u8 *, 0x33A) > 0) {
                            goto block_444;
                        }
                        break;
                    case 6:                         /* switch 32 */
                        goto block_444;
                    case 7:                         /* switch 32 */
                        if (!(M2C_FIELD(arg0, u8 *, 0x948) & 1)) {
                            goto block_444;
                        }
                        break;
                    }
                    break;
                case 3:                             /* switch 31 */
                    switch (var_s0_9) {             /* switch 33; irregular */
                    case 0:                         /* switch 33 */
                        if ((s32) M2C_FIELD(arg0, u8 *, 0x33A) < 2) {
                            goto block_444;
                        }
                        break;
                    case 1:                         /* switch 33 */
                        if (M2C_FIELD(arg0, u8 *, 0x33A) == 0) {
                            goto block_444;
                        }
                        break;
                    }
                    break;
                }
                flSetRenderState((var_s0_9 + 0x3A) & 0xFF, temp_a1_11);
                var_s0_9 += 1;
                var_s3_7 += 4;
            } while (var_s0_9 < *temp_s1_9);
            return;
        }
        break;
    case 19:                                        /* switch 1 */
    case 24:                                        /* switch 1 */
        var_s1_3 = 0;
        var_s4_2 = (arg1 * 0x8C) + arg2;
        if (M2C_FIELD(var_s4_2, s32 *, 4) > 0) {
            temp_s6 = (s32 *)(var_s4_2 + 4);
            temp_s3_4 = (s64) (((s64) ((s64) ((s32) M2C_FIELD(&game_w, u16 *, 0x1E) % 9) << 0x30) >> 0x30) << 0x30) >> 0x30;
            do {
                temp_s0_3 = temp_s2 + (M2C_FIELD(var_s4_2, s32 *, 8) * 0x4C);
                M2C_FIELD(temp_s0_3, M2C_UNK32 *, 0x10) = (M2C_UNK32) M2C_FIELD(arg0, M2C_UNK32 *, 0x798);
                switch (var_s1_3) {                 /* switch 34 */
                case 2:                             /* switch 34 */
                case 3:                             /* switch 34 */
                case 4:                             /* switch 34 */
                    switch (temp_s3_4) {            /* switch 35 */
                    case 0:                         /* switch 35 */
                    case 1:                         /* switch 35 */
                        if ((var_s1_3 != 4) && (var_s1_3 != 3)) {

                        } else {
                            M2C_FIELD(temp_s0_3, M2C_UNK32 *, 0x10) = 0;
                        }
                        break;
                    case 3:                         /* switch 35 */
                    case 4:                         /* switch 35 */
                    case 5:                         /* switch 35 */
                    case 6:                         /* switch 35 */
                        if ((var_s1_3 != 3) && (var_s1_3 != 2)) {

                        } else {
                            M2C_FIELD(temp_s0_3, M2C_UNK32 *, 0x10) = 0;
                        }
                        break;
                    default:                        /* switch 35 */
                        if ((var_s1_3 != 4) && (var_s1_3 != 2)) {

                        } else {
                            M2C_FIELD(temp_s0_3, M2C_UNK32 *, 0x10) = 0;
                        }
                        break;
                    }
                    break;
                case 1:                             /* switch 34 */
                case 5:                             /* switch 34 */
                case 6:                             /* switch 34 */
                case 7:                             /* switch 34 */
                    temp_a0_2 = M2C_FIELD(arg0, u16 *, 0x2DC);
                    switch (temp_a0_2) {            /* switch 36 */
                    case 0x3E9:                     /* switch 36 */
                    case 0x3EA:                     /* switch 36 */
                    case 0x3EB:                     /* switch 36 */
                    case 0x3EE:                     /* switch 36 */
                    case 0x3EF:                     /* switch 36 */
                    case 0x3F2:                     /* switch 36 */
                    case 0x3F3:                     /* switch 36 */
                    case 0x3F4:                     /* switch 36 */
                        if ((temp_a0_2 == 0x3F4) && (em_frame_check2(arg0, 0, 72.0f) == 0)) {
                            if ((var_s1_3 != 7) && (var_s1_3 != 6)) {

                            } else {
                                M2C_FIELD(temp_s0_3, M2C_UNK32 *, 0x10) = 0;
                            }
                        } else if (M2C_FIELD(&game_w, u16 *, 0x1E) & 1) {
                            if (var_s1_3 != 7) {

                            } else {
                                M2C_FIELD(temp_s0_3, M2C_UNK32 *, 0x10) = 0;
                            }
                        } else if ((var_s1_3 != 6) && (var_s1_3 != 5) && (var_s1_3 != 1)) {

                        } else {
                            M2C_FIELD(temp_s0_3, M2C_UNK32 *, 0x10) = 0;
                        }
                        break;
                    default:                        /* switch 36 */
                        if ((var_s1_3 != 7) && (var_s1_3 != 6)) {

                        } else {
                            M2C_FIELD(temp_s0_3, M2C_UNK32 *, 0x10) = 0;
                        }
                        break;
                    }
                    break;
                }
                flSetRenderState((var_s1_3 + 0x3A) & 0xFF, temp_s0_3);
                var_s1_3 += 1;
                var_s4_2 += 4;
            } while ((s32) var_s1_3 < *temp_s6);
            return;
        }
        break;
    case 21:                                        /* switch 1 */
        var_s3_8 = (arg1 * 0x8C) + arg2;
        var_s0_10 = 0;
        if (M2C_FIELD(var_s3_8, s32 *, 4) > 0) {
            temp_s1_10 = (s32 *)(var_s3_8 + 4);
            do {
                temp_a1_12 = temp_s2 + (M2C_FIELD(var_s3_8, s32 *, 8) * 0x4C);
                M2C_FIELD(temp_a1_12, M2C_UNK32 *, 0x10) = (M2C_UNK32) M2C_FIELD(arg0, M2C_UNK32 *, 0x798);
                switch (arg1) {                     /* switch 37; irregular */
                case 0:                             /* switch 37 */
                    switch (var_s0_10) {            /* switch 38; irregular */
                    case 2:                         /* switch 38 */
                        M2C_FIELD(temp_a1_12, M2C_UNK32 *, 0x10) = 0;
                        break;
                    case 3:                         /* switch 38 */
                        if (!(M2C_FIELD(arg0, u8 *, 0x948) & 1)) {
                            M2C_FIELD(temp_a1_12, M2C_UNK32 *, 0x10) = 0;
                        }
                        break;
                    }
                    break;
                case 1:                             /* switch 37 */
                    if (var_s0_10 != 2) {

                    } else if (!(M2C_FIELD(arg0, u8 *, 0x948) & 1)) {
                        M2C_FIELD(temp_a1_12, M2C_UNK32 *, 0x10) = 0;
                    }
                    break;
                case 2:                             /* switch 37 */
                    if (var_s0_10 != 2) {

                    } else {
                        M2C_FIELD(temp_a1_12, M2C_UNK32 *, 0x10) = 0;
                    }
                    break;
                }
                flSetRenderState((var_s0_10 + 0x3A) & 0xFF, temp_a1_12);
                var_s0_10 += 1;
                var_s3_8 += 4;
            } while (var_s0_10 < *temp_s1_10);
            return;
        }
        break;
    case 22:                                        /* switch 1 */
        var_s3_9 = (arg1 * 0x8C) + arg2;
        var_s0_11 = 0;
        if (M2C_FIELD(var_s3_9, s32 *, 4) > 0) {
            temp_s1_11 = (s32 *)(var_s3_9 + 4);
            do {
                temp_a1_13 = temp_s2 + (M2C_FIELD(var_s3_9, s32 *, 8) * 0x4C);
                M2C_FIELD(temp_a1_13, M2C_UNK32 *, 0x10) = (M2C_UNK32) M2C_FIELD(arg0, M2C_UNK32 *, 0x798);
                switch (arg1) {                     /* switch 39; irregular */
                case 0:                             /* switch 39 */
                    if (var_s0_11 != 2) {

                    } else {
block_545:
                        M2C_FIELD(temp_a1_13, M2C_UNK32 *, 0x10) = 0;
                    }
                    break;
                case 1:                             /* switch 39 */
                    if (var_s0_11 != 1) {

                    } else if (!(M2C_FIELD(arg0, u8 *, 0x948) & 1)) {
                        goto block_545;
                    }
                    break;
                case 2:                             /* switch 39 */
                    switch (var_s0_11) {            /* switch 40; irregular */
                    case 4:                         /* switch 40 */
                        if ((s32) M2C_FIELD(arg0, u8 *, 0x33A) > 0) {
                            goto block_545;
                        }
                        break;
                    case 5:                         /* switch 40 */
                        goto block_545;
                    case 6:                         /* switch 40 */
                        if (!(M2C_FIELD(arg0, u8 *, 0x948) & 1)) {
                            goto block_545;
                        }
                        break;
                    }
                    break;
                case 3:                             /* switch 39 */
                    switch (var_s0_11) {            /* switch 41; irregular */
                    case 0:                         /* switch 41 */
                        if ((s32) M2C_FIELD(arg0, u8 *, 0x33A) < 2) {
                            goto block_545;
                        }
                        break;
                    case 1:                         /* switch 41 */
                        if (M2C_FIELD(arg0, u8 *, 0x33A) == 0) {
                            goto block_545;
                        }
                        break;
                    }
                    break;
                }
                flSetRenderState((var_s0_11 + 0x3A) & 0xFF, temp_a1_13);
                var_s0_11 += 1;
                var_s3_9 += 4;
            } while (var_s0_11 < *temp_s1_11);
            return;
        }
        break;
    case 26:                                        /* switch 1 */
        temp_a2_2 = M2C_FIELD(arg0, s8 *, 0x45F);
        temp_s1_12 = arg0 + 0x444;
        temp_f20 = 1.0f - (0.8039216f * ((f32) temp_a2_2 / 60.0f));
        temp_f21 = 1.0f - (0.17254901f * ((f32) temp_a2_2 / 60.0f));
        if (M2C_FIELD(arg0, u16 *, 0x2DC) == 0x459) {
            if (em_frame_check2(arg0, 0, 10.0f) != 0) {
                var_s0_12 = 0;
            } else {
                var_s0_12 = 1;
            }
        } else {
            var_s0_12 = 0;
        }
        var_s3_10 = (arg1 * 0x8C) + arg2;
        var_s6_5 = 0;
        if (M2C_FIELD(var_s3_10, s32 *, 4) > 0) {
            temp_s7_2 = (s32 *)(var_s3_10 + 4);
            do {
                temp_a1_14 = temp_s2 + (M2C_FIELD(var_s3_10, s32 *, 8) * 0x4C);
                M2C_FIELD(temp_a1_14, M2C_UNK32 *, 0x10) = (M2C_UNK32) M2C_FIELD(arg0, M2C_UNK32 *, 0x798);
                switch (arg1) {                     /* switch 42; irregular */
                case 0:                             /* switch 42 */
                    switch (var_s6_5) {             /* switch 43; irregular */
                    case 0:                         /* switch 43 */
                        M2C_FIELD(temp_a1_14, f32 *, 4) = temp_f21;
                        M2C_FIELD(temp_a1_14, f32 *, 8) = temp_f20;
                        M2C_FIELD(temp_a1_14, f32 *, 0xC) = temp_f20;
                        break;
                    case 4:                         /* switch 43 */
                        temp_v0_5 = M2C_FIELD(temp_s1_12, u8 *, 0x1A);
                        if ((s32) temp_v0_5 > 0) {
                            if (var_s0_12 != 0) {
                                if (temp_v0_5 != 1) {
                                    goto block_569;
                                }
                            } else {
block_569:
                                M2C_FIELD(temp_a1_14, M2C_UNK32 *, 0x10) = 0;
                            }
                        }
                        break;
                    case 7:                         /* switch 43 */
                        M2C_FIELD(temp_a1_14, M2C_UNK32 *, 0x10) = 0;
                        break;
                    }
                    break;
                case 1:                             /* switch 42 */
                    if (var_s6_5 != 2) {

                    } else if (!(M2C_FIELD(arg0, u8 *, 0x948) & 1)) {
                        M2C_FIELD(temp_a1_14, M2C_UNK32 *, 0x10) = 0;
                    }
                    break;
                case 2:                             /* switch 42 */
                    switch (var_s6_5) {             /* switch 44; irregular */
                    case 4:                         /* switch 44 */
                        M2C_FIELD(temp_a1_14, M2C_UNK32 *, 0x10) = 0;
                        break;
                    case 5:                         /* switch 44 */
                        if (!(M2C_FIELD(arg0, u8 *, 0x948) & 1)) {
                            M2C_FIELD(temp_a1_14, M2C_UNK32 *, 0x10) = 0;
                        }
                        break;
                    case 6:                         /* switch 44 */
                        temp_v1_4 = M2C_FIELD(temp_s1_12, u8 *, 0x1A);
                        if ((temp_v1_4 == 0) || ((var_s0_12 != 0) && (temp_v1_4 == 1))) {
                            M2C_FIELD(temp_a1_14, M2C_UNK32 *, 0x10) = 0;
                        }
                        break;
                    }
                    break;
                }
                flSetRenderState((var_s6_5 + 0x3A) & 0xFF, temp_a1_14);
                var_s6_5 += 1;
                var_s3_10 += 4;
            } while (var_s6_5 < *temp_s7_2);
            return;
        }
        break;
    default:                                        /* switch 1 */
        var_s3_11 = (arg1 * 0x8C) + arg2;
        var_s0_13 = 0;
        if (M2C_FIELD(var_s3_11, s32 *, 4) > 0) {
            temp_s1_13 = (s32 *)(var_s3_11 + 4);
            do {
                temp_a1_15 = temp_s2 + (M2C_FIELD(var_s3_11, s32 *, 8) * 0x4C);
                M2C_FIELD(temp_a1_15, M2C_UNK32 *, 0x10) = (M2C_UNK32) M2C_FIELD(arg0, M2C_UNK32 *, 0x798);
                flSetRenderState((var_s0_13 + 0x3A) & 0xFF, temp_a1_15);
                var_s0_13 += 1;
                var_s3_11 += 4;
            } while (var_s0_13 < *temp_s1_13);
        }
        break;
    }
}


