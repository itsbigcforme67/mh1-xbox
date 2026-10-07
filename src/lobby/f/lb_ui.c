/* lb_ui - one translation unit 0x00590D40-0x005931B0 (lbtu3). */
#define flfntLocate flfntLocate_hdr
#define DispDialogData DispDialogData_hdr
#include "lbui_proto.h"
#undef flfntLocate
#undef DispDialogData
#pragma readonly_strings on
/* sprite template for Put_2TF / Put_sprite_rotate (helpLineTbl entries, stride 0x14) */
typedef struct { s16 x; s16 y; s16 w; s16 h; u8 pad08[4]; s16 u0; s16 v0; s16 u1; s16 v1; } DLGSPR;
typedef struct { f32 f[5]; } DLGF5;
void Put_sprite_rotate();
/* dialog frame: top/bottom edge strips of 0x28 high tiles then the two rotated side strips (near-match) */
void Paint_square();
/* menu frame: filled body (Paint_square or 5-high strips) + four 20-unit edge strips + four 7x6 corners (near-match) */
void put_button_help(int a, int b, int c, u16 d);
/* sprite template for Put_2TF / Put_sprite_rotate (helpLineTbl entries, stride 0x14) */
/* dialog frame: top/bottom edge strips of 0x28 high tiles then the two rotated side strips (near-match) */
/* menu frame: filled body (Paint_square or 5-high strips) + four 20-unit edge strips + four 7x6 corners (near-match) */
/* sprite template for Put_2TF / Put_sprite_rotate (helpLineTbl entries, stride 0x14) */
/* dialog frame: top/bottom edge strips of 0x28 high tiles then the two rotated side strips (near-match) */
/* menu frame: filled body (Paint_square or 5-high strips) + four 20-unit edge strips + four 7x6 corners (near-match) */
/* sprite template for Put_2TF / Put_sprite_rotate (helpLineTbl entries, stride 0x14) */
/* dialog frame: top/bottom edge strips of 0x28 high tiles then the two rotated side strips (near-match) */
/* menu frame: filled body (Paint_square or 5-high strips) + four 20-unit edge strips + four 7x6 corners (near-match) */
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
extern int strlen();
/* sprite template for Put_2TF / Put_sprite_rotate (helpLineTbl entries, stride 0x14) */
/* dialog frame: top/bottom edge strips of 0x28 high tiles then the two rotated side strips (near-match) */
/* menu frame: filled body (Paint_square or 5-high strips) + four 20-unit edge strips + four 7x6 corners (near-match) */
void Lb_eat();
asm void lb_eat_set();
void Lb_eat_to_bell();
void Lb_eat_to_rcpt();
void Lb_eat_to_eat();
void Lb_eat_to_end();
int event_eat_rcpt();
void event_eat_trans_ot0();
void event_eat_set_msg();
void event_eat_trans_ot1();
void SetDialogData_HTML();
void SetDialogYesNo();
void SetDialogData();
asm void set_dialog_square();
void SetSceneTitle();
void SetSceneSubTitle();
void SetSceneSubTitleColor();
void SetHelpLineMsg();
void draw_dialog_square();
asm int Draw_menu_square();
void DispDialogData();
void DispSceneTitle();
void DispSceneSubTitle();
/* button help line of the plaza menus: which of the four buttons are shown for each menu / sub menu step (near-match) */

