/* Lobby: trade, sleep, exit (SLPM_654.95 lobby overlay 0x5D0750-). Whole file; runs split into lb_lNN.c */
#include "lobby.h"

int trade_get_ck_005D0750(PLW *pl) {
    s16 i;
    PLW *o;
    u16 item;
    if (Online_ck() == 0) {
        return 0;
    }
    if (pl->work936 != 0) {
        return 0;
    }
    o = player_work;
    i = 0;
    do {
        if (o->be_flag != 0) {
            if (i != pl->id && (s16)act_ck(o, 0, 0x2D) != 0 && o->work909 == pl->id && flvecCalcDistance(pl->pos, o->pos) <= 300.0f) {
                item = o->work904;
                if (Item_data[item][2] < 3) {
                    if ((s16)Ud_item_num_ck(item) == 0) {
                        if ((s16)Ud_item_search_space() != 0) {
                            pl->work904 = o->work904;
                            pl->work906 = o->work906;
                            if (Pl_master_ck(pl) == 1) {
                                Lb_send_item_request(CWPLAYER(i) + 0x132C, o);
                            }
                            Lb_Pl_act_set2(pl, 0, 0x2E, 0);
                        } else {
                            set01_set2(lit_516_00664D50);
                            goto set936;
                        }
                    } else if ((s16)Ud_item_num_ck2(o->work904) >= o->work906) {
                        pl->work904 = o->work904;
                        pl->work906 = o->work906;
                        if (Pl_master_ck(pl) == 1) {
                            Lb_send_item_request(CWPLAYER(i) + 0x132C, o);
                        }
                        Lb_Pl_act_set2(pl, 0, 0x2E, 0);
                    } else {
                        set01_set(1, 0xF, (s16)o->work904);
set936:
                        pl->work936 = 0x5A;
                    }
                    return 1;
                }
            }
        }
        i++;
        o++;
    } while (i < 8);
    return 0;
}

void pl_sleeping(PLW *pl) {
    s32 q[3];
    int t;
    u8 *src = lit_584_0064E1A8;
    *(long *)q = *(long *)src;
    q[2] = *(s32 *)(src + 8);
    if (pl->char0 == 0x1AB && (ran_suu(1) & 0xFFFF & 0x3F) == 0) {
        Lb_pl_chr_set(pl, 0x1AC, 0, 0);
    } else if (pl->char0 == 0x1AC && F(s32, pl, 0x194) == 0) {
        Lb_pl_chr_set(pl, 0x1AB, 0, 0);
    }
    t = *(u16 *)0x3F340E % 60;
    switch (t) {
    case 0:
    case 0xA:
    case 0x14:
        Eft06_set2(0.6f, pl, 4, 0x14, q);
    }
}

void lb_exit_save(PLW *pl) {
    pl_flag_clr(pl, 0x20000);
    Lb_Pl_act_set2(pl, 0, 0x35, 0);
    CW8(0x2C08) = 1;
    *(s8 *)0x3F36AB = 1;
    str_pause(0, 0);
    str_volume(0, 0);
    str_fadein_vol(0, 0x1E, D_32D471[game_w.stage * 2]);
}

void lb_goto_guest_room(PLW *pl, int no) {
    u8 *p;
    u8 v;
    u16 r;
    s32 *t;
    u8 m;
    u8 *c;
    switch (Lb_check_hotel(no)) {
    case 1:
        if (no == 0x55) {
            r = ran_suu(1);
            CW8(0x35D8) = (r % 27) * 2;
        }
        t = &lbs_command_jmp[0xB3];
        Gold_add(-t[no]);
        cnWrap_SoundRequest(8);
        p = D_3C7357 + no;
        v = *p;
        if (v < 0x96) {
            *p = v + 1;
        }
    case 2:
        ((u8 *)&lb_sys)[0x71] = 0;
        lb_sys.x03 = 5;
        F(s16, pl, 0x73A) = no;
        c = cw;
        m = (1 << (no - 0x51)) & 0xFF;
        c[0x35D7] = c[0x35D7] | m;
        lb_sys.x68 = 0x14;
        break;
    case 0:
        break;
    case 3:
        Lb_put_set01(0xA);
    }
}
