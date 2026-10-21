/* G online (PC): DispHelpLine, written from Monster Hunter G's code at lobby 0x5A1890 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 cw;
extern u8 helpLineTbl;
s32 Get_sw2();                               /* extern */
s32 Put_2TF();                             /* extern */
s32 SetFilterMode();                     /* extern */
s32 SetTextureStage();                   /* extern */
s32 flSetRenderState();         /* extern */
s32 flfntLocate();                      /* extern */
s32 flfntSetSize();             /* extern */
s32 font_print_sp();                    /* extern */
s32 font_set_palette();                  /* extern */
s32 reload_tex();               /* extern */
extern s16 g_lb_654CFC;
extern s16 g_lb_654CFE;
extern s32 helpLineStr;

void DispHelpLine(void) {
    s32 temp_s0;
    s32 temp_v0;
    s32 temp_v0_2;
    s32 var_v0;

    temp_s0 = Get_sw2(0) & 0xFFFF;
    reload_tex(1, 0x180);
    SetTextureStage(0x180);
    SetFilterMode(1);
    flSetRenderState(0x60, 0);
    if ((*(u8 *)((u8 *)((*(void **)&cw)) + 0xB99)) == 1) {
        var_v0 = 0xA0302000;
    } else {
        var_v0 = 0xC8302000;
    }
    (*(s32 *)((u8 *)&helpLineTbl + 0x1C)) = var_v0;
    (*(s16 *)((u8 *)&helpLineTbl + 0x14)) = g_lb_654CFC - 0x24;
    (*(s16 *)((u8 *)&helpLineTbl + 0x16)) = g_lb_654CFE - 6;
    Put_2TF(&(*(s16 *)((u8 *)&helpLineTbl + 0x14)));
    if ((*(s32 *)((u8 *)&helpLineStr + 0x4)) >= 0) {
        temp_v0 = (*(s32 *)((u8 *)&helpLineStr + 0x8)) + 1;
        (*(s32 *)((u8 *)&helpLineStr + 0x8)) = temp_v0;
        if (temp_v0 > 0) {
            temp_v0_2 = (*(s32 *)((u8 *)&helpLineStr + 0x4)) + 1;
            (*(s32 *)((u8 *)&helpLineStr + 0x8)) = 0;
            (*(s32 *)((u8 *)&helpLineStr + 0x4)) = temp_v0_2;
            if (temp_v0_2 >= 0x101) {
                (*(s32 *)((u8 *)&helpLineStr + 0x4)) = -1;
            }
        }
    }
    if (temp_s0 & 0xFFFF & 0x100) {
        (*(s32 *)((u8 *)&helpLineStr + 0x4)) = -1;
    }
    flfntSetSize(0x12, 0x12);
    font_set_palette(0);
    if (helpLineStr != 0) {
        flfntLocate(g_lb_654CFC, g_lb_654CFE);
        font_print_sp((s16) (*(s32 *)((u8 *)&helpLineStr + 0x4)), helpLineStr);
    }
}

