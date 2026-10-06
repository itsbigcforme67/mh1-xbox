/* adxs_nm - SLPM_654.95 0x00100380-0x00100E88 (g_Adx_server.s): ADX server wrapper (Adx_server), the file load
   queue (load_bin = blocking load, load_bin_req/load_task = background queue of 64 entries x 0x100 bytes) and the
   two BGM stream channels (str_*: play/pause/stop, master volume, fades). Working file: matching functions are
   split into runs. Names of fields are guesses from use. */
#include "types.h"
#include "game.h"

extern GAME_W game_w;
extern u8 *cw;
extern s32 ConnWork[];
extern s32 v_counter;
extern s32 ps2_v_counter;
extern s32 pub_flag;
extern s32 adxt[2];
extern u8 system_w[];
extern u8 adx_cnfvol_tbl[8];
extern s16 adx_vol_tbl[];

int ADXF_OpenAfs(int, int);
int ADXF_Open(void *, int);
int ADXF_GetFsizeSct(int);
void ADXF_ReadNw(int, int, void *);
int ADXF_GetStat(int);
void ADXF_Close(int);
void ADXF_Stop(int);
int Online_ck(void);
void AQ_exec_load(void);
void Quest_timer_calc(int);
void func_5AD620(int);
void *memset(void *, int, u32);
void str_server(void);
void load_task(void);
void ADXT_Pause(int, int);
void ADXT_Stop(int);
int ADXT_GetStat(int);
void ADXT_StartAfs(int, int, int);
void ADXT_StartSeamless(int);
void ADXT_SetOutVol(int, int);
void ADXT_SetOutputMono(int);
void flSndOutputMode(int);

void Adx_server(void) {
    str_server();
    load_task();
}

/* Blocking AFS load: open file id (hi 16 bits partition, low 16 bits file), read it into buf and wait,
   keeping the quest timer and network running (checked every v-blank); returns 1. */
int load_bin(u32 id, void *buf) {
    u16 part = id >> 16;
    u16 file = id;
    int h;
    int st;

    do {
        do {
            h = ADXF_OpenAfs(part, file);
        } while (h == 0);
        st = ADXF_GetFsizeSct(h);
        ADXF_ReadNw(h, st, buf);
        for (;;) {
            st = ADXF_GetStat(h);
            if (st != 2) {
                break;
            }
            if (v_counter != ps2_v_counter) {
                v_counter = ps2_v_counter;
                if (Online_ck() == 1) {
                    AQ_exec_load();
                    if (game_w.x1DC != 0 && cw[0x35D5] != 0 && (s8)cw[0x2C07] == 0) {
                        func_5AD620(ConnWork[1]);
                    }
                }
                Quest_timer_calc(1);
            }
        }
        ADXF_Close(h);
    } while (st != 3);
    return 1;
}

typedef struct LOADW {
    s8 busy;            /* 0x00 queue not empty */
    s8 state;           /* 0x01 0 = idle/start next, 1 = waiting for the read */
    u8 _pad02[2];
    s32 rd;             /* 0x04 queue entry being served */
    s32 wr;             /* 0x08 next free entry */
    s32 handle;         /* 0x0C open ADXF handle, 0 = none */
} LOADW;

typedef struct LOADQ {
    s32 kind;           /* 0x00 -1 free, 0 = AFS id in id, 1 = file name at 0x10, 2 = call func */
    u32 id;             /* 0x04 AFS id (partition << 16 | file) or function pointer for kind 2 */
    s32 nargs;          /* 0x08 kind 2: how the arguments are passed */
    void *dst;          /* 0x0C destination buffer */
    s32 arg[4];         /* 0x10 file name / call arguments */
    u8 _pad20[0xE0];
} LOADQ;

extern LOADW load_w;
extern LOADQ load_q[64];

void load_work_init(void) {
    int i;

    memset(&load_w, 0, 0x10);
    for (i = 0; i < 64; i++) {
        load_q[i].kind = -1;
    }
}

