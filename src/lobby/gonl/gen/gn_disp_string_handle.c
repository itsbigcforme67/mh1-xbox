/* G online (PC): disp_string_handle, written from Monster Hunter G's code at lobby 0x5BA950 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 cw;
extern u8 net_etc_mes_tbl;
extern u8 tl_mail_tbl;
s32 cnWrap_SetFontSize();                /* extern */
s32 flfntLocate();                  /* extern */
s32 flfntSetSize();             /* extern */
s32 font_print();                            /* extern */
s32 font_print_double(); /* extern */
s32 memset();        /* extern */
s32 sprintf();                    /* extern */
s32 strcpy();                  /* extern */

void disp_string_handle(s32 arg0, s32 arg1) {
    u8 g_frame[0x50] __attribute__((aligned(16)));   /* G's stack frame */
#define sp30 (*(s32 *)(g_frame + 0x30))
    s32 temp_s0;
    s64 temp_s0_2;
    s64 var_v0;
    u8 *temp_v1;

    memset(&sp30, 0, 0x1E);
    temp_v1 = ((arg1 & 0xFF) * 0x11) + (*(s32 *)&cw);
    if ((*(s8 *)((u8 *)(temp_v1) + 0x2B)) == 0) {
        sprintf(&sp30, (*(s32 *)((u8 *)&net_etc_mes_tbl + 0x20)));
    } else {
        strcpy(&sp30, temp_v1 + 0x2B);
    }
    cnWrap_SetFontSize(0x41A00000);
    temp_s0 = arg1 & 0xFF;
    var_v0 = 4;
    if ((arg0 & 0xFF) != temp_s0) {
        var_v0 = 0;
    }
    flfntSetSize(0x18, 0x14);
    temp_s0_2 = (temp_s0 * 0x58) + 0x62;
    font_print_double(0x1A4, ((s16) (temp_s0_2)), 1, ((s8) ((((s8) (var_v0))))), &sp30);
    flfntSetSize(0x14, 0x14);
    flfntLocate(0x168, ((s16) (temp_s0_2)));
    font_print((*(s32 *)((u8 *)&tl_mail_tbl + 0x8)));
}

