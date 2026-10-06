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

typedef struct SELEM { u8 _p00[2]; u8 cur; u8 _p03[0x9C - 3]; u16 kind[4]; u16 cnt[4]; } SELEM;

void disp_em_select(SELEM *w, u8 *unused) {
    u8 pal[4];
    s16 i;
    s16 sum;
    s16 y;

    flfntSetSize(0x14, 0x14);
    sum = 0;
    i = 0;
    do {
        if (i == w->cur) {
            pal[i] = 2;
        } else {
            pal[i] = 0;
        }
        y = (12 + i * 2) * 15;
        disp_str(0xC8, y, (int)em_name[w->kind[i]], pal[i]);
        flfntLocate(0x190, y);
        font_set_palette(pal[i]);
        font_print(lit_945_0035B160, w->cnt[i]);
        sum += (s16)w->cnt[i];
        i++;
    } while (i < 4);
    flfntLocate(0xC8, (s16)(y + 0x50));
    if (sum >= 0x10) {
        font_set_palette(2);
        font_print(lit_946_0035B170);
    } else {
        font_set_palette(0);
        font_print(lit_947_0035B190);
    }
}

#define PSW16(o) (*(u16 *)(Psw + (o)))

void em_select(u8 *t) {
    u8 *sw = select_w;
    u16 pad = PSW16(0x3A) | (PSW16(0x26) | (PSW16(4) | PSW16(0x18)));
    u16 *e;
    u16 v;
    u16 sum;

    switch (SELB(1)) {
    case 0:
        sw[1]++;
        sw[2] = 0;
        break;
    case 1:
        if (pad & 0x2000) {
            if (sw[2] <= 0) {
                sw[2] = 3;
            } else {
                sw[2]--;
            }
        }
        if (pad & 0x1000) {
            if (sw[2] >= 3) {
                sw[2] = 0;
            } else {
                sw[2]++;
            }
        }
        if (pad & 0x400) {
            e = (u16 *)((u8 *)(sw[2] * 2) + (int)sw + 0x9C);
            v = *e;
            if (v >= 0x22) {
                *e = 0;
            } else {
                *e = v + 1;
            }
        }
        if (pad & 0x800) {
            e = (u16 *)((u8 *)(sw[2] * 2) + (int)sw + 0x9C);
            v = *e;
            if (v <= 0) {
                *e = 0x22;
            } else {
                *e = v - 1;
            }
        }
        if (pad & 0x40) {
            e = (u16 *)((u8 *)(sw[2] * 2) + (int)sw + 0xA4);
            v = *e;
            if (v != 0) {
                *e = v - 1;
            }
        }
        if (pad & 0x220) {
            e = (u16 *)((u8 *)(sw[2] * 2) + (int)sw + 0xA4);
            v = *e;
            if (v < 0xF) {
                *e = v + 1;
            }
        }
        if (pad & 0x8000) {
            sum = 0;
            sum += *(u16 *)(sw + 0xA4);
            sum += *(u16 *)(sw + 0xA6);
            sum += *(u16 *)(sw + 0xA8);
            sum += *(u16 *)(sw + 0xAA);
            if (sum < 0x10) {
                system_w[3] = 1;
                all_reset();
                Tsk_Exit(t);
                GW8(0x12) = 0;
            }
        }
        break;
    }
    disp_em_select((SELEM *)sw, t);
}

void sel_default_set(void) {
    u8 *sw;
    int i;
    u8 *s2;
    u8 *s1;
    u8 *pw;

    sw = select_w;
    i = 0;
    s2 = sw;
    do {
        if (Online_ck() == 1) {
            if (i < GW8(0xD3)) {
                s1 = sw + i;
                s1[0x8C] = 0;
            } else {
                s1 = sw + i;
                ((s8 *)s1)[0x8C] = -1;
            }
        } else if (i == 0) {
            s1 = sw + i;
            s1[0x8C] = 0;
        } else {
            s1 = sw + i;
            s1[0x8C] = 1;
        }
        s1[0xC] = (i & 1) != 0;
        s1[0x94] = 0;
        if (i == GW8(0xD1)) {
            s1[0x54] = 0;
            if (User_data[0] != 0) {
                u8 *ud = User_data + 0x3CC;
                *(s16 *)(s2 + 0x5C) = *(s16 *)(User_data + 0x3CC);
                *(s16 *)(s2 + 0x5E) = *(s16 *)(ud + 2);
                *(s16 *)(s2 + 0x60) = *(s16 *)(ud + 4);
            } else {
                s2[0x5D] = 6;
                *(s16 *)(s2 + 0x5E) = 0;
                *(s16 *)(s2 + 0x60) = 0;
            }
        } else {
            if (Online_ck() == 1) {
                s1[0x54] = 0;
                pw = s1 + 0x54;
            } else {
                pw = s1 + 0x54;
                s1[0x54] = ((int)(ran_suu(1) & 0xFFFF) % 4) + 1;
            }
            if (equip_weapon[*pw] != 1) {
                s2[0x5D] = 6;
                *(s16 *)(s2 + 0x5E) = 0;
            } else {
                s2[0x5D] = 7;
                *(s16 *)(s2 + 0x5E) = 0;
            }
            *(s16 *)(s2 + 0x60) = 0;
        }
        s1[0x14] = 0;
        s1[0x1C] = 4;
        i++;
        s2[0x24] = 1;
        s2[0x25] = 1;
        s2[0x26] = 1;
        s2[0x27] = 1;
        s2[0x28] = 1;
        s2[0x29] = 1;
        s2 += 6;
    } while (i < 8);
    SELSH(0x9C) = em_softdip_ck();
    SELSH(0xA4) = 1;
    SELSH(0x9E) = 0;
    SELSH(0xA6) = 0;
    SELSH(0xA0) = 0;
    SELSH(0xA8) = 0;
    SELSH(0xA2) = 0;
    SELSH(0xAA) = 0;
}