void load_bin_req(u32 id, void *dst) {
    LOADQ *q = &load_q[load_w.wr];
    int n;

    q->kind = 0;
    q->id = id;
    q->dst = dst;
    load_w.busy = 1;
    n = load_w.wr + 1;
    load_w.wr = n;
    load_w.wr = n % 64;
}

void next_que_set(LOADQ *q);
void call_func(LOADQ *q);

void load_task(void) {
    LOADW *w = &load_w;
    LOADQ *q = &load_q[load_w.rd];

    switch (w->state) {
    case 0:
        if (w->busy == 0) {
            return;
        }
        if (w->handle != 0) {
            ADXF_Stop(w->handle);
            ADXF_Close(w->handle);
        }
        if (q->kind == 0) {
            w->handle = ADXF_OpenAfs((q->id >> 16) & 0xFFFF, q->id & 0xFFFF);
            if (w->handle == 0) {
                return;
            }
            ADXF_ReadNw(w->handle, ADXF_GetFsizeSct(w->handle), q->dst);
            w->state++;
        } else if (q->kind == 2) {
            call_func(q);
            next_que_set(q);
        } else if (q->kind == 1) {
            w->handle = ADXF_Open(q->arg, 0);
            if (w->handle == 0) {
                return;
            }
            ADXF_ReadNw(w->handle, ADXF_GetFsizeSct(w->handle), q->dst);
            w->state++;
        }
        return;
    case 1: {
        int st = ADXF_GetStat(w->handle);

        if (st != 2) {
            ADXF_Close(w->handle);
            w->handle = 0;
            if (st != 4) {
                next_que_set(q);
                w->state = 0;
            }
        }
        break;
    }
    }
}

void next_que_set(LOADQ *q) {
    LOADW *w = &load_w;
    int n;

    q->kind = -1;
    n = w->rd + 1;
    w->rd = n;
    w->rd = n % 64;
    if (load_q[w->rd].kind < 0) {
        w->busy = 0;
    }
}

s8 load_busy_ck(void) {
    return load_w.busy;
}

/* Calls the function queued as kind 2 (q->id holds the address, nargs picks how many of the four
   queued arguments are real; unused ones are passed along anyway). */
void call_func(LOADQ *q) {
    void (*f)() = (void (*)())q->id;
    s32 a = q->arg[0];
    s32 b = q->arg[1];
    s32 c = q->arg[2];
    s32 d = q->arg[3];

    int n = q->nargs;

    if (n == 0) {
        f(n, b, c, d);
    } else if (n == 1) {
        f(a, b, c, d);
    } else if (n == 2) {
        f(a, b, c, d);
    } else if (n == 3) {
        f(a, b, c, d);
    } else if (n == 4) {
        f(a, b, c, d);
    }
}

/* AFS file size in bytes (sectors * 0x800). */
int afs_file_length(u32 id) {
    u16 file = id;
    u16 part = id >> 16;
    int h;
    int n;

    do {
        n = part;
        h = ADXF_OpenAfs(n, file);
    } while (h == 0);
    n = ADXF_GetFsizeSct(h);
    ADXF_Close(h);
    return n << 11;
}

typedef struct STRW {
    s32 stat;           /* 0x00 ADXT_GetStat result */
    s16 vol_max;        /* 0x04 option volume of the channel (adx_cnfvol_tbl) */
    s16 vol;            /* 0x06 current volume (index into adx_vol_tbl) */
    s16 _pad08;
    s16 fade_from;      /* 0x0A */
    s16 fade_to;        /* 0x0C */
    s16 fade_len;       /* 0x0E total frames of the fade */
    s16 fade_cnt;       /* 0x10 frames left */
    s16 _pad12;
} STRW;                 /* 0x14 */

extern STRW str_w[2];

void str_pause(int ch, int on);
void str_stop(int ch);
void str_volume(int ch, int vol);
void str_play_sub(int ch, int file, int vol, int pause);
void str_fadein_vol(int ch, int frames, int vol);
void str_master_vol(int update);
void str_init(void);

