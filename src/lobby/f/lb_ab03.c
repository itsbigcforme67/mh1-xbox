/* lb_ab03 - lobby member change check 0x005CD2C0-0x005CD384: lb_member_changeCheck (1 when a present member's mini data differ from lbCommer's). lb_check_mini_data takes 3 arguments
   (index, commer, commer + 0x1C); i is an int wrapped with (s8)(i + 1); the declaration order r, i, pl, s1, c was found by permuting. Whole file in lb_ab.c. */
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
int lb_member_changeCheck(void) {
    int r;
    int i;
    u8 *pl;
    int s1;
    u8 *c;
    r = 0;
    i = 0;
    pl = (u8 *)player_work;
    s1 = 0;
    c = (u8 *)lbCommer;
    do {
        if (*pl != 0 && memcmp(c + 0x1C, cw + s1 + 0x1346, 0x40) != 0 && lb_check_mini_data(i, c, c + 0x1C) == 1) {
            r = 1;
        }
        pl += 0xA00;
        i = (s8)(i + 1);
        s1 += 0x2FC;
        c += 0x5C;
    } while (i < 8);
    return r;
}
