/* lbui - lobby.bin 0x00590D40-0x0059DB40: plaza / lobby UI (eat scene, dialogs, plaza menus, chat, mail, friends).
 * Near-match file in address order (tools/lbmerge.py ... include/lbui_proto.h lbui.h). */
#pragma readonly_strings on
#include "lbui_proto.h"

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
        break;
    case 7:
        if (lb_sys.x76 == 0) {
            lb_sys.x06 = 0;
            lb_sys.x68 = 0;
            Lb_Pl_act_set(pl, 0, 0x4D, 0);
            Lbc_init_network_work();
        }
        break;
    case 8:
        break;
    }
}

void lb_eat_set(void) {
    s8 flag[15];
    s8 stage;
    int i, tries, n, k;
    s8 *f;
    EATENT *e;
    EATENT *e2;

    stage = game_w.stage - 0x51;
    flMemset(flag, 0, 15);
    n = 0;
    tries = 0;
    do {
        f = &flag[(u16)ran_suu(1) % 15];
        if (*f == 0) {
            n++;
            *f = 1;
            if (n >= 10) {
                break;
            }
        }
        tries++;
    } while (tries < 100);
    if (n < 10) {
        for (i = 0; i < 10; i++) {
            if (flag[i] == 0) {
                n++;
                flag[i] = 1;
                if (n >= 10) {
                    break;
                }
            }
        }
    }
    e = eat_data[stage];
    e2 = e;
    k = 0;
    for (i = 0; i < 15; i++) {
        if (flag[i] != 0) {
            strcpy(eat_data_name[k], e->name);
            eat_data_type[k] = e2->type;
            k++;
        }
        e++;
        e2++;
    }
}

void Lb_eat_to_bell(void) {
    LBS8(6) = 1;
}

void Lb_eat_to_rcpt(void) {
    LBS8(6) = 3;
}

void Lb_eat_to_eat(void) {
    LBS8(6) = 4;
}

