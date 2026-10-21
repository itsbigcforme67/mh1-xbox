/* G online (PC): Plaza_disp_ReibunEdit, written from Monster Hunter G's code at lobby 0x58F740 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Put_page_num();
s32 put_main_cursor();
s32 Reibun_print();                 /* extern */
s32 flfntLocate();                  /* extern */
s32 flfntSetSize();             /* extern */
s32 font_set_palette();                  /* extern */
extern u8 reibun_edit;
extern u8 str_tbl_reibun0;

void Plaza_disp_ReibunEdit(void) {
    s32 var_s1;
    s64 var_s0;
    u8 *var_s3;

    Put_page_num(0x20E, 0x78, ((s16) ((((*(u8 *)((u8 *)&reibun_edit + 0x3)) / 6) + ((u8) (*(u8 *)((u8 *)&reibun_edit + 0x3)) >> 0x1F)))), 2, 0);
    flfntSetSize(0x12, 0x12);
    font_set_palette(0);
    var_s1 = 6;
    var_s0 = 0xA4;
    var_s3 = (u8 *)&str_tbl_reibun0 + ((((*(u8 *)((u8 *)&reibun_edit + 0x3)) / 6) + ((u8) (*(u8 *)((u8 *)&reibun_edit + 0x3)) >> 0x1F)) * 0x60);
    do {
        flfntLocate(0xDA, var_s0);
        Reibun_print(0x2C, (*(s32 *)((u8 *)(var_s3) + 0xC)));
        var_s1 -= 1;
        var_s3 += 0x10;
        var_s0 = ((s16) ((var_s0 + 0x16)));
    } while (var_s1 != 0);
    put_main_cursor(((s32) (*(u8 *)((u8 *)(&reibun_edit) + 3)) % 6) + 1);
}