void str_init(void) {
    int i;
    STRW *w;

    for (i = 0, w = str_w; i < 2; i++, w++) {
        str_stop(i);
        str_pause(i, 0);
        memset(w, 0, 0x14);
    }
}

void str_play(int ch, int file) {
    str_play_sub(ch, file, str_w[ch].vol_max, 0);
}

void str_play_vol(int ch, int file, int vol) {
    str_play_sub(ch, file, vol, 0);
}

void str_play_f_vol(int ch, int file, int frames, int vol) {
    str_play_sub(ch, file, 0, 0);
    str_fadein_vol(ch, frames, vol);
}

void str_play_sub(int ch, int file, int vol, int pause) {
    str_pause(ch, pause);
    str_volume(ch, vol);
    ((STRW *)((u8 *)&str_w + ch * 0x14))->fade_cnt = 0;
    if (pub_flag == 0) {
        if (file >= 0) {
            ADXT_StartAfs(adxt[ch], 0, file);
        } else {
            ADXT_StartSeamless(adxt[ch]);
        }
    }
}

void str_pause(int ch, int on) {
    ADXT_Pause(adxt[ch], on);
}

void str_stop(int ch) {
    ADXT_Stop(adxt[ch]);
}

void str_stop_all(void) {
    str_init();
    str_master_vol(0);
}

void str_master_vol(int update) {
    int i;
    STRW *w;

    for (i = 0, w = str_w; i < 2; i++, w++) {
        if (i == 0) {
            w->vol_max = adx_cnfvol_tbl[system_w[0x36]];
        } else {
            w->vol_max = adx_cnfvol_tbl[system_w[0x37]];
        }
        if (update == 1) {
            str_volume(i, w->vol_max);
        }
    }
}

void str_volume(int ch, int vol) {
    STRW *w = &str_w[ch];
    int v;

    if (w->vol_max < vol) {
        vol = w->vol_max;
    }
    w->vol = vol;
    v = (int)(f32)adx_vol_tbl[w->vol];
    if (v < -999) {
        v = -999;
    }
    ADXT_SetOutVol(adxt[ch], v);
}

void str_outmode(int mode) {
    ADXT_SetOutputMono(mode ^ 1);
    flSndOutputMode(mode);
}

int str_getstat(int ch) {
    return str_w[ch].stat;
}

void str_fadeout(int ch, int frames) {
    STRW *w = &str_w[ch];

    w->fade_from = w->vol;
    w->fade_to = 0;
    w->fade_cnt = frames;
    w->fade_len = frames;
}

void str_fadein(int ch, int frames) {
    STRW *w = &str_w[ch];

    w->fade_from = w->vol;
    w->fade_to = w->vol_max;
    w->fade_cnt = frames;
    w->fade_len = frames;
}

void str_fadein_vol(int ch, int frames, int vol) {
    STRW *w = &str_w[ch];

    w->fade_from = w->vol;
    w->fade_to = vol;
    w->fade_cnt = frames;
    w->fade_len = frames;
}

void str_server(void) {
    int i;
    STRW *w;
    int *a;

    for (i = 0, w = str_w, a = adxt; i < 2; i++, a++, w++) {
        if (w->fade_cnt > 0 && w->stat == 3) {
            w->fade_cnt--;
            str_volume(i, w->fade_from + (w->fade_to - w->fade_from) * (w->fade_len - w->fade_cnt) / w->fade_len);
        }
        w->stat = ADXT_GetStat(*a);
    }
}

/* ===== 0x00100E20-0x00101E38: card work reset, effect work pool (eft_work: 0x80 entries of 0x40 bytes) ===== */
extern u8 card_w[];
extern u8 card_w2[];

void Card_task(void) {
}

void init_card_w(void) {
    memset(card_w, 0, 0x84);
    memset(card_w2, 0, 0x84);
    system_w[0x3C] = 0;
}

int save_file_req(void) {
    return 1;
}

