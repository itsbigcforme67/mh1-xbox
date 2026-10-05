/* camr_nm - near-matches of f_cam_223B50 (SLPM_654.95 0x00223B50-0x00225200),
 * not built. dDivComplex is 13 of 34 instructions off (float register
 * numbers only: the original loads b->re/b->im into f3/f4 after the 0.0
 * constant, d and 1/d took f0-f2). */
#include "types.h"

typedef struct DCMPLX {
    f32 re;
    f32 im;
} DCMPLX;

void dDivComplex(DCMPLX *r, DCMPLX *a, DCMPLX *b) {
    DCMPLX t;
    f32 d;
    f32 inv;

    d = b->re * b->re + b->im * b->im;
    if (d != 0.0f) {
        inv = 1.0f / d;
        t.re = inv * (a->re * b->re + a->im * b->im);
        t.im = inv * (a->im * b->re - a->re * b->im);
        *r = t;
    } else {
        *r = *a;
    }
}

/* ZoomRateCalc 8/34, ZoomBaseAngleRail 1/10 (addu operand order), RollAngleRail
 * 11/28 (same): near-matches. */
f32 ZoomRateCalc(f32 x, f32 *z) {
    if (x <= z[2]) {
        return z[4];
    }
    if (!(x < z[3])) {
        return z[5];
    }
    if (z[3] == z[2]) {
        return 0.5f * (z[4] + z[5]);
    }
    return (z[5] - z[4]) / (z[3] - z[2]) * (x - z[2]) + z[4];
}

f32 ZoomBaseAngleRail(f32 t, f32 *rail, int i)
{
  f32 new_var;
  f32 new_var2;
  new_var2 = rail[i + 0x81];
  if (1)
  {
    new_var = rail[i - -0x80];
    return (new_var * (1.0f - t)) + (new_var2 * t);
  }
}

f32 RollAngleRail(f32 t, s16 *rail, int i) {
    union {
        s32 w;
        s16 h[2];
    } u;

    u.w = (s32)(65536.0f * t) * (rail[i + 0x121] - rail[i + 0x120]);
    u.h[1] += rail[i + 0x120];
    return 0.000095873799f * u.h[1];
}

#include "pl.h"
#include "game.h"

#ifndef NULL
#define NULL 0
#endif

/* Quest state, only the fields used here (QUEST_W is declared per file). */
typedef struct QW {
    u8 _pad00[0x34];
    s16 x34;            /* 0x34 */
    u8 _pad36[0x3C - 0x36];
    PLW *p3C;           /* 0x3C */
    s32 flags;          /* 0x40 bit 0: clear camera wanted */
    u8 _pad44[0xB0 - 0x44];
    PLW *pB0;           /* 0xB0 */
} QW;

extern QW quest_w;
extern u8 quest_clear_camera_tbl[];
void DemoCameraRequest(int, s32);

/* QuestClearCameraRequest (0x225D80): when a quest ends, start the demo camera
 * (number from quest_clear_camera_tbl by PLW.kind, fixed numbers for kind 2 and
 * 7) on the player that finished it. Not compared yet. */
void QuestClearCameraRequest(void) {
    PLW *p;
    int k;
    u8 no;

    if (quest_w.flags & 1) {
        p = quest_w.p3C;
        if (p != NULL || ((p = quest_w.pB0) != NULL && ((k = p->kind) == 7 || k == 2))) {
            if (game_w.stage == *((u8 *)p + 0x736)) {
                switch (p->kind) {
                case 7:
                    if (game_w.stage == 0xC) {
                        no = 0x1D;
                        if (quest_w.x34 == 0) {
                            goto call;
                        } else {
                            return;
                        }
                    }
                    break;
                case 2:
                    no = 0x20;
                    if (quest_w.x34 == 0) {
                        goto call;
                    }
                    return;
                default:
                    no = quest_clear_camera_tbl[p->kind];
                    if (no != 0xFF) {
                    call:
                        DemoCameraRequest(no, (s32)p);
                    }
                    break;
                }
            }
        }
    }
}
