/* Lobby: per-frame common move (SLPM_654.95 lobby overlay 0x5D6420-). Whole file; runs split into lb_pNN.c */
#include "lobby_f.h"

int Lb_move_common(void) {
    PLW *pl2;
    int i2;
    u8 *e;
    int i;
    u8 *c;
    PLW *pl;
    int j;
    u8 t;
    int r;
    u8 *k;
    e = em_work;
    if (lb_sys.x72 != 0) {
        lb_sys.x72 = lb_sys.x72 - 1;
    }
    if (lb_sys.x87 != 0) {
        lb_sys.x87 = lb_sys.x87 - 1;
    }
    if (lb_sys.x8D != 0) {
        lb_sys.x8D = lb_sys.x8D - 1;
    }
    lb_sys.x8E = lb_sys.x8E + 1;
    F(s16, lbShop, 0x8C) = Lb_shop_sw(0);
    Lb_check_newCommer();
    i = 0;
    c = (u8 *)lbCommer;
    pl = player_work;
    CW8(3) = 0;
    do {
        if (*(s8 *)c != 0 && PLU8(pl, 0x736) != 0 && ((k = cw, ((s8 *)k)[3] == 0) || memcmp(k + 3, c, 8) < 0)) {
            memcpy(cw + 3, c, 8);
        }
        i++;
        c += 0x5C;
        pl++;
    } while ((u32)i < 8);
    if (lb_sys.x7C != 0) {
        r = lb_sys.x7C - 1;
        lb_sys.x7C = r;
        if (r == 0 && lb_sys.x80 != 0) {
            Npc_se_req(lb_sys.x80, lb_sys.x84, lb_sys.x80 + 0xAC, 2);
        }
    }
    ItemCopy_Ud2Pl(&player_work[game_w.master]);
    Info_control();
    Lb_pl_move();
    i2 = 0;
    do {
        if (*e != 0) {
            old_pos_save(e);
            if (e[0x1E] == 0) {
                if (enemy_mv(e) == 0) {
                    enemy_mk(e);
                    em_ride_sub(e);
                }
            } else if (Lb_npc_mv(e) == 0) {
                Lb_npc_mk(e);
            }
        }
        i2++;
        e += 0xA10;
    } while ((u32)i2 < 0x14);
    player_mk();
    yure_move();
    Lb_check_target();
    lb_check_status();
    CameraMove();
    light_move();
    move_eft();
    move_set();
    move_stage();
    Pit_mv_lb();
    Lb_cockpit_move();
    j = 0;
    pl2 = player_work;
    do {
        if (pl2->be_flag != 0 && pl2->x01 != 0) {
            r = Lb_check_pl_load((s8)j);
            if (r != 0) {
                F(f32, pl2->work564, 8) = pl2->pos[0];
                F(f32, pl2->work564, 0xC) = pl2->pos[1];
                F(f32, pl2->work564, 0x10) = pl2->pos[2];
                r = add_prim(ot1, pl2->work564, 0x20);
            }
        }
        j++;
        pl2++;
    } while ((u32)j < 8);
    return r;
}
