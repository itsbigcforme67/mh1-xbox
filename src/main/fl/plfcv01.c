/* pl library fcurve helpers (SLPM_654.95 0x00192E30-0x001930F8): plFCVSetBaseAddress, plFCVFcurveInterpolateHermite, plGetFcurveStartTime, plGetFcurveTime,
 * plGetFcurveEndTime. An fcurve is {u8 type; u8 chan; u16 count; offset to keys (base_addr_0038A308 relative)}; types 0x21-0x23 hold float keys (time,value[,slopes]),
 * 0x11-0x13 s16 keys, 0x10 / 0x20 are constants. */
#include "types.h"

extern int base_addr_0038A308;
float plGetFcurveEndTime();

void plFCVSetBaseAddress(int a) {
    base_addr_0038A308 = a;
}

/* original bytes: build/raw/plFCVFcurveInterpolateHermite.inc (config/c_rawfuncs.txt); the C below is a near-match, used by the PC build */
#ifdef __MWERKS__
asm float plFCVFcurveInterpolateHermite(float t, float v0, float t0, float s0, float v1, float t1, float s1)
{
#include "plFCVFcurveInterpolateHermite.inc"
}
#else
float plFCVFcurveInterpolateHermite(float t, float v0, float t0, float s0, float v1, float t1, float s1) {
    float d = t1 - t0;
    float nv;
    float x = t - t0;
    float inv = 1.0f / d;
    float x2 = x * x;
    float inv2 = inv * inv;
    float a = (3.0f * (x * x)) * inv2;
    float b = x2 * inv;
    float x3 = x2 * x;
    float c = inv2 * x3;
    float e = (2.0f * c) * inv;

    nv = v1;
    return (((v0 * (1.0f + (e - ((3.0f * (x * x)) * inv2)))) + (nv * ((-e) + a))) + (s0 * (x + ((c - b) - b)))) + (s1 * (c - b));
}
#endif

float plGetFcurveStartTime(u8 *f) {
    float r;
    u8 *k = (u8 *)(*(int *)(f + 4) + base_addr_0038A308);

    switch (f[0]) {
    case 0x20:
        r = 0;
        break;
    case 0x21:
        r = *(float *)(k + 4);
        break;
    case 0x22:
        r = *(float *)(k + 4);
        break;
    case 0x23:
        r = *(float *)(k + 8);
        break;
    case 0x10:
        r = 0;
        break;
    case 0x11:
        r = *(s16 *)(k + 2);
        break;
    case 0x12:
        r = *(s16 *)(k + 2);
        break;
    case 0x13:
        r = *(s16 *)(k + 6);
        break;
    }
    return r;
}

float plGetFcurveTime() {
    return plGetFcurveEndTime();
}

float plGetFcurveEndTime(u8 *f) {
    float r;
    u8 *k = (u8 *)(*(int *)(f + 4) + base_addr_0038A308);

    switch (f[0]) {
    case 0x20:
        r = 0;
        break;
    case 0x21:
        r = *(float *)(k + *(u16 *)(f + 2) * 8 - 4);
        break;
    case 0x22:
        r = *(float *)(k + *(u16 *)(f + 2) * 16 - 0xC);
        break;
    case 0x23:
        r = *(float *)(k + *(u16 *)(f + 2) * 20 - 0xC);
        break;
    case 0x10:
        r = 0;
        break;
    case 0x11:
        r = *(s16 *)(k + *(u16 *)(f + 2) * 4 - 2);
        break;
    case 0x12:
        r = *(s16 *)(k + *(u16 *)(f + 2) * 8 - 6);
        break;
    case 0x13:
        r = *(s16 *)(k + *(u16 *)(f + 2) * 12 - 6);
        break;
    }
    return r;
}
