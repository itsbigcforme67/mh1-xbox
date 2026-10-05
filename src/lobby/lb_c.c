/* Lobby: small helpers (SLPM_654.95 lobby overlay). Whole file; matching runs split into lb_cNN.c */
#include "lobby.h"

void lb_rule_seet_trans_ot2(s32 *p) {
    if (SoftKeyboard_alive_check() != 0) {
        font_set_stack_no(p[6]);
        DispSoftkeyboard(1);
    }
}

void Lb_Matching(void) {
    u8 m = CW8(0x32C5);
    if (m == 1) {
        Lbs_MatchStart(m);
    }
}

int get_questLevelNum(void) {
    if (Online_ck() == 1) {
        return 6;
    }
    return 5;
}

int Chk_lb_status(int n) {
    return lb_sys.x68 == n;
}

void Lb_menu_quest_info(s32 *p) {
    *(s32 **)0x3C74D4 = p;
    *(s32 *)0x3C7450 = p[4];
    *(s32 *)0x3C7454 = p[2];
}

u8 *GetAdrsMiniData(int id) {
    return CWPLAYER(id & 0xFF) + 0x1346;
}

void Lb_put_msg_type2(s16 *p) {
    flfntLocate(p[0], p[1]);
    font_print(lit_429_00664C38, *(s32 *)(p + 2));
}

void Lb_put_msg2(int a0, int a1, char *msg) {
    flfntLocate();
    font_print(lit_429_00664C38, msg);
    strlen_sp(msg);
}

void Lb_pl_chr_set0(PLW *pl) {
    pl->work81D = 0;
    lb_pl_chr_set_com();
}

void Lb_pl_sw_set(void) {
    lb_sw_set_sub(game_w.master);
}

void Lb_Pl_act_set2(PLW *pl) {
    Lb_Pl_act_set();
    PLU8(pl, 0x6FF) = 1;
}

void Lb_chidori_cnt_up(u8 *pl) {
    if (pl[0x8EC] == 0) {
        pl[0x90F]++;
        if (pl[0x90F] >= 0xA) {
            pl[0x8EC] = 1;
        }
    }
}
