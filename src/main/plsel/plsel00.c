/* plsel00 - select screen task and set-up (SLPM_654.95 0x0014E0C0-0x0014E2C4): Plsel_task, init_plsel, disp_str. Whole file in plsel_nm.c. */
/* plsel_nm - SLPM_654.95 0x0014E0C0-0x0014EAFC, whole file (not built; matching runs are plsel0N.c).
 * Debug / network player and monster select screen ("Plsel" task): init_plsel loads the select textures,
 * player_sel/player_wait run the per-player ready handshake (net_send_sys messages 2 and 8), em_select is the
 * debug monster picker (4 slots of kind + count, at most 16 monsters in total) with disp_em_select drawing it,
 * sel_default_set fills select_w with the default characters. Field names are guesses from use. */
#include "types.h"
extern u8 select_w[];
extern u8 game_w[];
extern u8 system_w[];
#define SELB(o) (*(u8 *)(select_w + (o)))
#define SELS8(o) (*(s8 *)(select_w + (o)))
#define SELH(o) (*(u16 *)(select_w + (o)))
#define SELSH(o) (*(s16 *)(select_w + (o)))
#define GW8(o) (*(u8 *)(game_w + (o)))
#define GWS8(o) (*(s8 *)(game_w + (o)))
extern s32 mem_tex[];
extern char *em_name[];
extern u8 Psw[];
extern u8 User_data[];
extern u8 equip_weapon[];
extern s32 PIT_TEX[];
extern s32 SEL_TEX[];
extern char lit_596_0035B158[];
extern char lit_945_0035B160[];
extern char lit_946_0035B170[];
extern char lit_947_0035B190[];
void flReloadTexture();
void net_send_sys();
void trans();
void all_reset();
void clr_pl_work();
void clr_stg_work();
void fade_reset();
void mkTexture();
void load_texlist();
void flfntSetSize();
void flfntLocate();
void font_set_palette();
void font_print();
void Tsk_Exit();
void Quest_error_set2();
int Online_ck();
void init_select_work();
s16 em_softdip_ck();
u32 ran_suu();
void sel_default_set(void);
void init_plsel(u8 *t);
void player_sel(u8 *t);
void player_wait(u8 *t);
void em_select(u8 *t);
/* Task entry of the select screen: reloads the select texture, runs the step (0 init, 1 player select, 2 wait, 3 monster
   select), sends a keep-alive (net_send_sys 8) every 60 frames and draws. */
void Plsel_task(u8 *t) {
    s16 n;

    if (mem_tex[1] != 0) {
        flReloadTexture(1, &mem_tex[1]);
    }
    switch (t[8]) {
    case 0:
        init_plsel(t);
        break;
    case 1:
        player_sel(t);
        break;
    case 2:
        player_wait(t);
        break;
    case 3:
        em_select(t);
        break;
    }
    n = SELSH(4) + 1;
    SELSH(4) = n;
    if (n >= 0x3C) {
        SELSH(4) = 0;
        net_send_sys(8, GW8(0xD1));
    }
    trans();
}
void init_plsel(u8 *t) {
    all_reset();
    clr_pl_work();
    clr_stg_work();
    fade_reset();
    mkTexture(0, 1, 0);
    load_texlist(PIT_TEX[0], 2, 0);
    load_texlist(SEL_TEX[0], 0x9A, 0);
    t[0x14] = 0;
    t[8]++;
}
void disp_str(int x, int y, int s, int pal) {
    flfntSetSize(0x14, 0x14);
    flfntLocate(x, y);
    font_set_palette(pal & 0xFF);
    font_print(lit_596_0035B158, s);
}
typedef struct SELEM { u8 _p00[2]; u8 cur; u8 _p03[0x9C - 3]; u16 kind[4]; u16 cnt[4]; } SELEM;
#define PSW16(o) (*(u16 *)(Psw + (o)))
