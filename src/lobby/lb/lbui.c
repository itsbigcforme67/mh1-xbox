/* lbui, run 1: Lb_eat_to_bell .. event_eat_rcpt (lobby.bin 0x005910E0-0x005915F4): the matching functions of lbui_nm.c. */
#pragma readonly_strings on
#include "lbui_proto.h"


/* sprite template for Put_2TF / Put_sprite_rotate (helpLineTbl entries, stride 0x14) */
typedef struct { s16 x; s16 y; s16 w; s16 h; u8 pad08[4]; s16 u0; s16 v0; s16 u1; s16 v1; } DLGSPR;
typedef struct { f32 f[5]; } DLGF5;
void Put_sprite_rotate();

/* dialog frame: top/bottom edge strips of 0x28 high tiles then the two rotated side strips (near-match) */

void Paint_square();

/* menu frame: filled body (Paint_square or 5-high strips) + four 20-unit edge strips + four 7x6 corners (near-match) */

void put_button_help(int a, int b, int c, u16 d);

/* button help line of the plaza menus: which of the four buttons are shown for each menu / sub menu step (near-match) */

void Lb_eat_to_bell(void) {
    LBS8(6) = 1;
}

void Lb_eat_to_rcpt(void) {
    LBS8(6) = 3;
}

void Lb_eat_to_eat(void) {
    LBS8(6) = 4;
}

void Lb_eat_to_end(void) {
    LBS8(6) = 5;
}

int event_eat_rcpt(w)
LB_NETW *w;
{
    s8 stage;
    int sw;
    s8 a;
    s8 b;
    u16 key;
    int i;
    int t;
    EATRES *p2;

    stage = game_w.stage - 0x51;
    sw = (u16)Get_sw2(0);
    switch (w->depth) {
    case 0:
        sw = (u16)sw & 0xFFFF;
        if (sw & 0x20) {
            w->depth = w->depth + 1;
            w->cur = w->menu;
            cnWrap_SoundRequest(0);
        } else if (sw & 0x40) {
            cnWrap_SoundRequest(3);
            return 1;
        } else if (sw & 0x2000) {
            if (w->menu == 0) {
                w->menu = 9;
            } else {
                w->menu = w->menu - 1;
            }
            cnWrap_SoundRequest(1);
        } else if (sw & 0x1000) {
            t = w->menu + 1;
            w->menu = t;
            if ((t & 0xFF) >= 10) {
                w->menu = 0;
            }
            cnWrap_SoundRequest(1);
        }
        break;
    case 1:
        sw = (u16)sw & 0xFFFF;
        if (sw & 0x20) {
            if (w->cur == w->menu) {
                cnWrap_SoundRequest(7);
            } else {
                w->depth = w->depth + 1;
                cnWrap_SoundRequest(0);
            }
        } else if (sw & 0x40) {
            w->depth = w->depth - 1;
            cnWrap_SoundRequest(3);
        } else if (sw & 0x2000) {
            if (w->cur == 0) {
                w->cur = 9;
            } else {
                w->cur = w->cur - 1;
            }
            cnWrap_SoundRequest(1);
        } else if (sw & 0x1000) {
            t = w->cur + 1;
            w->cur = t;
            if ((t & 0xFF) >= 10) {
                w->cur = 0;
            }
            cnWrap_SoundRequest(1);
        }
        break;
    case 2:
        sw = (u16)sw & 0xFFFF;
        if (sw & 0x20) {
            if ((u8)w->x0A == 1) {
                cnWrap_SoundRequest(3);
                return 1;
            }
            cnWrap_SoundRequest(8);
            a = eat_data_type[w->cur];
            b = eat_data_type[w->menu];
            if (b < a) {
                key = (b << 8) | a;
            } else {
                key = (a << 8) | b;
            }
            pRes = eat_result[stage];
            i = 0;
            if (pRes->key != 0xFF) {
                while (1) {
                    if (pRes->key == (key & 0xFFFF)) {
                        w->x06 = pRes->idx;
                        break;
                    }
                    i = (i + 1) & 0xFFFF;
                    if (i > 0x32) {
                        w->x06 = 0;
                        break;
                    }
                    pRes = pRes + 1;
                    if (pRes->key == 0xFF) {
                        break;
                    }
                }
            }
            t = (u8)w->x06;
            if (t == 0 || t == 0xFF) {
                eatResult = 1;
            } else {
                if ((f32)Status_add_tbl[pRes->idx].s3 + ((f32)Status_add_tbl[pRes->idx].s2 + (f32)(Status_add_tbl[pRes->idx].s0 + Status_add_tbl[pRes->idx].s1)) > 0.0f) {
                    eatResult = 2;
                } else {
                    eatResult = 0;
                }
            }
            p2 = pRes;
            *(s8 *)0x3F3603 = Status_add_tbl[p2->idx].s2;
            *(s8 *)0x3F3604 = Status_add_tbl[p2->idx].s3;
            *(s8 *)0x3F3605 = Status_add_tbl[p2->idx].s0;
            *(s16 *)0x3F3606 = Status_add_tbl[p2->idx].s1;
            return 0;
        }
        if (sw & 0x40) {
            w->depth = w->depth - 1;
            cnWrap_SoundRequest(3);
        } else if (sw & 0x3000) {
            w->x0A = (u8)w->x0A ^ 1;
            cnWrap_SoundRequest(1);
        }
        break;
    }
    return 2;
}
