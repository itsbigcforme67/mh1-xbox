/* fcv01 - fcurve base address (SLPM_654.95 0x00170230-0x00170238): flFCVSetBaseAddress. Whole file in fcv_nm.c. */
/* fcv_nm - SLPM_654.95 0x00170230-0x001710E0 (f_flfcvgetvaluelinear.s): function-curve (fcurve) value lookup of the fl library:
   find the key interval around time t (Seek*, hint = last index, searched up/down from it) and interpolate (VU0 routines
   flFCVFcurveInterpolate*, still asm). Key layouts: Linear {?, time, value} 8 bytes, Hermite 12?, Complex 0x14, Short = s16 pairs. */
#include "types.h"
typedef struct FCV {            /* curve header */
    u8 type;                    /* 0x00 0x21 linear, 0x22 hermite, 0x23 complex, 0x11-0x13 the 16 bit versions */
    u8 _pad01;
    u16 num;                    /* 0x02 number of keys */
    s32 off;                    /* 0x04 key array offset from the base address */
} FCV;
typedef struct KEYL {           /* linear key: time at +4 relative to the pointer used below */
    f32 t;
    f32 v;
} KEYL;
extern s32 base_addr_0038A258;
f32 flFCVFcurveInterpolateLinear(f32, f32, f32, f32, f32);
s16 flFCVSeekFcurveKeyLinear(f32 t, FCV *c, KEYL **k, KEYL **n, s16 hint);
f32 flFCVFcurveInterpolateHermite(f32, f32, f32, f32, f32, f32, f32);
s16 flFCVSeekFcurveKeyHermite(f32 t, FCV *c, KEYL **k, KEYL **n, s16 hint);
s16 flFCVSeekFcurveKeyComplex(f32 t, FCV *c, KEYL **k, KEYL **n, s16 hint);
s16 flFCVSeekFcurveKeyLinearShort(f32 t, FCV *c, KEYL **k, KEYL **n, s16 hint);
s16 flFCVSeekFcurveKeyHermiteShort(f32 t, FCV *c, KEYL **k, KEYL **n, s16 hint);
s16 flFCVSeekFcurveKeyComplexShort(f32 t, FCV *c, KEYL **k, KEYL **n, s16 hint);
void flFCVSetBaseAddress(int a) {
    base_addr_0038A258 = a;
}
/* Dispatches on the curve type (0x21 linear, 0x22 hermite, 0x23 complex, 0x11-0x13 the 16 bit versions); the value stays in f0. */
f32 flFCVGetValueLinear(f32, FCV *, s16 *);
f32 flFCVGetValueHermite(f32, FCV *, s16 *);
f32 flFCVGetValueComplex(f32, FCV *, s16 *);
f32 flFCVGetValueLinearShort(f32, FCV *, s16 *);
f32 flFCVGetValueHermiteShort(f32, FCV *, s16 *);
f32 flFCVGetValueComplexShort(f32, FCV *, s16 *);
