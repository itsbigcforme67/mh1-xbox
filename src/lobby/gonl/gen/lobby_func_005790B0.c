/* G online (PC): lobby_func_005790B0, written from Monster Hunter G's code at lobby 0x5790B0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 g_main_2EFBAC;
extern u8 g_main_2EFBAC;
extern u8 tl_mail_tbl;
s32 flfntLocate();                  /* extern */
s32 flfntSetSize();             /* extern */
s32 font_set_palette();                  /* extern */
s32 func_0012BDD0();        /* extern */
s32 han2zen();              /* extern */
s32 sprintf(); /* extern */
extern u8 g_lb_63E348;
extern u8 g_lb_63E350;
extern u8 lit_2316;

void lobby_func_005790B0(void) {
    union { s32 v; u8 b[0x30]; } u_spA0;
#define spA0 (u_spA0.v)
    union { s32 v; u8 b[0x90]; } u_sp40;
#define sp40 (u_sp40.v)
    s32 var_s0;
    s64 var_s1;
    s64 var_s2;
    u8 *temp_a0;

    flfntSetSize(0x14, 0x14);
    font_set_palette(0);
    var_s2 = 0;
    var_s1 = 0xBC;
    var_s0 = 0;
    do {
        flfntLocate(0x34, (s64) (var_s1 << 0x30) >> 0x30);
        temp_a0 = (*(s32 *)&g_main_2EFBAC) + var_s0;
        if ((*(s8 *)((u8 *)(temp_a0) + 0x9A)) != 0) {
            sprintf(&sp40, &g_lb_63E348, temp_a0 + 0x9A);
            han2zen(&sp40, &spA0);
            sprintf(&sp40, &g_lb_63E350, (*(u8 *)((u8 *)&tl_mail_tbl + 0xC)), &spA0);
            func_0012BDD0(&lit_2316, &sp40);
        }
        var_s1 += 0x18;
        var_s2 = (s64) ((var_s2 + 1) << 0x38) >> 0x38;
        var_s0 += 0x5C;
    } while (var_s2 < 3);
}

