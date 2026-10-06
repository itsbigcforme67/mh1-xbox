/* lb_by13 - agent B promoted near-match 0x005C2990-0x005C2B08: server_select_sub_03 (first drafted by tools/lbauto.py). */
#include "lobby_f.h"
extern char test_server_sel_disp[];
extern char netr_sub01_tbl[];
extern char ss_text_lobby_trans_ot[];
extern char netr_sub01_col[];
typedef struct { s16 x0000; s8 x0002; u8 x0003; u8 pad0004[0x4]; u8 x0008; u8 pad0009[0x3]; s8 x000C; } ARG_server_select_sub_03_arg0;

void server_select_sub_03(ARG_server_select_sub_03_arg0 *arg0) {
    u8 temp_v1;

    temp_v1 = arg0->x0003;
    switch (temp_v1) {                              /* irregular */
    case 0:
        arg0->x0003 = (u8) (temp_v1 + 1);
        arg0->x0000 = 0x260;
        SetDialogData(0x40, 0);
        arg0->x000C = 1;
        return;
    case 1:
        arg0->x0000 = (s16) (arg0->x0000 - 1);
        arg0->x000C = 1;
        if ((arg0->x0000 < 0x224) && (tk_sw_on_ck(0x20) != 0)) {
            cnWrap_SoundRequest(0);
            arg0->x0002 = 2;
            arg0->x0003 = 0U;
            Lbc_set_prim(&test_server_sel_disp, 0, &ss_text_lobby_trans_ot);
            F(s32, &netr_sub01_tbl, 0x1C) = *(s32 *)((u8 *)&netr_sub01_col + (arg0->x0008 * 4));
            str_play_vol(0, 0x47, 0x3C);
            *(s8 *)0x3F3415 = 0x47;
            return;
        }
        if (arg0->x0000 < 0) {
            arg0->x0002 = 2;
            arg0->x0003 = 0U;
            Lbc_set_prim(&test_server_sel_disp, 0, &ss_text_lobby_trans_ot);
            F(s32, &netr_sub01_tbl, 0x1C) = *(s32 *)((u8 *)&netr_sub01_col + (arg0->x0008 * 4));
            str_play_vol(0, 0x47, 0x3C);
            *(u8 *)0x3F3415 = 0x47;
        }
    }
}
