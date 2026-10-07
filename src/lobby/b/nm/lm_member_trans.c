/* lm_member_trans (0x5B3220): 5/146 differ: the switch variable lands in a0 instead of a1 (original keeps pNet live across the ladder). Rewritten with array indexing (pointer-bump form was 139 off). Not built. */
#include "lobby_b.h"
extern char *pl_name_tbl[];
extern char plData[];
extern char *tbl;
extern struct { u8 _pad[0x14]; u8 x14; } PitMenu;
void lm_member_trans(void) {
    u8 sp[8];
    int i;
    int j;
    int y;
    int k;
    s32 sel;
    u8 st;

    st = pNet->depth;
    switch (st) {
    case 0:
        Disp_lb_menu();
        j = 0;
        for (i = 0; i < 8; i++) {
            if (i != game_w.master) {
                if (*(u8 *)&player_work[i] != 0) {
                    pl_name_tbl[j] = (char *)&lb_player[i] + 4;
                    if (Online_ck() == 1) {
                        if (PitMenu.x14 != 0) {
                            pl_name_tbl[j] = (char *)&lb_player[i] + 0x24;
                        }
                        sp[i] = cw[i * 0x2FC + 0x1348];
                    }
                } else {
                    pl_name_tbl[j] = tbl;
                }
                j++;
            }
        }
        sel = pNet->menu;
        if (game_w.master < sel) {
            sel--;
        }
        *(char ***)(plData + 0xC) = pl_name_tbl;
        DispFrameList(plData, 0, sel);
        y = 0x3C;
        for (i = 0; i < 8; i++) {
            if (i != game_w.master) {
                if (*(u8 *)&player_work[i] != 0 && Lb_get_pl_stat2((s8)i) == 0) {
                    Lb_put_status(0x1A4, y, 0x14, -1, sp[i]);
                }
                y = (s16)(y + 0x16);
            }
        }
        break;
    case 1:
    case 2:
        Lb_PlayerStatus((u8 *)lb_player + pNet->menu * 0x38, pNet->step);
        if (pNet->step == 0) {
            Disp_FriendListEntry(0x110, (pNet->depth - 1) & 0xFF);
        }
        break;
    }
}
