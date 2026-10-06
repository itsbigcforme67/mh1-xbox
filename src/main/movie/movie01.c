/* SLPM_654.95 0x0022FC50-0x0022FDD0: movie_draw .. movie_draw. See movie_nm.c. */
#include "types.h"

typedef struct {
    u8 _p00[0x14];
    void (*x14)();      /* 0x14 destroy */
    u8 _p18[4];
    void (*x1C)();      /* 0x1C stop */
    int (*x20)();       /* 0x20 status */
    u8 _p24[8];
    void (*x2C)();      /* 0x2C set volume */
} MWPLY_VT;

typedef struct {
    MWPLY_VT *vt;
} MWPLY;

typedef struct {
    s32 file;       /* 0x0 */
    s32 fmt;        /* 0x4 */
    u16 time;       /* 0x8 (x1000 -> total frames/time) */
    u16 w;          /* 0xA */
    u16 h;          /* 0xC */
    u8 x0E;         /* 0xE */
    u8 x0F;         /* 0xF */
} SFD_TBL;

typedef struct {
    s32 handle;     /* 0x00 flPS2 system memory handle */
    MWPLY *ply;     /* 0x04 */
    s32 cprm[12];   /* 0x08 player create params (0x30 bytes) */
    s32 frm[19];    /* 0x38 copy of the current frame info */
    s32 x84;        /* 0x84 */
    s32 ready;      /* 0x88 a frame has arrived */
    s32 tex;        /* 0x8C texture handle */
    u16 w;          /* 0x90 */
    u16 h;          /* 0x92 */
    s8 stereo;      /* 0x94 */
    s8 no;          /* 0x95 */
    u8 _pad96[2];
} SFD_W;

typedef struct {
    s16 x, y, w, h;     /* 0x00 */
    u32 col;            /* 0x08 */
    s16 u, v, u2, v2;   /* 0x0C */
} SPR;

extern SFD_W sfd_work;
extern SFD_TBL sfd_tbl[];
extern u32 mem_tex[];
extern u8 system_w[];
extern u8 adx_cnfvol_tbl[];
extern s16 adx_vol_tbl[];

void *memset();
void flCompact();
void flCompactVRAM();
int flPS2GetSystemBuffAdrs();
int flPS2GetSystemMemoryHandle();
int flSfdCreateTextureEX();
int mwPlyCalcWorkCprmSfd();
int mwPlyCreateSofdec();
void mwPlyStartAfs();
void mwPlyStartSeamless();
void mwPlyEntryAfs();
void mwPlyGetCurFrm();
void mwPlyRelCurFrm();
void flSfdReloadTexture();
void flPS2ReleaseSystemMemory();
void release_texture();
void SetFilterMode();
void SetTrnslMode();
void flSetRenderState();
void flps0008();
void movie_add_list();
void movie_stop();
void movie_reset();










void movie_draw(void)
{
    SFD_W *w = &sfd_work;
    SPR spr;

    if (w->tex != 0 && w->ready != 0) {
        SetFilterMode(1);
        flSetRenderState(4, w->tex);
        flSetRenderState(0x60, 0);
        flSetRenderState(0x6D, 7);
        SetTrnslMode(1, 0);
        flSetRenderState(0x64, 0x20000);
        flSetRenderState(0x6C, 0);
        if (w->no == 8) {
            spr.x = -1;
            spr.y = 1;
            spr.w = (w->w << 9) / 320;
            spr.h = w->h - 1;
            spr.v = 1;
            spr.u = 0;
            spr.u2 = w->w - 1;
            spr.v2 = w->h - 2;
        } else {
            spr.x = -1;
            spr.y = 0;
            spr.w = w->w * 2;
            spr.h = 0x1C0;
            spr.v = 0x20;
            spr.u = 0;
            spr.u2 = w->w - 1;
            spr.v2 = 0x1DF;
        }
        spr.col = -1;
        flps0008(&spr);
        flSetRenderState(0x6D, 3);
        flSetRenderState(0x64, 0);
        flSetRenderState(0x6C, 1);
    }
}