void Lb_eat()
{
    u8 *pl;

    pl = (u8 *)player_work + game_w.master * 0xA00;
    switch (lb_sys.x06) {
    case 0:
        lb_sys.x06 = 8;
        Lbc_init_network_work(pl);
        Lbc_set_prim(event_eat_trans_ot0, event_eat_trans_ot1, 0);
        lb_eat_set();
        break;
    case 1:
        lb_sys.x06 = lb_sys.x06 + 1;
        Lb_act_set(pl, 0, 0x56);
        break;
    case 3:
        switch ((s8)event_eat_rcpt(network_work)) {
        case 0:
            lb_sys.x06 = 8;
            ((LB_CW *)cw)->x35D6 = 1;
            break;
        case 1:
            lb_sys.x06 = 7;
            break;
        case 2:
            break;
        }
        break;
    case 4:
        lb_sys.x06 = 8;
        Lb_act_set(pl, 0, 0x61);
        break;
    case 5:
        if ((u8)pNet->x06 == 0) {
            set01_set2(lit_216_0065B900);
            cnWrap_SoundRequest(2);
            lb_sys.x06 = lb_sys.x06 + 1;
        } else {
            event_eat_set_msg();
            lb_sys.x06 = lb_sys.x06 + 1;
        }
        break;
    case 6:
        if (lb_sys.x76 == 0) {
            lb_sys.x06 = 0;
            lb_sys.x68 = 0;
            Lbc_init_network_work(pl);
        }
        return;
    case 7:
        if (lb_sys.x76 == 0) {
            lb_sys.x06 = 0;
            lb_sys.x68 = 0;
            Lb_Pl_act_set(pl, 0, 0x4D, 0);
            Lbc_init_network_work();
        }
        return;
    case 8:
        break;
    }
}

asm void lb_eat_set()
{
#include "lb_eat_set.inc"
}

/* button help line of the plaza menus: which of the four buttons are shown for each menu / sub menu step (near-match) */

void Lb_eat_to_bell() {
    LBS8(6) = 1;
}

void Lb_eat_to_rcpt() {
    LBS8(6) = 3;
}

void Lb_eat_to_eat() {
    LBS8(6) = 4;
}

void Lb_eat_to_end() {
    LBS8(6) = 5;
}

int event_eat_rcpt(w)
LB_NETW *w;
{
    s8 stage;
    int sw;
    s8 a;
    s8 b;
    u16 key;
    int i;
    int t;
    EATRES *p2;

    stage = game_w.stage - 0x51;
    sw = (u16)Get_sw2(0);
    switch (w->depth) {
    case 0:
        sw = (u16)sw & 0xFFFF;
        if (sw & 0x20) {
            w->depth = w->depth + 1;
            w->cur = w->menu;
            cnWrap_SoundRequest(0);
        } else if (sw & 0x40) {
            cnWrap_SoundRequest(3);
            return 1;
        } else if (sw & 0x2000) {
            if (w->menu == 0) {
                w->menu = 9;
            } else {
                w->menu = w->menu - 1;
            }
            cnWrap_SoundRequest(1);
        } else if (sw & 0x1000) {
            t = w->menu + 1;
            w->menu = t;
            if ((t & 0xFF) >= 10) {
                w->menu = 0;
            }
            cnWrap_SoundRequest(1);
        }
        break;
    case 1:
        sw = (u16)sw & 0xFFFF;
        if (sw & 0x20) {
            if (w->cur == w->menu) {
                cnWrap_SoundRequest(7);
            } else {
                w->depth = w->depth + 1;
                cnWrap_SoundRequest(0);
            }
        } else if (sw & 0x40) {
            w->depth = w->depth - 1;
            cnWrap_SoundRequest(3);
        } else if (sw & 0x2000) {
            if (w->cur == 0) {
                w->cur = 9;
            } else {
                w->cur = w->cur - 1;
            }
            cnWrap_SoundRequest(1);
        } else if (sw & 0x1000) {
            t = w->cur + 1;
            w->cur = t;
            if ((t & 0xFF) >= 10) {
                w->cur = 0;
            }
            cnWrap_SoundRequest(1);
        }
        break;
    case 2:
        sw = (u16)sw & 0xFFFF;
        if (sw & 0x20) {
            if ((u8)w->x0A == 1) {
                cnWrap_SoundRequest(3);
                return 1;
            }
            cnWrap_SoundRequest(8);
            a = eat_data_type[w->cur];
            b = eat_data_type[w->menu];
            if (b < a) {
                key = (b << 8) | a;
            } else {
                key = (a << 8) | b;
            }
            pRes = eat_result[stage];
            i = 0;
            if (pRes->key != 0xFF) {
                while (1) {
                    if (pRes->key == (key & 0xFFFF)) {
                        w->x06 = pRes->idx;
                        break;
                    }
                    i = (i + 1) & 0xFFFF;
                    if (i > 0x32) {
                        w->x06 = 0;
                        break;
                    }
                    pRes = pRes + 1;
                    if (pRes->key == 0xFF) {
                        break;
                    }
                }
            }
            t = (u8)w->x06;
            if (t == 0 || t == 0xFF) {
                eatResult = 1;
            } else {
                if ((f32)Status_add_tbl[pRes->idx].s3 + ((f32)Status_add_tbl[pRes->idx].s2 + (f32)(Status_add_tbl[pRes->idx].s0 + Status_add_tbl[pRes->idx].s1)) > 0.0f) {
                    eatResult = 2;
                } else {
                    eatResult = 0;
                }
            }
            p2 = pRes;
            *(s8 *)0x3F3603 = Status_add_tbl[p2->idx].s2;
            *(s8 *)0x3F3604 = Status_add_tbl[p2->idx].s3;
            *(s8 *)0x3F3605 = Status_add_tbl[p2->idx].s0;
            *(s16 *)0x3F3606 = Status_add_tbl[p2->idx].s1;
            return 0;
        }
        if (sw & 0x40) {
            w->depth = w->depth - 1;
            cnWrap_SoundRequest(3);
        } else if (sw & 0x3000) {
            w->x0A = (u8)w->x0A ^ 1;
            cnWrap_SoundRequest(1);
        }
        break;
    }
    return 2;
}

