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

/* 0: idle, 1: fading, 2: reached the end value */
int Fade_busy_ck(void) {
    FADE_W *w = &fade_w;

    if (fade_w.state != 0 && w->req != 0) {
        return (w->alpha == fade_data[w->cur - 1].end) ? 2 : 1;
    }
    return 0;
}

void fade_draw(void) {
    f32 t;
    f32 q[9];
    FADE_W *w = &fade_w;

    if (w->req != 0) {
        t = (f32)w->alpha / 256.0f;
        if (t > 0.0f) {
            SetTrnslMode(4, 5);
            flSetRenderState(0x6D, 7);
            q[0] = w->ent->x;
            q[1] = w->ent->y;
            q[2] = w->ent->z;
            q[3] = 1.0f;
            q[4] = q[0] + w->ent->w;
            q[5] = q[1] + w->ent->h;
            q[6] = q[2];
            q[7] = 1.0f;
            *(u32 *)&q[8] = (((u32)(255.0f * t) << 24) & 0xFF000000) | (fade_rgb_110[w->ent->rgb] & 0xFFFFFF);
            flps0D00(q);
            flSetRenderState(0x6D, 3);
        }
    }
}

void fade_set(int n) {
    FADE_W *w = &fade_w;
    FADE_ENT *e;
    int a;
    int b;

    fade_w.req = n;
    e = &fade_data[fade_w.req - 1];
    a = e->end - e->start;
    if (a < 0) {
        a = -a;
    }
    b = e->step;
    if (b < 0) {
        b = -b;
    }
    w->prev = a / b;
    w->cur = w->req;
    w->alpha = fade_data[w->cur - 1].start;
}

void fade_reset(void) {
    fade_w.state = 0;
    fade_w.req = 0;
    fade_w.ent = 0;
    fade_w.prev = 0;
}
