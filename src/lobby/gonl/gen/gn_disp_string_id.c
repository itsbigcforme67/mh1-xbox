/* G online (PC): disp_string_id, written from Monster Hunter G's code at lobby 0x5BA860 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 cw;
extern u8 net_etc_mes_tbl;
extern u8 tl_mail_tbl;
s32 flfntLocate();                  /* extern */
s32 flfntSetSize();             /* extern */
s32 font_print();                            /* extern */
s32 font_print_double(); /* extern */
s32 han2zen();                 /* extern */
s32 memset();        /* extern */
s32 strcpy();                     /* extern */

void disp_string_id(s32 arg0, s32 arg1) {
    u8 g_frame[0x50] __attribute__((aligned(16)));   /* G's stack frame */
#define sp30 (*(s32 *)(g_frame + 0x30))
    s32 temp_v1_2;
    s64 temp_s0;
    s64 var_v0;
    u8 *temp_v1;

    memset(&sp30, 0, 0x1E);
    temp_v1 = ((arg1 & 0xFF) * 8) + (*(s32 *)&cw);
    if ((*(s8 *)((u8 *)(temp_v1) + 0xB)) == 0) {
        strcpy(&sp30, (*(u8 *)((u8 *)&net_etc_mes_tbl + 0x20)));
    } else {
        han2zen(temp_v1 + 0xB, &sp30);
    }
    flfntSetSize(0x14, 0x14);
    temp_v1_2 = arg1 & 0xFF;
    var_v0 = 4;
    if ((arg0 & 0xFF) != temp_v1_2) {
        var_v0 = 0;
    }
    temp_s0 = (temp_v1_2 * 0x58) + 0x7A;
    font_print_double(0x1A4, (s64) (temp_s0 << 0x30) >> 0x30, 1, (s64) (var_v0 << 0x38) >> 0x38);
    flfntLocate(0x168, (s64) (temp_s0 << 0x30) >> 0x30);
    font_print((*(u8 *)((u8 *)&tl_mail_tbl + 0xC)));
}

