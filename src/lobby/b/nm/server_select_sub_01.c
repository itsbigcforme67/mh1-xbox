#include "lobby_f.h"
extern char test_server_sel_disp[];
extern char netr_sub01_tbl[];
extern char Disp_back[];
extern char ss_text_lobby_trans_ot[];
extern char netr_sub01_col[];
extern char ss_text_lobby_trans_ot[];
typedef struct { u8 pad0000[0x2]; u8 x0002; s8 x0003; u8 pad0004[0x2]; u8 x0006; u8 pad0007[0x1]; u8 x0008; } ARG_server_select_sub_01_arg0;
void server_select_sub_01(ARG_server_select_sub_01_arg0 *arg0) {
    if (arg0->x0006 == 0) {
        arg0->x0002 = (u8) (arg0->x0002 + 1);
        arg0->x0003 = 0;
        Lbc_set_prim(&test_server_sel_disp, 0, &ss_text_lobby_trans_ot);
        F(s32, &netr_sub01_tbl, 0x1C) = *((u8 *)&netr_sub01_col + (arg0->x0008 * 4));
        str_play_vol(0, 0x47, 0x3C);
        *(s8 *)0x3F3415 = 0x47;
        return;
    }
    arg0->x0002 = 3U;
    arg0->x0003 = 0;
    arg0->x0006 = 0U;
    Lbc_set_prim(0, &Disp_back, &ss_text_lobby_trans_ot);
}
