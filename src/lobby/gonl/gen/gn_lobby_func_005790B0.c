/* G online (PC): lobby_func_005790B0, written from Monster Hunter G's code at lobby 0x5790B0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 flfntLocate();
s32 flfntSetSize();
s32 font_set_palette();
s32 han2zen();
extern u8 g_main_2EFBAC;
extern u8 g_main_2EFBAC;
extern u8 tl_mail_tbl;
s32 func_0012BDD0();        /* extern */
s32 sprintf(); /* extern */
extern s32 tl_etc;
extern s32 g_lb_63E348;
extern s32 g_lb_63E350;
extern s32 lit_2316;

void lobby_func_005790B0(void) {
    u8 g_frame[0xD0] __attribute__((aligned(16)));   /* G's stack frame */
#define sp40 (*(s32 *)(g_frame + 0x40))
#define spA0 (*(s32 *)(g_frame + 0xA0))
    s32 temp_a5;
    s32 var_s0;
    s32 var_s1;
    s32 var_s2;
    u8 *temp_a0;

    flfntSetSize(0x14, 0x14);
    font_set_palette(0);
    var_s2 = 0;
    var_s1 = 0xBC;
    var_s0 = 0;
    do {
        flfntLocate(0x34, (s32) (((s16) (var_s1))));
        temp_a0 = (*(s32 *)&g_main_2EFBAC) + var_s0;
        if ((*(s8 *)((u8 *)(temp_a0) + 0x9A)) != 0) {
            sprintf(&sp40, &g_lb_63E348, temp_a0 + 0x9A);
            han2zen(&sp40, &spA0);
            temp_a5 = (*(s32 *)&g_main_2EFBAC);
            sprintf(&sp40, &g_lb_63E350, (*(void * *)((u8 *)&tl_mail_tbl + 0xC)), &spA0, (*(s32 *)((u8 *)&tl_mail_tbl + 0x8)), temp_a5 + var_s0 + 0xA2, (*(s32 *)((u8 *)(((u8 *)&tl_etc + ((*(u8 *)((u8 *)((temp_a5 + (((s8) (var_s2))))) + 0x23C)) * 4))) + 0x2C)));
            func_0012BDD0(&lit_2316, &sp40);
        }
        var_s1 += 0x18;
        var_s2 = ((s8) ((var_s2 + 1)));
        var_s0 += 0x5C;
    } while (var_s2 < 3);
}

