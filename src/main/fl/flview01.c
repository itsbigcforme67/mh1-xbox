/* fl library viewport: flmatrMakeViewport (SLPM_654.95 0x001717C0-0x001718AC) builds matrix slot idx of flMATRIX for a viewport rectangle (x, y, w, h) with
 * depth range n..f (flmatMakeViewport), updates the clip projection and view-projection matrices and sends the scissor. In non-interlaced mode
 * (flPs2State word 4 == 0) y and h are halved. flmatMakeViewport / flPS2MakeClipViewport follow in flview01_nm.c. Names are guesses. */
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
