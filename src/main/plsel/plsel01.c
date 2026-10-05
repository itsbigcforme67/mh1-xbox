/* SLPM_654.95 0x0014E2D0-0x0014E4EC: player_sel .. player_wait. See plsel_nm.c. */
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
void init_plsel();


typedef struct SELEM { u8 _p00[2]; u8 cur; u8 _p03[0x9C - 3]; u16 kind[4]; u16 cnt[4]; } SELEM;


#define PSW16(o) (*(u16 *)(Psw + (o)))



void player_sel(u8 *t) {
    s16 i;
    u8 *g;
    u16 me = GW8(0xD1);
    u8 *sw = select_w;
    u8 k = t[9];

    switch (k) {
    case 0:
        t[9] = k + 1;
        init_select_work(k);
        sel_default_set();
        sw[me + 0x8C] = 1;
        net_send_sys(2, GW8(0xD1));
        break;
    case 1:
        if (Online_ck() == 1 && *(game_w + 0x208 + GW8(0xD1)) == 0xFF) {
            GWS8(0) = 5;
            Quest_error_set2();
            Tsk_Exit(t);
            return;
        }
        i = 0;
        g = game_w;
        do {
            if (((s8 *)(sw + i))[0x8C] == 0 && g[0x208] != 0xFF) {
                return;
            }
            i++;
            g++;
        } while (i < 4);
        t[8]++;
        sw[1] = 0;
        break;
    }
}

void player_wait(u8 *t) {
    s16 c;
    u8 *sw = select_w;

    switch (SELB(1)) {
    case 0:
        sw[1]++;
        *(s16 *)(sw + 8) = 0x1E;
        net_send_sys(2, GW8(0xD1));
    case 1:
        c = *(s16 *)(sw + 8) - 1;
        *(s16 *)(sw + 8) = c;
        if (c <= 0) {
            system_w[3] = 1;
            all_reset();
            Tsk_Exit(t);
            GW8(0x12) = 0;
        }
        break;
    }
}