void event_eat_trans_ot0(a)
u8 *a;
{
    s16 x;
    s16 y;
    s16 i;

    font_set_stack_no(*(int *)(a + 0x18));
    if (LBS8(6) == 3) {
        x = pfl_menu_449[0];
        y = pfl_menu_449[1];
        flfntSetSize(0x12, 0x12);
        font_set_palette(0);
        i = 0;
        do {
            y += 0x16;
            flfntLocate(x, y);
            font_print(lit_473_0065B948, eat_data_name[i]);
            i++;
        } while (i < 10);
        switch (pNet->depth) {
        case 0:
            DispFrameList(pfl_menu_449, lit_474_0065B950, pNet->menu);
            DispFrameMessage(frame_matA_450, 0);
            font_set_palette(5);
            flfntLocate(frame_matA_450[0], frame_matA_450[1]);
            font_print(lit_475_0065B960);
            font_set_palette(3);
            y = frame_matA_450[1] + 0x16;
            x = frame_matA_450[0];
            flfntLocate(x, y);
            font_print(lit_476_0065B970);
            break;
        case 1:
            DispFrameList(pfl_menu_449, lit_474_0065B950, pNet->cur);
            DispFrameMessage(frame_matA_450, 0);
            font_set_palette(5);
            flfntLocate(frame_matA_450[0], frame_matA_450[1]);
            font_print(lit_475_0065B960);
            font_set_palette(0);
            y = frame_matA_450[1] + 0x16;
            x = frame_matA_450[0];
            flfntLocate(x, y);
            font_print(lit_473_0065B948, eat_data_name[pNet->menu]);
            DispFrameMessage(frame_matB_451, 0);
            font_set_palette(5);
            flfntLocate(frame_matB_451[0], frame_matB_451[1]);
            font_print(lit_477_0065B980);
            font_set_palette(3);
            y = frame_matB_451[1] + 0x16;
            x = frame_matB_451[0];
            flfntLocate(x, y);
            font_print(lit_476_0065B970);
            break;
        case 2:
            DispFrameList(pfl_menu_449, lit_474_0065B950, -1);
            DispFrameMessage(frame_matA_450, 0);
            font_set_palette(5);
            flfntLocate(frame_matA_450[0], frame_matA_450[1]);
            font_print(lit_475_0065B960);
            font_set_palette(0);
            y = frame_matA_450[1] + 0x16;
            x = frame_matA_450[0];
            flfntLocate(x, y);
            font_print(lit_473_0065B948, eat_data_name[pNet->menu]);
            DispFrameMessage(frame_matB_451, 0);
            font_set_palette(5);
            flfntLocate(frame_matB_451[0], frame_matB_451[1]);
            font_print(lit_477_0065B980);
            font_set_palette(0);
            y = frame_matB_451[1] + 0x16;
            x = frame_matB_451[0];
            flfntLocate(x, y);
            font_print(lit_473_0065B948, eat_data_name[pNet->cur]);
            DispFrameList(eat_command_452, 0, (u8)pNet->x0A);
            break;
        }
    }
}

