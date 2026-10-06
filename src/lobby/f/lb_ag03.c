/* lb_ag03 - sprite/cursor bar 0x005CC590-0x005CC920: lb_put_sprite. Whole file in lb_ag.c. D_6EAE5A/D_6EAE78 are lb_sys name fields as objects of their own (config/lobby_aliases.txt). */
#include "lobby_f.h"
extern PLW player_work[];
void reload_tex();
void SetTextureStage();
void SetFilterMode();
void flSetRenderState();
void Sel_csr_disp(s16, s16, s16, s16, int);
int Lb_check_hotel();
int strlen();
void Lb_put_icon();
void Lb_put_button();
extern char D_6EAE5A[];
extern char D_6EAE78[];
extern char D_6EAE5A[];
extern char D_6EAE78[];
void lb_put_sprite(void) {
    PLW *pl;
    s16 a3;
    u8 *e;
    int r;
    a3 = 0x1E;
    pl = &player_work[game_w.master];
    if (*(s8 *)0x6EAE5A != 0 || *(int *)0x6EAEB8 == 8) {
        if (*(s8 *)0x6EAE78 != 0) {
            a3 += 30;
        }
        Sel_csr_disp(0x14, 0x1C, 0x1F4, a3, 0x80006000);
        e = (u8 *)pl->fish878;
        if (e != 0 && *(int *)0x6EAEB8 == 8) {
            if (*(u16 *)(e + 2) != 6) {
                return;
            }
        }
        reload_tex(1, 0x157);
        SetTextureStage(0x157);
        SetFilterMode(1);
        flSetRenderState(0x60, 0);
        e = (u8 *)pl->fish878;
        if (e != 0) {
            switch (*(u16 *)(e + 2)) {
            case 18:
                Lb_put_button(0x14, 0x1E, 3);
                Lb_put_icon((s16)((s16)strlen(D_6EAE5A) * 10 + 0x50), 0x1E, 0xD, -1);
                return;
            case 19:
                r = Lb_check_hotel(0x52);
                switch (r) {
                case 2:
                    Lb_put_icon((s16)((s16)strlen(D_6EAE5A) * 10 + 0x50), 0x1E, 0xD, -1);
                case 3:
                case 1:
                    Lb_put_button(0x14, 0x1E, 3);
                }
                r = Lb_check_hotel(0x53);
                switch (r) {
                case 2:
                    Lb_put_icon((s16)((s16)strlen(D_6EAE78) * 10 + 0x50), 0x38, 0xD, -1);
                case 3:
                case 1:
                    Lb_put_button(0x14, 0x38, 1);
                    return;
                }
                break;
            case 20:
                r = Lb_check_hotel(0x54);
                switch (r) {
                case 2:
                    Lb_put_icon((s16)((s16)strlen(D_6EAE5A) * 10 + 0x50), 0x1E, 0xD, -1);
                case 3:
                case 1:
                    Lb_put_button(0x14, 0x1E, 3);
                }
                r = Lb_check_hotel(0x55);
                switch (r) {
                case 2:
                    Lb_put_icon((s16)((s16)strlen(D_6EAE78) * 10 + 0x50), 0x38, 0xD, -1);
                case 3:
                case 1:
                    Lb_put_button(0x14, 0x38, 1);
                    return;
                }
                break;
            default:
                Lb_put_button(0x14, 0x1E, 3);
                break;
            }
        }
    }
}
