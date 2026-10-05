/* Render state cache and font setup. SLPM_654.95 0x00161480-0x001619A0 (f_font, first half):
 * SetTrnslMode/SetFilterMode/SetOpeMode/SetTextureStage keep the last value in system_w and only
 * call flSetRenderState on a change; InitRenderState resets all of it; ot_init sets up the
 * ordering tables; font_set loads the font and its palettes. */
#include "types.h"
#include "sysw.h"

extern u32 src_mode_255[10];
extern u32 dst_mode_256[10];
extern u32 ope_mode_287[3];
extern u32 filter_mode_265[2];
extern u32 mem_tex[];
extern u32 nfcol_tbl[][3];
extern u32 nfrvcol_tbl[][4];
extern u8 ot0[], ot1[], ot2[], ot3[];
extern u8 ot4[4], ot5[4], ot6[4], ot7[4], ot8[4];
extern int font_reset_flag;

typedef struct NP { u8 _pad00[0xC]; void *area; } NP;
extern NP *np;

void flSetRenderState(int, u32);
void plplInit(int, void *);
void flfntStackReset(void);
void flfntDraw(int);
int flfntGetSystemMemorySize(void);
void flGetFrame(void *);
void *flAllocMemory(int);
void flfntCreate(void *);
void flfntInit(void);
void flfntSetHalftype(int);
void flfntSetPalData(int, u32, u32, u32, u32);
int load_file_mdl(void *, int);

void SetTrnslMode(int src, int dst) {
    if (system_w.src_mode != src || system_w.dst_mode != dst) {
        system_w.src_mode = src;
        system_w.dst_mode = dst;
        flSetRenderState(0x5E, src_mode_255[src] | dst_mode_256[dst]);
    }
}

void SetFilterMode(int mode) {
    if (system_w.filter != mode) {
        system_w.filter = mode;
        flSetRenderState(0x63, filter_mode_265[mode]);
    }
}

void SetTextureStage(int n) {
    system_w.tex_stage = n;
    flSetRenderState(4, mem_tex[n]);
}

void InitRenderState(int soft) {
    f32 far_ = 20000.0f;
    f32 near_ = 30000.0f;

    if (soft == 0) {
        flSetRenderState(0xE, 0);
        flSetRenderState(1, 0x200);
        flSetRenderState(0x15, 2);
        flSetRenderState(0xF, 0x80FFFFFF);
        flSetRenderState(0x10, *(u32 *)&far_);
        flSetRenderState(0x11, *(u32 *)&near_);
        flSetRenderState(0x5F, 4);
    }
    system_w.x3D = 0x80;
    system_w.src_mode = 4;
    system_w.filter = 1;
    system_w.dst_mode = 5;
    system_w.tex_stage = 0xFFFF;
    system_w.ope = 0;
    system_w.x3E = 1;
    flSetRenderState(0x5E, 0x32);
    flSetRenderState(0x63, 0);
    flSetRenderState(0xD, 0);
    flSetRenderState(0x60, 0);
}

void SetOpeMode(int mode) {
    if (system_w.ope != mode) {
        system_w.ope = mode;
        flSetRenderState(0xD, ope_mode_287[mode]);
    }
}

void ot_init(void) {
    plplInit(0x40, ot0);
    plplInit(0x20, ot1);
    plplInit(0x10, ot2);
    plplInit(8, ot3);
    plplInit(1, ot4);
    plplInit(1, ot5);
    plplInit(1, ot6);
    plplInit(1, ot7);
    plplInit(1, ot8);
}
