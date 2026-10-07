/* test_server_sel_disp (0x5C2CF0): server select screen draw. 20/202 differ: the original keeps text_lobby_msg in s0 and adds 0x28 to it for the second text item (we fold it into the offsets). Not built. */
#define flfntLocate flfntLocate_hdr
#define font_print_double font_print_double_hdr
#include "lobby_a.h"
#undef flfntLocate
#undef font_print_double
void font_print_double(int, int, int, int, char *);
typedef struct { s16 x, y; s16 pad[6]; } FM;
typedef struct { s16 x, y; char *s; } TI;
extern FM lit_281_00617FB0;
extern FM lit_283_00617FC0;
extern char lit_297_0065EC58[];
extern char lit_298_0065EC68[];
extern char netr_sub01_tbl[];
extern char BsLbsInfo[];
extern char bsCsvWork[];
extern u8 *text_lobby_msg[3];
extern u8 USER_x;
void cnWrap_SetFontSize(f32);
void han2zen(char *, char *);
void Lb_put_icon(int, int, int, int);
void test_server_sel_disp(u8 *arg0) {
    u8 *m;
    char sp170[0x100];
    char sp70[0x100];
    FM a;
    FM b;
    char sp20[0x30];

    a = lit_281_00617FB0;
    b = lit_283_00617FC0;
    font_set_stack_no(*(int *)(arg0 + 0x18));
    reload_tex(1, 0x154);
    SetTextureStage(0x154);
    SetFilterMode(1);
    flSetRenderState(0x60, 0);
    Put_2TF(netr_sub01_tbl);
    DispFrameMessage(&a, 0);
    DispFrameMessage(&b, 0);
    reload_tex(1, 0x157);
    SetTextureStage(0x157);
    DispSceneTitle();
    DispHelpLine();
    Put_2TF(netr_sub01_tbl + 0x14);
    Lb_put_icon(0x103, 0xAE, 0, 0xFF00FF00);
    Lb_put_icon(0x225, 0xAE, 1, 0xFF00FF00);
    cnWrap_SetFontColor(0);
    cnWrap_SetFontSize(24.0f);
    cnWrap_SetFontSize(20.0f);
    sprintf(sp20, lit_297_0065EC58, BsLbsInfo + network_work[8] * 0x102 + 0xD);
    font_print_double((s16)(0x1A4 - strlen(sp20) * 5), 0x6E, 1, 0, sp20);
    m = text_lobby_msg[0];
    font_print_double(*(s16 *)(m + 0x20), *(s16 *)(m + 0x22), 1, 0, *(char **)(m + 0x24));
    font_print_double(*(s16 *)(m + 0x20) + 10.0f * (u32)strlen(*(char **)(m + 0x24)), *(s16 *)(m + 0x22), 1, 0, bsCsvWork + network_work[8] * 0x21 + 0x2522);
    m += 0x28;
    font_print_double(*(s16 *)m, *(s16 *)(m + 2), 1, 0, *(char **)(m + 4));
    font_print_double(*(s16 *)m + 10.0f * (u32)strlen(*(char **)(m + 4)), *(s16 *)(m + 2), 1, 0, bsCsvWork + network_work[8] * 0x21 + 0x266C);
    sprintf(sp170, lit_298_0065EC68, network_work[8] + 1, USER_x);
    han2zen(sp170, sp70);
    font_print_double((s16)(b.x + 0xC), b.y, 1, 0, sp70);
}
