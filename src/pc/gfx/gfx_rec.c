/*
 * gfx_rec.c - record the draws of one game tick and replay them per frame.
 *
 * The PS2 game draws while its tasks run: the title screen, the logos and
 * the character creation screen call flps0008 / font_draw / trans() from
 * inside the task of that frame. On the PC, game ticks (30 per second) and
 * drawn frames are not tied together: a frame may follow several ticks or
 * none. So while such a tick runs, the gfx calls are recorded (render
 * states, 2D triangle lists, clays, host callbacks) instead of drawn, and
 * every frame replays the list of the last finished tick.
 *
 * Backend-independent: the backend's gfx_set_render_state, gfx_draw_2d
 * and gfx_execute_clay call gfx_rec_* first and return when it took the
 * call.
 */
#include "gfx.h"

#include <stdlib.h>
#include <string.h>

enum { REC_STATE, REC_2D, REC_CLAY, REC_CALL };

typedef struct {
    int kind;
    int state;              /* REC_STATE */
    uintptr_t v;
    int ptr;                /* v points at m (a copied matrix/float) */
    float m[16];
    int w, h, n;            /* REC_2D: data offsets */
    size_t pos, st, col;    /* (size_t)-1: none */
    gfx_clay *clay;         /* REC_CLAY */
    void (*fn)(void *);     /* REC_CALL */
    void *arg;
} rec_cmd;

typedef struct {
    rec_cmd *cmd;
    size_t n, cap;
    unsigned char *data;
    size_t nd, capd;
} rec_list;

static rec_list list[2];
static int building = -1, shown = -1;
int gfx_rec_on;

static rec_cmd *push(rec_list *l)
{
    if (l->n == l->cap) {
        l->cap = l->cap ? l->cap * 2 : 256;
        l->cmd = realloc(l->cmd, l->cap * sizeof *l->cmd);
    }
    memset(&l->cmd[l->n], 0, sizeof l->cmd[0]);
    return &l->cmd[l->n++];
}

static size_t copy(rec_list *l, const void *p, size_t n)
{
    size_t o;
    if (!p)
        return (size_t)-1;
    while (l->nd + n + 16 > l->capd) {
        l->capd = l->capd ? l->capd * 2 : 1 << 16;
        l->data = realloc(l->data, l->capd);
    }
    o = (l->nd + 15) & ~(size_t)15;
    memcpy(l->data + o, p, n);
    l->nd = o + n;
    return o;
}

void gfx_rec_begin(void)
{
    building = shown == 0 ? 1 : 0;
    list[building].n = 0;
    list[building].nd = 0;
    gfx_rec_on = 1;
}

void gfx_rec_end(void)
{
    gfx_rec_on = 0;
    if (building >= 0)
        shown = building;
    building = -1;
}

/* forget the recorded frame (leaving a recorded screen for 3D play) */
void gfx_rec_clear(void)
{
    gfx_rec_on = 0;
    shown = building = -1;
}

int gfx_rec_have(void) { return shown >= 0; }

int gfx_rec_state(int state, uintptr_t v)
{
    rec_cmd *c;
    if (!gfx_rec_on)
        return 0;
    c = push(&list[building]);
    c->kind = REC_STATE;
    c->state = state;
    c->v = v;
    switch (state) {
    case GFX_RS_VIEW: case GFX_RS_WORLD: case GFX_RS_PROJECTION:
        memcpy(c->m, (const float *)v, sizeof c->m);
        c->ptr = 1;
        break;
    case GFX_RS_TEXMAT:
        if (v) {
            memcpy(c->m, (const float *)v, sizeof c->m);
            c->ptr = 1;
        }
        break;
    case GFX_RS_FOG_START: case GFX_RS_FOG_END:
        c->m[0] = *(const float *)v;
        c->ptr = 1;
        break;
    }
    return 1;
}

int gfx_rec_2d(int w, int h, int n, const float *pos, const float *st, const uint8_t *col)
{
    rec_list *l;
    rec_cmd *c;
    if (!gfx_rec_on)
        return 0;
    l = &list[building];
    c = push(l);
    c->kind = REC_2D;
    c->w = w;
    c->h = h;
    c->n = n;
    c->pos = copy(l, pos, sizeof(float) * 2 * (size_t)n);
    c->st = copy(l, st, sizeof(float) * 2 * (size_t)n);
    c->col = copy(l, col, 4 * (size_t)n);
    return 1;
}

int gfx_rec_clay(gfx_clay *clay)
{
    rec_cmd *c;
    if (!gfx_rec_on)
        return 0;
    c = push(&list[building]);
    c->kind = REC_CLAY;
    c->clay = clay;
    return 1;
}

/* a host draw (e.g. the hunters of the character screen) at this point
 * of the recorded order */
void gfx_rec_call(void (*fn)(void *), void *arg)
{
    rec_cmd *c;
    if (!gfx_rec_on) {
        fn(arg);
        return;
    }
    c = push(&list[building]);
    c->kind = REC_CALL;
    c->fn = fn;
    c->arg = arg;
}

void gfx_rec_replay(void)
{
    rec_list *l;
    size_t i;
    if (shown < 0 || gfx_rec_on)
        return;
    l = &list[shown];
    for (i = 0; i < l->n; i++) {
        rec_cmd *c = &l->cmd[i];
        switch (c->kind) {
        case REC_STATE:
            gfx_set_render_state(c->state, c->ptr ? (uintptr_t)c->m : c->v);
            break;
        case REC_2D:
            gfx_draw_2d(c->w, c->h, c->n, (const float *)(l->data + c->pos),
                        c->st == (size_t)-1 ? NULL : (const float *)(l->data + c->st),
                        c->col == (size_t)-1 ? NULL : (const uint8_t *)(l->data + c->col));
            break;
        case REC_CLAY:
            gfx_execute_clay(c->clay);
            break;
        case REC_CALL:
            c->fn(c->arg);
            break;
        }
    }
}
