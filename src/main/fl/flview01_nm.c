/* NEAR-MATCH (not linked): flmatMakeViewport (SLPM_654.95 0x001718B0, 142 of 214 instructions differ) and flPS2MakeClipViewport (0x00171BE0, 47 of 64):
 * the logic is complete (ACR viewport matrix, the viewport matrix, the flViewport* globals the clip code uses); only the instruction schedule differs. */
#include "types.h"

extern u8 flMATRIX[];
extern f32 flPS2VIEWPORT[];
extern f32 flACRVIEWPORT[];
extern f32 flPS2VIEWPROJ[];
extern f32 flACRVIEWPROJ[];
extern u8 flPs2State[];
#define PS2S(T, o) (*(T *)(flPs2State + (o)))
extern f32 flViewportCX;
extern f32 flViewportCY;
extern f32 flViewportDX;
extern f32 flViewportDY;
extern f32 flViewportDW;
extern f32 flViewportDH;
extern f32 flViewportLX;
extern f32 flViewportLY;
extern int flWidth;
extern int flHeight;

void flmatInit(f32 *);
void flmatMul(f32 *, f32 *, f32 *);
void flPS2MakeClipProjection(f32 *);
void flPS2SendRenderState_SCISSOR();
void flmatMakeViewport(f32 *, f32, f32, int, int, int, int);
void flPS2MakeClipViewport(f32, f32, int, int, int, int);

void flmatrMakeViewport(int idx, f32 n, f32 f, int x, int y, int w, int h) {
    flmatMakeViewport((f32 *)(flMATRIX + idx * 0x40), n, f, x, y, w, h);
    flPS2MakeClipProjection((f32 *)(flMATRIX + 0x800));
    flmatMul(flPS2VIEWPROJ, (f32 *)(flMATRIX + 0x800), (f32 *)(flMATRIX + 0x880));
    flmatMul(flACRVIEWPROJ, (f32 *)(flMATRIX + 0x800), flACRVIEWPORT);
    if (PS2S(int, 4) == 0) {
        y /= 2;
        h /= 2;
    }
    flPS2SendRenderState_SCISSOR(x, y, w, h, 2);
}

void flmatMakeViewport(f32 *m, f32 n, f32 f, int x, int y, int w, int h) {
    f32 fx;
    f32 fy;
    f32 d;

    fx = (f32)(w / 2);
    fy = (f32)(h / 2);
    flmatInit(flACRVIEWPORT);
    flACRVIEWPORT[0] = fx;
    flACRVIEWPORT[5] = -fy;
    flACRVIEWPORT[14] = n;
    *(int *)(flACRVIEWPORT + 15) = 0x3F800000;
    d = f - n;
    flACRVIEWPORT[10] = d;
    flACRVIEWPORT[12] = (f32)x + fx;
    flACRVIEWPORT[13] = (f32)y + fy;
    if (PS2S(int, 4) == 0) {
        y /= 2;
        h /= 2;
    }
    fx = (f32)(w / 2);
    fy = (f32)(h / 2);
    flmatInit(m);
    m[0] = fx;
    m[5] = -fy;
    m[10] = d;
    m[12] = (f32)x + fx;
    m[13] = (f32)y + fy;
    *(int *)(m + 15) = 0x3F800000;
    m[10] = m[10] * (0.5f * -PS2S(f32, 0x44));
    m[12] = m[12] + (f32)(0x800 - flWidth / 2);
    m[13] = m[13] + (f32)(0x800 - flHeight / 2);
    m[14] = 0.5f * PS2S(f32, 0x44);
    flViewportCX = m[12];
    flViewportCY = m[13];
    flViewportDX = (f32)x;
    flViewportDY = (f32)y;
    flViewportDW = (f32)w;
    flViewportDH = (f32)h;
    flViewportLX = 2048.0f - flViewportCX;
    if (flViewportLX < 0.0f) {
        flViewportLX = flViewportLX * -1.0f;
    }
    flViewportLY = 2048.0f - flViewportCY;
    flViewportLX = 2048.0f - flViewportLX;
    if (flViewportLY < 0.0f) {
        flViewportLY = flViewportLY * -1.0f;
    }
    flViewportLY = 2048.0f - flViewportLY;
    flPS2MakeClipViewport(n, f, 0, 0, (int)flViewportLX, (int)flViewportLY);
}

void flPS2MakeClipViewport(f32 n, f32 f, int x, int y, int w, int h) {
    flmatInit(flPS2VIEWPORT);
    flPS2VIEWPORT[0] = (f32)(w / 2);
    *(int *)(flPS2VIEWPORT + 15) = 0x3F800000;
    flPS2VIEWPORT[10] = f - n;
    flPS2VIEWPORT[12] = flViewportCX;
    flPS2VIEWPORT[13] = flViewportCY;
    flPS2VIEWPORT[5] = -(f32)(h / 2);
    flPS2VIEWPORT[10] = flPS2VIEWPORT[10] * (0.5f * -PS2S(f32, 0x44));
    flPS2VIEWPORT[14] = 0.5f * PS2S(f32, 0x44);
}
