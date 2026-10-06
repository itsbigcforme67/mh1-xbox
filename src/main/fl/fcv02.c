/* fcv02 - fcurve value lookup and key search (SLPM_654.95 0x00170300-0x001710E0): flFCVGetValue{Linear,Hermite,Complex}[Short] and flFCVSeekFcurveKey*. Whole file in fcv_nm.c. flFCVGetValue2 (0x170240) stays asm (register choice). */
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
/* Dispatches on the curve type (0x21 linear, 0x22 hermite, 0x23 complex, 0x11-0x13 the 16 bit versions); the value stays in f0. */
f32 flFCVGetValueLinear(f32, FCV *, s16 *);
f32 flFCVGetValueHermite(f32, FCV *, s16 *);
f32 flFCVGetValueComplex(f32, FCV *, s16 *);
f32 flFCVGetValueLinearShort(f32, FCV *, s16 *);
f32 flFCVGetValueHermiteShort(f32, FCV *, s16 *);
f32 flFCVGetValueComplexShort(f32, FCV *, s16 *);
f32 flFCVGetValueLinear(f32 t, FCV *c, s16 *hint) {
    KEYL *k0 = 0;
    KEYL *k1 = 0;

    *hint = flFCVSeekFcurveKeyLinear(t, c, &k0, &k1, *hint);
    if (k1 == 0) {
        return k0->t;
    }
    return flFCVFcurveInterpolateLinear(t, k0->t, k0->v, k1->t, k1->v);
}
f32 flFCVGetValueHermite(f32 t, FCV *c, s16 *hint) {
    f32 *k0 = 0;
    f32 *k1 = 0;

    *hint = flFCVSeekFcurveKeyHermite(t, c, (KEYL **)&k0, (KEYL **)&k1, *hint);
    if (k1 == 0) {
        return k0[0];
    }
    return flFCVFcurveInterpolateHermite(t, k0[0], k0[1], k0[3], k1[0], k1[1], k1[2]);
}
f32 flFCVGetValueComplex(f32 t, FCV *c, s16 *hint) {
    f32 *k0 = 0;
    f32 *k1 = 0;

    *hint = flFCVSeekFcurveKeyComplex(t, c, (KEYL **)&k0, (KEYL **)&k1, *hint);
    if (k1 == 0) {
        return k0[1];
    }
    switch (*(s32 *)k0) {
    case 0x10000:
        return flFCVFcurveInterpolateLinear(t, k0[1], k0[2], k1[1], k1[2]);
    case 0x20000:
        return flFCVFcurveInterpolateHermite(t, k0[1], k0[2], k0[4], k1[1], k1[2], k1[3]);
    default:
        return 0.0f;
    }
}
f32 flFCVGetValueLinearShort(f32 t, FCV *c, s16 *hint) {
    s16 *k0 = 0;
    s16 *k1 = 0;

    *hint = flFCVSeekFcurveKeyLinearShort(t, c, (KEYL **)&k0, (KEYL **)&k1, *hint);
    if (k1 == 0) {
        return k0[0];
    }
    return flFCVFcurveInterpolateLinear(t, k0[0], k0[1], k1[0], k1[1]);
}
f32 flFCVGetValueHermiteShort(f32 t, FCV *c, s16 *hint) {
    s16 *k0 = 0;
    s16 *k1 = 0;

    *hint = flFCVSeekFcurveKeyHermiteShort(t, c, (KEYL **)&k0, (KEYL **)&k1, *hint);
    if (k1 == 0) {
        return k0[0];
    }
    return flFCVFcurveInterpolateHermite(t, k0[0], k0[1], k0[3], k1[0], k1[1], k1[2]);
}
f32 flFCVGetValueComplexShort(f32 t, FCV *c, s16 *hint) {
    s16 *k0 = 0;
    s16 *k1 = 0;

    *hint = flFCVSeekFcurveKeyComplexShort(t, c, (KEYL **)&k0, (KEYL **)&k1, *hint);
    if (k1 == 0) {
        return k0[2];
    }
    switch (*(s32 *)k0) {
    case 0x10000:
        return flFCVFcurveInterpolateLinear(t, k0[2], k0[3], k1[2], k1[3]);
    case 0x20000:
        return flFCVFcurveInterpolateHermite(t, k0[2], k0[3], k0[5], k1[2], k1[3], k1[4]);
    default:
        return 0.0f;
    }
}
s16 flFCVSeekFcurveKeyLinear(f32 t, FCV *c, KEYL **k, KEYL **n, s16 hint) {
    u8 *base;
    u8 *p;
    f32 tp;
    int num;

    base = (u8 *)(c->off + base_addr_0038A258);
    *k = 0;
    *n = 0;
    if (t <= *(f32 *)(base + 4) || (num = c->num) == 1) {
        *k = (KEYL *)base;
        *n = 0;
        return 0;
    }
    if (!(t < *(f32 *)((u8 *)(num * 8) + (int)base - 4))) {
        *k = (KEYL *)(base + (num - 1) * 8);
        *n = 0;
        return c->num - 1;
    }
    if (hint < 0) {
        hint = 0;
    } else if (hint >= num) {
        hint = num - 1;
    }
    base += hint * 8;
    p = base;
    for (;;) {
        tp = *(f32 *)(p + 4);
        if (tp == t) {
            *k = (KEYL *)p;
            *n = 0;
            return hint;
        }
        if (tp < t && t < *(f32 *)(p + 0xC)) {
            *k = (KEYL *)p;
            *n = (KEYL *)(p + 8);
            return hint;
        }
        if (t < tp) {
            p -= 8;
            hint--;
        } else {
            p += 8;
            hint++;
        }
    }
}
s16 flFCVSeekFcurveKeyHermite(f32 t, FCV *c, KEYL **k, KEYL **n, s16 hint) {
    u8 *base;
    u8 *p;
    f32 tp;
    int num;

    base = (u8 *)(c->off + base_addr_0038A258);
    *k = 0;
    *n = 0;
    if (t <= *(f32 *)(base + 4) || (num = c->num) == 1) {
        *k = (KEYL *)base;
        *n = 0;
        return 0;
    }
    if (!(t < *(f32 *)((u8 *)(num * 16) + (int)base - 12))) {
        *k = (KEYL *)(base + (num - 1) * 16);
        *n = 0;
        return c->num - 1;
    }
    if (hint < 0) {
        hint = 0;
    } else if (hint >= num) {
        hint = num - 1;
    }
    base += hint * 16;
    p = base;
    for (;;) {
        tp = *(f32 *)(p + 4);
        if (tp == t) {
            *k = (KEYL *)p;
            *n = 0;
            return hint;
        }
        if (tp < t && t < *(f32 *)(p + 20)) {
            *k = (KEYL *)p;
            *n = (KEYL *)(p + 16);
            return hint;
        }
        if (t < tp) {
            p -= 16;
            hint--;
        } else {
            p += 16;
            hint++;
        }
    }
}
s16 flFCVSeekFcurveKeyComplex(f32 t, FCV *c, KEYL **k, KEYL **n, s16 hint) {
    u8 *base;
    u8 *p;
    f32 tp;
    int num;

    base = (u8 *)(c->off + base_addr_0038A258);
    *k = 0;
    *n = 0;
    if (t <= *(f32 *)(base + 8) || (num = c->num) == 1) {
        *k = (KEYL *)base;
        *n = 0;
        return 0;
    }
    if (!(t < *(f32 *)((u8 *)(num * 20) + (int)base - 12))) {
        *k = (KEYL *)(base + (num - 1) * 20);
        *n = 0;
        return c->num - 1;
    }
    if (hint < 0) {
        hint = 0;
    } else if (hint >= num) {
        hint = num - 1;
    }
    base += hint * 20;
    p = base;
    for (;;) {
        tp = *(f32 *)(p + 8);
        if (tp == t) {
            *k = (KEYL *)p;
            *n = 0;
            return hint;
        }
        if (tp < t && t < *(f32 *)(p + 28)) {
            *k = (KEYL *)p;
            *n = (KEYL *)(p + 20);
            return hint;
        }
        if (t < tp) {
            p -= 20;
            hint--;
        } else {
            p += 20;
            hint++;
        }
    }
}
s16 flFCVSeekFcurveKeyLinearShort(f32 t, FCV *c, KEYL **k, KEYL **n, s16 hint) {
    u8 *base;
    u8 *p;
    s16 tp;
    int num;

    base = (u8 *)(c->off + base_addr_0038A258);
    *k = 0;
    *n = 0;
    if (t <= (f32)*(s16 *)(base + 2) || (num = c->num) == 1) {
        *k = (KEYL *)base;
        *n = 0;
        return 0;
    }
    if (!(t < (f32)*(s16 *)((u8 *)(num * 4) + (int)base - 2))) {
        *k = (KEYL *)(base + (num - 1) * 4);
        *n = 0;
        return c->num - 1;
    }
    if (hint < 0) {
        hint = 0;
    } else if (hint >= num) {
        hint = num - 1;
    }
    base += hint * 4;
    p = base;
    for (;;) {
        tp = *(s16 *)(p + 2);
        if ((f32)tp == t) {
            *k = (KEYL *)p;
            *n = 0;
            return hint;
        }
        if ((f32)tp < t && t < (f32)*(s16 *)(p + 6)) {
            *k = (KEYL *)p;
            *n = (KEYL *)(p + 4);
            return hint;
        }
        if (t < (f32)tp) {
            p -= 4;
            hint--;
        } else {
            p += 4;
            hint++;
        }
    }
}
s16 flFCVSeekFcurveKeyHermiteShort(f32 t, FCV *c, KEYL **k, KEYL **n, s16 hint) {
    u8 *base;
    u8 *p;
    s16 tp;
    int num;

    base = (u8 *)(c->off + base_addr_0038A258);
    *k = 0;
    *n = 0;
    if (t <= (f32)*(s16 *)(base + 2) || (num = c->num) == 1) {
        *k = (KEYL *)base;
        *n = 0;
        return 0;
    }
    if (!(t < (f32)*(s16 *)((u8 *)(num * 8) + (int)base - 6))) {
        *k = (KEYL *)(base + (num - 1) * 8);
        *n = 0;
        return c->num - 1;
    }
    if (hint < 0) {
        hint = 0;
    } else if (hint >= num) {
        hint = num - 1;
    }
    base += hint * 8;
    p = base;
    for (;;) {
        tp = *(s16 *)(p + 2);
        if ((f32)tp == t) {
            *k = (KEYL *)p;
            *n = 0;
            return hint;
        }
        if ((f32)tp < t && t < (f32)*(s16 *)(p + 10)) {
            *k = (KEYL *)p;
            *n = (KEYL *)(p + 8);
            return hint;
        }
        if (t < (f32)tp) {
            p -= 8;
            hint--;
        } else {
            p += 8;
            hint++;
        }
    }
}
s16 flFCVSeekFcurveKeyComplexShort(f32 t, FCV *c, KEYL **k, KEYL **n, s16 hint) {
    u8 *base;
    u8 *p;
    s16 tp;
    int num;

    base = (u8 *)(c->off + base_addr_0038A258);
    *k = 0;
    *n = 0;
    if (t <= (f32)*(s16 *)(base + 6) || (num = c->num) == 1) {
        *k = (KEYL *)base;
        *n = 0;
        return 0;
    }
    if (!(t < (f32)*(s16 *)((u8 *)(num * 12) + (int)base - 6))) {
        *k = (KEYL *)(base + (num - 1) * 12);
        *n = 0;
        return c->num - 1;
    }
    if (hint < 0) {
        hint = 0;
    } else if (hint >= num) {
        hint = num - 1;
    }
    base += hint * 12;
    p = base;
    for (;;) {
        tp = *(s16 *)(p + 6);
        if ((f32)tp == t) {
            *k = (KEYL *)p;
            *n = 0;
            return hint;
        }
        if ((f32)tp < t && t < (f32)*(s16 *)(p + 18)) {
            *k = (KEYL *)p;
            *n = (KEYL *)(p + 12);
            return hint;
        }
        if (t < (f32)tp) {
            p -= 12;
            hint--;
        } else {
            p += 12;
            hint++;
        }
    }
}