typedef struct EFTL {
    u8 used;            /* 0x00 */
    u8 on;              /* 0x01 */
    u8 type;            /* 0x02 */
    u8 sub;             /* 0x03 */
    u8 _pad04[8];
    struct EFTL *prev;  /* 0x0C */
    struct EFTL *next;  /* 0x10 */
    void (*trans)(struct EFTL *);   /* 0x14 */
    void *heap;         /* 0x18 work from the set work heap */
    u8 heap_pos;        /* 0x1C */
    u8 heap_n;          /* 0x1D */
    u8 _pad1E[2];
    void (*move)(struct EFTL *);    /* 0x20 */
    u8 _pad24[0x34 - 0x24];
    s32 owner;          /* 0x34 */
    s32 prim;           /* 0x38 */
    s16 prim_no;        /* 0x3C */
    u8 prim2;           /* 0x3E */
    u8 _pad3F;
} EFTL;                 /* 0x40 */

extern EFTL eft_work[0x80];
extern EFTL **eft_sp;
extern EFTL *eft_w_top;
extern s32 eft_ctr;

int get_start_heap();
int get_heap_ptr(int);
void set_used_heap(int, int);
void clr_used_heap(int, int);

void init_eft_work(void) {
    int i;
    EFTL *e;

    memset(eft_work, 0, 0x2000);
    eft_w_top = 0;
    e = &eft_work[0x7F];
    eft_sp = (EFTL **)eft_work;
    for (i = 0; i < 0x80; i++, e--) {
        *--eft_sp = e;
    }
    game_w.meat_num = 0;
    eft_ctr = 0x80;
}

void push_eft_work(EFTL *e);

void clr_eft_work(void) {
    EFTL *e = eft_w_top;

    if (e != 0) {
        do {
            if (e->used != 0 && e->prim2 == 0) {
                EFTL *t = e;
                e = e->next;
                push_eft_work(t);
            } else {
                e = e->next;
            }
        } while (e != 0);
    }
}

EFTL *pull_eft_work(int n) {
    int pos;
    EFTL *e;

    if ((pos = get_start_heap()) < 0) {
        return 0;
    }
    if (eft_ctr == 0) {
        return 0;
    }
    eft_ctr--;
    e = *eft_sp++;
    e->prev = 0;
    e->next = eft_w_top;
    eft_w_top = e;
    if (e->next != 0) {
        e->next->prev = e;
    }
    e->used = 1;
    e->prim2 = 0;
    e->heap = (void *)get_heap_ptr(pos);
    e->heap_pos = pos;
    e->heap_n = n;
    set_used_heap(pos, n);
    return e;
}

EFTL *pull_eft_work2(int n) {
    int pos;
    EFTL *e;
    EFTL *t;
    EFTL *nx;

    if ((pos = get_start_heap()) < 0) {
        return 0;
    }
    if (eft_ctr == 0) {
        return 0;
    }
    eft_ctr--;
    t = eft_w_top;
    e = *eft_sp++;
    if (t == 0) {
        eft_w_top = e;
        e->prev = 0;
    } else {
        nx = t->next;
        if (nx != 0) {
            do {
                t = nx;
                nx = nx->next;
            } while (nx != 0);
        }
        t->next = e;
        e->prev = t;
    }
    e->next = 0;
    e->used = 1;
    e->prim2 = 0;
    e->heap = (void *)get_heap_ptr(pos);
    e->heap_pos = pos;
    e->heap_n = n;
    set_used_heap(pos, n);
    return e;
}

void push_eft_work(EFTL *e) {
    if (e->prev == 0) {
        eft_w_top = e->next;
    } else {
        e->prev->next = e->next;
    }
    if (e->next != 0) {
        e->next->prev = e->prev;
    }
    eft_ctr++;
    *--eft_sp = e;
    clr_used_heap(e->heap_pos, e->heap_n);
    memset(e, 0, 8);
    e->prim_no = 0;
    e->prim = 0;
}

void move_eft(void) {
    EFTL *e = eft_w_top;

    if (e != 0) {
        do {
            if (e->used != 0) {
                e->move(e);
            }
            e = e->next;
        } while (e != 0);
    }
}

