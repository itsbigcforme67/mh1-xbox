/* camr01 - f_camr 0x002246F0-0x00224718: ZoomBaseAngleRail. Whole file in camr_nm.c. */
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


f32 ZoomBaseAngleRail(f32 t, f32 *rail, int i)
{
  f32 new_var;
  f32 new_var2;
  new_var2 = rail[i + 0x81];
  if (1)
  {
    new_var = rail[i - -0x80];
    return (new_var * (1.0f - t)) + (new_var2 * t);
  }
}
