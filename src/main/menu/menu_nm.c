/* menu_nm - f_menu (SLPM_654.95 0x00127440-0x00134950, main.bin): the whole pit menu file as near-match C, not built; matching runs are built from it as menuNN.c.
 * Field meanings are guesses. */
#include "menu.h"
#include "em.h"
#include "pl.h"

extern u8 Psw[];
extern u8 enemy_icon_tbl[];
extern f32 map_size[][2];
extern u8 room_member_id[];

void *memset(void *, int, int);
void Chat_log_clear(void);
void pit_prim_init(void);
u16 pit_key_repeat(u16, u16);
void SoftKeyboard_exit(void);
int Quest_time_get(int);
void se_req(int, int, int);
int Online_ck(void);
int Pl_Skill_ck(PLW *, int);
void trans_pit_0();
void trans_pit_1();
void trans_pit_2();
void trans_pit_1_lb();
void trans_pit_2_lb();
void func_5B3D70();
void func_609750();
void font_print_uf(char *, int);
u8 *func_5B4D30(u8);

void PitWork_init(void) {
    lpPit = &pit_work;
    memset(lpPit, 0, 0x90);
    GWS8(0xE) = 0;
    memset(&PitMenu, 0, 0x1764);
    lpPit->x3C = 10;
    Chat_log_clear();
}

void Pit_init(void) {
    PLW *p;
    u16 z = 0;

    pit_prim_init();
    pit_prim[0].trans = trans_pit_0;
    pit_prim[1].trans = trans_pit_1;
    pit_prim[2].trans = trans_pit_2;
    lpPit->pl = &player_work[game_w.master];
    *(s32 *)&lpPit->x04 = 0;
    pit_key_repeat(0, 0);
    lpPit->x40 = 0;
    GWS8(0xE) = 0;
    SoftKeyboard_exit();
    Chat_log_clear();
    PitMenu.open = 0;
    PitMenu.x0F = z;
    PitMenu.x0C = 0;
    lpPit->x80 = -1;
    PitMenu.x17 = 3;
    PitMenu.x15 = 1;
    PitMenu.x16 = 0;
    PitMenu.x18 = 0;
    PitMenu.x14 = 0;
    lpPit->map_sx = 1.0f / map_size[game_w.x2E][0];
    lpPit->map_sy = 1.0f / map_size[game_w.x2E][1];
    lpPit->x34[3] = -1;
    lpPit->x34[2] = -1;
    lpPit->x34[1] = -1;
    lpPit->x34[0] = -1;
    *(s8 *)((u8 *)lpPit + 0x3E) = 0;
    *(s8 *)((u8 *)lpPit + 0x3F) = 0;
    lpPit->x41 = 0;
    PitMenu.x10 = 0;
    lpPit->x84 = 0;
    lpPit->x52 = 0;
    lpPit->x8D = 0;
    lpPit->x28 = 4;
    lpPit->x29 = 0;
    lpPit->x2B = 0;
    p = lpPit->pl;
    lpPit->x5A = *(u16 *)((u8 *)p + 0x888);
    lpPit->x5E = *(u16 *)((u8 *)p + 0x88E);
    lpPit->lb = 0;
    lpPit->x8A = GW8(0x1DD);
    lpPit->x8B = game_w.x0F;
    lpPit->x8C = FLDS8(option_w, 3);
    PitMenu.x00 = 0;
    PitMenu.x08 = -1;
    PitMenu.x06 = 0;
    lpPit->x10[0] = -1;
    lpPit->x10[1] = -1;
    lpPit->x10[2] = -1;
    lpPit->x10[3] = -1;
    if (game_w.x1DC == 1) {
        func_5B3D70();
        func_609750();
        lpPit->lb = 1;
        pit_prim[1].trans = trans_pit_1_lb;
        pit_prim[2].trans = trans_pit_2_lb;
    } else {
        lpPit->lb = 0;
        lpPit->time0 = Quest_time_get(0);
        lpPit->time1 = Quest_time_get(1);
    }
}

