/* BGM server (SLPM_654.95 0x0021D5E0-0x0021DBE0): bgm_server, adx_se_set/stop, die_bgm_set. See bgm_nm.c. */
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












void bgm_server(void) {
    if (*(u16 *)(game_w + 0x26) > 0) {
        *(u16 *)(game_w + 0x26) = *(u16 *)(game_w + 0x26) - 1;
    }
    if (game_w[0x10] & 2) {
        if (str_getstat(1) >= 4) {
            if (game_w[0x11] != 0) {
                str_fadein_vol(0, 0xF, 0x7F);
            } else {
                str_fadein_vol(0, 0xF, Snd_bgm_tbl[1 + game_w[0x14] * 2]);
            }
            game_w[0x10] = game_w[0x10] & 0xFD;
        }
    }
    if (game_w[0x11] < 7) {
        int r = Quest_clear_ck(1);
        if (r != 0) {
            if (r == 1) {
                if (*(s32 *)(quest_w + 0x40) & 2) {
                    game_w[0x11] = 7;
                    *(u16 *)(game_w + 0x26) = 0x50;
                } else {
                    str_fadeout(0, 0xF);
                    *(u16 *)(game_w + 0x26) = 0xF;
                    game_w[0x11] = 8;
                }
            } else {
                str_fadeout(0, 0xF);
                *(u16 *)(game_w + 0x26) = 0xF;
                game_w[0x11] = 10;
            }
        }
    }
    switch (game_w[0x11]) {
    case 0:
        if (em_status_ck() != 0) {
            game_w[0x11]++;
            str_fadeout(0, 0xF);
            *(u16 *)(game_w + 0x26) = 0xF;
        }
        break;
    case 1:
        if (*(u16 *)(game_w + 0x26) > 0) break;
        game_w[0x11]++;
        *(u16 *)(game_w + 0x26) = 0x96;
        str_play_f_vol(0, 0x56, 0xF, 0x7F);
        game_w[0x25] = 0x56;
        break;
    case 2:
        if (*(u16 *)(game_w + 0x26) > 0) break;
        switch (em_status_ck()) {
        case 0:
            game_w[0x11] = 5;
            str_fadeout(0, 0xF);
            break;
        case 1:
            break;
        case 2:
            game_w[0x11]++;
            str_fadeout(0, 0xF);
            *(u16 *)(game_w + 0x26) = 0xF;
            break;
        }
        break;
    case 3:
        if (*(u16 *)(game_w + 0x26) > 0) break;
        game_w[0x11]++;
        *(u16 *)(game_w + 0x26) = 0x96;
        fight_bgm_set();
        break;
    case 4:
        if (*(u16 *)(game_w + 0x26) > 0) break;
        switch (em_status_ck()) {
        case 0:
            *(u16 *)(game_w + 0x26) = 0xF;
            str_fadeout(0, 0xF);
            game_w[0x11]++;
            break;
        case 1:
            break;
        case 2:
            break;
        }
        break;
    case 5:
        if (*(u16 *)(game_w + 0x26) > 0) break;
        game_w[0x11] = 0;
        stage_bgm_set(game_w[0x14]);
        break;
    case 7:
        if (*(u16 *)(game_w + 0x26) > 0) break;
        str_fadeout(0, 0xF);
        *(u16 *)(game_w + 0x26) = 0xF;
        game_w[0x11] = 8;
    case 8:
        if (*(u16 *)(game_w + 0x26) > 0) break;
        game_w[0x11]++;
        str_play_f_vol(0, 0x49, 0xF, 0x7F);
        game_w[0x25] = 0x49;
    case 9:
        if (Quest_clear_ck(1) == -1) {
            game_w[0x11] = 0xB;
            str_play_f_vol(0, 0x57, 0xF, 0x7F);
            game_w[0x25] = 0x57;
        }
        break;
    case 10:
        if (*(u16 *)(game_w + 0x26) > 0) break;
        game_w[0x11]++;
        str_play_f_vol(0, 0x57, 0xF, 0x7F);
        game_w[0x25] = 0x57;
    case 11:
        if (Quest_clear_ck(1) == 1) {
            game_w[0x11] = 9;
            str_play_f_vol(0, 0x49, 0xF, 0x7F);
            game_w[0x25] = 0x49;
        }
        break;
    }
}

void adx_se_set(int a0, int id) {
    if (Pl_master_ck() == 1) {
        str_play(1, id);
        *(s16 *)(game_w + 0x1E0) = id;
        if (game_w[0x11] != 0 && Quest_clear_ck(1) == 0) {
            game_w[0x10] = game_w[0x10] | 2;
            str_fadein_vol(0, 0xF, 0x5F);
        }
    }
}

void adx_se_stop(void) {
    if (Pl_master_ck() == 1) {
        if (*(u16 *)(game_w + 0x1E0) == 0xA) {
            str_stop(1, *(u16 *)(game_w + 0x1E0));
        }
        if (game_w[0x10] & 2) {
            game_w[0x10] = game_w[0x10] & 0xFD;
            if (game_w[0x11] == 0) {
                str_fadein_vol(0, 0xF, Snd_bgm_tbl[1 + game_w[0x14] * 2]);
                return;
            }
            str_fadein_vol(0, 0xF, 0x7F);
        }
    }
}

void die_bgm_set(void) {
    game_w[0x11] = 6;
    str_play_f_vol(0, 0x4A, 0xF, 0x7F);
}
