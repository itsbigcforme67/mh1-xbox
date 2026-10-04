#ifndef FL_H
#define FL_H
/* "fl": Capcom's PS2 graphics library (flPS2*, flmat*). */
#include "types.h"

typedef f32 FLMAT[4][4];

void flmatInit(FLMAT *);
void flmatRotXYZ33(FLMAT *, f32, f32, f32);
void flmatSetTrans(FLMAT *, f32, f32, f32);
void flSetRenderState(int state, u32 value);   /* pointers are passed cast to u32 */

#endif
