/* G online (PC): lb_put_room_member_005B1930, written from Monster Hunter G's code at lobby 0x58B270 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

#ifdef __MWERKS__
s32 Get_weapon_job();
#else
unsigned char Get_weapon_job();   /* the real return type (tools/check_kr_returns.py) */
#endif
s32 Lb_put_job();
s32 flfntSetSize();
s32 font_print_double();
s32 han2zen();
extern u8 tl_mail_tbl;
s32 sprintf();   /* extern */
extern s8 join_member;
extern s32 lit_586_0065E320;

void lb_put_room_member_005B1930(void) {
    u8 g_frame[0xE0] __attribute__((aligned(16)));   /* G's stack frame */
#define sp40 (*(s32 *)(g_frame + 0x40))
#define spA0 (*(s32 *)(g_frame + 0xA0))
    s32 var_s1;
    s32 var_s2;
    s8 *var_s0;

    var_s2 = 0xA4;
    flfntSetSize(0x12, 0x12);
    var_s1 = 0;
    var_s0 = &join_member;
    do {
        if (*var_s0 != 0) {
            Lb_put_job(0x167, (s32) (((s16) (((((s16) (var_s2))) + 2)))), 0x20, -1, Get_weapon_job(var_s0 + 0x26) & 0xFF, 1);
            sprintf(&spA0, &lit_586_0065E320, (*(s32 *)((u8 *)&tl_mail_tbl + 0x8)), var_s0 + 8);
            font_print_double(0x193, (s32) var_s2, 1, 0, &spA0);
            var_s2 = ((s16) ((var_s2 + 0x14)));
            sprintf(&spA0, &lit_586_0065E320, (*(s32 *)((u8 *)&tl_mail_tbl + 0xC)), var_s0);
            han2zen(&spA0, &sp40);
            font_print_double(0x193, (s32) var_s2, 1, 0, &sp40);
        }
        var_s0 += 0x5C;
        var_s1 = ((s16) ((var_s1 + 1)));
        var_s2 = ((s16) ((var_s2 + 0x1A)));
    } while (var_s1 < 4);
    flfntSetSize(0x12, 0x12);
}