void trans_eft(void) {
    EFTL *e = eft_w_top;

    if (e != 0) {
        do {
            if (e->trans != 0 && e->used != 0 && e->on != 0 && (e->type != 4 || e->sub != 0)) {
                e->trans(e);
            }
            e = e->next;
        } while (e != 0);
    }
}

void trans_eft_up(void) {
    EFTL *e = eft_w_top;

    if (e != 0) {
        do {
            if (e->trans != 0 && e->used != 0 && e->on != 0 && e->type == 4 && e->sub == 0) {
                e->trans(e);
            }
            e = e->next;
        } while (e != 0);
    }
}

typedef struct EKEY3 { f32 t; f32 v[3]; } EKEY3;    /* time + vector key (terminated by t == -1) */
typedef struct EKEY1 { f32 t; f32 v; } EKEY1;       /* time + scalar key */

/* Linear interpolation of a keyed vector track at time t (keys sorted by time, end marker t = -1). */
void eft_vec_linear(f32 t, EKEY3 *k, f32 *out) {
    f32 a;

    for (;; k++) {
        if (k->t == -1.0f) {
            return;
        }
        if (t == k->t) {
            out[0] = k->v[0];
            out[1] = k->v[1];
            out[2] = k->v[2];
            return;
        }
        if (!(t <= k->t) && t < k[1].t) {
            a = (t - k->t) / (k[1].t - k->t);
            out[0] = k->v[0] + a * (k[1].v[0] - k->v[0]);
            out[1] = k->v[1] + a * (k[1].v[1] - k->v[1]);
            out[2] = k->v[2] + a * (k[1].v[2] - k->v[2]);
            return;
        }
    }
}

void eft_alpha_linear(f32 t, EKEY1 *k, f32 *out) {
    f32 a;

    for (;; k++) {
        if (k->t == -1.0f) {
            return;
        }
        if (t == k->t) {
            *out = k->v;
            return;
        }
        if (!(t <= k->t) && t < k[1].t) {
            a = (t - k->t) / (k[1].t - k->t);
            *out = k->v + a * (k[1].v - k->v);
            return;
        }
    }
}

typedef struct ERGBA { s32 frame; u32 col; } ERGBA;   /* frame + packed color key (frame -1 ends) */

/* Linear interpolation of a keyed RGBA track at a frame, each byte channel separately. */
void eft_rgba_linear(ERGBA *k, s32 frame, u32 *out) {
    f32 a;
    u32 c0, c1;
    u32 p3, p2, p1, p0;
    u32 q3, q2, q1, q0;
    u32 d3, d2, d1, d0;

    for (;; k++) {
        if (k->frame == -1) {
            return;
        }
        if (frame == k->frame) {
            *out = k->col;
            return;
        }
        if (k->frame < frame && frame < k[1].frame) {
            c0 = k->col;
            c1 = k[1].col;
            p2 = (c0 >> 16) & 0xFF;
            p3 = (c0 >> 24) & 0xFF;
            p1 = (c0 >> 8) & 0xFF;
            p0 = c0 & 0xFF;
            q3 = (c1 >> 24) & 0xFF;
            a = (f32)(frame - k->frame) / (f32)(k[1].frame - k->frame);
            d3 = a * ((f32)q3 - (f32)p3);
            q2 = (c1 >> 16) & 0xFF;
            d2 = a * ((f32)q2 - (f32)p2);
            q1 = (c1 >> 8) & 0xFF;
            d1 = a * ((f32)q1 - (f32)p1);
            q0 = c1 & 0xFF;
            d0 = a * ((f32)q0 - (f32)p0);
            *out = ((p0 + (d0 & 0xFF)) & 0xFF) | (((p1 + (d1 & 0xFF)) & 0xFF) << 8) | (((p3 + (d3 & 0xFF)) & 0xFF) << 24)
                   | (((p2 + (d2 & 0xFF)) & 0xFF) << 16);
            return;
        }
    }
}

