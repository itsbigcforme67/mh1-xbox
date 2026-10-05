/* emt01 - monster draw hooks 0x0010CD30-0x0010CE94: em15_trans_sub, em21_trans_sub, em_trans_sub. Whole file in emtrans_nm.c. */
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





void em15_trans_sub(EMT *w, int part) {
    MAT4 m;

    flmatInit(&m);
    switch (part) {
    case 2:
        m[3][0] = (f32)(game_w.x1E & 0x3F) / 64.0f;
        break;
    }
    flSetRenderState(0x19, (int)&m);
}

void em21_trans_sub(EMT *w, int part) {
    MAT4 m;

    flmatInit(&m);
    switch (part) {
    case 4:
        m[3][1] = 1.0f - (f32)(game_w.x1E & 0x3F) / 64.0f;
        break;
    }
    flSetRenderState(0x19, (int)&m);
}

void em_trans_sub(EMT *w, int part) {
    switch (w->kind) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
        break;
    case 15:
        em15_trans_sub(w, part);
        break;
    case 16:
    case 17:
    case 18:
    case 19:
    case 20:
        break;
    case 21:
        em21_trans_sub(w, part);
        break;
    case 22:
    case 23:
    case 24:
    case 25:
    case 26:
    case 27:
    case 28:
    case 29:
    case 30:
    case 31:
    case 32:
    case 33:
    case 34:
        break;
    }
}
