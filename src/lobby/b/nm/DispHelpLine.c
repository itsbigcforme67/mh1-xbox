#include "lobby_a.h"
extern char helpLineTbl[];
extern char helpLineStr[];
extern char helpLineStr[];
extern char helpLineStr[];
extern char helpLineStr[];
extern char helpLineStr[];
extern char helpLineStr[];
extern char helpLineStr[];
extern char helpLineStr[];
extern char helpLineStr[];
extern char helpLineStr[];
extern char helpLineStr[];
void DispHelpLine(void) {
    s32 temp_s0;
    s32 temp_v0;
    s32 temp_v0_2;

    temp_s0 = Get_sw2(0) & 0xFFFF;
    reload_tex(1, 0x157);
    SetTextureStage(0x157);
    SetFilterMode(1);
    flSetRenderState(0x60, 0);
    Put_2TF((int)&helpLineTbl + 0x14);
    if (F(s32, &helpLineStr, 4) >= 0) {
        temp_v0 = F(s32, &helpLineStr, 8) + 1;
        F(s32, &helpLineStr, 8) = temp_v0;
        if (temp_v0 > 0) {
            temp_v0_2 = F(s32, &helpLineStr, 4) + 1;
            F(s32, &helpLineStr, 8) = 0;
            F(s32, &helpLineStr, 4) = temp_v0_2;
            if (temp_v0_2 >= 0x101) {
                F(s32, &helpLineStr, 4) = -1;
            }
        }
    }
    if (temp_s0 & 0xFFFF & 0x100) {
        F(s32, &helpLineStr, 4) = -1;
    }
    flfntSetSize(0x12, 0x12);
    font_set_palette(0);
    if (F(int, &helpLineStr, 0) != 0) {
        flfntLocate(F(s16, F(int, &helpLineStr, 0), 0), F(s16, F(int, &helpLineStr, 0), 2));
        font_print2((s16) F(s32, &helpLineStr, 4), F(s32, F(int, &helpLineStr, 0), 4));
    }
}