void flmatMakeScale(void *, f32, f32, f32);
void flmatRotX33(void *, f32);
void flmatRotY33(void *, f32);
void flmatRotZ33(void *, f32);
void flmatRotXYZ33(void *, f32, f32, f32);
void flmatSetTrans(void *, f32, f32, f32);
void flSetRenderState(int, u32);
void Material_set_sub(int, void *);
void clay_attr_set(int);
void clay_attr_reset(void);
void flExecuteClay(int, int);
void SetFilterMode(int);
void SetTrnslMode(int, int);
void SetOpeMode(int);
void Eft_rendope_set(int);

/* Builds a scale / rotation / translation matrix: flags & 0xE picks the rotation axes (8 = X, 4 = Y, 2 = Z, 0xE = all). */
void make_mat_srt(f32 *scale, f32 *rot, f32 *pos, int flags, void *mat) {
    f32 px = pos[0];
    f32 py = pos[1];
    f32 pz = pos[2];

    flmatMakeScale(mat, scale[0], scale[1], scale[2]);
    switch ((u16)flags & 0xE) {
    case 0xE:
        flmatRotXYZ33(mat, rot[0], rot[1], rot[2]);
        break;
    case 8:
        flmatRotX33(mat, rot[0]);
        break;
    case 4:
        flmatRotY33(mat, rot[1]);
        break;
    case 2:
        flmatRotZ33(mat, rot[2]);
        break;
    }
    flmatSetTrans(mat, px, py, pz);
}

/* Draws a clay model with a transform and an alpha (flags & 1: only alpha, else white + alpha). */
int eft_trans_sub(f32 alpha, s32 *clay, int mtx, int flags, int mat) {
    if (clay == 0) {
        return 0;
    }
    if (clay[0] == -1) {
        return 0;
    }
    flSetRenderState(0x60, 0);
    flSetRenderState(0x1A, mtx);
    if ((u16)flags & 1) {
        flSetRenderState(0x67, ((u32)(255.0f * alpha) & 0xFF) << 24);
    } else {
        flSetRenderState(0x67, (((u32)(255.0f * alpha) & 0xFF) << 24) | 0xFFFFFF);
    }
    Material_set_sub(mat, clay);
    clay_attr_set(clay[0x88 / 4]);
    Eft_rendope_set(flags);
    flExecuteClay(clay[0], 0);
    clay_attr_reset();
    return 1;
}

int eft_trans_sub_col(s32 *clay, int mtx, u32 col, int flags, int mat) {
    if (clay == 0) {
        return 0;
    }
    if (clay[0] == -1) {
        return 0;
    }
    flSetRenderState(0x60, 0);
    flSetRenderState(0x1A, mtx);
    flSetRenderState(0x67, col);
    Material_set_sub(mat, clay);
    clay_attr_set(clay[0x88 / 4]);
    Eft_rendope_set(flags);
    flExecuteClay(clay[0], 0);
    clay_attr_reset();
    return 1;
}

int eft_trans_sub_opa(s32 *clay, int mtx, int mat) {
    if (clay == 0) {
        return 0;
    }
    if (clay[0] == -1) {
        return 0;
    }
    flSetRenderState(0x60, 0x80);
    flSetRenderState(0x1A, mtx);
    Material_set_sub(mat, clay);
    clay_attr_set(clay[0x88 / 4]);
    flExecuteClay(clay[0], 0);
    clay_attr_reset();
    return 1;
}

void Eft_rendope_set(int flags) {
    u16 f = flags;

    if (f & 0x10) {
        SetFilterMode(0);
    }
    switch (f & 0xE) {
    case 2:
        SetTrnslMode(4, 1);
        break;
    case 4:
        SetTrnslMode(4, 5);
        break;
    case 8:
        SetTrnslMode(1, 1);
        SetOpeMode(2);
        break;
    }
}

int Niku_ok_ck(void) {
    return game_w.meat_num < 3;
}

int Kaeru_ck(s32 owner) {
    EFTL *e;
    s16 i;

    for (i = 0, e = eft_work; i < 0x80; i++, e++) {
        if (e->used != 0 && e->type == 0x16 && e->sub == 1 && e->owner == owner) {
            return 1;
        }
    }
    return 0;
}
