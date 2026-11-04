/* f_menu run 1 (SLPM_654.95 0x00127950-0x00127C6C): select_yes_no, Cockpit_menu_chk*, enemy_mark_chk, menu_exit, player_name_print, player_name_id_print. Whole file in menu_nm.c. */
#include "g_alias.h"
#include "lbsys_g.h"
extern u8 lb_sys[];
#if defined(__MWERKS__) && !defined(VERSION_MHG)
extern u8 D_6EAEBC[];       /* lb_sys + LBM(0x6C) (the PS2 MH1 main module only sees the address name) */
#define LB_X6C (*(int *)D_6EAEBC)
#else
#define LB_X6C (*(int *)(lb_sys + LBSYS_OFS(0x6C)))
#endif
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

#if !defined(G_PART)
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
#endif

#if !defined(G_PART) || G_PART == 1
int Cockpit_menu_chk(void) {
    if (GW8(0xE) != 0) {
        return 1;
    }
    if (PitMenu.open != 0) {
        return 1;
    }
    return (lpPit->x07 != 0) ? 1 : 0;
}
#endif

#if !defined(G_PART) || G_PART == 1
int Cockpit_menu_chk_lobby(void) {
    if (Online_ck() == 1 && game_w.x1DC == 1) {
        return LB_X6C;
    }
    return 0;
}
#endif

#if !defined(G_PART) && defined(VERSION_MHG) && !defined(VERSION_MH2)
u8 sub_main_func_004F0020(PLW *pl, int id);     /* pl53.c: the active skill of skill kind id (0 = none) */
/* G lpPit+0x96 (in include/menu.h's _pad91): the monster-mark mode of this map draw: 0, 2 / 4 by the Detect skill,
 * |1 while the psychoserum runs */
#define PIT_MAP_MODE (((u8 *)lpPit)[0x96])
s16 Pl_item_num_ck(PLW *pl, int id);

/* G main 0x1207E0 (called first by disp_whole_map and disp_partial_map): the map's monster-mark mode */
void g_map_mark_mode(PLW *pl) {
    u8 s = sub_main_func_004F0020(pl, 0x1B);     /* skill kind 0x1B: Detect */

    PIT_MAP_MODE = 0;
    if (s != 0) {
        PIT_MAP_MODE = (s == 0x28) ? 4 : 2;  /* 0x28 Auto-marking; any other Detect skill hides the marks */
    }
    if (pl->work930 != 0) {                      /* psychoserum running */
        PIT_MAP_MODE |= 1;
    }
}

/* G main 0x21EA40 (from disp_whole_map): is the whole map shown */
int g_map_whole(PLW *pl) {
    u8 s = sub_main_func_004F0020(pl, 0x23);     /* skill kind 0x23: Map */

    if (s != 0) {
        return s == 0x2D;                        /* 0x2D Map always; another Map-kind skill hides it */
    }
    return Pl_item_num_ck(pl, 506) != 0;         /* G item 506: the map */
}
#endif

#if !defined(G_PART)
int enemy_mark_chk(PLW *pl, EMW *em) {
    u8 c;

#if defined(VERSION_MHG) && !defined(VERSION_MH2)
    /* G enemy_mark_chk (main 0x119330) takes only the monster: small monsters by paint only, the others by the
     * map mode of this draw (g_map_mark_mode), plus 1 when painted; mode 2 alone (another Detect skill) hides them */
    u8 v;

    (void)pl;
    if (em->be_flag == 0) {
        return 0;
    }
    c = enemy_icon_tbl[em->kind];
    if (c == 0xFF || (c & 0xC0) == 0x80) {
        return em->x56A != 0;
    }
    v = PIT_MAP_MODE;
    if (em->x56A != 0) {
        v |= 1;
    }
    return v == 2 ? 0 : v;
#endif
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
#endif

#if !defined(G_PART) || G_PART == 2
void menu_exit(void) {
    lpPit->x05 = 0;
    lpPit->x40 = 0;
    GWS8(0xE) = 0;
    PitMenu.x10 = 0;
    lpPit->x84 = 0;
}
#endif

/* Prints a player name, at most 11 characters, ending with a cut mark. */
#if !defined(G_PART)
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
#endif

#if !defined(G_PART)
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
#endif
