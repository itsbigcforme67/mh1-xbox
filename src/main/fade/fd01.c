/* fd01 - screen fade 0x0010EC20-0x0010ED6C: Fade_task. Whole file in fade_nm.c. */
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






void Fade_task(FTASK *t) {
    FADE_W *w = &fade_w;
    FADE_ENT *e;

    switch (t->step) {
    case 0:
        memset(w, 0, 0x10);
        w->ent = 0;
        t->step++;
    case 1:
        switch (w->state) {
        case 0:
            if (w->req == 0) {
                break;
            }
            w->state++;
        reset:
            w->cur = w->req;
            w->alpha = fade_data[w->cur - 1].start;
        case 1:
            if (w->req == 0) {
                w->state = 0;
                w->ent = 0;
                break;
            }
            if (w->req != w->cur) {
                goto reset;
            }
            e = &fade_data[w->cur - 1];
            w->ent = e;
            w->prev = w->alpha;
            if (w->alpha != e->end) {
                w->alpha += e->step;
            } else if (e->hold != 0) {
                w->state = 0;
                w->req = 0;
                w->ent = 0;
            }
            break;
        }
        break;
    }
}
