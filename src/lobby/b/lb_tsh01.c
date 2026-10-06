/* lb_tsh01 - agent C 0x005C2490-0x005C261C: transSelectHandleName (handle-name select screen draw: two 16-byte frame records copied from rodata, 3-row list). */
#define flfntLocate flfntLocate_hdr
#define font_print_double font_print_double_hdr
#include "lobby_a.h"
#undef flfntLocate
#undef font_print_double
void font_print_double(int, int, int, int, char *);
extern char textLobbyTbl[];
extern char lit_337_0065EC40[];
extern char lit_312_0065EC30[];
extern char D_3C6FC8[];
typedef struct { s16 x, y; s16 pad[6]; } FM;
extern FM lit_324_00617F70;
extern FM lit_326_00617F80;
void disp_string_id(u8, u8, u8);
void disp_string_handle(u8, u8, u8);
void transSelectHandleName(u8 *arg0) {
    FM a;
    FM b;
    s32 i;

    Get_sw2(0);
    a = lit_324_00617F70;
    b = lit_326_00617F80;
    reload_tex(1, 0x14D);
    SetTextureStage(0x14D);
    SetFilterMode(1);
    flSetRenderState(0x60, 0);
    Put_2TF(textLobbyTbl + 0x28);
    DispFrameMessage(&a, 0);
    a.y = a.y + 0x58;
    DispFrameMessage(&a, 0);
    a.y = a.y + 0x58;
    DispFrameMessage(&a, 0);
    flfntSetSize(0x14, 0x14);
    font_print_double(0x32, 0x6A, 1, 5, lit_337_0065EC40);
    DispFrameMessage(&b, 0);
    font_print_double(0x34, 0x90, 1, 0, lit_312_0065EC30);
    flfntSetSize(0x18, 0x14);
    font_print_double(0x70, 0x90, 1, 0, D_3C6FC8);
    for (i = 0; i < 3; i++) {
        disp_string_id(arg0[8], i, i);
        disp_string_handle(arg0[8], i, i);
    }
    DispHelpLine();
    DispSceneTitle();
}