/* button help line of the plaza menus: which of the four buttons are shown for each menu / sub menu step (near-match) */

void event_eat_set_msg() {
    char sp10[0x100];
    s16 v;

    set01_set2(pRes->msg);
    v = Status_add_tbl[pRes->idx].s0;
    if (v != 0) {
        if (v > 0) {
            sprintf(sp10, lit_520_0065B990);
        } else if (v < 0) {
            sprintf(sp10, lit_521_0065B9B0);
        }
        set01_set2_use_mem(sp10);
    }
    v = Status_add_tbl[pRes->idx].s1;
    if (v != 0) {
        if (v > 0) {
            sprintf(sp10, lit_522_0065B9D0);
        } else if (v < 0) {
            sprintf(sp10, lit_523_0065B9F0);
        }
        set01_set2_use_mem(sp10);
    }
    v = Status_add_tbl[pRes->idx].s2;
    if (v != 0) {
        if (v > 0) {
            sprintf(sp10, lit_524_0065BA10);
        } else if (v < 0) {
            sprintf(sp10, lit_525_0065BA30);
        }
        set01_set2_use_mem(sp10);
    }
    v = Status_add_tbl[pRes->idx].s3;
    if (v != 0) {
        if (v > 0) {
            sprintf(sp10, lit_526_0065BA50);
        } else if (v < 0) {
            sprintf(sp10, lit_527_0065BA70);
        }
        set01_set2_use_mem(sp10);
    }
}

void event_eat_trans_ot1(a)
u8 *a;
{
    font_set_stack_no(*(int *)(a + 0x18));
}

void SetDialogData_HTML(arg0)
int arg0;
{
    htmlStr = arg0;
    dialogData.html = 1;
    set_dialog_square(0x1F4, 0x17C);
}

void SetDialogYesNo(v)
s8 v;
{
    dialogData.yesno = v;
    pNet->yesno = v;
}

void SetDialogData(id, kind)
int id;
s8 kind;
{
    int lines;
    int maxw = 0;
    char *p;
    char *line;
    int w;

    if (dialogData.id != id || dialogData.html != kind) {
        dialogData.id = id;
        lines = 0;
        dialogData.msg = netDialogMessage[id];
        dialogData.html = kind;
        dialogData.x04 = 0;
        dialogData.x06 = 0;
        p = dialogData.msg;
        line = p;
        if (*p != 0) {
            do {
                p = (char *)strchr(p, 0xA);
                lines++;
                if (p != 0) {
                    w = p - line;
                    line = p;
                } else {
                    w = strlen_sp(line);
                }
                if (w < 0) {
                    w = -w;
                }
                if (maxw < w) {
                    maxw = w;
                }
                if (p == 0) {
                    break;
                }
                p++;
            } while (*p != 0);
        }
        switch (kind) {
        case 2:
        case 4:
            dialogData.yesno = 0;
        case 3:
            lines += 2;
            break;
        }
        dialogData.lines = lines;
        set_dialog_square(maxw * 10 + 0x3C, lines * 20 + 0x3C);
    }
}

asm void set_dialog_square()
{
#include "set_dialog_square.inc"
}

/* button help line of the plaza menus: which of the four buttons are shown for each menu / sub menu step (near-match) */

void SetSceneTitle(a, b)
int a;
int b;
{
    pSceneTitle = text_lobby_msg[a];
    pSceneTitle = pSceneTitle + b;
}

void SetSceneSubTitle(a, b, c)
int a;
int b;
char *c;
{
    subTitleCol = 0xFF2A0000;
    pSceneSubTitle = text_lobby_msg[a];
    pSceneSubTitle = pSceneSubTitle + b;
    strcpy(pSceneSubTitle->s, c);
}

void SetSceneSubTitleColor(c)
int c;
{
    subTitleCol = c;
}

