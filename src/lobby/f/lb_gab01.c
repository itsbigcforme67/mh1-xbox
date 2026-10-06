/* lb_gab01 - near-match fixes 0x005CD4A0-0x005CD6CC: Lb_check_newCommer. Whole file in lb_ab.c. */
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

void Lb_check_newCommer(int a) {
    int i;
    u8 *pl;
    u8 st;
    if (*(s8 *)(cw + 0x2C08) != 0) {
        if (lb_member_outCheck() == 1) {
            st = game_w.stage;
            if (st != 0x4C) {
                if (st == 0x4D) {
                    goto b5;
                }
            } else {
b5:
                flCompact(st);
                lb_sys.x8D = 3;
            }
            lb_sys.chair_mask = 0;
            st = D_3E4C05[game_w.master * 0xA00];
            switch (st) {
            case 0x4C:
            case 0x54:
            case 0x59:
            case 0x5A:
            case 0x5C:
            case 0x5D:
            case 0x5E:
            case 0x60:
            case 0x4E:
            case 0x2A:
            case 0x29:
            case 0x2B:
                Lb_send_myChair();
            }
            return;
        }
        if (lb_sys.x8D == 0) {
            if (lb_member_changeCheck() == 1) {
                st = game_w.stage;
                if (st != 0x4C) {
                    if (st == 0x4D) {
                        goto b27;
                    }
                } else {
b27:
                    flCompact(st);
                    lb_sys.x8D = 3;
                }
                return;
            }
            if (lb_sys.x8D == 0) {
                lb_member_inCheck();
                st = game_w.stage;
                if (st != 0x4D) {
                    if (st == 0x4C) {
                        goto b33;
                    }
                } else {
b33:
                    i = 0;
                    pl = (u8 *)player_work;
                    do {
                        if (*pl != 0 && *(s8 *)(cw + (s8)i + 0x2BFE) == 0) {
                            Lb_player_load(pl);
                        }
                        i = (s8)(i + 1);
                        pl += 0xA00;
                    } while (i < 8);
                }
            }
        }
    }
}
