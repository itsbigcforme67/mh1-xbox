/* adxs01 - ADX server entry (SLPM_654.95 0x00100380-0x001003A4): Adx_server. Whole file in adxs_nm.c. */
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