void SetHelpLineMsg(a, b)
int a;
int b;
{
    *(u8 **)helpLineStr = (u8 *)text_lobby_msg[a];
    *(int *)(helpLineStr + 4) = 0;
    *(int *)(helpLineStr + 8) = 0;
    *(u8 **)helpLineStr = *(u8 **)helpLineStr + b * 8;
}

void draw_dialog_square(void) {
    DLGSPR sp;
    DLGSPR *t;
    s16 tw;

    Put_2TF((u8 *)helpLineTbl + 0x28);
    t = (DLGSPR *)((u8 *)helpLineTbl + 0x64);
    *(DLGF5 *)&sp = *(DLGF5 *)t;
    sp.h = 0x28;
    sp.w = 0xA;
    while (sp.y < t->y + t->h - sp.h) {
        Put_2TF(&sp);
        sp.y += (s16)(sp.h - 1);
    }
    sp.h = t->y + t->h - sp.y;
    tw = sp.h;
    sp.v1 = sp.v0 + (s16)(0.025f * (20.0f * tw));
    Put_2TF(&sp);
    t = (DLGSPR *)((u8 *)helpLineTbl + 0x78);
    *(DLGF5 *)&sp = *(DLGF5 *)t;
    sp.h = 0x28;
    sp.w = 0xA;
    while (sp.y < t->y + t->h - sp.h) {
        Put_2TF(&sp);
        sp.y += (s16)(sp.h - 1);
    }
    sp.h = t->y + t->h - sp.y;
    tw = sp.h;
    sp.v1 = sp.v0 + (s16)(0.025f * (20.0f * tw));
    Put_2TF(&sp);
    t = (DLGSPR *)((u8 *)helpLineTbl + 0x3C);
    *(DLGF5 *)&sp = *(DLGF5 *)t;
    sp.h = 8;
    sp.w = 0x33;
    sp.x -= 7;
    while (sp.x < (t->x + t->w) + 0x10 - sp.w) {
        Put_sprite_rotate(&sp, 2);
        sp.x += (s16)(sp.w - 2);
    }
    sp.w = (t->x + t->w) + 8 - sp.x;
    tw = sp.w;
    sp.v1 = sp.v0 + (s16)(0.025f * (20.0f * tw));
    Put_sprite_rotate(&sp, 2);
    t = (DLGSPR *)((u8 *)helpLineTbl + 0x50);
    *(DLGF5 *)&sp = *(DLGF5 *)t;
    sp.h = 8;
    sp.w = 0x32;
    sp.x -= 7;
    while (sp.x < (t->x + t->w) + 0x10 - sp.w) {
        Put_sprite_rotate(&sp, 2);
        sp.x += (s16)(sp.w - 2);
    }
    sp.w = (t->x + t->w) + 8 - sp.x;
    tw = sp.w;
    sp.v1 = sp.v0 + (s16)(0.025f * (20.0f * tw));
    Put_sprite_rotate(&sp, 2);
}

asm int Draw_menu_square()
{
#include "Draw_menu_square.inc"
}

void DispDialogData() {
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

/* button help line of the plaza menus: which of the four buttons are shown for each menu / sub menu step (near-match) */

void DispSceneTitle() {
    reload_tex(1, 0x157);
    SetTextureStage(0x157);
    SetFilterMode(1);
    flSetRenderState(0x60, 0);
    Put_2TF(helpLineTbl);
    if (pSceneTitle != 0) {
        flfntSetSize(0x1E, 0x1E);
        font_print_double(pSceneTitle->x, pSceneTitle->y, 1, 0, pSceneTitle->s);
    }
}

void DispSceneSubTitle() {
    flfntSetSize(0x12, 0x12);
    if (CW->x35D5 != 0 && *(s8 *)(game_w.master + (int)cw + 0x2BFE) != 0) {
        Draw_menu_square(0xD4, 0x30, 0xC0, 0x20, 0, 0);
        font_print_double(pSceneSubTitle->x, 0x38, 1, 0, pSceneSubTitle->s);
        return;
    }
    Draw_menu_square(0xD4, 0x44, 0xC0, 0x20, 1, subTitleCol);
    font_print_double(pSceneSubTitle->x, pSceneSubTitle->y, 1, 0, pSceneSubTitle->s);
}

