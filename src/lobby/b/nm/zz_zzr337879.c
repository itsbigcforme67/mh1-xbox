#include "lobby_b.h"
extern int tbl;
extern int pl_name_tbl[];
extern char plData[];
void lm_member_trans(void) {
    u8 st[8];
    u8 *pw;
    s32 off;
    int *np;
    s32 y;
    u8 *sq;
    u8 *lp;
    s32 j;
    u8 a;
    s32 k;
    u8 *sp;

    switch (pNet->depth) {
    case 0:
        Disp_lb_menu();
        j = 0;
        pw = (u8 *)player_work;
        lp = (u8 *)lb_player;
        np = pl_name_tbl;
        off = 0;
        sq = st;
        sp = sq;
        do {
            if (j != game_w.master) {
                if (*pw != 0) {
                    *np = (int)(lp + 4);
                    if (Online_ck() == 1) {
                        if (*(u8 *)0x39DAD4 != 0) {
                            *np = (int)(lp + 0x24);
                        }
                        *sp = cw[off + 0x1348];
                    }
                } else {
                    *np = tbl;
                }
                np++;
            }
            j++;
            pw += 0xA00;
            lp += 0x38;
            off += 0x2FC;
            sp++;
        } while (j < 8);
        a = pNet->menu;
        if (game_w.master < a) {
            a = a - 1;
        }
        *(int *)(plData + 0xC) = (int)pl_name_tbl;
        DispFrameList(plData, 0, a);
        y = 0x3C;
        j = 0;
        pw = (u8 *)player_work;
        do {
            if (j != game_w.master) {
                if (*pw != 0 && Lb_get_pl_stat2((s8)j) == 0) {
                    Lb_put_status(0x1A4, y, 0x14, -1, *sq);
                }
                y = (s16)(y + 0x16);
            }
            j++;
            pw += 0xA00;
            sq++;
        } while (j < 8);
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
