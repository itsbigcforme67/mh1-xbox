/* BGM setters (SLPM_654.95 0x0021DDC0-0x0021DF80): lobby_bgm_set2, fight_bgm_set, stage_bgm_etc_ck, demo_bgm_set. See bgm_nm.c. */
#include "types.h"

extern u8 game_w[];
extern u8 player_work[];
extern u8 em_work[];
extern u8 quest_w[];
extern u8 Snd_bgm_tbl[];
extern u8 Snd_rev_data_tbl[];
extern s32 Snd_rev_set_tbl[][4];
extern u8 fight_bgm_tbl[8];
extern u8 stage_bgm_etc_tbl[];

void fight_bgm_set();
void stage_bgm_set();
int Pl_master_ck();
int Quest_clear_bit_ck();
int Quest_clear_ck();
int flSndSetRev();
int str_fadein();
int str_fadein_vol();
int str_fadeout();
int str_getstat();
int str_pause();
int str_play();
int str_play_f_vol();
int str_stop();
int str_volume();












void lobby_bgm_set2(int n) {
    str_play(0, n & 0xFF);
    game_w[0x25] = n;
}

void fight_bgm_set(void) {
    u8 id;

    if (game_w[0x2E] >= 2 && game_w[0x2E] < 8) {
        id = fight_bgm_tbl[game_w[0x2E]];
    } else {
        switch (game_w[0x14]) {
        case 14:
            id = 0x4E;
            break;
        case 30:
            id = 0x4B;
            break;
        case 28:
            id = 0x4C;
            break;
        case 11:
            id = 0x4D;
            break;
        case 12:
            id = 0x47;
            break;
        default:
            id = 0x4E;
            break;
        }
    }
    str_play_f_vol(0, id, 0xF, 0x7F);
    game_w[0x25] = id;
}

int stage_bgm_etc_ck(int n) {
    u8 *p = stage_bgm_etc_tbl;

    while (*p != 0xFF) {
        if (*p == (n & 0xFF)) {
            return p[1];
        }
        p += 2;
    }
    return 0;
}

void demo_bgm_set(int n) {
    game_w[0x11] = 0;
    str_play_f_vol(0, n, 0xF, 0x7F);
    game_w[0x25] = n;
}
