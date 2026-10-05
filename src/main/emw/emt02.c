/* emt02 - monster draw hooks 0x0010EBF0-0x0010EC00: em_effect_move. Whole file in emtrans_nm.c. */
#include "types.h"

typedef f32 MAT4[4][4];

typedef struct EMT {
    u8 _pad00[2];
    u8 kind;
    u8 _pad03[0x3CC - 3];
    void (**prog)(struct EMT *);
} EMT;

typedef struct GWT { u8 _pad00[0x1E]; u16 x1E; } GWT;
extern GWT game_w;

void flmatInit(void *);
void flSetRenderState(int, int);
void em15_trans_sub(EMT *, int);
void em21_trans_sub(EMT *, int);





void em_effect_move(EMT *w) {
    w->prog[3](w);
}
