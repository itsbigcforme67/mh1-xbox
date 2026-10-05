/* Player code (SLPM_654.95 0x00149980-0x00149E2C): pl_die000 (death reaction: lie down, quest failure messages, respawn position
   after 0x258 frames in a coop game), pl_die (dispatcher). */
#include "pl.h"
#include "game.h"
#include "plf.h"

typedef struct { u8 _pad00[0x14]; s32 x14; } PL_QUEST_W;
extern PL_QUEST_W quest_w;
extern u8 lit_6243[], lit_6244[], lit_6245[];
s8 Quest_remuneration_calc(void);
void Quest_forfeit_message(void);
void Quest_restart(void);
void die_bgm_set(void);
void PlayerDieCameraRequest(void);

void pl_die000(PLW *pl, s32 arg1) {
    int c;
    u8 v;
    u8 idx;
    u16 r;
    u8 s;

    pl->work40E = 0xA;
    Pl_view_reset(pl);
    pl->work8C2 = 0;
    pl->x8C6 = 0;
    pl_flag_clr(pl, 0x80000);
    v = pl->work56B;
    if (v & 0xF) {
        pl->work56B = v & 0xF0;
        func_549200(pl, 4);
    }
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        switch (arg1) {
        case 0:
            pl_chr_set2(pl, 0xD2, 0, 0);
            break;
        case 1:
            pl_chr_set2(pl, 0xD2, 0, 0x8C);
            break;
        case 2:
            pl_chr_set2(pl, 0xD1, 0, 0x8C);
            pl_flag_set(pl, 0x20000);
            break;
        }
        vib_set_pl(pl, 3);
        action_timer_calc(pl, 0);
        pl_voice_req(pl, 0x22);
        pl_flag_clr(pl, 2);
        pl_flag_clr(pl, 0x8000);
        pl->flag12 = 0;
        c = (s8)Quest_remuneration_calc();
        if (c != 0) {
            pl->x05++;
        }
        if (Pl_master_ck(pl) == 0) {
            set01_set(3, 0, (s16)pl->id);
            if ((s8)c < 2) {
                Quest_forfeit_message();
            }
            if ((quest_w.x14 <= 0) && ((s8)c == 1)) {
                set01_set2(lit_6243);
            }
            break;
        }
        v = pl->work91E;
        if (v < 0x3F) {
            pl->work91E = v + 1;
        } else {
            pl->work91E = 0x63;
        }
        PlayerDieCameraRequest();
        set01_set2(lit_6244);
        if ((s8)c < 2) {
            Quest_forfeit_message();
        }
        if (pl->x05 < 2) {
            Quest_restart();
            die_bgm_set();
            break;
        }
        if ((s8)c == 1) {
            set01_set2(lit_6243);
        }
        set01_set2(lit_6245);
        break;
    case 1:
        if ((pl->char0 == 0xD2) && (frame_check(164.0f, pl, 0) != 0) && !((r = ran_suu(1)) & 0x3F)) {
            pl_chr_set2(pl, 0xDF, 0, 0);
        } else if ((pl->char0 == 0xDF) && (pl->work194 == 0)) {
            pl_chr_set2(pl, 0xD2, 4, 0xA4);
        }
        if (Game_clear_ck(2) == 1) {
            pl->x05++;
            break;
        }
        if ((pl->work39C == 0x12C) && (Pl_master_ck(pl) == 1)) {
            set01_set(0, 7, 0);
        }
        if ((pl->work39C == 0x258) && (Pl_master_ck(pl) == 1)) {
            idx = game_w.x2F;
            pl->x738 = 2;
            pl->work73C = stage_start_pos[idx][0];
            pl->work740 = stage_start_pos[idx][1];
            pl->work744 = stage_start_pos[idx][2];
            pl->work570 = stage_start_ang[idx];
            pl->x73A = idx;
            net_send_pl(pl, 4, 0);
        }
        break;
    case 2:
        break;
    }
}

void pl_die(PLW *pl) {
    pl->work8C2 = 0;
    pl->x8C6 = 0;
    switch (pl->flag15) {
    case 0:
        pl_die000(pl, 0);
        break;
    case 1:
        pl_die000(pl, 1);
        break;
    case 2:
        pl_die000(pl, 2);
        break;
    default:
        pl_to_normal(pl, 0, 4, 0);
    }
}
