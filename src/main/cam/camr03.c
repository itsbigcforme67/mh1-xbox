/* camr03 - f_camr 0x00224D40-0x00224DC8: dDivComplex. Whole file in camr_nm.c. */
#include "types.h"

typedef struct DCMPLX {
    f32 re;
    f32 im;
} DCMPLX;





#include "pl.h"
#include "game.h"

#ifndef NULL
#define NULL 0
#endif

/* Quest state, only the fields used here (QUEST_W is declared per file). */
typedef struct QW {
    u8 _pad00[0x34];
    s16 x34;            /* 0x34 */
    u8 _pad36[0x3C - 0x36];
    PLW *p3C;           /* 0x3C */
    s32 flags;          /* 0x40 bit 0: clear camera wanted */
    u8 _pad44[0xB0 - 0x44];
    PLW *pB0;           /* 0xB0 */
} QW;

extern QW quest_w;
extern u8 quest_clear_camera_tbl[];
void DemoCameraRequest(int, s32);


void dDivComplex(DCMPLX *r, DCMPLX *a, DCMPLX *b)
{
  DCMPLX t;
  f32 d;
  d = (b->re * b->re) + (b->im * b->im);
  if (d != 0.0f)
  {
    d = 1.0f / d;
    t.re = d * ((a->re * b->re) + (a->im * b->im));
    t.im = d * ((a->im * b->re) - (a->re * b->im));
    *r = t;
  }
  else
  {
    *r = *a;
  }
}
