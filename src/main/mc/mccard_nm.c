/* Memory card screens (save/load flow UI), SLPM_654.95 main 0x281BC0-0x2860D0.
 * card_w (CARDW) is the work block of the card screens. Each screen (Atld = auto load, Optsv = options save,
 * Cmsv = common save, Conld = continue load, Onsv1/Ofsv0 = online/offline save, Easysv = easy save) is a
 * step machine: CardXxx(w) switches on w->rno (set by mc_r_no_set) and calls CardXxxNN(w).
 * Results of McActResult() are stored per port in w->res[]: 0 ok, -0xFF no card, -0xFE unformatted,
 * -0xFD no file, -0xFC not enough space, -0x100 I/O error, -1 busy.
 * mc_mes_disp draws message idx of McardMsgTbl centred from line y; the numeric message ids are guesses
 * (not named). All field names are guesses. */
#include "types.h"

typedef struct CARDW {
    u8 rno;         /* 0x00 step (mc_r_no_set) */
    u8 sub;         /* 0x01 sub step */
    u8 _pad02[2];
    s16 timer;      /* 0x04 */
    u8 _pad06[0x12];
    s32 port;       /* 0x18 selected memory card port / slot */
    u8 _pad1C[0x10];
    u8 frame[4];    /* 0x2C list frame kinds (trans_card_0) */
    u8 csr_on;      /* 0x30 selection cursor shown */
    u8 sel;         /* 0x31 yes/no or slot selection */
    u8 btn_on;      /* 0x32 button icon shown */
    u8 _pad33;
    s32 op;         /* 0x34 operation (McOperationSet) */
    s32 done;       /* 0x38 finished flag */
    u16 pad;        /* 0x3C pressed buttons */
    u8 _pad3E[2];
    s32 res[2];     /* 0x40 result per port */
    u8 _pad48[4];
    s16 csr[4];     /* 0x4C cursor rect x,y,w,h */
    s16 bx;         /* 0x54 button x */
    s16 by;         /* 0x56 button y */
    s16 blink;      /* 0x58 */
    s16 blinkf;     /* 0x5A */
    s16 msg;        /* 0x5C message id */
} CARDW;

extern CARDW card_w;
extern u8 card_prim[];
extern u8 system_w[];
extern u8 edit_w[];
extern u8 *data_load_ptr;
extern u8 **McardMsgTbl[];
extern int yn_tbl[2];
extern int mc_sel_tbl[2];
extern int card_list_frame_tbl_00355430[];
extern char lit_496_00384EE8[];

void flfntSetSize();
void flfntLocate(s16, s16);
void font_set_palette();
void font_print_sp();
void font_set_stack_no();
void flSetRenderState();
void SetTrnslMode();
void se_req();
void Disp_button();
void Sel_csr_disp();
void DispFrameListA();
void disp_savesel_waku();
void Quest_price_return();
void *memset();
int strlen_sp();
int McActResult();
int McActNewChk();
int McActConChk();
void McActNewClr();
void McActCheckSet();
void McActLoadSet();
void McActSaveSet();
void McActSave0Set();
void McActFormatSet();
void card_data_init();
void user_data_clr();
void save_data_sub();
void encode_data_002814E0();
int decode_to_ck();
void PatchLoadinDNAS_Init();
int PatchLoadinDNAS_Main();

void trans_card_0();
void mc_r_no_set();
int mc_yn_ck(struct CARDW *, int, s16, u8 *);
int mc_sel_ck(struct CARDW *, s16, s16, u8 *, int);
int mc_ok_ck(struct CARDW *, s16, s16, int);
int mc_remove_ck();

s16 mc_mes_disp(int, s16, int);
void Mem_mes_disp(s16 y, int idx)
{
    mc_mes_disp(0, y, idx);
}

s16 mc_mes_disp(int x, s16 y, int idx)
{
    u8 **p;
    u8 *s;

    flfntSetSize(0x12, 0x12);
    font_set_palette(0);
    p = McardMsgTbl[idx];
    s = *p;
    if (s != 0) {
        do {
            if (*s != 0) {
                flfntLocate(0x140 - strlen_sp(s) * 0x12 / 4, y);
                font_print_sp(s);
            }
            p++;
            s = *p;
            y += 0x12;
        } while (s != 0);
    }
    return y - 0x12;
}

void mc_r_no_set(w, n)
CARDW *w;
s8 n;
{
    w->rno = n;
    w->sub = 0;
    w->blink = 15;
    w->blinkf = 1;
}

