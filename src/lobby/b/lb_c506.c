/* lb_c506 - agent C round 5 0x005B1E90-0x005B2168: check_erase_dialog (static, formerly lb_by133) + tk_dialog_mv02 (dialog open/close animation; one TU so the static callee keeps a0; cnWrap_PushWork receives arg0 untouched; float x goes through an int local). */
#include "lobby_a.h"
typedef struct { u8 pad0[4]; s16 x4; u8 pad6[0xE]; s16 x14; u8 pad16[2]; char *x18; } DLGW;
typedef struct { u8 pad0[4]; u8 st; u8 sub; u8 pad6[0x12]; DLGW *w; } DLG;
typedef struct { u8 pad0[0x2C5D]; char x2C5D[1]; } CWS_td;
void nwDispStr_Html(f32, f32, f32, char *);
void cnWrap_FontDisp(f32, f32, f32, char *);
void cnWrap_PushWork();
void cnWrap_SetFontSize(f32);
static int check_erase_dialog(u8 *arg0) {
    u8 a;
    u8 b;

    a = cw[0x2F79];
    b = *(u8 *)(*(u8 **)(arg0 + 0x18) + 0x14);
    if (a == b) return 1;
    if (a == 0x4C) return 1;
    return 0;
}
void tk_dialog_mv02(u8 *arg0) {
    DLGW *w;
    u8 st;
    int x;
    int len;

    st = arg0[4];
    w = *(DLGW **)(arg0 + 0x18);
    switch (st) {
    case 0:
        if (check_erase_dialog(arg0) != 0) {
            if (cw[0x2F79] != -1U) {
                cw[0x2F78] = 0;
            }
            cnWrap_PushWork(arg0);
            return;
        }
        switch (arg0[5]) {
        case 0:
            arg0[5]++;
            w->x4 = 0x10;
        case 1:
            w->x4--;
            if (w->x4 != 0) {
                return;
            }
            arg0[5]++;
            w->x4 = 0x10;
        case 2:
            w->x4--;
            if (w->x4 == 0) {
                arg0[4]++;
                arg0[5] = 0;
                cw[0x2F78] = 2;
                if (w->x14 == 1) {
                    cw[0x2F77] = 1;
                    return;
                }
            }
            break;
        }
        break;
    case 1:
        if (check_erase_dialog(arg0) != 0) {
            arg0[4] = 2;
            arg0[5] = 0;
        }
        if (w->x18 != 0) {
            if (w->x14 == 1) {
                cnWrap_SetFontSize(20.0f);
                cnWrap_SetFontColor(2);
                len = strlen((char *)(cw + 0x2C5D));
                x = 320.0f - 0.5f * (20.0f * len / 2.0f);
                cnWrap_FontDisp(x, 26.0f, 2.0f, (char *)(cw + 0x2C5D));
                nwDispStr_Html(122.0f, 49.0f, 2.0f, w->x18);
                return;
            }
            nwDispStr_Html(122.0f, 68.0f, 2.0f, w->x18);
            return;
        }
        break;
    case 2:
        switch (arg0[5]) {
        case 0:
            arg0[5]++;
            w->x4 = 8;
        case 1:
            w->x4--;
            if (w->x4 == 0) {
                arg0[1] = 0;
                if (cw[0x2F79] != -1U) {
                    cw[0x2F78] = 0;
                }
                cnWrap_PushWork(arg0);
            }
            break;
        }
        break;
    }
}
