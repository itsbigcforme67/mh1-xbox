/* camr02 - f_camr 0x00224720-0x00224790: RollAngleRail. Whole file in camr_nm.c. */
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


f32 RollAngleRail(f32 t, s16 *rail, int i)
{
  s16 (*new_var)[2];
  union 
  {
    s32 w;
    s16 h[2];
  } u;
  u.w = ((s32) (65536.0f * t)) * (rail[i + 0x121] - rail[i + 0x120]);
  u.h[1] += rail[i + 0x120];
  return 0.000095873799f * (*(new_var = &u.h))[1];
}
