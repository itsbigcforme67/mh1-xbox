/* fd03 - Fade_busy_ck (SLPM_654.95 0x0010ED70-0x0010EDD8): returns 0 idle, 1 fading, 2 alpha at the end value (a guess). Whole file in fade_nm.c. */
/* Screen fade. SLPM_654.95 0x0010EC20-0x0010F050 (g_Fade_task): a task that steps the fade
 * alpha towards the end value of fade_data[req-1] (set by fade_set), fade_draw paints a
 * full-screen quad with it. fade_data entries are 0x1C bytes. */
#include "types.h"
typedef struct FADE_ENT {
    u8 hold;                    /* 0x00 non-zero: fade is cleared when the end value is reached */
    u8 rgb;                     /* 0x01 index into fade_rgb_110 */
    u8 _pad02[2];
    f32 x, y, z;                /* 0x04 quad origin */
    s16 w, h;                   /* 0x10 quad size */
    s16 start;                  /* 0x14 first alpha */
    s16 end;                    /* 0x16 last alpha */
    s16 step;                   /* 0x18 alpha per frame */
    u8 _pad1A[2];
} FADE_ENT;
typedef struct FADE_W {
    u8 state;                   /* 0x00 0 idle, 1 running */
    u8 _pad01[3];
    s16 req;                    /* 0x04 requested fade number (1-based), 0 none */
    s16 cur;                    /* 0x06 fade number being run */
    s16 alpha;                  /* 0x08 0..256 */
    s16 prev;                   /* 0x0A alpha of the last frame (fade_set stores the frame count here) */
    FADE_ENT *ent;              /* 0x0C */
} FADE_W;
typedef struct FTASK { u8 _pad00[8]; u8 step; } FTASK;
extern FADE_W fade_w;
extern FADE_ENT fade_data[];
extern u32 fade_rgb_110[];
void *memset(void *, int, unsigned);
void SetTrnslMode(int, int);
void flSetRenderState(int, int);
void flps0D00(void *);
/* 0: idle, 1: fading, 2: reached the end value */
int Fade_busy_ck(void) {
    FADE_W *w = &fade_w;
    FADE_ENT *d;

    if (fade_w.state != 0 && w->req != 0) {
    } else {
        return 0;
    }
    d = &fade_data[w->cur - 1];
    return (w->alpha != d->end) ? 1 : 2;
}
