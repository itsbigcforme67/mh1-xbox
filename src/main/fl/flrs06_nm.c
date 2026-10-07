/* NEAR-MATCH (not linked): flPS2InitRenderState 60 of 108 instructions differ (register choice for the width/height loops: the original keeps flWidth in a0 and the constant 1 in v1). */
/* fl library: flPS2InitRenderState (SLPM_654.95 0x00177570-0x00177718) sends the initial GS render state (alpha, test, z buffer, scissor, fog colour,
 * TEX1 for both contexts), resets the FBA flag, computes the frame texture scale (screen size over the next power of two) and sets flPS2INITMATRIX
 * to the identity. */
#include "types.h"

extern int flSystemRenderOperation;
extern int flFogColor;
extern int flWidth;
extern int flHeight;
extern u32 flPs2FBA;
extern f32 flPS2FrameTexScaleX;
extern f32 flPS2FrameTexScaleY;
extern f32 flPS2INITMATRIX[];

void flInitPhaseStarted();
void flInitPhaseFinished();
int flPS2SendRenderState_ALPHA(int, int);
int flPS2SendRenderState_TEST(int, int);
int flPS2SendRenderState_ZBUF(int, int);
int flPS2SendRenderState_SCISSOR(int, int, int, int, int);
int flPS2SendRenderState_FOGCOL(u32);
int flPS2SendRenderState_TEX1(int, int);

void flPS2InitRenderState(void) {
    int w;
    int h;

    flInitPhaseStarted();
    flPS2SendRenderState_ALPHA(flSystemRenderOperation, 2);
    flPS2SendRenderState_TEST(flSystemRenderOperation, 2);
    flPS2SendRenderState_ZBUF(flSystemRenderOperation, 2);
    flPS2SendRenderState_SCISSOR(0, 0, flWidth, flHeight, 2);
    flPS2SendRenderState_FOGCOL(flFogColor);
    flPS2SendRenderState_TEX1(flSystemRenderOperation, 0);
    flPs2FBA = 0;
    flInitPhaseFinished();
    w = 0;
    if (flWidth >= 2) {
        do {
            w++;
        } while ((1 << w) < flWidth);
    }
    h = 0;
    flPS2FrameTexScaleX = (f32)flWidth / (f32)(1 << w);
    if (flHeight >= 2) {
        do {
            h++;
        } while ((1 << h) < flHeight);
    }
    flPS2INITMATRIX[1] = 0.0f;
    flPS2INITMATRIX[2] = 0.0f;
    flPS2INITMATRIX[3] = 0.0f;
    flPS2INITMATRIX[4] = 0.0f;
    flPS2INITMATRIX[0] = 1.0f;
    flPS2INITMATRIX[5] = 1.0f;
    flPS2INITMATRIX[6] = 0.0f;
    flPS2INITMATRIX[7] = 0.0f;
    flPS2INITMATRIX[8] = 0.0f;
    flPS2INITMATRIX[9] = 0.0f;
    flPS2INITMATRIX[10] = 1.0f;
    flPS2INITMATRIX[15] = 1.0f;
    flPS2INITMATRIX[11] = 0.0f;
    flPS2INITMATRIX[12] = 0.0f;
    flPS2INITMATRIX[13] = 0.0f;
    flPS2INITMATRIX[14] = 0.0f;
    flPS2FrameTexScaleY = (f32)flHeight / (f32)(1 << h);
}
