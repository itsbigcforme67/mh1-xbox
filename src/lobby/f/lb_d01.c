/* lb_d01 - browser: BsInit01_LoadWait 0x005F2E80-0x005F2FEC (waits for the inner image, then sets up the first page request). Hand-written from the asm. */
#include "lobby_f.h"
extern BSSYS *bsSys;
extern s8 BsHScrlBarReq;
extern u8 BsSoftKbdReq;
extern s8 BsTtlBarReq;
extern s8 BsVScrlBarReq;
extern char FirstURL[];
extern char D_530380[];
extern char D_53038B[];
void BsTextureLoad();
void LoadInnerImage();
void SetNextURL();
void BsCsInit();
void *memset();
char *strncpy();
void BsSetRenderState();
void To_BodyMain_ReqSrc();
void BsInit01_LoadWait() {
    BsTextureLoad(3, &bsSys->x03);
    if (bsSys->x03 == 4) {
        bsSys->x03 = 0;
        LoadInnerImage();
        BsVScrlBarReq = 2;
        BsTtlBarReq = 1;
        BsHScrlBarReq = 2;
        SetNextURL(FirstURL);
        bsSys->x10 = 0;
        F(s32, bsSys, 8) = 0;
        bsSys->x2F = 0;
        bsSys->x30 = 0;
        BsCsInit();
        memset(bsSys->id2, 0, 0x11);
        memset(bsSys->id2 + 0x11, 0, 0x11);
        memset(bsSys->id1, 0, 0x11);
        memset(bsSys->id1 + 0x11, 0, 0x11);
        memset(bsSys->id3, 0, 0x11);
        memset(bsSys->id3 + 0x11, 0, 0x11);
        bsSys->x32 = 0;
        bsSys->x31 = 0;
        strncpy(bsSys->id2, D_530380, 10);
        strncpy(bsSys->id2 + 0x11, D_53038B, 8);
        bsSys->x2E = 0;
        BsSoftKbdReq = 2;
        BsSetRenderState(0x14, 0xFF000001);
        To_BodyMain_ReqSrc();
    }
}
