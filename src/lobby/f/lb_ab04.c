/* lb_ab04 - lobby member out-check 0x005CD0F0-0x005CD2B8: lb_member_outCheck (a member that left lbCommer: free its player/prim, clear the client records; returns 1 if any left).
   Declaration order = descending register (s5 result, i, c, off, pl, lp); i is an int wrapped with (s8)(i + 1). Whole file in lb_ab.c. */
#include "lobby_f.h"
extern PLW player_work[];
extern char lit_236_00664CC0[];
extern u8 D_3E4C05[];
extern u8 pl01_adr_tbl[];
void armor_model_free();
void release_prim();
void Chat_log_add(int, void *);
int lb_check_mini_data();
void Lb_set_mini_data_to_pl();
void Lb_set_player();
void flCompact();
void Lb_send_myChair();
void Lb_player_load();
void com_motion_load();
void Lbc_connect();
void npc_create_model();
void Lb_trans_pl();
char *strcpy();
int lb_member_outCheck(void) {
    char buf[0x120];
    int sp[0x40 / 4];
    int s5;
    int i;
    u8 *c;
    int off;
    u8 *pl;
    u8 *lp;
    s5 = 0;
    i = 0;
    c = (u8 *)lbCommer;
    off = 0;
    pl = (u8 *)player_work;
    lp = (u8 *)lb_player;
    do {
        if (*(s8 *)c == 0 && *(s8 *)(cw + off + 0x132C) != 0) {
            pl[1] = 0;
            s5 = 1;
            pl[0] = 0;
            armor_model_free(pl);
            if (*(s32 *)(pl + 0x564) != 0) {
                release_prim(*(s16 *)(pl + 0x568));
                *(s32 *)(pl + 0x564) = 0;
            }
            if (cw[0x35D5] != 0 && (cw + off + 0x1334) != 0) {
                memset(buf, 0, 0x120);
                buf[0x11F] = 6;
                buf[0x11E] = 6;
                buf[0x11D] = 6;
                sprintf(buf + 0x1C, lit_236_00664CC0, cw + off + 0x1334);
                Chat_log_add(0, buf);
            }
            memset(pl, 0, 0xA00);
            memset(cw + off + 0x132C, 0, 8);
            memset(cw + off + 0x1334, 0, 8);
            memset(cw + off + 0x1346, 0, 8);
            *(s8 *)(lp + 0x24) = 0;
        }
        c += 0x5C;
        i = (s8)(i + 1);
        off += 0x2FC;
        pl += 0xA00;
        lp += 0x38;
    } while (i < 8);
    return s5;
}