void Pit_reset(void) {
    PLW *p;
    u16 z = 0;

    pit_prim_init();
    pit_prim[0].trans = trans_pit_0;
    pit_prim[1].trans = trans_pit_1;
    pit_prim[2].trans = trans_pit_2;
    lpPit->pl = &player_work[game_w.master];
    *(s32 *)&lpPit->x04 = 0;
    pit_key_repeat(0, 0);
    lpPit->x40 = 0;
    GWS8(0xE) = 0;
    SoftKeyboard_exit();
    PitMenu.open = 0;
    PitMenu.x0F = z;
    PitMenu.x0C = 0;
    lpPit->x34[3] = -1;
    lpPit->x34[2] = -1;
    lpPit->x34[1] = -1;
    lpPit->x34[0] = -1;
    *(s8 *)((u8 *)lpPit + 0x3F) = 0;
    lpPit->x41 = 0;
    PitMenu.x10 = 0;
    lpPit->x84 = 0;
    lpPit->x52 = 0;
    lpPit->x8D = 0;
    lpPit->x28 = 4;
    lpPit->x29 = 0;
    lpPit->x2B = 0;
    p = lpPit->pl;
    lpPit->x5A = *(u16 *)((u8 *)p + 0x888);
    lpPit->x5E = *(u16 *)((u8 *)p + 0x88E);
    PitMenu.x00 = 0;
    PitMenu.x08 = -1;
    PitMenu.x06 = 0;
    lpPit->x10[0] = -1;
    lpPit->x10[1] = -1;
    lpPit->x10[2] = -1;
    lpPit->x10[3] = -1;
    if (game_w.x1DC == 1) {
        func_5B3D70();
        func_609750();
        lpPit->lb = 1;
        pit_prim[1].trans = trans_pit_1_lb;
        pit_prim[2].trans = trans_pit_2_lb;
    } else {
        lpPit->lb = 0;
    }
}

void select_yes_no(int unused, u16 mask) {
    u16 k = FLD16(Psw, 4) & mask;

    if (lpPit->yn == 0) {
        if (k & 0x1400) {
            lpPit->yn = 1;
            se_req(7, 0x16, 0);
        }
    } else if (k & 0x2800) {
        lpPit->yn = 0;
        se_req(7, 0x16, 0);
    }
}

int Cockpit_menu_chk(void) {
    if (GW8(0xE) != 0) {
        return 1;
    }
    if (PitMenu.open != 0) {
        return 1;
    }
    return (lpPit->x07 != 0) ? 1 : 0;
}

int Cockpit_menu_chk_lobby(void) {
    if (Online_ck() == 1 && game_w.x1DC == 1) {
        return *(int *)0x6EAEBC;
    }
    return 0;
}

int enemy_mark_chk(PLW *pl, EMW *em) {
    u8 c;

    if (em->be_flag == 0) {
        return 0;
    }
    if (em->x56A != 0) {
        return 1;
    }
    c = enemy_icon_tbl[em->kind];
    if (c == 0xFF) {
        return 0;
    }
    if ((c & 0xC0) == 0x80) {
        return 0;
    }
    if (pl->work930 != 0) {
        return 1;
    }
    return Pl_Skill_ck(pl, 0x29);
}

void menu_exit(void) {
    lpPit->x05 = 0;
    lpPit->x40 = 0;
    GWS8(0xE) = 0;
    PitMenu.x10 = 0;
    lpPit->x84 = 0;
}

/* Prints a player name, at most 11 characters, ending with a cut mark. */
void player_name_print(char *name) {
    char buf[16];
    int n = 11;
    char *d = buf;

    do {
        *d = *name;
        if (*name == 0) {
            font_print_uf(buf, n);
            return;
        }
        d++;
        name++;
    } while (--n != 0);
    buf[9] = -0x5B;
    buf[10] = 0;
    font_print_uf(buf, n);
}

void player_name_id_print(PLW *pl) {
    char *name = (char *)pl + 0x8D4;

    if (Online_ck() == 1 && PitMenu.x14 != 0) {
        if (game_w.x1DC == 0) {
            name = (char *)room_member_id + *(u16 *)((u8 *)pl + 0xC) * 8;
        } else {
            name = (char *)func_5B4D30(*(u16 *)((u8 *)pl + 0xC));
        }
    }
    player_name_print(name);
}

u16 pit_key_repeat(u16 now, u16 hold) {
    u16 k = now & 0x3C00;
    u16 h;

    if (k != 0) {
        lpPit->key = k;
        lpPit->rep = 8;
        return (s16)k;
    }
    h = hold & 0x3C00;
    if (h == 0) {
        return lpPit->key = 0;
    }
    lpPit->rep--;
    if (lpPit->rep <= 0) {
        lpPit->rep = 3;
        return lpPit->key &= h;
    }
    return 0;
}
