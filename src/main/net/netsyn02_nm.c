/* Network play sync, player packets (SLPM_654.95 0x001BA2C0-0x001BA8E0): net_receive_pl.
 * The payload (packet + 4) of kinds 1..8 is copied into the player work (slot - 1 unless the packet is a forced update). */
#include "types.h"
#include "netsyn.h"
#include "game.h"

void net_receive_pl(int no, u8 *buf, int force) {
    NPLRX *p;
    NPLV *pl;
    NPLV *q;
    u8 kind;
    u16 u;
    int n;

    if (Online_ck() != 0 && Now_Game_ck() != 0) {
        if (force == 0 && no - 1 == game_w.master) {
            return;
        }
        kind = buf[0];
        p = (NPLRX *)(buf + 4);
        pl = (NPLV *)(player_work + (no - 1) * 0xA00);
        switch (kind) {
        case 1:
            pl->x4D5 = p->a.b0;
            pl->x12 = p->a.b1;
            if (p->a.flags & 4) {
                pl->posx = p->a.f4;
                pl->posy = p->a.f8;
                pl->posz = p->a.fC;
                pl->x818 = 0;
            } else {
                pl->x800 = p->a.f4;
                pl->x804 = p->a.f8;
                pl->x808 = p->a.fC;
                Pl_adj_calc(pl, 0xA, kind);
            }
            if (p->a.flags & 8) {
                pl->xA4 = p->a.u10;
                pl->ang_y = pl->xA4;
            } else {
                pl->ang_y = p->a.u10;
            }
            Pl_act_set(pl, p->a.b12, p->a.b13, 0);
            pl->x88A = p->a.u24;
            pl->x56C = p->a.b15;
            pl->cnt39A = p->a.u16_;
            pl->x880 = p->a.b1C;
            pl->x56B = p->a.b1D;
            pl->vital = p->a.s18;
            pl->x748 = p->a.s1A;
            pl->x882 = p->a.s20;
            pl->x792 = p->a.s22;
            pl->x887 = p->a.b1E;
            pl->x8C9 = p->a.c1F;
            pl->x01 = p->a.b28;
            pl->x736 = p->a.b14;
            pl->x6AC = p->a.u26;
            pl->x890 = pl->posx;
            pl->x894 = pl->posy;
            pl->x898 = pl->posz;
            if (pl->x14 == 0 && Pl_master_ck(pl) == 0) {
                switch (pl->x15) {
                case 0x18:
                case 0x14:
                    Oki_item_set(pl);
                    break;
                case 0x2F:
                    Ana_item_set(pl);
                    break;
                case 0x63:
                    Fue_item_set(pl);
                    break;
                case 0x70:
                    Taru_item_set(pl);
                    break;
                case 0x71:
                    Basic_item_set(pl);
                    break;
                }
            }
            break;
        case 2:
            pl->ang_y = p->b.uC;
            pl->x800 = p->b.f0;
            pl->x804 = p->b.f4;
            pl->x808 = p->b.f8;
            Pl_adj_calc(pl, 0x14, kind);
            pl->vital = p->b.sE;
            pl->x748 = p->b.s10;
            pl->x736 = p->b.b12;
            pl->x4D5 = p->b.b13;
            pl->x792 = p->b.s14;
            pl->x882 = p->b.s16_;
            pl->x890 = p->b.f0;
            pl->x894 = p->b.f4;
            pl->x898 = p->b.f8;
            break;
        case 3:
        case 4:
        case 5:
            pl->x73C = p->c.f0;
            pl->x740 = p->c.f4;
            pl->x744 = p->c.f8;
            pl->x570 = p->c.uC;
            pl->ang_y = p->c.uC;
            pl->x736 = p->c.bE;
            pl->x56B = p->c.bF;
            if (kind == 4) {
                pl->x01 = 0;
            } else {
                if (kind == 3) {
                    pl->x738 = 0;
                } else {
                    pl->x738 = 1;
                }
                pl_init_sub(pl);
            }
            pl->posx = pl->x73C;
            pl->posy = pl->x740;
            pl->posz = pl->x744;
            pl->x890 = pl->x73C;
            pl->x894 = pl->x740;
            pl->x898 = pl->x744;
            break;
        case 6:
            pl->x4D5 = p->d.b0;
            pl->x12 = p->d.b1;
            if (p->d.flags & 4) {
                pl->posx = p->d.f4;
                pl->posy = p->d.f8;
                pl->posz = p->d.fC;
                pl->x818 = 0;
            } else {
                pl->x800 = p->d.f4;
                pl->x804 = p->d.f8;
                pl->x808 = p->d.fC;
                Pl_adj_calc(pl, 0xA, kind);
            }
            if (p->d.flags & 8) {
                pl->xA4 = p->d.u10;
                pl->ang_y = pl->xA4;
            } else {
                pl->ang_y = p->d.u10;
            }
            Pl_act_set(pl, p->d.b12, p->d.b13, 0);
            pl->x88A = p->d.u1E;
            pl->x56C = p->d.b15;
            pl->cnt39A = p->d.u16_;
            pl->x792 = p->d.s20;
            pl->x909 = p->d.b18;
            pl->x904 = p->d.u1A;
            pl->x906 = p->d.u1C;
            pl->x890 = pl->posx;
            pl->x894 = pl->posy;
            pl->x898 = pl->posz;
            break;
        case 7:
            if (p->e.b3 == game_w.master) {
                q = (NPLV *)(player_work + game_w.master * 0xA00);
                q->x90A = p->e.b2;
                if (act_ck(q, 0, 0x67) != 0 && q->x909 == p->e.b2 && q->x904 == p->e.u4) {
                    n = q->x906;
                    if (n == p->e.s6 && Pl_item_num_ck(q, q->x904) >= n) {
                        Pl_item_stack(q, q->x904, -n);
                        net_send_pl(q, 8, 1);
                        q->x90A = 0xFF;
                        return;
                    }
                }
                net_send_pl(q, 8, 2);
            }
            break;
        case 8:
            if (p->e.b2 == game_w.master) {
                q = (NPLV *)(player_work + game_w.master * 0xA00);
                if (p->e.b0 & 1) {
                    q->x90A = 1;
                    u = p->e.u4;
                    Pl_item_stack(q, u, p->e.s6);
                    set01_set(1, 0xD, (s16)u);
                    return;
                }
                q->x90A = 2;
            }
            break;
        }
    }
}
