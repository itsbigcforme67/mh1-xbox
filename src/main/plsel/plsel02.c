/* SLPM_654.95 0x0014E680-0x0014E8D8: em_select .. em_select. See plsel_nm.c. */
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
