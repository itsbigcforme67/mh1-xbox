/* f_menu run 1 (SLPM_654.95 0x00127950-): select_yes_no, Cockpit_menu_chk*, enemy_mark_chk, menu_exit, player_name_print, player_name_id_print. Whole file in menu_nm.c. */
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
