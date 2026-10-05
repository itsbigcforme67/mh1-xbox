#ifndef FRAME_H
#define FRAME_H
/* Motion system types (main 0x125340-0x1267BC, src/main/frame/f_frame*.c).
 * PLW and EMW share this layout for their motion layers; only f_frame uses
 * the FRW view. Field names are guesses from how the code uses them. */
#include "types.h"
#include "fl.h"

/* One motion layer, at actor +0x194 + n * 0x50. */
typedef struct FRMT {
    s32 stat;       /* 0x00 (0x194) 1 while playing, 0 once the end is passed */
    s32 chr_no;     /* 0x04 (0x198) */
    f32 frame;      /* 0x08 (0x19C) current frame */
    f32 spd;        /* 0x0C (0x1A0) frames per tick */
    f32 loopfr;     /* 0x10 (0x1A4) loop start frame (flGetMotionSetLoopInfo) */
    f32 end;        /* 0x14 (0x1A8) last frame (flGetMotionSetTime) */
    s32 loop;       /* 0x18 (0x1AC) non-zero: the motion loops */
    f32 b_frame;    /* 0x1C (0x1B0) frame of the motion being blended in */
    f32 b_step;     /* 0x20 (0x1B4) blend rate step per tick */
    f32 b_loopfr;   /* 0x24 (0x1B8) */
    f32 b_end;      /* 0x28 (0x1BC) */
    s32 b_loop;     /* 0x2C (0x1C0) */
    s32 b_dir;      /* 0x30 (0x1C4) 1/-1 while blending (-1: no root motion), else 0 */
    s32 b_cnt;      /* 0x34 (0x1C8) blend ticks left */
    f32 b_rate;     /* 0x38 (0x1CC) blend position 0..1 */
    u32 *b_han;     /* 0x3C (0x1D0) handle slot of the new motion */
    f32 ofs[3];     /* 0x40 (0x1D4) layer 0 only: root offset last tick */
    f32 old;        /* 0x4C (0x1E0) frame before this tick's step */
} FRMT;

/* Model work's motion part (actor +0x50C). */
typedef struct FRMOT {
    u8 _pad00[0xD0];
    s32 xD0;        /* 0xD0 passed to flCalcTransVelocity */
} FRMOT;

typedef struct FRSKL {
    u8 _pad00[0x200];
    f32 vel[3];     /* 0x200 root translation gathered by the fl motion code */
} FRSKL;

typedef struct FRMDL {
    u8 _pad00[0x24];
    FRSKL *skl;     /* 0x24 */
    u8 _pad28[0x44 - 0x28];
    FRMOT *mot0;    /* 0x44 motion player of the current motions */
    u8 _pad48[0x54 - 0x48];
    FRMOT *mot1;    /* 0x54 motion player of the motion blended in */
} FRMDL;

/* The parts of PLW/EMW this file uses. */
typedef struct FRW {
    u8 be_flag;     /* 0x000 */
    u8 _pad001[0xC - 0x1];
    u16 id;         /* 0x00C player number */
    u8 _pad00E[0x10 - 0xE];
    u8 x10;         /* 0x010 0: player */
    u8 _pad011[0x1E - 0x11];
    u8 x1E;         /* 0x01E non-zero: NPC (move) */
    u8 x1F;         /* 0x01F */
    u8 _pad020[0xA0 - 0x20];
    s32 ang[3];     /* 0x0A0 */
    f32 pos[3];     /* 0x0AC */
    f32 scl[3];     /* 0x0B8 */
    u8 _pad0C4[0x194 - 0xC4];
    FRMT mt[4];     /* 0x194 */
    u8 _pad2D4[0x2DC - 0x2D4];
    u16 chr[4];     /* 0x2DC motion id per layer */
    u8 _pad2E4[0x300 - 0x2E4];
    u16 layers;     /* 0x300 number of motion layers in use */
    u8 _pad302[0x34F - 0x302];
    u8 mdl_no;      /* 0x34F monster model number */
    u8 _pad350[0x50C - 0x350];
    FRMDL *mdl;     /* 0x50C */
    u8 _pad510[0x720 - 0x510];
    u8 sub_on[4];   /* 0x720 non-zero: blend a second motion over layer n */
    u16 sub_chr[4]; /* 0x724 its motion id */
    u16 sub_rate[4];/* 0x72C its weight, 0..100 */
} FRW;

extern u32 motion_set_handle_tbl[];
extern s32 com_mot_han_ofs[];
extern s32 pl_mot_han_ofs[][8];
extern s32 em_mot_han_ofs[][8];
extern u8 *pl_area_top;
extern u8 *data_load_ptr;
extern char lit_277_003584C0[];

void flGetFrame(void *);
void flReleaseFrame(void *);
void plCreateMotionSetFromAAN(void *, u8 *, u8 *);
u32 flCreateMotionSetHandle(void *);
void flSetMotionEx(FRMOT *, u32, u16);
f32 flGetMotionSetTime(u32);
s32 flGetMotionSetLoopInfo(u32, f32 *);
void flPlayMotionExSI(f32, FRMOT *, u16);
void flBlendMotionEx(FRMOT *, FRMOT *, u16, f32, f32);
void flCalcTransVelocity(f32, f32, f32 *, s32);
f32 plFCVFcurveInterpolateHermite(f32, f32, f32, f32, f32, f32, f32);
void system_error(char *, int, int, int);
u8 Em_max_parts_get(s16);
void cpRotMatrix(s32 *, FLMAT *);
void flmatMul(FLMAT *, FLMAT *, FLMAT *);
void cpApplyMatrix(FLMAT *, f32 *, f32 *);
s32 pl_flag_ck(FRW *, int);

int aan_ctr_get(u8 *aan, int bank);
u8 *aan_ofs_calc(u8 *aan, int no);
void frame_init_b(FRW *w, int n);
void frame_init(FRW *w, int frame, int blend, int n);
int frame_move(FRW *w);
void create_plcom_motion(void);
void create_pl_motion(int pl);
void create_em_motion(int no, s16 em);

#endif