int mc_yn_ck(CARDW *w, int x, s16 y, u8 *sel)
{
    int i;
    int px;
    int *t;

    if (*sel == 0) {
        if (w->pad & 0x400) {
            *sel ^= 1;
            se_req(7, 0x16, 0);
        }
    } else if (w->pad & 0x800) {
        *sel ^= 1;
        se_req(7, 0x16, 0);
    }
    flfntSetSize(0x12, 0x12);
    i = 0;
    px = 248;
    t = yn_tbl;
    do {
        if (*sel == i) {
            font_set_palette(5);
        } else {
            font_set_palette(0);
        }
        flfntLocate(px, y);
        font_print_sp(lit_496_00384EE8, *t);
        i++;
        px += 0x5A;
        t++;
    } while (i < 2);
    w->csr_on = 1;
    if (*sel == 0) {
        w->csr[0] = 266;
    } else {
        w->csr[0] = 365;
    }
    w->csr[1] = y - 2;
    w->csr[2] = 108;
    w->csr[3] = 22;
    if (w->pad & 0x20) {
        if (*sel == 0) {
            se_req(7, 0x13, 0);
            return 0;
        }
        se_req(7, 0x14, 0);
        return 1;
    }
    if (w->pad & 0x40) {
        se_req(7, 0x14, 0);
        return 1;
    }
    return -1;
}

int mc_sel_ck(CARDW *w, s16 x, s16 y, u8 *sel, int hide)
{
    int i;
    int y0;
    s16 y1;
    int py;
    int *t;

    if (hide == 0 && (w->pad & 0x3000)) {
        *sel ^= 1;
        se_req(7, 0x16, 0);
    }
    flfntSetSize(0x12, 0x12);
    t = mc_sel_tbl;
    i = 0;
    y0 = y + 0x12;
    y1 = y0;
    py = y1;
    do {
        if (*sel == i) {
            font_set_palette(5);
        } else {
            font_set_palette(0);
        }
        flfntLocate(x, py);
        font_print_sp(lit_496_00384EE8, *t);
        i++;
        py += 0x24;
        t++;
    } while (i < 2);
    if (hide == 0) {
        w->csr_on = 1;
        w->csr[0] = x + 0x63;
        if (*sel == 0) {
            w->csr[1] = y0;
        } else {
            w->csr[1] = y1 + 0x24;
        }
        w->csr[1] -= 2;
        w->csr[2] = 306;
        w->csr[3] = 22;
        if (w->pad & 0x20) {
            se_req(7, 0x13, 0);
            return *sel;
        }
        if (w->pad & 0x40) {
            se_req(7, 0x14, 0);
            return 2;
        }
    }
    return -1;
}

int mc_ok_ck(CARDW *w, s16 x, s16 y, int kind)
{
    w->bx = x;
    w->by = y;
    w->blink--;
    if (w->blink <= 0) {
        w->blink = 15;
        w->blinkf ^= 1;
    }
    if (kind != 2) {
        w->btn_on = w->blinkf;
    } else {
        w->btn_on = 0;
    }
    if (w->pad & 0x20) {
        if (kind == 0 || kind == 2) {
            se_req(7, 0x13, 0);
        } else if (kind == 3) {
            se_req(7, 0x2E, 0);
        } else {
            se_req(7, 0x14, 0);
        }
        return 1;
    }
    return 0;
}

int mc_remove_ck(port)
int port;
{
    if (McActNewChk() != 0) {
        return 1;
    } else {
        return McActConChk(port) == 0;
    }
}

void McOperationSet(op)
u8 op;
{
    int i;
    u8 *p;

    i = 0;
    p = card_prim;
    *(u8 *)&card_w = 0;
    *((u8 *)&card_w + 1) = 0;
    card_w.done = 0;
    card_w.op = op;
    card_w.res[0] = -100;
    card_w.res[1] = -100;
    do {
        memset(p, 0, 0x20);
        i++;
        p += 0x20;
    } while (i < 2);
    system_w[0x12] = 0;
    *(void **)(card_prim + 0x14) = (void *)trans_card_0;
    if (op != 8 && op != 7 && op != 6) {
        return;
    }
    Quest_price_return(op, trans_card_0);
}

