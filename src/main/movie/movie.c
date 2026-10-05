/* Movie (Sofdec) playback wrapper, SLPM_654.95 main 0x22F7B0-0x22FDD0:
 * movie_reset/start/request/add_list/stop/exit/status_ck/server/draw.
 * sfd_work holds the CRI player handle and the texture the frames are
 * uploaded into; movie_draw draws that texture as a full-screen sprite.
 * Field names are guesses. */
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

void movie_reset(void)
{
    memset(&sfd_work, 0, sizeof(sfd_work));
}

int movie_start(no)
int no;
{
    s32 *c;
    SFD_W *w;
    u16 pw, ph;
    SFD_TBL *t;

    t = &sfd_tbl[no];
    flCompactVRAM();
    flCompact();
    w = &sfd_work;
    c = sfd_work.cprm;
    sfd_work.no = no;
    memset(c, 0, 0x30);
    c[8] = 0x11;
    c[0] = t->fmt;
    c[1] = t->time * 1000;
    pw = t->w;
    c[2] = pw;
    ph = t->h;
    c[3] = ph;
    c[4] = t->x0E;
    c[5] = t->x0F;
    c[7] = (mwPlyCalcWorkCprmSfd(c) + 0x3F) & ~0x3F;
    sfd_work.handle = flPS2GetSystemMemoryHandle(c[7] + 0x40, 1);
    c[6] = (flPS2GetSystemBuffAdrs(sfd_work.handle) + 0x3F) & ~0x3F;
    memset((void *)c[6], 0, (u32)c[7]);
    sfd_work.ply = (MWPLY *)mwPlyCreateSofdec(c);
    if (sfd_work.ply == 0) {
        return -1;
    }
    w->tex = flSfdCreateTextureEX(&w->ply, pw, ph);
    if (w->tex == 0) {
        return -1;
    }
    mem_tex[0x155] = w->tex;
    w->w = pw;
    w->h = ph;
    if (t->fmt == 1) {
        w->stereo = 1;
    } else {
        w->stereo = 0;
    }
    return 0;
}

void movie_request(no, add)
int no;
int add;
{
    SFD_TBL *t = &sfd_tbl[no];
    SFD_W *w = &sfd_work;
    s16 vol;

    movie_stop();
    if (add == 0) {
        mwPlyStartAfs(w->ply, 0, t->file);
    } else {
        movie_add_list(no);
        mwPlyStartSeamless(w->ply);
    }
    if (w->stereo == 1) {
        vol = adx_vol_tbl[adx_cnfvol_tbl[system_w[0x36]]];
        if (vol < -0x3C0) {
            vol = -0x3C0;
        }
        w->ply->vt->x2C(w->ply, vol);
    }
}

void movie_add_list(no)
int no;
{
    SFD_W *w = &sfd_work;
    mwPlyEntryAfs(w->ply, 0, sfd_tbl[no].file);
}

void movie_stop(void)
{
    SFD_W *w = &sfd_work;
    w->ready = 0;
    if (w->ply != 0) {
        w->ply->vt->x1C(w->ply);
    }
}

void movie_exit(void)
{
    SFD_W *w = &sfd_work;
    movie_stop();
    if (w->ply != 0) {
        if (w->tex != 0) {
            release_texture(0x155, 1);
        }
        w->ply->vt->x14(w->ply);
        flPS2ReleaseSystemMemory(w->handle);
    }
    movie_reset();
}

int movie_status_ck(void)
{
    SFD_W *w = &sfd_work;
    return w->ply->vt->x20(w->ply) & 0xFF;
}

int movie_server(void)
{
    s32 frm[20];
    s32 *src;
    s32 *dst;
    int i;
    SFD_W *w = &sfd_work;

    if (sfd_work.ply == 0) {
        return 0;
    }
    mwPlyGetCurFrm(w->ply, frm);
    if (frm[0] != 0) {
        src = frm;
        dst = w->frm;
        i = 19;
        do {
            *dst = *src;
            i--;
            src++;
            dst++;
        } while (i > 0);
        w->ready = 1;
        flSfdReloadTexture(w->tex, w->frm, w->w, w->h);
        mwPlyRelCurFrm(w->ply);
    }
    return w->ready;
}
