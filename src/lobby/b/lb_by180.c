/* lb_by180 - agent B 0x00592B50-0x00593070: DispDialogData (offline village dialog box: message lines, yes/no choices). */
#define flfntLocate flfntLocate_hdr
#define DispDialogData DispDialogData_hdr
#include "lbui.h"
#undef flfntLocate
#undef DispDialogData
void flfntLocate(s16 x, s16 y);
extern char lit_902_0065DA40[];
extern void Sel_csr_disp();
extern void nwDispStr_Html(float a, float b, float c, int s);
extern void Disp_back();
extern void Lb_put_inputMsgForHTML();
extern void draw_dialog_square();
extern int strlen();

void DispDialogData(void) {
    char buf[0x80];
    char *p;
    char *nl;
    s16 y;
    int len;
    int w;

    u16 t;
    t = dialogData.x06 + 1;
    dialogData.x06 = t;
    if (t >= 0x28) {
        dialogData.x06 = 0;
    }
    if (dialogData.html == 1 && *(u8 *)(cw + 0x2C33) != 8) {
        Disp_back();
    }
    reload_tex(1, 0x157);
    SetTextureStage(0x157);
    SetFilterMode(1);
    flSetRenderState(0x60, 0);
    draw_dialog_square();
    if (dialogData.html == 5) {
        if (dialogData.x06 >= 0x15) {
            return;
        }
    }
    flfntSetSize(0x14, 0x14);
    if (dialogData.html != 1) {
        y = *(s16 *)(helpLineTbl + 0x2A) + 0x1E;
        p = dialogData.msg;
        if (*p != 0) {
            do {
                memcpy(buf, p, 0x80);
                nl = (char *)strchr(buf, 0xA);
                if (nl != 0) {
                    *nl = 0;
                }
                len = strlen_sp(buf);
                flfntLocate(0x140 - len * 10 / 2, y);
                font_print_sp(buf);
                if (nl != 0) {
                    y += 0x14;
                    p += strlen(buf) + 1;
                } else {
                    break;
                }
            } while (*p != 0);
        }
        switch (dialogData.html) {
        case 2:
            if (dialogData.yesno == 0) {
                font_set_palette(6);
                y += 0x28;
                flfntLocate(0xFA, y);
                font_print(lit_902_0065DA40, ((int *)tl_etc)[1]);
                font_set_palette(0xA);
                flfntLocate(0x154, y);
                font_print(lit_902_0065DA40, ((int *)tl_etc)[2]);
                Sel_csr_disp(0x10E, (s16)(y - 2), 0x50, 0x18, 0xB0008000);
            } else {
                font_set_palette(0xA);
                y += 0x28;
                flfntLocate(0xFA, y);
                font_print(lit_902_0065DA40, ((int *)tl_etc)[1]);
                font_set_palette(6);
                flfntLocate(0x154, y);
                font_print(lit_902_0065DA40, ((int *)tl_etc)[2]);
                Sel_csr_disp(0x172, (s16)(y - 2), 0x64, 0x18, 0xB0008000);
            }
            break;
        case 3:
            font_set_palette(6);
            y += 0x28;
            flfntLocate(0x12C, y);
            font_print(lit_902_0065DA40, ((int *)tl_etc)[1]);
            Sel_csr_disp(0x140, (s16)(y - 2), 0x50, 0x18, 0xB0008000);
            break;
        case 4:
            if (dialogData.yesno == 0) {
                font_set_palette(6);
                y += 0x28;
                flfntLocate(0xC4, y);
                font_print(lit_902_0065DA40, ((int *)tl_etc)[6]);
                font_set_palette(0xA);
                flfntLocate(0x154, y);
                font_print(lit_902_0065DA40, ((int *)tl_etc)[7]);
                Sel_csr_disp(0xF6, (s16)(y - 2), 0x8C, 0x18, 0xB0008000);
            } else {
                font_set_palette(0xA);
                y += 0x28;
                flfntLocate(0xC4, y);
                font_print(lit_902_0065DA40, ((int *)tl_etc)[6]);
                font_set_palette(6);
                flfntLocate(0x154, y);
                font_print(lit_902_0065DA40, ((int *)tl_etc)[7]);
                Sel_csr_disp(0x1A1, (s16)(y - 2), 0xC2, 0x18, 0xB0008000);
            }
            break;
        }
    } else {
        nwDispStr_Html(100.0f, 60.0f, 1.0f, htmlStr);
        if (*(u8 *)(cw + 0x2F6E) == 0) {
            Lb_put_inputMsgForHTML();
        }
    }
}