void trans_card_0(void)
{
    CARDW *w = &card_w;
    int i;
    u8 *p;

    flSetRenderState(0x60, 0);
    SetTrnslMode(4, 5);
    font_set_stack_no(4);
    i = 0;
    p = (u8 *)w;
    do {
        if (p[0x2C] != 0) {
            if (p[0x2C] == 6) {
                disp_savesel_waku(edit_w, 2, 0);
            } else if (p[0x2C] == 5) {
                disp_savesel_waku(edit_w, 0, 0);
            } else if (card_w.op == 1) {
                DispFrameListA(card_list_frame_tbl_00355430[p[0x2C]], 0, -1, 255);
            } else {
                DispFrameListA(card_list_frame_tbl_00355430[p[0x2C]], 0, -1, 128);
            }
        }
        i++;
        p++;
    } while (i < 4);
    if (w->csr_on != 0) {
        Sel_csr_disp(w->csr[0], w->csr[1], w->csr[2], w->csr[3], 0xFF20C0C0);
    }
    if (w->btn_on != 0) {
        switch (w->op) {
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
            Disp_button(1.0f, 0, w->bx, w->by, 0x11A);
            break;
        default:
            Disp_button(1.0f, 0, w->bx, w->by, 8);
            break;
        }
    }
    font_set_stack_no(0);
}

void CardAtld00(w)
CARDW *w;
{
    mc_r_no_set(w, 1);
    w->port = 0;
    w->timer = 30;
    card_data_init();
    McActSave0Set(w->port, data_load_ptr, 0);
}

void CardAtld01(w)
CARDW *w;
{
    int r;

    mc_mes_disp(100, 136, 1);
    if (w->timer <= 0) {
        r = McActResult();
        w->res[w->port] = r;
        switch (r) {
        case 0:
            mc_r_no_set(w, 2);
            return;
        case -1:
            break;
        case -253:
        case -254:
            if (w->port == 1) {
                w->done = 1;
                return;
            }
        case -252:
        case -255:
        default:
            if (w->port == 0) {
                w->port = 1;
                McActSave0Set(w->port, data_load_ptr, 0);
                return;
            }
            if (w->res[0] == -253 || w->res[0] == -254) {
                w->done = 1;
                return;
            }
            if (w->res[1] == -253 || w->res[1] == -254) {
                w->done = 1;
                return;
            }
            w->sel = 1;
            if (w->res[0] == -252 && w->res[1] == -252) {
                w->port = 0;
                mc_r_no_set(w, 5);
                w->msg = 47;
                return;
            }
            if (w->res[0] == -252) {
                w->port = 0;
                mc_r_no_set(w, 5);
                w->msg = 6;
                return;
            }
            if (w->res[1] == -252) {
                w->port = 1;
                mc_r_no_set(w, 5);
                w->msg = 6;
                return;
            }
            mc_r_no_set(w, 5);
            w->msg = 5;
            break;
        }
    }
}

void CardAtld10(w)
CARDW *w;
{
    mc_r_no_set(w, 3);
    McActLoadSet(w->port, data_load_ptr);
    mc_mes_disp(100, 136, 2);
    w->timer = 30;
}

void CardAtld11(w)
CARDW *w;
{
    int r;

    mc_mes_disp(100, 136, 2);
    if (w->timer <= 0) {
        r = McActResult();
        w->res[w->port] = r;
        switch (r) {
        case 0:
            if (decode_to_ck(w) != 0) {
                mc_r_no_set(w, 5);
                w->msg = 3;
                w->sel = 1;
                return;
            }
            mc_r_no_set(w, 6);
            w->timer = 120;
            save_data_sub(0, 0x1F);
            PatchLoadinDNAS_Init();
            return;
        case -1:
            break;
        case -253:
        case -252:
        case -255:
        default:
            mc_r_no_set(w, 5);
            w->msg = 3;
            w->sel = 1;
            break;
        }
    }
}

void CardAtld12(w)
CARDW *w;
{
    if (mc_ok_ck(w, 478, mc_mes_disp(100, 136, 4) + 0x12, 2) != 0 || w->timer <= 0) {
        mc_r_no_set(w, 7);
    }
}

void CardAtld13(w)
CARDW *w;
{
    switch (mc_yn_ck(w, 82, mc_mes_disp(100, 136, w->msg) + 0x12, &w->sel)) {
    case 1:
        w->rno = 0;
        break;
    case 0:
        mc_r_no_set(w, 7);
        break;
    }
}

void CardAtld14(w)
CARDW *w;
{
    mc_mes_disp(100, 136, 4);
    if (PatchLoadinDNAS_Main() != 0) {
        mc_r_no_set(w, 4);
    }
}

void CardAtld(w)
CARDW *w;
{
    switch (w->rno) {
    case 0:
        CardAtld00(w);
        break;
    case 1:
        CardAtld01(w);
        break;
    case 2:
        CardAtld10(w);
        break;
    case 3:
        CardAtld11(w);
        break;
    case 4:
        CardAtld12(w);
        break;
    case 5:
        CardAtld13(w);
        break;
    case 6:
        CardAtld14(w);
        break;
    case 7:
        w->done = 1;
        break;
    }
}
