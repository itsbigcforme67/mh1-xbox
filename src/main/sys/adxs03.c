/* adxs03 - BGM stream channels (SLPM_654.95 0x00100910-0x00100E18): str_init .. str_server. Whole file in adxs_nm.c. */
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
void push_eft_work(EFTL *e);
typedef struct EKEY3 { f32 t; f32 v[3]; } EKEY3;
/* time + vector key (terminated by t == -1) */
typedef struct EKEY1 { f32 t; f32 v; } EKEY1;
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
