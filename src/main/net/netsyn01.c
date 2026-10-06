/* Network play sync, player packets (SLPM_654.95 0x001B9F70-0x001BA2C0): net_send_pl.
 * The packet is built in a local union of per-kind layouts (cmd, len, 0, 0, then kind specific fields), then queued with AQ_data_put. */
#include "types.h"
#include "netsyn.h"

void net_send_pl(NPLV *pl, u8 kind, s16 arg2) {
    NPLPK pk;
    int k;

    if (Online_ck() != 0 && Now_Game_ck() != 0 && pl->x10 == 0) {
        if (Pl_master_ck(pl) != 0) {
            k = kind;
            pk.a.x3 = 0;
            pk.a.x2 = 0;
            switch (k) {
            case 1:
                pk.a.cmd = kind;
                pk.a.len = 0x2D;
                pk.a.b4 = pl->x4D5;
                pk.a.b5 = pl->x12;
                pk.a.s6 = arg2;
                pk.a.f8 = pl->posx;
                pk.a.fC = pl->posy;
                pk.a.f10 = pl->posz;
                pk.a.u14 = pl->ang_y;
                pk.a.b16 = pl->x14;
                pk.a.b17 = pl->x15;
                pk.a.u28 = pl->x88A;
                pk.a.b19 = pl->x56C;
                pk.a.u1A = pl->cnt39A;
                pk.a.c20 = pl->x880;
                pk.a.b21 = pl->x56B;
                pk.a.s1C = pl->vital;
                pk.a.s1E = pl->x748;
                pk.a.s24 = pl->x882;
                pk.a.s26 = pl->x792;
                pk.a.b22 = pl->x887;
                pk.a.c23 = pl->x8C9;
                pk.a.b2C = pl->x01;
                pk.a.b18 = pl->x736;
                pk.a.u2A = pl->x6AC;
                net_plpos_set(pl, &pl->posx);
                break;
            case 2:
                pk.b.cmd = kind;
                pk.b.len = 0x1C;
                pk.b.f4 = pl->posx;
                pk.b.f8 = pl->posy;
                pk.b.fC = pl->posz;
                pk.b.u10 = pl->ang_y;
                pk.b.s12 = pl->vital;
                pk.b.s14 = pl->x748;
                pk.b.b16 = pl->x736;
                pk.b.b17 = pl->x4D5;
                pk.b.s18 = pl->x792;
                pk.b.s1A = pl->x882;
                net_plpos_set(pl, &pl->posx);
                break;
            case 3:
            case 4:
            case 5:
                pk.c.cmd = kind;
                pk.c.len = 0x14;
                pk.c.f4 = pl->x73C;
                pk.c.f8 = pl->x740;
                pk.c.fC = pl->x744;
                pk.c.u10 = pl->x570;
                pk.c.b12 = pl->x73A;
                pk.c.b13 = pl->x56B;
                net_plpos_set(pl, &pl->x73C);
                break;
            case 6:
                pk.d.cmd = kind;
                pk.d.len = 0x26;
                pk.d.b4 = pl->x4D5;
                pk.d.b5 = pl->x12;
                pk.d.s6 = arg2;
                pk.d.f8 = pl->posx;
                pk.d.fC = pl->posy;
                pk.d.f10 = pl->posz;
                pk.d.u14 = pl->ang_y;
                pk.d.b16 = pl->x14;
                pk.d.b17 = pl->x15;
                pk.d.u22 = pl->x88A;
                pk.d.b19 = pl->x56C;
                pk.d.u1A = pl->cnt39A;
                pk.d.u24 = pl->x792;
                pk.d.b1C = pl->x909;
                pk.d.u1E = pl->x904;
                pk.d.u20 = pl->x906;
                net_plpos_set(pl, &pl->posx);
                break;
            case 7:
            case 8:
                pk.e.cmd = kind;
                pk.e.len = 0xC;
                if (k == 7) {
                    pk.e.b6 = pl->id;
                    pk.e.b7 = arg2;
                } else {
                    pk.e.b4 = arg2;
                    pk.e.b6 = pl->x90A;
                    pk.e.b7 = pl->id;
                }
                pk.e.u8 = pl->x904;
                pk.e.sA = pl->x906;
                break;
            }
            pl->x90E = 0x1E;
            AQ_data_put(pl->id + 1, (u8 *)&pk, 0);
        }
    }
}