void Lb_eat_to_end(void) {
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
    int x;
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

void event_eat_set_msg(void) {
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

void set_dialog_square(w, h)
int w;
int h;
{
    int hh;
    int ww;
    int y0;
    int x0;
    int w2;
    int h8;
    u8 *t;
    u8 *t2;
    u8 *t3;

    hh = h >> 1;
    if (h < 0) {
        hh = (h + 1) >> 1;
    }
    t = (u8 *)helpLineTbl + 0x28;
    y0 = 0xE0 - hh;
    ww = w >> 1;
    if (w < 0) {
        ww = (w + 1) >> 1;
    }
    w2 = ww * 2;
    x0 = 0x140 - ww;
    *(s16 *)((u8 *)helpLineTbl + 0x28) = x0;
    *(s16 *)(t + 4) = w2;
    *(s16 *)(t + 2) = y0;
    h8 = h + 8;
    *(s16 *)(t + 6) = h;
    *(s16 *)(t + 0x16) = y0;
    *(s16 *)(t + 0x14) = x0;
    *(s16 *)(t + 0x18) = w2;
    t2 = t + 0x28;
    *(s16 *)(t2 + 2) = y0 + h;
    *(s16 *)(t + 0x28) = *(s16 *)((u8 *)helpLineTbl + 0x3C);
    *(s16 *)(t2 + 4) = *(s16 *)((u8 *)helpLineTbl + 0x40);
    *(s16 *)(t2 + 0x16) = y0;
    *(s16 *)(t2 + 0x1A) = h8;
    *(s16 *)(t2 + 0x14) = 0x138 - ww;
    t3 = t2 + 0x28;
    *(s16 *)(t3 + 2) = y0;
    *(s16 *)(t3 + 6) = h8;
    *(s16 *)(t2 + 0x28) = ww + 0x140;
}

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

/* sprite template for Put_2TF / Put_sprite_rotate (helpLineTbl entries, stride 0x14) */
typedef struct { s16 x; s16 y; s16 w; s16 h; u8 pad08[4]; s16 u0; s16 v0; s16 u1; s16 v1; } DLGSPR;
typedef struct { f32 f[5]; } DLGF5;
void Put_sprite_rotate();

/* dialog frame: top/bottom edge strips of 0x28 high tiles then the two rotated side strips (near-match) */
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
    sp.v1 = sp.v0 + (s16)(0.025f * ((f32)sp.h * 20.0f));
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
    sp.v1 = sp.v0 + (s16)(0.025f * ((f32)sp.h * 20.0f));
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
    Put_sprite_rotate(&sp, 2, tw);
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
    Put_sprite_rotate(&sp, 2, tw);
}

void Paint_square();

/* menu frame: filled body (Paint_square or 5-high strips) + four 20-unit edge strips + four 7x6 corners (near-match) */
int Draw_menu_square(arg0, arg1, w, h, mode, col)
s16 arg0;
s16 arg1;
s16 w;
s16 h;
s8 mode;
int col;
{
    DLGSPR sp;
    DLGSPR *t;
    int x6;
    int y6;
    int xr;
    int var_s0;
    int var_fp;
    int temp_s3;
    int temp_s3_2;
    int temp_s7;
    int temp_s6;
    s16 temp_s5;
    s16 temp_s2;
    s16 temp_s0;
    s16 tw;
    struct { s16 y; s16 x; } pos;

    pos.x = arg0;
    pos.y = arg1;
    reload_tex(1, 0x157);
    SetTextureStage(0x157);
    if (mode == 1) {
        var_s0 = w;
        var_fp = h;
        y6 = pos.y + 6;
        x6 = pos.x + 6;
        Paint_square(x6, y6, var_s0 - 12, var_fp - 12, col);
    } else {
        t = (DLGSPR *)((u8 *)helpLineTbl + 0x8C);
        var_s0 = w;
        var_fp = h;
        x6 = pos.x + 6;
        y6 = pos.y + 6;
        *(DLGF5 *)&sp = *(DLGF5 *)t;
        temp_s3 = var_fp + pos.y - 6;
        sp.x = x6;
        sp.y = y6;
        sp.w = var_s0 - 12;
        sp.h = 5;
        while (temp_s3 >= sp.y + sp.h) {
            Put_2TF(&sp);
            sp.y += (s16)(sp.h - 1);
        }
        tw = sp.h;
        sp.h = tw - ((sp.y + tw) - temp_s3);
        Put_2TF(&sp);
    }
    t = (DLGSPR *)((u8 *)helpLineTbl + 0xA0);
    t->x = x6;
    t->y = pos.y;
    t->w = 0x14;
    t->h = 6;
    xr = var_s0 + pos.x;
    temp_s3_2 = xr - 6;
    *(DLGF5 *)&sp = *(DLGF5 *)t;
    while (temp_s3_2 >= sp.x + sp.w) {
        Put_2TF(&sp);
        sp.x += (s16)(sp.w - 1);
    }
    tw = sp.w;
    temp_s7 = var_s0 + pos.x;
    temp_s6 = temp_s7 - 6;
    sp.w = tw - ((sp.x + tw) - temp_s6);
    sp.u1 = sp.u0 + (s16)(0.05f * (20.0f * sp.w));
    Put_2TF(&sp);
    t = (DLGSPR *)((u8 *)helpLineTbl + 0xB4);
    temp_s5 = pos.y + var_fp - 6;
    t->y = temp_s5;
    t->x = x6;
    t->w = 0x14;
    t->h = 6;
    *(DLGF5 *)&sp = *(DLGF5 *)t;
    while (temp_s6 >= sp.x + sp.w) {
        Put_2TF(&sp);
        sp.x += (s16)(sp.w - 1);
    }
    tw = sp.w;
    sp.w = tw - ((sp.x + tw) - temp_s3_2);
    sp.u1 = sp.u0 + (s16)(0.05f * (20.0f * sp.w));
    Put_2TF(&sp);
    t = (DLGSPR *)((u8 *)helpLineTbl + 0xC8);
    t->x = pos.x - 1;
    t->y = y6;
    t->w = 7;
    t->h = 0x14;
    *(DLGF5 *)&sp = *(DLGF5 *)t;
    if (sp.y + sp.h < temp_s5) {
        do {
            Put_2TF(&sp);
            sp.y += (s16)(sp.h - 1);
        } while (sp.y + sp.h < temp_s5);
    }
    temp_s2 = var_fp + pos.y - 6;
    tw = sp.h;
    sp.h = tw - ((sp.y + tw) - temp_s2);
    sp.v1 = sp.v0 + (s16)(0.05f * (20.0f * sp.h));
    Put_2TF(&sp);
    t = (DLGSPR *)((u8 *)helpLineTbl + 0xDC);
    t->x = xr - 7;
    t->y = y6;
    t->h = 0x14;
    t->w = 7;
    *(DLGF5 *)&sp = *(DLGF5 *)t;
    if (sp.y + sp.h < temp_s2) {
        do {
            Put_2TF(&sp);
            sp.y += (s16)(sp.h - 1);
        } while (sp.y + sp.h < temp_s5);
    }
    tw = sp.h;
    sp.h = tw - ((sp.y + tw) - temp_s2);
    sp.v1 = sp.v0 + (s16)(0.05f * (20.0f * sp.h));
    Put_2TF(&sp);
    t = (DLGSPR *)((u8 *)helpLineTbl + 0xF0);
    t->x = pos.x;
    t->y = pos.y;
    t->w = 7;
    t->h = 6;
    Put_2TF(t);
    t = (DLGSPR *)((u8 *)helpLineTbl + 0x104);
    temp_s0 = temp_s7 - 8;
    t->x = temp_s0;
    t->y = pos.y;
    t->w = 7;
    t->h = 6;
    Put_2TF(t);
    t = (DLGSPR *)((u8 *)helpLineTbl + 0x118);
    t->x = pos.x;
    t->y = temp_s2;
    t->w = 7;
    t->h = 6;
    Put_2TF(t);
    t = (DLGSPR *)((u8 *)helpLineTbl + 0x12C);
    t->x = temp_s0;
    t->y = temp_s2;
    t->w = 7;
    t->h = 6;
    Put_2TF(t);
}

void DispSceneTitle(void) {
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

void DispSceneSubTitle(void) {
    flfntSetSize(0x12, 0x12);
    if (CW->x35D5 != 0 && *(s8 *)(game_w.master + (int)cw + 0x2BFE) != 0) {
        Draw_menu_square(0xD4, 0x30, 0xC0, 0x20, 0, 0);
        font_print_double(pSceneSubTitle->x, 0x38, 1, 0, pSceneSubTitle->s);
        return;
    }
    Draw_menu_square(0xD4, 0x44, 0xC0, 0x20, 1, subTitleCol);
    font_print_double(pSceneSubTitle->x, pSceneSubTitle->y, 1, 0, pSceneSubTitle->s);
}

void put_button_help(int a, int b, int c, u16 d);

/* button help line of the plaza menus: which of the four buttons are shown for each menu / sub menu step (near-match) */
void DispButtonHelp(n)
LB_NETW *n;
{
    u16 pad;
    int p;

    pad = n->x28;
    if (n->x0C == 0) {
        flfntSetSize(0x12, 0x12);
        reload_tex(1, 0x157);
        SetTextureStage(0x157);
        switch (n->sel) {
        case 14:
            put_button_help(0, 0, 2, (u16)Get_sw2(0) & 0x100);
            put_button_help(1, 1, 3, (u16)Get_sw2(0) & 0x200);
            return;
        case 0:
            if (n->step != 3) {
                p = pad & 0xFFFF;
                put_button_help(1, 2, 2, p & 0x100 & 0xFFFF);
                put_button_help(3, 3, 0, p & 0x20 & 0xFFFF);
            }
            put_button_help(2, 4, 1, pad & 0xFFFF & 0x40 & 0xFFFF);
            return;
        case 1:
            p = pad & 0xFFFF;
            put_button_help(2, 4, 1, p & 0x40 & 0xFFFF);
            put_button_help(3, 3, 0, p & 0x20 & 0xFFFF);
            return;
        case 3:
            switch (n->step) {
            case 0:
            case 1:
            case 2:
            case 3:
            case 9:
                p = pad & 0xFFFF;
                put_button_help(0, 5, 6, p & 0x80 & 0xFFFF);
                put_button_help(1, 0xC, 3, p & 0x200 & 0xFFFF);
                put_button_help(2, 4, 1, p & 0x40 & 0xFFFF);
                put_button_help(3, 6, 0, p & 0x20 & 0xFFFF);
                return;
            case 4:
                p = pad & 0xFFFF;
                put_button_help(0, 5, 6, p & 0x80 & 0xFFFF);
                put_button_help(1, 0xC, 3, p & 0x200 & 0xFFFF);
                put_button_help(2, 4, 1, p & 0x40 & 0xFFFF);
                return;
            case 5:
                p = pad & 0xFFFF;
                put_button_help(2, 4, 1, p & 0x40 & 0xFFFF);
                put_button_help(3, 3, 0, p & 0x20 & 0xFFFF);
                return;
            }
            break;
        case 5:
            switch (n->step) {
            case 0:
            case 5:
            case 6:
            case 8:
            case 11:
                p = pad & 0xFFFF;
                put_button_help(1, 8, 3, p & 0x200 & 0xFFFF);
                put_button_help(2, 4, 1, p & 0x40 & 0xFFFF);
                put_button_help(3, 6, 0, p & 0x20 & 0xFFFF);
                return;
            default:
                put_button_help(3, 3, 0, pad & 0xFFFF & 0x20 & 0xFFFF);
            case 9:
            case 10:
                put_button_help(2, 4, 1, pad & 0xFFFF & 0x40 & 0xFFFF);
                return;
            }
            break;
        case 6:
            switch (n->step) {
            case 6:
            case 8:
            case 9:
            case 11:
                p = pad & 0xFFFF;
                put_button_help(1, 8, 3, p & 0x200 & 0xFFFF);
                put_button_help(2, 4, 1, p & 0x40 & 0xFFFF);
                put_button_help(3, 6, 0, p & 0x20 & 0xFFFF);
                return;
            default:
                put_button_help(3, 3, 0, pad & 0xFFFF & 0x20 & 0xFFFF);
            case 10:
                put_button_help(2, 4, 1, pad & 0xFFFF & 0x40 & 0xFFFF);
                return;
            }
            break;
        case 4:
            switch (n->step) {
            case 0:
            case 1:
                p = pad & 0xFFFF;
                put_button_help(0, 0xD, 6, p & 0x80 & 0xFFFF);
                put_button_help(2, 4, 1, p & 0x40 & 0xFFFF);
                put_button_help(3, 0xB, 0, p & 0x20 & 0xFFFF);
                return;
            case 2:
                p = pad & 0xFFFF;
                put_button_help(1, 0xA, 3, p & 0x200 & 0xFFFF);
                put_button_help(2, 4, 1, p & 0x40 & 0xFFFF);
                return;
            case 3:
                p = pad & 0xFFFF;
                put_button_help(2, 4, 1, p & 0x40 & 0xFFFF);
                put_button_help(3, 3, 0, p & 0x20 & 0xFFFF);
                return;
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
            case 12:
                p = pad & 0xFFFF;
                put_button_help(2, 4, 1, p & 0x40 & 0xFFFF);
                put_button_help(3, 3, 0, p & 0x20 & 0xFFFF);
                return;
            }
            break;
        case 7:
            put_button_help(2, 4, 1, pad & 0xFFFF & 0x40 & 0xFFFF);
            return;
        case 8:
        case 10:
            p = pad & 0xFFFF;
            put_button_help(2, 4, 1, p & 0x40 & 0xFFFF);
            put_button_help(3, 0xE, 0, p & 0x20 & 0xFFFF);
            return;
        case 9:
            if ((s32)n->step >= 3) {
                p = pad & 0xFFFF;
                put_button_help(2, 4, 1, p & 0x40 & 0xFFFF);
                put_button_help(3, 3, 0, p & 0x20 & 0xFFFF);
                return;
            }
            p = pad & 0xFFFF;
            put_button_help(1, 0xF, 2, p & 0x100 & 0xFFFF);
            put_button_help(2, 4, 1, p & 0x40 & 0xFFFF);
            put_button_help(3, 3, 0, p & 0x20 & 0xFFFF);
            return;
        case 11:
            p = pad & 0xFFFF;
            put_button_help(1, 1, 3, p & 0x200 & 0xFFFF);
            put_button_help(2, 4, 1, p & 0x40 & 0xFFFF);
            break;
        }
    }
}

void LBDisp_NowLoading2(a)
int a;
{
    switch (a) {
    case 0:
        break;
    case 1:
        SetDialogData(2, 5);
        break;
    case 2:
        SetDialogData(0, 5);
        break;
    case 3:
        SetDialogData(1, 5);
        break;
    }
    DispDialogData();
}

int Lbs_plaza(a)
LB_NETW *a;
{
    LB_TXT *t = text_lobby_msg[2];

    a->x28 = Get_sw2(0);
    switch (a->depth) {
    case 0:
        sprintf(t->s, lit_193_0065DBE8, Get_ServerName(), PlazaInfo[ClassInfo.plaza - 1].name);
        SetSceneTitle(2, 0);
        SetHelpLineMsg(2, 2);
        Lbc_set_prim(Lbs_plaza_trans, plaza_trans_ot0, plaza_trans_ot1);
        lobby_bgm_set2(0x48);
        a->cur = 2;
        a->depth++;
    case 1:
        plaza_selectMenu(a);
        break;
    case 2:
        plaza_moveMain();
        break;
    }
    return a->x10;
}

static void tl_menu_cursor_up(m)
LB_TLMENU *m;
{
    u8 *tbl = plazaMenuTbl[m->menu];
    int i;
    u8 *p;

    m->x28 = 0;
    do {
        if (m->cur == 0) {
            i = 0;
            p = tbl;
            do {
                if (p[0x24] == 2) {
                    break;
                }
                i++;
                p += 0x24;
            } while (i < 0x14);
            m->cur = i;
        } else {
            m->cur--;
        }
    } while (tbl[m->cur * 0x24] != 1);
}

static void tl_menu_cursor_down(m)
LB_TLMENU *m;
{
    u8 *tbl = plazaMenuTbl[m->menu];
    u8 t;

    for (;;) {
        if (tbl[m->cur * 0x24] == 2) {
            m->cur = 0;
        } else {
            m->cur++;
        }
        t = tbl[m->cur * 0x24];
        if (t == 1) {
            return;
        }
        if (t == 2) {
            m->cur = 0;
        }
    }
}

void plaza_selectMenu(a)
LB_NETW *a;
{
    u8 *tbl = plazaMenuTbl[a->menu];
    int sw;
    u8 *t2;
    int v;
    int off;

    v = (u16)Get_sw2(0);
    a->x28 = v;
    SetSceneSubTitle(2, 1, tl_etc[0]);
    sw = v & 0xFFFF;
    a->sel = 0xE;
    if (sw & 0x2000) {
        tl_menu_cursor_up(a);
        off = a->cur * 0x24;
        SetHelpLineMsg(2, *(u16 *)(off + (int)tbl + 2) + 2);
        cnWrap_SoundRequest(1);
        return;
    }
    if (sw & 0x1000) {
        tl_menu_cursor_down(a);
        off = a->cur * 0x24;
        SetHelpLineMsg(2, *(u16 *)(off + (int)tbl + 2) + 2);
        cnWrap_SoundRequest(1);
        return;
    }
    if (sw & 0xC00) {
        a->menu ^= 1;
        t2 = plazaMenuTbl[a->menu];
        if (t2[a->cur * 0x24] != 1) {
            tl_menu_cursor_down(a);
        }
        off = a->cur * 0x24;
        SetHelpLineMsg(2, *(u16 *)(off + (int)t2 + 2) + 2);
        cnWrap_SoundRequest(1);
        return;
    }
    if (sw & 0x20) {
        off = a->cur * 0x24;
        a->sel = *(u16 *)(off + (int)tbl + 2);
        a->depth++;
        if (a->sel != 0xC) {
            SetSceneSubTitle(2, 1, tbl + a->cur * 0x24 + 4);
        }
        SetHelpLineMsg(2, a->sel + 0x10);
        cnWrap_SoundRequest(0);
        switch (a->sel) {
        case 0:
        case 2:
        case 3:
        case 5:
        case 6:
        case 7:
        case 0xB:
            plaza_moveMain(a->sel);
            break;
        }
        a->x28 = 0;
        return;
    }
    if ((*(u16 *)0x3F3714 & 0x100) || kb_chat_in_chk() == 1) {
        a->sel = 0xE;
        cnWrap_SoundRequest(0xE);
        a->depth++;
        return;
    }
    if (sw & 0x200) {
        Name_ID_change();
    }
}

void plaza_backToServer(a)
LB_NETW *a;
{
    int sw = Get_sw2(0) & 0xFFFF;

    if (BsLbsCount > 1) {
        switch (a->step) {
        case 0:
            a->step++;
            SetDialogData(0x28, 2);
            SetDialogYesNo(1);
            break;
        case 1:
            a->x28 = Get_sw_on2(0);
            a->x0C = 1;
            switch (Lb_select()) {
            case 0:
                a->x10 = 2;
                fade_set(0xA);
                str_stop(0);
                str_stop(1);
                break;
            case 3:
                tl_exit_sub_menu(1);
                break;
            }
            break;
        }
    } else {
        switch (a->step) {
        case 0:
            a->step++;
            SetDialogData(0x14, 3);
            break;
        case 1:
            a->x0C = 1;
            if ((u16)sw & 0x20) {
                tl_exit_sub_menu(0);
            }
            break;
        }
    }
}

int getUserInfo(void) {
    switch (Lbs_SeekId()) {
    case 0:
        Lbc_RequestNetComment(CW->x2F80);
        return 0;
    case 1:
        return 1;
    default:
        return 2;
    }
}

int Lb_get_comment(a)
int a;
{
    int id = Lb_get_plID() & 0xFF;

    if (id != 0xFF) {
        memset(CW->comment[id], 0, 0x62);
        Lbc_RequestNetComment(a);
        return 1;
    }
    return 0;
}

int mail_input(a, buf)
LB_NETW *a;
int buf;
{
    s8 r;

    Get_sw(0);
    Get_kb_input();
    switch (a->x05) {
    case 0:
        a->x05++;
        SoftKeyboard_pos_set(100.0f, 0x140);
        SoftKeyboard_set(1, 0xE, 0x7E, buf);
        break;
    case 1:
        r = SoftKeyboard_move(buf, *(s16 *)0x3F3710, *(s16 *)0x3F3714);
        switch (r) {
        case 1:
        case -1:
            a->x05++;
            break;
        }
        break;
    case 2:
        SoftKeyboard_exit();
        a->x05 = 0;
        return 1;
    }
    return 0;
}

void get_friend_page_num(a)
LB_NETW *a;
{
    a->x26 = net_Check_FriendSuu(Friend_data, 0x32);
    if (a->x26 % 7 != 0) {
        a->x26 = a->x26 / 7 + 1;
        return;
    }
    a->x26 = a->x26 / 7;
    if (a->x26 == 0) {
        a->x26 = 1;
    }
}

int get_page_num(a, b)
s16 a;
s16 b;
{
    int r;

    if (a % b != 0) {
        return (s16)(a / b + 1);
    }
    r = (s16)(a / b);
    if (r == 0) {
        r = 1;
    }
    return r;
}

int my_comment_input(buf)
int buf;
{
    s8 r;

    Get_sw(0);
    Get_kb_input();
    switch (pNet->x05) {
    case 0:
        pNet->x05++;
        SoftKeyboard_pos_set(100.0f, 0x140);
        SoftKeyboard_set(1, 0xE, 0x61, buf);
        break;
    case 1:
        r = SoftKeyboard_move(buf, *(s16 *)0x3F3710, *(s16 *)0x3F3714);
        switch (r) {
        case 1:
        case -1:
            pNet->x05++;
            break;
        }
        break;
    case 2:
        SoftKeyboard_exit();
        pNet->x05 = 0;
        return 1;
    }
    return 0;
}

void plaza_checkMyStatus(void) {
    int sw = Get_sw2(0) & 0xFFFF;
    s16 *p;
    int t;
    s16 v;

    switch (pNet->step) {
    case 0:
        pNet->x24 = 0;
        pNet->step++;
        CW->x30B4 = ClassInfo.plaza;
        CW->x30B6 = ClassInfo.lobby;
        break;
    case 1:
        pNet->x28 = Get_sw_on2(0);
        t = sw & 0xFFFF;
        if (t & 0x800) {
            p = &pNet->x24;
            if (*p == 0) {
                *p = 2;
            } else {
                *p = *p - 1;
            }
            cnWrap_SoundRequest(1);
        } else if (t & 0x400) {
            v = pNet->x24 + 1;
            pNet->x24 = v;
            if (v > 2) {
                pNet->x24 = 0;
            }
            cnWrap_SoundRequest(1);
        } else if (t & 0x40) {
            tl_exit_sub_menu(0);
        }
        break;
    }
}

int plaza_setMyComment(void) {
    LB_NETW *n = pNet;
    int sw;
    int t;
    u8 *stp;

    sw = Get_sw2(0) & 0xFFFF;
    stp = &n->step;

    switch (*stp) {
    case 0:
        (*stp)++;
        memcpy(CW->comment[game_w.master], D_3C73B4, 0x62);
        break;
    case 1:
        t = sw & 0xFFFF;
        if (t & 0x20) {
            (*stp)++;
            cnWrap_SoundRequest(0);
            break;
        }
        if (t & 0x40) {
            memcpy(D_3C73B4, CW->comment[game_w.master], 0x62);
            return 3;
        }
        break;
    case 2:
        if (my_comment_input(CW->comment[game_w.master]) == 1) {
            KinshiYogo_chk(CW->comment[game_w.master]);
            memcpy(D_3C73B4, CW->comment[game_w.master], 0x62);
            pNet->step--;
        }
        break;
    }
    return 2;
}

int plaza_req_input(a, buf)
LB_NETW *a;
int buf;
{
    s8 r;

    Get_sw(0);
    switch (a->x05) {
    case 0:
        a->x05++;
        SoftKeyboard_pos_set(100.0f, 0x140);
        if (a->x04 == 1) {
            SoftKeyboard_set(0, 6, 6, buf);
        } else {
            SoftKeyboard_set(3, 0xF, 8, buf);
        }
        break;
    case 1:
        r = SoftKeyboard_move(buf, *(s16 *)0x3F3710, *(s16 *)0x3F3714);
        switch (r) {
        case 0:
            break;
        case 1:
            a->x05++;
            a->x06 = 0;
            break;
        case -1:
            a->x05++;
            a->x06 = 0;
            break;
        }
        break;
    case 2:
        SoftKeyboard_exit();
        a->x05 = 0;
        return 1;
    }
    return 0;
}

int getHandleFromID(a)
LB_NETW *a;
{
    switch (a->x04) {
    case 0:
        a->x04++;
        a->x06 = 0;
    case 1:
        a->x04++;
        strcpy(SearchCondition.s, CW->x2F80);
        SearchCondition.len = strlen(CW->x2F80);
        SearchCondition.flag = 1;
        break;
    case 2:
        switch (Lbc_ConditionSearch(&SearchCondition, 1)) {
        case 0:
            if (SearchResult[0] != 0) {
                memcpy(CW->x2F80 + 8, SearchResult + 0xC, 0x11);
                return 0;
            }
            SetDialogData(0x29, 3);
            return 1;
        case 1:
            SetDialogData(0x29, 3);
            return 1;
        }
        break;
    }
    return 2;
}

int lb_chatMemberCheck(void) {
    u8 *p;
    s16 i;
    LB_NETW *n;

    switch (pNet->x04) {
    case 0:
        p = (u8 *)chatIDList + pNet->idx * 8;
        if ((s8)p[0] != 0) {
            memcpy(CW->x2F80, p, 8);
            pNet->x04++;
            break;
        }
        return 0;
    case 1:
        switch (Lbs_SeekId()) {
        case 0:
            if (ClassInfo.plaza == CW->x30B4 && CW->x30B6 == 0) {
                n = pNet;
                chatListFlag = chatListFlag | (1 << n->idx);
            } else {
                n = pNet;
                chatListFlag = chatListFlag & ~(1 << n->idx);
            }
            i = n->idx + 1;
            n->idx = i;
            if (i < 8) {
                pNet->x04 = 0;
                break;
            }
            return 0;
        case 1:
            n = pNet;
            chatListFlag = chatListFlag & ~(1 << n->idx);
            i = n->idx + 1;
            n->idx = i;
            if (i < 8) {
                pNet->x04 = 0;
                break;
            }
            return 0;
        }
        break;
    }
    return 2;
}

int Lb_checkChatID(id)
u8 *id;
{
    s8 i;
    u8 *p = (u8 *)chatIDList;

    for (i = 0; ; ) {
        if (memcmp(p, id, 8) == 0) {
            return 1;
        }
        i++;
        p += 8;
        if (i >= 7) {
            return 0;
        }
    }
}

int Lb_addChatMember(id, handle)
u8 *id;
u8 *handle;
{
    s8 i;
    s8 j;
    u8 *p;
    s8 k;

    p = (u8 *)chatIDList;
    for (i = 0; ; ) {
        if (memcmp(p, id, 8) == 0) {
            return 1;
        }
        i = (s8)(i + 1);
        p += 8;
        if (i >= 7) {
            break;
        }
    }
    j = 0;
    p = (u8 *)chatIDList;
    for (;;) {
        if (*(s8 *)p == 0) {
            k = j;
            CW->chatmode++;
            memcpy((u8 *)chatIDList + k * 8, id, 8);
            memcpy((u8 *)chatHandleList + k * 0x10, handle, 0x10);
            return 0;
        }
        j = (s8)(j + 1);
        p += 8;
        if (j >= 7) {
            return -1;
        }
    }
}

void Lb_clearChatMember(n)
s8 n;
{
    int i = n;
    u8 *hp;
    u8 *ip;
    int j;
    u8 *hp2;
    u8 *ip2;

    ip = (u8 *)chatIDList + i * 8;
    if (*(s8 *)ip != 0) {
        CW->chatmode--;
        if (i + 1 < 8) {
            hp = (u8 *)chatHandleList + i * 0x10;
            do {
                j = (s8)i + 1;
                if (j == 7) {
                    memset(ip, 0, 8);
                    memset(hp, 0, 0x10);
                } else {
                    ip2 = (u8 *)chatIDList + j * 8;
                    memcpy(ip, ip2, 8);
                    hp2 = (u8 *)chatHandleList + j * 0x10;
                    memcpy(hp, hp2, 0x10);
                    memset(ip2, 0, 8);
                    memset(hp2, 0, 0x10);
                }
                ip += 8;
                i = (s8)(i + 1);
                hp += 0x10;
            } while (i + 1 < 8);
        }
    }
}

void Lb_clearChatID(id)
u8 *id;
{
    int i;
    u8 *p = (u8 *)chatIDList;

    for (i = 0; ; ) {
        if (memcmp(p, id, 8) == 0) {
            Lb_clearChatMember(i);
            return;
        }
        i = (s8)(i + 1);
        p += 8;
        if (i >= 7) {
            return;
        }
    }
}

void Lb_clearChatList(void) {
    s8 i = 0;
    u8 *a = (u8 *)chatIDList;
    u8 *b = (u8 *)chatHandleList;

    CW->chatmode = 0;
    do {
        memset(a, 0, 8);
        memset(b, 0, 0x10);
        i++;
        a += 8;
        b += 0x10;
    } while (i < 7);
}

void plaza_ReibunEdit(void) {
    int sw = Get_sw2(0) & 0xFFFF;

    switch (pNet->step) {
    case 0:
        Plaza_ReibunEdit_i();
        pNet->step++;
        break;
    case 1:
        pNet->x28 = Get_sw_on2(0);
        if ((u16)Plaza_ReibunEdit_mv(sw) & 0x40) {
            tl_exit_sub_menu(0);
        }
        break;
    }
}

void plaza_checkChatLog(void) {
    int sw = Get_sw2(0) & 0xFFFF;

    switch (pNet->step) {
    case 0:
        Plaza_chatlog_i();
        pNet->step++;
        break;
    case 1:
        pNet->x28 = Get_sw_on2(0);
        if ((u16)sw & 0x40) {
            Plaza_chatlog_i();
            tl_exit_sub_menu(0);
            break;
        }
        Plaza_chatlog_mv(sw);
        break;
    }
}

void plaza_logOut(a)
LB_NETW *a;
{
    int sw = Get_sw2(0) & 0xFFFF;
    int t;

    switch (a->step) {
    case 0:
        a->step++;
        SetDialogData(0x27, 2);
        SetDialogYesNo(1);
        return;
    case 1:
        t = sw & 0xFFFF;
        a->x0C = 1;
        if (t & 0x20) {
            a->step++;
            return;
        }
        if (t & 0x800) {
            if (a->yesno != 0) {
                SetDialogYesNo(0);
                cnWrap_SoundRequest(1);
                return;
            }
        } else if (t & 0x400) {
            if (a->yesno != 1) {
                SetDialogYesNo(1);
                cnWrap_SoundRequest(1);
                return;
            }
        } else {
            if (t & 0x40) {
                if (a->yesno != 1) {
                    SetDialogYesNo(1);
                    cnWrap_SoundRequest(1);
                    return;
                }
                a->step++;
                return;
            }
        }
        break;
    case 2:
        if (a->yesno == 0) {
            a->step++;
            cnWrap_SoundRequest(0);
            fade_set(1);
            return;
        }
        tl_exit_sub_menu(0);
        return;
    case 3:
        if ((Fade_busy_ck() & 0xFF) != 1) {
            To_LogOut(1);
        }
        break;
    }
}

void plaza_chatMain(a)
LB_NETW *a;
{
    int tbl = (int)plazaMenuTbl[a->menu];
    int off;

    a->x28 = Get_sw(0);
    switch (a->step) {
    case 0:
        a->step++;
        Plaza_chat_init();
        break;
    case 1:
        a->x28 = Get_sw_on2(0);
        if (Plaza_chat_move(*(u16 *)0x3F3714) == -1) {
            a->step++;
        }
        break;
    case 2:
        a->step++;
        break;
    case 3:
        tl_exit_sub_menu(1);
        off = a->cur * 0x24;
        SetHelpLineMsg(2, *(u16 *)(off + tbl + 2) + 2);
        break;
    }
}

void tl_exit_sub_menu(silent)
int silent;
{
    if (!(silent & 0xFF)) {
        cnWrap_SoundRequest(3);
    }
    SetHelpLineMsg(2, pNet->sel + 2);
    pNet->depth--;
    pNet->step = 0;
    pNet->x04 = 0;
    pNet->x05 = 0;
    pNet->x0A = 0;
    pNet->x24 = 0;
    pNet->x12 = 0;
    pNet->x28 = 0;
    pNet->x26 = 0;
    pNet->sel = 0xE;
    pNet->x0D = 1;
}

void put_plaza_menu(a)
LB_NETW *a;
{
    u8 *p;
    int y;

    y = 0x56;
    p = plazaMenuTbl[a->menu];

    flfntSetSize(0x12, 0x12);
    if (p[0] != 2) {
        do {
            if (a->depth == 2) {
                Lb_put_msg2(0x10, y, p + 4);
            } else {
                font_print_double(0x10, y, 1, 0, p + 4);
            }
            p += 0x24;
            y = (s16)(y + 0x16);
        } while (p[0] != 2);
    }
}

void put_titles(a, b, c)
int a;
int b;
int c;
{
    flfntSetSize(0x12, 0x12);
    font_print_double(a, b, 1, 5, c);
}

void put_titles2(a)
u8 *a;
{
    put_titles(*(s16 *)a, *(s16 *)(a + 2), *(int *)(a + 4));
}

void put_mainWindow(x, y)
int x;
int y;
{
    int xs = (s16)x;
    int ys;

    Draw_menu_square((s16)(xs - 6), y, 0x1A0, 0x110, 1, 0xFF2A0000);
    ys = (s16)y;
    Draw_square(x, (s16)(ys + 0x26), 0x192, 1, 0xFF602020);
    Draw_square(x, (s16)(ys + 0xEC), 0x192, 1, 0xFF602020);
}

void put_mainWindowTex(x, y)
int x;
int y;
{
    int xs = (s16)x;
    int ys;

    Draw_menu_square((s16)(xs - 6), y, 0x1A0, 0x110, 0, 0);
    ys = (s16)y;
    Draw_square(x, (s16)(ys + 0x26), 0x192, 1, 0xFF602020);
    Draw_square(x, (s16)(ys + 0xEC), 0x192, 1, 0xFF602020);
}

int Lb_get_cursor_col(void) {
    f32 a = 0.0000958738f * (f32)(u32)(u16)((System_timer & 0x3F) << 10);

    return (((s8)(int)(80.0f * flSin(a)) + 0x9F) << 24) | 0xFF00;
}

void plaza_checkMyStatusTrans(void) {
    disp_status(0xD8, 0x50, CW->x440, CW->x448, my_user_mini_data, *(s8 *)((u8 *)pNet + 0x24), 3, D_3C73B4);
}

void plaza_chatTrans(void) {
    s16 idx;

    switch (CW->chatmode) {
    case 0:
        Put_megaphone(0x1F6, 0x32, 3);
        idx = 0;
        break;
    case 1:
        Put_megaphone(0x1F6, 0x32, 1);
        idx = 1;
        break;
    default:
        Put_megaphone(0x1F6, 0x32, 2);
        idx = 2;
        break;
    }
    flfntSetSize(0x16, 0x16);
    font_print_double(0x22E, 0x33, 1, 0, tl_etc[3 + idx]);
}

void plaza_trans_ot1(a)
u8 *a;
{
    font_set_stack_no(*(int *)(a + 0x18));
    if (SoftKeyboard_alive_check() != 0) {
        DispSoftkeyboard(1);
    }
    if (pNet->x0C == 1) {
        DispDialogData(pNet->x0C);
        Lb_on_dialog();
        pNet->x0C = 0;
    }
}

void Get_PlazaName(dst)
char *dst;
{
    sprintf(dst, lit_193_0065DBE8, Get_ServerName(), PlazaInfo[ClassInfo.plaza - 1].name);
}

void Get_LobbyName(dst)
char *dst;
{
    sprintf(dst, lit_193_0065DBE8, Get_ServerName(), LobbyInfo[ClassInfo.lobby - 1].name);
}

void Lbs_load(void) {
    load_pit();
    load_texlist(*(int *)0x3876A8, 0x14D, 0);
}

void Lbc_release(void) {
    release_texture(0x118, 0x15);
}

char *GetRoomRule(void) {
    return RoomRule;
}

int Lbs_MatchStart(void) {
    cnLBS_MatchStart();
    return 1;
}

void lb_npc_effect_move(em)
void *em;
{
    (*(void (**)())(*(int *)((u8 *)em + 0x3CC) + 0xC))(em);
}
