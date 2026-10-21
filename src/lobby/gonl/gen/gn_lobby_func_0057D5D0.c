/* G online (PC): lobby_func_0057D5D0, written from Monster Hunter G's code at lobby 0x57D5D0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 DispHelpLine();
s32 DispSceneTitle();
s32 Lb_put_icon_free();
s32 Paint_square();
s32 Put_2TF();
s32 SetTextureStage();
s32 SetTrnslMode();
s32 flfntLocate();
s32 flfntSetSize();
s32 font_print_double();
s32 font_set_palette();
s32 font_set_stack_no();
s32 lobby_func_005926C0();
s64 lobby_func_005935C0();
s32 lobby_func_005936B0();
s32 reload_tex();
extern u8 BsLbsCount;
extern u8 ClassInfo;
extern u8 cw;
extern u8 lb_rule_member;
extern u8 net_etc_mes_tbl;
extern u8 textLobbyTbl;
s32 Lb_put_button();   /* extern */
s32 font_print();            /* extern */
s32 func_0012A800();                       /* extern */
extern u8 g_lb_5FBBF0;
extern void *g_lb_636CD0;
extern u8 g_lb_63E530;
extern u8 PlazaInfo;

void lobby_func_0057D5D0(void *arg0) {
    u8 g_frame[0xC0] __attribute__((aligned(16)));   /* G's stack frame */
#define spA0 (*(s16 *)(g_frame + 0xA0))
#define spA2 (*(s16 *)(g_frame + 0xA2))
#define spA4 (*(s16 *)(g_frame + 0xA4))
#define spA6 (*(s16 *)(g_frame + 0xA6))
#define spA8 (*(s32 *)(g_frame + 0xA8))
#define spAC (*(s32 *)(g_frame + 0xAC))
#define spB0 (*(s32 *)(g_frame + 0xB0))
#define spB4 (*(s32 *)(g_frame + 0xB4))
    u8 *var_a4;   /* (s32 *) */
    u8 *var_s1;   /* (s32 *) */
    u8 *var_s2;   /* (s32 *) */
    u8 *var_s4_2;   /* (s32 *) */
    f32 temp_f2;
    f32 var_a0_2;
    f32 var_a0_3;
    f32 var_a0_4;
    f32 var_f3;
    f32 var_f4;
    f32 var_f5;
    s16 temp_v0;
    s32 var_a0;
    s32 var_a5;
    s32 var_s3;
    s32 var_s4;
    s32 var_s7;
    s32 var_v1;
    s32 temp_a2;
    s32 temp_fp;
    s32 temp_s0;
    void **var_s0;

    font_set_stack_no((*(s32 *)((u8 *)(arg0) + 0x18)));
    lobby_func_005936B0(M2C_ERROR(/* Read from unset register $a0 */), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
    reload_tex(1, 0x17B);
    SetTextureStage(0x17B);
    flfntSetSize(0x12, 0x12);
    var_s3 = 0;
    var_s2 = &(*(u8 *)((u8 *)&textLobbyTbl + 0x50));
    var_s1 = &g_lb_5FBBF0;
    var_s0 = &g_lb_636CD0;
    do {
        var_s4 = 4;
        if (var_s3 == (*(u8 *)((u8 *)((*(void **)&cw)) + 0xBA1))) {
            SetTrnslMode(4, 1);
            Put_2TF(var_s2);
            SetTrnslMode(4, 5);
        } else {
            var_s4 = 0;
        }
        font_print_double((s32) ((s32) (lobby_func_005935C0(*var_s0, (s32) (*(s16 *)((u8 *)(var_s1) + 0)), 0x12, M2C_ERROR(/* Read from unset register $a3 */)) << 0x30) >> 0x30), (s32) (*(s16 *)((u8 *)(var_s1) + 2)), 1, var_s4, *var_s0);
        var_s3 += 1;
        var_s2 += 0x14;
        var_s1 += 4;
        var_s0 += 4;
    } while (var_s3 < 5);
    reload_tex(1, 0x180);
    SetTextureStage(0x180);
    var_s7 = 0;
    var_s4_2 = &g_lb_5FBBF0;
    do {
        var_a0 = 0;
        var_v1 = 0;
        var_a5 = 0;
        if ((s32) (*(u16 *)((u8 *)&ClassInfo + 0x2)) > 0) {
            var_a4 = &PlazaInfo;
            do {
                if ((var_s7 + 0x31) == (*(s8 *)((u8 *)(var_a4) + 0x5B))) {
                    var_v1 += (*(u16 *)((u8 *)(var_a4) + 8));
                    var_a0 += (*(u16 *)((u8 *)(var_a4) + 6));
                }
                var_a5 += 1;
                var_a4 += 0x15C;
            } while (var_a5 < (s32) (*(u16 *)((u8 *)&ClassInfo + 0x2)));
        }
        temp_v0 = (*(s16 *)((u8 *)(var_s4_2) + 0)) - 0x46;
        temp_a2 = (*(s16 *)((u8 *)(var_s4_2) + 2)) + 0x23;
        spA0 = temp_v0;
        spA4 = temp_v0;
        spA2 = (s16) temp_a2;
        temp_s0 = ((s16) (temp_a2));
        spA6 = temp_s0 + 0xC;
        spA8 = 0xFF00FF00;
        spB0 = 0xFF00FF00;
        if (var_v1 != 0) {
            temp_f2 = (f32) var_a0 / (f32) var_v1;
            spA4 += M2C_BITWISE(s32, (140.0f * temp_f2));
            var_f5 = 0.0f + (temp_f2 * 255.0f);
            var_f4 = 255.0f + (temp_f2 * -255.0f);
            var_f3 = 0.0f + (temp_f2 * 0.0f);
        } else {
            var_f5 = 0.0f;
            var_f4 = 255.0f;
            var_f3 = 0.0f;
        }
        if (!(var_f5 >= 2.1474836e9f)) {
            var_a0_2 = var_f5;
        } else {
            var_a0_2 = M2C_BITWISE(f32, (M2C_BITWISE(s32, (var_f5 - 2.1474836e9f)) | 0x80000000));
        }
        if (!(var_f4 >= 2.1474836e9f)) {
            var_a0_3 = var_f4;
        } else {
            var_a0_3 = M2C_BITWISE(f32, (M2C_BITWISE(s32, (var_f4 - 2.1474836e9f)) | 0x80000000));
        }
        if (!(var_f3 >= 2.1474836e9f)) {
            var_a0_4 = var_f3;
        } else {
            var_a0_4 = M2C_BITWISE(f32, (M2C_BITWISE(s32, (var_f3 - 2.1474836e9f)) | 0x80000000));
        }
        temp_fp = ((s16) ((((s16) ((s32) temp_v0)))));
        spAC = (M2C_BITWISE(s32, var_a0_4) & 0xFF) + (((M2C_BITWISE(s32, var_a0_2) & 0xFF) << 0x10) + 0xFF000000 + ((M2C_BITWISE(s32, var_a0_3) & 0xFF) << 8));
        spB4 = spAC;
        Paint_square((s32) (((s16) ((temp_fp + 4)))), (s32) (((s16) ((temp_s0 + 3)))), 0x8C, 0xC, -0xFEFEFF);
        func_0012A800(&spA0);
        Lb_put_icon_free((s32) (((s16) ((temp_fp - 0x10)))), (s32) (((s16) ((temp_s0 - 7)))), 0x18, -1, 0x2E);
        var_s7 += 1;
        var_s4_2 += 4;
    } while (var_s7 < 5);
    DispSceneTitle(M2C_ERROR(/* Read from unset register $a0 */), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
    if ((s32) (*(u8 *)((u8 *)((*(void **)&cw)) + 0xB9A)) >= 2) {
        DispHelpLine(M2C_ERROR(/* Read from unset register $a0 */), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
    }
    if ((s32) (*(u8 *)&BsLbsCount) >= 2) {
        lobby_func_005926C0(0, 0x54, 0xC8, 0x1A, 0x80006000, 0);
        Lb_put_button(0x14, 0x56, 1);
        flfntSetSize(0x12, 0x12);
        flfntLocate(0x2E, 0x58);
        font_set_palette(0);
        font_print(&g_lb_63E530, (*(s32 *)((u8 *)&net_etc_mes_tbl + 0x18)), (*(s32 *)((u8 *)&lb_rule_member + 0x24)));
    }
}

