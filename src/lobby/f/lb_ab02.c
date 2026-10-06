/* lb_ab02 - lobby member in-check 0x005CD390-0x005CD49C: lb_member_inCheck (a member that appeared in lbCommer but not yet in the client work: copy id/name/mini data, set up the player).
   i is an int that is wrapped with (s8)(i + 1) and cast (s8) where used as an index (no sign extension before the calls), `base` (cw + off) is declared first. Whole file in lb_ab.c. */
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
void lb_member_inCheck(void) {
    u8 *base;
    int i;
    u8 *c;
    int off;
    u8 *c2;
    u8 *pl;
    i = 0;
    c = (u8 *)lbCommer;
    off = 0;
    c2 = c;
    pl = (u8 *)player_work;
    do {
        if (*(s8 *)c != 0) {
            base = cw + off;
            if (*(s8 *)(base + 0x132C) == 0) {
                strcpy((char *)(base + 0x132C), (char *)c2);
                strcpy((char *)(cw + off + 0x1334), (char *)c + 8);
                memcpy(cw + off + 0x1346, c + 0x1C, 0x40);
                Lb_set_mini_data_to_pl(i, c + 0x1C);
                Lb_set_player(i & 0xFF, c, c + 8);
                *(s8 *)(cw + (s8)i + 0x2BFE) = 0;
                pl[1] = 1;
            }
        }
        c += 0x5C;
        i = (s8)(i + 1);
        off += 0x2FC;
        c2 += 0x5C;
        pl += 0xA00;
    } while (i < 8);
}
