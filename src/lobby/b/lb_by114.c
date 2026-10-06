/* lb_by114 - agent B promoted near-match 0x005C2820-0x005C2988: server_select_sub_02 (first drafted by tools/lbauto.py). */
#include "lobby_f.h"
extern s8 ret_stat_0038A904;
extern u8 BsLbsCount;
extern char netr_sub01_tbl[];
extern char bsCsvWork[];
extern char netr_sub01_col[];
typedef struct { s16 x0000; s8 x0002; u8 x0003; u8 pad0004[0x4]; u8 x0008; } ARG_server_select_sub_02_arg0;

void server_select_sub_02(ARG_server_select_sub_02_arg0 *arg0) {
    u8 temp_a0;

    temp_a0 = arg0->x0003;
    switch (temp_a0) {                              /* irregular */
    case 0:
        tk_lever_ck((u8 *)arg0 + 8, (BsLbsCount - 1) & 0xFF, 1);
        F(s32, &netr_sub01_tbl, 0x1C) = ((s32 *)&netr_sub01_col)[arg0->x0008];
        if (tk_sw_on_ck(0x20) != 0) {
            if (atoi((u8 *)&bsCsvWork + (arg0->x0008 * 0x21) + 0x266C) != 0) {
                arg0->x0003 = (u8) (arg0->x0003 + 1);
                cnWrap_SoundRequest(0);
                arg0->x0000 = 0x28;
                str_fadeout(0, 0xF);
                return;
            }
            cnWrap_SoundRequest(3);
            return;
        }
        if (tk_sw_on_ck(0x40) != 0) {
            arg0->x0002 = 4;
            arg0->x0003 = 0U;
            cnWrap_SoundRequest(3);
            return;
        }
        break;
    case 1:
        arg0->x0000 = (s16) (arg0->x0000 - 1);
        if (arg0->x0000 < 0) {
            ret_stat_0038A904 = 0;
            return;
        }
        break;
    case 2:
        arg0->x0000 = (s16) (arg0->x0000 - 1);
        if (arg0->x0000 < 0) {
            ret_stat_0038A904 = -2;
        }
        break;
    }
}
