/* adxs04 - card work reset and effect work pool (SLPM_654.95 0x00100E20-0x00101508): Card_task, init_card_w, save_file_req, init_eft_work .. eft_alpha_linear. Whole file in adxs_nm.c. */
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
void next_que_set(LOADQ *q);
void call_func(LOADQ *q);
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
} STRW;
/* 0x14 */
extern STRW str_w[2];
void str_pause(int ch, int on);
void str_stop(int ch);
void str_volume(int ch, int vol);
void str_play_sub(int ch, int file, int vol, int pause);
void str_fadein_vol(int ch, int frames, int vol);
void str_master_vol(int update);
void str_init(void);
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
} EFTL;
/* 0x40 */
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
typedef struct EKEY3 { f32 t; f32 v[3]; } EKEY3;
/* time + vector key (terminated by t == -1) */
typedef struct EKEY1 { f32 t; f32 v; } EKEY1;
/* time + scalar key */
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
typedef struct ERGBA { s32 frame; u32 col; } ERGBA;
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
