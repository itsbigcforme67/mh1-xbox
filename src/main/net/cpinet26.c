/* cpinet26 - CpInetPppGetStatus (SLPM_654.95 0x00236160-0x00236370): reads the IOP PPP status (Ave_PppStatus) and maps its two state codes
   (a 0..9 phase and a 0..13 detail) to the game's own numbering through switches that are the identity except for the default cases (8 and 11).
   Struct layouts are guesses from use. Written new in this pass. Two jump tables in main:rodata. */
#include "types.h"
typedef struct PPPSTX { s32 v0; s32 v1; s16 v2; s16 h2A; s32 v3; s16 h30; s16 h32; s32 rest[5]; } PPPSTX;
typedef struct PPPOUT { s32 x00; s32 x04; s32 x08; s16 x0C; s16 x0E; s16 x10; } PPPOUT;
int Ave_PppStatus();
int CpInetPppGetStatus(PPPOUT *o) {
    PPPSTX st;
    int r;

    r = (s16)Ave_PppStatus(&st);
    if (r >= 0) {
    } else {
        return r;
    }
    o->x04 = st.v0;
    o->x00 = st.v1;
    switch (st.v2) {
    case 0:
        o->x0C = 0;
        break;
    case 1:
        o->x0C = 1;
        break;
    case 2:
        o->x0C = 2;
        break;
    case 3:
        o->x0C = 3;
        break;
    case 4:
        o->x0C = 4;
        break;
    case 5:
        o->x0C = 5;
        break;
    case 6:
        o->x0C = 6;
        break;
    case 7:
        o->x0C = 7;
        break;
    default:
        o->x0C = 8;
        break;
    case 9:
        o->x0C = 9;
        break;
    }
    o->x08 = st.v3;
    o->x0E = st.h30;
    switch (st.h32) {
    case 0:
        o->x10 = 0;
        break;
    case 1:
        o->x10 = 1;
        break;
    case 2:
        o->x10 = 2;
        break;
    case 3:
        o->x10 = 3;
        break;
    case 4:
        o->x10 = 4;
        break;
    case 5:
        o->x10 = 5;
        break;
    case 6:
        o->x10 = 6;
        break;
    case 7:
        o->x10 = 7;
        break;
    case 8:
        o->x10 = 8;
        break;
    case 9:
        o->x10 = 9;
        break;
    case 10:
        o->x10 = 10;
        break;
    default:
        o->x10 = 11;
        break;
    case 12:
        o->x10 = 12;
        break;
    case 13:
        o->x10 = 13;
        break;
    }
    return r;
}
