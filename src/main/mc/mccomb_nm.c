/* Save data packing for the memory card, SLPM_654.95 main 0x2814E0-0x281C00:
 * encode_data / decode_data scramble the save image (header: u16 version 0x100,
 * u16 key seed, u16 checksum, u16 0x5963, then 0x8A20 u16 words XORed with a
 * key stream key = key * 0xB0 % 65363), check_sum_*, copy of the option block,
 * the patch buffer and the three character slots (0x480 bytes each) to or from
 * data_load_ptr. Layout of the image (offsets into data_load_ptr): 0x208 patch
 * (0x10000), 0x10208 patch tail (0x40), 0x10248 save time (McReadClock), 0x10250
 * options (0x10), 0x10260 slot 0..2 (0x480 each), 0x11000 options part 2
 * (0x21C), 0x1121C/0x1121E option flags. Meanings are guesses. */
#include "types.h"

extern u8 *data_load_ptr;
extern u8 option_w[];
extern u8 patch_buff[];

int ran_suu();
void *memcpy();
void *memset();
void flMemset();
void McReadClock();
void Init_reibun();
void system_w_set();
void McActInit();
void load_file_mdl();
int McActAvailSet();
int mc_copy_opt_only();
int mc_copy_patch();
int user_data_copy2();
int user_data_clr();

/* Memory card screens (save/load flow UI), SLPM_654.95 main 0x281BC0-0x2860D0.
 * card_w (CARDW) is the work block of the card screens. Each screen (Atld = auto load, Optsv = options save,
 * Cmsv = common save, Conld = continue load, Onsv1/Ofsv0 = online/offline save, Easysv = easy save) is a
 * step machine: CardXxx(w) switches on w->rno (set by mc_r_no_set) and calls CardXxxNN(w).
 * Results of McActResult() are stored per port in w->res[]: 0 ok, -0xFF no card, -0xFE unformatted,
 * -0xFD no file, -0xFC not enough space, -0x100 I/O error, -1 busy.
 * mc_mes_disp draws message idx of McardMsgTbl centred from line y; the numeric message ids are guesses
 * (not named). All field names are guesses. */

typedef struct CARDW {
    u8 rno;         /* 0x00 step (mc_r_no_set) */
    u8 sub;         /* 0x01 sub step */
    u8 x02;         /* 0x02 mode flag (Cmsv: 1 = save to a slot that may be used) */
    u8 _pad03;
    s16 timer;      /* 0x04 */
    u8 _pad06[0x12];
    s32 port;       /* 0x18 selected memory card port / slot */
    u8 _pad1C[0x10];
    u8 frame[4];    /* 0x2C list frame kinds (trans_card_0) */
    u8 csr_on;      /* 0x30 selection cursor shown */
    u8 sel;         /* 0x31 yes/no or slot selection */
    u8 btn_on;      /* 0x32 button icon shown */
    u8 x33;         /* 0x33 */
    s32 op;         /* 0x34 operation (McOperationSet) */
    s32 done;       /* 0x38 finished flag */
    u16 pad;        /* 0x3C pressed buttons */
    u16 held;       /* 0x3E held buttons */
    s32 res[2];     /* 0x40 result per port */
    u8 _pad48[4];
    s16 csr[4];     /* 0x4C cursor rect x,y,w,h */
    s16 bx;         /* 0x54 button x */
    s16 by;         /* 0x56 button y */
    s16 blink;      /* 0x58 */
    s16 blinkf;     /* 0x5A */
    s16 msg;        /* 0x5C message id */
    u16 sum_a;      /* 0x5E written by check_sum_set */
    s32 sum_b;      /* 0x60 */
    s32 sum_c;      /* 0x64 */
} CARDW;

extern CARDW card_w;
extern u8 card_prim[];
extern u8 system_w[];
extern u16 Psw[];
extern u8 ot8[4];
extern u8 edit_w[];
extern u8 option_w[];
extern u8 *data_load_ptr;
extern u8 **McardMsgTbl[];
extern int yn_tbl[2];
extern int mc_sel_tbl[2];
extern int card_list_frame_tbl_00355430[];
extern char lit_496_00384EE8[];

void add_prim2();
void McActMain();
void CardAtld(), CardOptsv(), CardEasysv(), CardCmsv(), CardConld(), CardOfsv0(), CardOnsv1();
void flfntSetSize();
void flfntLocate(s16, s16);
void font_set_palette();
void font_print_sp();
void font_set_stack_no();
void flSetRenderState();
void SetTrnslMode();
void se_req();
void Disp_button(float, int, int, int, int);
void Sel_csr_disp();
void DispFrameListA();
void disp_savesel_waku();
void disp_savesel_moji();
void func_534650();
void check_sum_set();
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
int user_data_clr();
int save_data_sub();
void encode_data_002814E0();
int decode_to_ck();
int check_sum_ck();
void Save_userdata();
int mc_copy_opt_only();
int user_data_copy2();
void McActInit();
extern u8 select_w[];
void PatchLoadinDNAS_Init();
int PatchLoadinDNAS_Main();

void trans_card_0();
static void mc_r_no_set();
int mc_yn_ck(struct CARDW *, int, s16, u8 *);
int mc_sel_ck(struct CARDW *, s16, s16, u8 *, int);
int mc_ok_ck(struct CARDW *, s16, s16, int);
int mc_remove_ck();

s16 mc_mes_disp(int, s16, int);

void encode_data_002814E0(buf)
u16 *buf;
{
    int r;
    int i;
    u16 key;
    u16 *sum;

    r = ran_suu(0);
    key = r & 0xFFFF;
    i = 0;
    *buf++ = 0x100;
    *buf++ = r;
    *buf = 0;
    sum = buf++;
    *buf++ = 0x5963;
    for (i = 0; i < 0x8A20; i++) {
        *sum = *sum + *buf;
        *buf ^= key;
        buf++;
        if ((key & 0xFFFF) == 0) {
            key = 1;
        }
        key = ((key & 0xFFFF) * 0xB0) % 65363 & 0xFFFF;
    }
}

static void decode_data(out, buf)
u16 *out;
u16 *buf;
{
    int i;
    u16 key;
    u16 stored;
    int sum;

    sum = 0;
    i = 0;
    out[8] = *buf != 0x100;
    buf++;
    key = *buf++;
    stored = *buf++;
    buf++;
    do {
        *buf ^= key;
        sum = (sum + *buf) & 0xFFFF;
        buf++;
        if ((key & 0xFFFF) == 0) {
            key = 1;
        }
        key = ((key & 0xFFFF) * 0xB0) % 65363 & 0xFFFF;
        i++;
    } while (i < 0x8A20);
    out[9] = (stored & 0xFFFF) != (sum & 0xFFFF);
}

void check_sum_set(s)
u8 *s;
{
    u8 *d = data_load_ptr;
    s32 *t = (s32 *)(d + 0x10248);

    *(u16 *)(s + 0x5E) = *(u16 *)(d + 4);
    *(s32 *)(s + 0x60) = t[0];
    *(s32 *)(s + 0x64) = t[1];
}

int check_sum_ck(s)
u8 *s;
{
    u8 *d = data_load_ptr;
    s32 *t = (s32 *)(d + 0x10248);

    if (*(u16 *)(s + 0x5E) != *(u16 *)(d + 4)) {
        return 0;
    }
    if (*(s32 *)(s + 0x60) != t[0]) {
        return 0;
    }
    return *(s32 *)(s + 0x64) == t[1];
}

static int decode_to_ck(out)
u16 *out;
{
    decode_data(out, data_load_ptr);
    if (out[8] != 0 || out[9] != 0) {
        return 1;
    }
    return 0;
}

int mc_copy_opt_only(save)
int save;
{
    u8 *d = data_load_ptr;

    if (save == 0) {
        memcpy(option_w, d + 0x10250, 0x10);
        memcpy(option_w + 0xDB0, d + 0x11000, 0x21C);
        *(s16 *)(option_w + 0xFCC) |= *(s16 *)(d + 0x1121C);
        option_w[0xFCE] = d[0x1121E];
        Init_reibun();
        system_w_set();
    } else {
        memcpy(d + 0x10250, option_w, 0x10);
        memcpy(d + 0x11000, option_w + 0xDB0, 0x21C);
        *(s16 *)(d + 0x1121C) = *(s16 *)(option_w + 0xFCC);
        d[0x1121E] = option_w[0xFCE];
    }
    return 1;
}

int mc_copy_patch(save)
int save;
{
    u8 *d = data_load_ptr;

    if (save == 0) {
        memcpy(patch_buff, d + 0x208, 0x10000);
        memcpy(patch_buff + 0x20000, d + 0x10208, 0x40);
    } else {
        memcpy(d + 0x208, patch_buff, 0x10000);
        memcpy(d + 0x10208, patch_buff + 0x20000, 0x40);
    }
    return 1;
}

int user_data_copy2(slot, save)
int slot;
int save;
{
    u8 *d = data_load_ptr;
    int off;
    u8 *a;
    u8 *b;

    if (save == 0) {
        off = (u8)slot * 0x480;
        a = option_w + off + 0x10;
        b = d + off + 0x10260;
    } else {
        off = (u8)slot * 0x480;
        b = option_w + off + 0x10;
        a = d + off + 0x10260;
    }
    memcpy(a, b, 0x480);
    if (option_w[0x10 + off] == 0) {
        return 1 << (slot & 0xFF);
    }
    return 0;
}

int user_data_clr(slot)
int slot;
{
    flMemset(option_w + (slot & 0xFF) * 0x480 + 0x10, 0, 0x480);
    return 1;
}

void User_data_init(void)
{
    user_data_clr(0);
    user_data_clr(1);
    user_data_clr(2);
}

int save_data_sub(save, mask)
int save;
int mask;
{
    int r;
    u8 *d = data_load_ptr;

    r = 0;
    if (save == 1) {
        McReadClock(d + 0x10248);
    }
    if (mask & 1) {
        mc_copy_opt_only(save);
    } else if (save == 0) {
        *(s16 *)(option_w + 0xFCC) |= *(s16 *)(d + 0x1121C);
    }
    if (mask & 2) {
        r |= user_data_copy2(0, save);
    }
    if (mask & 4) {
        r |= user_data_copy2(1, save);
    }
    if (mask & 8) {
        r |= user_data_copy2(2, save);
    }
    if (mask & 0x10) {
        mc_copy_patch(save);
    }
    return r;
}

void card_data_init(w)
u8 *w;
{
    u8 *d = data_load_ptr;

    memset(d, 0, 0x11450);
    McActInit(0);
    load_file_mdl(d + 0x12000, 0x6D3);
    *(s32 *)(w + 0x48) = McActAvailSet(d + 0x12000);
}

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

static void mc_r_no_set(w, n)
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

/* near-match, 31/116 differ (only s0-s4 allocation: original shares y's register with i) */
int mc_sel_ck(CARDW *w, s16 x, s16 y, u8 *sel, int hide)
{
    int py;
    int *t;
    s16 y1;
    int i;
    int y0;

    if (hide == 0 && (w->pad & 0x3000)) {
        *sel ^= 1;
        se_req(7, 0x16, 0);
    }
    flfntSetSize(0x12, 0x12);
    y1 = y0 = y + 0x12;
    i = 0;
    py = y1;
    t = mc_sel_tbl;
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
            w->csr[1] = (y0 = y1) + 0x24;
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
    if (McActNewChk() != 0) return 1;
    if (McActConChk(port) != 0) return 0;
    return 1;
}

void McOperationSet(op)
u8 op;
{
    int i;
    u8 *p;
    void *tc;

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
    tc = (void *)trans_card_0;
    *(void **)(card_prim + 0x14) = tc;
    switch (op) {
    case 6:
    case 7:
    case 8:
        Quest_price_return(op, tc);
        break;
    }
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
        switch (p[0x2C]) {
        default:
            if (card_w.op == 1) {
                DispFrameListA(card_list_frame_tbl_00355430[p[0x2C]], 0, -1, 255);
            } else {
                DispFrameListA(card_list_frame_tbl_00355430[p[0x2C]], 0, -1, 128);
            }
            break;
        case 5:
            disp_savesel_waku(edit_w, 0, 0);
            break;
        case 6:
            disp_savesel_waku(edit_w, 2, 0);
            break;
        case 0:
            break;
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

void CardOptsv00(w)
CARDW *w;
{
    mc_r_no_set(w, 1);
    w->sel = 1;
    w->port = 0;
}

void CardOptsv01(w)
CARDW *w;
{
    switch (mc_yn_ck(w, 203, mc_mes_disp(221, 136, 9) + 0x24, &w->sel)) {
    case 1:
        mc_r_no_set(w, 12);
        break;
    case 0:
        mc_r_no_set(w, 2);
        w->sub = 0;
        w->port = 0;
        break;
    }
}

void CardOptsv02(w)
CARDW *w;
{
    int t;

    mc_mes_disp(40, 352, 8);
    switch (w->sub) {
    case 0:
        switch (mc_sel_ck(w, 221, 136, (u8 *)&w->port, 0)) {
        case 0:
        case 1:
            w->sub++;
            w->timer = 4;
            break;
        case 2:
            mc_r_no_set(w, 1);
            break;
        }
        break;
    case 1:
        mc_sel_ck(w, 221, 136, (u8 *)&w->port, 1);
        t = w->timer - 1;
        w->timer = t;
        if ((s16)t <= 0) {
            mc_r_no_set(w, 3);
            w->timer = 30;
            user_data_clr(0);
            user_data_clr(1);
            user_data_clr(2);
            card_data_init(w);
            McActSave0Set(w->port, data_load_ptr, 0);
        }
        break;
    }
}

void CardOptsv03(w)
CARDW *w;
{
    int r;

    mc_mes_disp(40, 352, 10);
    mc_sel_ck(w, 221, 136, (u8 *)&w->port, 1);
    if (w->timer <= 0) {
        r = McActResult();
        w->res[w->port] = r;
        switch (r) {
        case 0:
            mc_r_no_set(w, 6);
            w->timer = 30;
            McActNewClr();
            McActLoadSet(w->port, data_load_ptr);
            break;
        case -1:
            break;
        case -253:
            mc_r_no_set(w, 8);
            break;
        case -252:
            mc_r_no_set(w, 11);
            w->msg = 12;
            break;
        case -255:
            mc_r_no_set(w, 11);
            w->msg = 11;
            break;
        case -254:
            mc_r_no_set(w, 4);
            w->sel = 1;
            McActNewClr();
            McActCheckSet();
            break;
        default:
            mc_r_no_set(w, 11);
            w->msg = 20;
            break;
        }
    }
}

void CardOptsv04(w)
CARDW *w;
{
    s16 y;

    mc_sel_ck(w, 221, 136, (u8 *)&w->port, 1);
    y = mc_mes_disp(40, 352, 13);
    if (mc_remove_ck(w->port) != 0) {
        mc_r_no_set(w, 11);
        w->msg = 38;
        return;
    }
    switch (mc_yn_ck(w, 22, y + 0x12, &w->sel)) {
    case 1:
        mc_r_no_set(w, 2);
        break;
    case 0:
        mc_r_no_set(w, 5);
        w->timer = 30;
        McActFormatSet(w->port);
        break;
    }
}

void CardOptsv05(w)
CARDW *w;
{
    int r;

    mc_sel_ck(w, 221, 136, (u8 *)&w->port, 1);
    mc_mes_disp(40, 352, 14);
    if (w->timer <= 0) {
        r = McActResult();
        w->res[w->port] = r;
        switch (r) {
        case 0:
            mc_r_no_set(w, 8);
            break;
        case -1:
            break;
        case -256:
        default:
            mc_r_no_set(w, 11);
            w->msg = 15;
            break;
        }
    }
}

void CardOptsv06(w)
CARDW *w;
{
    int r;

    mc_sel_ck(w, 221, 136, (u8 *)&w->port, 1);
    mc_mes_disp(40, 352, 10);
    if (w->timer <= 0) {
        r = McActResult();
        w->res[w->port] = r;
        switch (r) {
        case 0:
            if (decode_to_ck(w) != 0) {
                mc_r_no_set(w, 11);
                w->msg = 20;
                return;
            }
            mc_r_no_set(w, 7);
            w->sel = 1;
            McActCheckSet();
            return;
        case -1:
            break;
        case -253:
        case -252:
        case -255:
        default:
            mc_r_no_set(w, 11);
            w->msg = 20;
            break;
        }
    }
}

void CardOptsv07(w)
CARDW *w;
{
    s16 y;

    mc_sel_ck(w, 221, 136, (u8 *)&w->port, 1);
    y = mc_mes_disp(40, 352, 16);
    if (mc_remove_ck(w->port) != 0) {
        mc_r_no_set(w, 11);
        w->msg = 38;
        return;
    }
    switch (mc_yn_ck(w, 22, y + 0x12, &w->sel)) {
    case 1:
        mc_r_no_set(w, 2);
        break;
    case 0:
        mc_r_no_set(w, 8);
        save_data_sub(0, 14);
        break;
    }
}

void CardOptsv08(w)
CARDW *w;
{
    mc_r_no_set(w, 9);
    w->timer = 30;
    save_data_sub(1, 31);
    encode_data_002814E0(data_load_ptr);
    McActSaveSet(w->port, data_load_ptr);
    mc_sel_ck(w, 221, 136, (u8 *)&w->port, 1);
    mc_mes_disp(40, 352, 17);
}

void CardOptsv09(w)
CARDW *w;
{
    int r;

    mc_sel_ck(w, 221, 136, (u8 *)&w->port, 1);
    mc_mes_disp(40, 352, 17);
    r = McActResult();
    w->res[w->port] = r;
    switch (r) {
    case 0:
        mc_r_no_set(w, 10);
        se_req(7, 25, 0);
        break;
    case -1:
        break;
    case -253:
    case -252:
    case -255:
    default:
        mc_r_no_set(w, 11);
        w->msg = 19;
        break;
    }
}

void CardOptsv10(w)
CARDW *w;
{
    mc_sel_ck(w, 221, 136, (u8 *)&w->port, 1);
    mc_mes_disp(40, 352, 18);
    if (mc_ok_ck(w, 562, 388, 0) != 0) {
        mc_r_no_set(w, 12);
    }
}

void CardOptsv11(w)
CARDW *w;
{
    mc_sel_ck(w, 221, 136, (u8 *)&w->port, 1);
    mc_mes_disp(40, 352, w->msg);
    if (mc_ok_ck(w, 562, 388, 1) != 0) {
        mc_r_no_set(w, 2);
    }
}

void CardOptsv(w)
CARDW *w;
{
    switch (w->rno) {
    case 0:
        CardOptsv00(w);
        break;
    case 1:
        CardOptsv01(w);
        break;
    case 2:
        CardOptsv02(w);
        break;
    case 3:
        CardOptsv03(w);
        break;
    case 4:
        CardOptsv04(w);
        break;
    case 5:
        CardOptsv05(w);
        break;
    case 6:
        CardOptsv06(w);
        break;
    case 7:
        CardOptsv07(w);
        break;
    case 8:
        CardOptsv08(w);
        break;
    case 9:
        CardOptsv09(w);
        break;
    case 10:
        CardOptsv10(w);
        break;
    case 11:
        CardOptsv11(w);
        break;
    case 12:
        w->done = 1;
        break;
    }
}

void CardCmsv00(w)
CARDW *w;
{
    mc_r_no_set(w, 2);
    w->sel = 0;
    w->port = 0;
    w->x33 = 0;
    user_data_clr(0);
    user_data_clr(1);
    user_data_clr(2);
}

void CardCmsv12(w)
CARDW *w;
{
    switch (mc_yn_ck(w, 82, mc_mes_disp(40, 352, 44) + 0x12, &w->sel)) {
    case 1:
        mc_r_no_set(w, 14);
        break;
    case 0:
        mc_r_no_set(w, 1);
        break;
    }
}

void CardCmsv01(w)
CARDW *w;
{
    int t;

    mc_mes_disp(40, 352, 8);
    switch (w->sub) {
    case 0:
        switch (mc_sel_ck(w, 221, 136, (u8 *)&w->port, 0)) {
        case 0:
        case 1:
            w->sub++;
            w->timer = 4;
            break;
        case 2:
            mc_r_no_set(w, 2);
            w->sel = 0;
            break;
        }
        break;
    case 1:
        mc_sel_ck(w, 221, 136, (u8 *)&w->port, 1);
        t = w->timer - 1;
        w->timer = t;
        if ((s16)t <= 0) {
            mc_r_no_set(w, 3);
            w->timer = 30;
            user_data_clr(0);
            user_data_clr(1);
            user_data_clr(2);
            card_data_init(w);
            McActSave0Set(w->port, data_load_ptr, 0);
        }
        break;
    }
}

void CardCmsv02(w)
CARDW *w;
{
    int r;

    mc_mes_disp(40, 352, 10);
    mc_sel_ck(w, 221, 136, (u8 *)&w->port, 1);
    if (w->timer <= 0) {
        r = McActResult();
        w->res[w->port] = r;
        switch (r) {
        case 0:
            mc_r_no_set(w, 4);
            w->timer = 30;
            McActNewClr();
            McActLoadSet(w->port, data_load_ptr);
            break;
        case -1:
            break;
        case -253:
        case -254:
            mc_r_no_set(w, 5);
            w->x02 = 0;
            edit_w[1] = 0;
            McActNewClr();
            McActCheckSet();
            break;
        case -252:
            mc_r_no_set(w, 12);
            w->msg = 12;
            break;
        case -255:
            mc_r_no_set(w, 12);
            w->msg = 11;
            break;
        default:
            mc_r_no_set(w, 12);
            w->msg = 20;
            break;
        }
    }
}

void CardCmsv03(w)
CARDW *w;
{
    int r;

    mc_sel_ck(w, 221, 136, (u8 *)&w->port, 1);
    mc_mes_disp(40, 352, 28);
    if (w->timer <= 0) {
        r = McActResult();
        w->res[w->port] = r;
        switch (r) {
        case 0:
            if (decode_to_ck(w) != 0) {
                mc_r_no_set(w, 12);
                w->msg = 20;
                return;
            }
            mc_r_no_set(w, 5);
            w->x02 = 1;
            w->sel = 1;
            edit_w[1] = 0;
            save_data_sub(0, 14);
            McActCheckSet();
            return;
        case -1:
            break;
        case -255:
            mc_r_no_set(w, 12);
            w->msg = 11;
            break;
        case -253:
        case -252:
        default:
            mc_r_no_set(w, 12);
            w->msg = 20;
            break;
        }
    }
}

void CardCmsv04(w)
CARDW *w;
{
    if (w->pad & 0x2000) {
        se_req(7, 22, 0);
        edit_w[1]--;
        if ((s8)edit_w[1] < 0) {
            edit_w[1] = 2;
        }
    }
    if (w->pad & 0x1000) {
        se_req(7, 22, 0);
        edit_w[1]++;
        if (edit_w[1] > 2) {
            edit_w[1] = 0;
        }
    }
    mc_mes_disp(40, 352, 42);
    if (mc_remove_ck(w->port) != 0) {
        mc_r_no_set(w, 12);
        w->msg = 38;
        return;
    }
    if (w->pad & 0x20) {
        se_req(7, 19, 0);
        mc_r_no_set(w, 6);
        w->sel = 1;
        if (w->x02 == 1) {
            if (option_w[0x10 + edit_w[1] * 0x480] == 0) {
                w->msg = 23;
            } else {
                w->msg = 22;
            }
        } else {
            w->msg = 43;
        }
    } else if (w->pad & 0x40) {
        se_req(7, 20, 0);
        mc_r_no_set(w, 1);
    }
    w->frame[1] = 5;
    disp_savesel_moji(edit_w, 0, 0);
}

void CardCmsv05(w)
CARDW *w;
{
    s16 y;

    y = mc_mes_disp(221, 136, w->msg);
    mc_mes_disp(40, 352, 42);
    if (mc_remove_ck(w->port) != 0) {
        mc_r_no_set(w, 12);
        w->msg = 38;
        return;
    }
    switch (mc_yn_ck(w, 203, y + 0x24, &w->sel)) {
    case 1:
        mc_r_no_set(w, 5);
        break;
    case 0:
        se_req(1, 115, 0);
        switch (w->res[w->port]) {
        case 0:
        case -253:
            mc_r_no_set(w, 9);
            break;
        case -254:
            mc_r_no_set(w, 7);
            w->sel = 1;
            break;
        default:
            mc_r_no_set(w, 12);
            w->msg = 19;
            break;
        }
        break;
    }
}

void CardCmsv06(w)
CARDW *w;
{
    s16 y;

    y = mc_mes_disp(40, 352, 13);
    mc_sel_ck(w, 221, 136, (u8 *)&w->port, 1);
    if (mc_remove_ck(w->port) != 0) {
        mc_r_no_set(w, 12);
        w->msg = 38;
        return;
    }
    switch (mc_yn_ck(w, 22, y + 0x12, &w->sel)) {
    case 1:
        mc_r_no_set(w, 1);
        break;
    case 0:
        mc_r_no_set(w, 8);
        w->timer = 30;
        McActFormatSet(w->port);
        break;
    }
}

void CardCmsv07(w)
CARDW *w;
{
    int r;

    mc_mes_disp(40, 352, 14);
    mc_sel_ck(w, 221, 136, (u8 *)&w->port, 1);
    if (w->timer <= 0) {
        r = McActResult();
        w->res[w->port] = r;
        switch (r) {
        case 0:
            mc_r_no_set(w, 9);
            break;
        case -1:
            break;
        case -256:
        default:
            mc_r_no_set(w, 12);
            w->msg = 15;
            break;
        }
    }
}

void CardCmsv08(w)
CARDW *w;
{
    mc_r_no_set(w, 10);
    w->timer = 30;
    func_534650(edit_w, edit_w[1]);
    option_w[0xFCE] = edit_w[1];
    save_data_sub(1, 31);
    encode_data_002814E0(data_load_ptr);
    McActNewClr();
    McActSaveSet(w->port, data_load_ptr);
    mc_mes_disp(40, 352, 17);
    mc_sel_ck(w, 221, 136, (u8 *)&w->port, 1);
}

void CardCmsv09(w)
CARDW *w;
{
    int r;

    mc_mes_disp(40, 352, 17);
    mc_sel_ck(w, 221, 136, (u8 *)&w->port, 1);
    r = McActResult();
    w->res[w->port] = r;
    switch (r) {
    case 0:
        mc_r_no_set(w, 11);
        decode_to_ck(w);
        check_sum_set(w);
        w->x33 = 1;
        se_req(7, 25, 0);
        break;
    case -1:
        break;
    case -253:
    case -252:
    case -255:
    default:
        mc_r_no_set(w, 12);
        w->msg = 19;
        break;
    }
}

void CardCmsv10(w)
CARDW *w;
{
    mc_mes_disp(40, 352, 24);
    mc_sel_ck(w, 221, 136, (u8 *)&w->port, 1);
    if (mc_ok_ck(w, 562, 388, 3) != 0) {
        mc_r_no_set(w, 13);
    }
}

void CardCmsv11(w)
CARDW *w;
{
    mc_mes_disp(40, 352, w->msg);
    mc_sel_ck(w, 221, 136, (u8 *)&w->port, 1);
    if (mc_ok_ck(w, 562, 388, 1) != 0) {
        mc_r_no_set(w, 1);
    }
}

void CardCmsv(w)
CARDW *w;
{
    switch (w->rno) {
    case 0:
        CardCmsv00(w);
        break;
    case 2:
        CardCmsv12(w);
        break;
    case 1:
        CardCmsv01(w);
        w->frame[1] = 2;
        break;
    case 3:
        CardCmsv02(w);
        w->frame[1] = 2;
        break;
    case 4:
        CardCmsv03(w);
        w->frame[1] = 2;
        break;
    case 5:
        CardCmsv04(w);
        break;
    case 6:
        CardCmsv05(w);
        w->frame[1] = 2;
        break;
    case 7:
        CardCmsv06(w);
        w->frame[1] = 2;
        break;
    case 8:
        CardCmsv07(w);
        w->frame[1] = 2;
        break;
    case 9:
        CardCmsv08(w);
        w->frame[1] = 2;
        break;
    case 10:
        CardCmsv09(w);
        w->frame[1] = 2;
        break;
    case 11:
        CardCmsv10(w);
        w->frame[1] = 2;
        break;
    case 12:
        CardCmsv11(w);
        w->frame[1] = 2;
        break;
    case 13:
        w->done = 1;
        break;
    case 14:
    default:
        w->done = 2;
        break;
    }
}

void CardConld00(w)
CARDW *w;
{
    mc_r_no_set(w, 1);
    w->port = 0;
    w->x33 = 0;
    user_data_clr(0);
    user_data_clr(1);
    user_data_clr(2);
}

void CardConld01(w)
CARDW *w;
{
    mc_mes_disp(40, 352, 21);
    switch (w->sub) {
    case 0:
        switch (mc_sel_ck(w, 221, 136, (u8 *)&w->port, 0)) {
        case 0:
        case 1:
            w->sub++;
            w->timer = 4;
            break;
        case 2:
            mc_r_no_set(w, 7);
            break;
        }
        break;
    case 1:
        mc_sel_ck(w, 221, 136, (u8 *)&w->port, 1);
        if (w->timer <= 0) {
            mc_r_no_set(w, 2);
            w->timer = 30;
            McActInit(0);
            McActSave0Set(w->port, data_load_ptr, 0);
        }
        break;
    }
}

void CardConld02(w)
CARDW *w;
{
    int r;

    mc_mes_disp(40, 352, 10);
    mc_sel_ck(w, 221, 136, (u8 *)&w->port, 1);
    if (w->timer <= 0) {
        r = McActResult();
        w->res[w->port] = r;
        switch (r) {
        case 0:
            mc_r_no_set(w, 3);
            w->timer = 30;
            McActNewClr();
            McActLoadSet(w->port, data_load_ptr);
            break;
        case -1:
            break;
        case -253:
        case -254:
            mc_r_no_set(w, 4);
            w->msg = 25;
            break;
        case -252:
            mc_r_no_set(w, 4);
            w->msg = 25;
            break;
        case -255:
            mc_r_no_set(w, 4);
            w->msg = 35;
            break;
        default:
            mc_r_no_set(w, 4);
            w->msg = 20;
            break;
        }
    }
}

void CardConld03(w)
CARDW *w;
{
    int r;

    mc_sel_ck(w, 221, 136, (u8 *)&w->port, 1);
    mc_mes_disp(40, 352, 28);
    if (w->timer <= 0) {
        r = McActResult();
        w->res[w->port] = r;
        switch (r) {
        case 0:
            if (decode_to_ck(w) != 0) {
                mc_r_no_set(w, 4);
                w->msg = 20;
                return;
            }
            check_sum_set(w);
            if (save_data_sub(0, 31) == 7) {
                mc_r_no_set(w, 4);
                w->msg = 26;
                return;
            }
            mc_r_no_set(w, 5);
            PatchLoadinDNAS_Init();
            w->x33 = 1;
            return;
        case -1:
            break;
        case -253:
        case -252:
        case -255:
        default:
            mc_r_no_set(w, 4);
            w->msg = 20;
            break;
        }
    }
}

void CardConld04(w)
CARDW *w;
{
    mc_sel_ck(w, 221, 136, (u8 *)&w->port, 1);
    mc_mes_disp(40, 352, w->msg);
    if (mc_ok_ck(w, 562, 388, 1) != 0) {
        mc_r_no_set(w, 1);
        w->sub = 0;
    }
}

void CardConld05(w)
CARDW *w;
{
    mc_sel_ck(w, 221, 136, (u8 *)&w->port, 1);
    mc_mes_disp(40, 352, 28);
    if (PatchLoadinDNAS_Main() != 0) {
        mc_r_no_set(w, 6);
    }
}

void CardConld(w)
CARDW *w;
{
    switch (w->rno) {
    case 0:
        CardConld00(w);
        w->frame[0] = 1;
        w->frame[1] = 2;
        break;
    case 1:
        CardConld01(w);
        w->frame[0] = 1;
        w->frame[1] = 2;
        break;
    case 2:
        CardConld02(w);
        w->frame[0] = 1;
        w->frame[1] = 2;
        break;
    case 3:
        CardConld03(w);
        w->frame[0] = 1;
        w->frame[1] = 2;
        break;
    case 4:
        CardConld04(w);
        w->frame[0] = 1;
        w->frame[1] = 2;
        break;
    case 5:
        CardConld05(w);
        w->frame[0] = 1;
        w->frame[1] = 2;
        break;
    case 6:
    default:
        w->done = 1;
        break;
    case 7:
        w->done = 2;
        break;
    }
}

void CardOnsv100(w)
CARDW *w;
{
    mc_r_no_set(w, 1);
    w->timer = 30;
    card_data_init();
    McActSave0Set(w->port, data_load_ptr, 0);
}

void CardOnsv101(w)
CARDW *w;
{
    int r;

    mc_mes_disp(100, 136, 36);
    if (w->timer <= 0) {
        r = McActResult();
        w->res[w->port] = r;
        switch (r) {
        case 0:
            if (McActNewChk(w->port) != 0) {
                mc_r_no_set(w, 2);
                McActLoadSet(w->port, data_load_ptr);
            } else {
                mc_r_no_set(w, 3);
            }
            break;
        case -1:
            break;
        case -253:
        case -254:
        case -252:
        case -255:
        default:
            mc_r_no_set(w, 6);
            w->msg = 37;
            w->timer = 60;
            break;
        }
    }
}

void CardOnsv102(w)
CARDW *w;
{
    int r;

    mc_mes_disp(100, 136, 36);
    r = McActResult();
    w->res[w->port] = r;
    switch (r) {
    case 0:
        if (decode_to_ck(w) != 0) {
            mc_r_no_set(w, 6);
            w->msg = 27;
            w->timer = 60;
            break;
        }
        if (check_sum_ck(w) == 0) {
            mc_r_no_set(w, 6);
            w->msg = 37;
            w->timer = 60;
        } else {
            mc_r_no_set(w, 3);
        }
        break;
    case -1:
        break;
    case -253:
    case -254:
    case -252:
    case -255:
    default:
        mc_r_no_set(w, 6);
        w->msg = 37;
        w->timer = 60;
        break;
    }
}

void CardOnsv103(w)
CARDW *w;
{
    mc_r_no_set(w, 4);
    Save_userdata(select_w[0xB6]);
    save_data_sub(1, 31);
    encode_data_002814E0(data_load_ptr);
    McActSaveSet(w->port, data_load_ptr);
    mc_mes_disp(100, 136, 31);
}

void CardOnsv104(w)
CARDW *w;
{
    int r;

    mc_mes_disp(100, 136, 31);
    r = McActResult();
    w->res[w->port] = r;
    switch (r) {
    case 0:
        McActNewClr();
        decode_to_ck(w);
        check_sum_set(w);
        w->x33 = 1;
        mc_r_no_set(w, 5);
        w->msg = 41;
        w->timer = 120;
        se_req(7, 25, 0);
        break;
    case -1:
        break;
    default:
        mc_r_no_set(w, 6);
        w->msg = 37;
        w->timer = 60;
        break;
    }
}

void CardOnsv105(w)
CARDW *w;
{
    mc_mes_disp(100, 136, w->msg);
    if (mc_ok_ck(w, 500, 244, 0) != 0) {
        mc_r_no_set(w, 8);
    }
}

void CardOnsv106(w)
CARDW *w;
{
    mc_mes_disp(100, 136, w->msg);
    if (mc_ok_ck(w, 500, 244, 1) != 0) {
        if (w->op == 10) {
            mc_r_no_set(w, 7);
            w->sel = 0;
        } else {
            mc_r_no_set(w, 7);
            w->sel = 1;
        }
    }
}

void CardOnsv107(w)
CARDW *w;
{
    s16 y;

    if (w->op == 10) {
        y = mc_mes_disp(100, 136, 46);
        switch (mc_yn_ck(w, 203, y + 0x24, &w->sel)) {
        case 1:
            mc_r_no_set(w, 9);
            break;
        case 0:
            mc_r_no_set(w, 1);
            card_data_init();
            McActSave0Set(w->port, data_load_ptr, 0);
            break;
        }
    } else {
        if (w->op == 7 || w->op == 6) {
            y = mc_mes_disp(100, 136, 45);
        } else {
            y = mc_mes_disp(100, 136, 39);
        }
        switch (mc_yn_ck(w, 203, y + 0x24, &w->sel)) {
        case 1:
            mc_r_no_set(w, 1);
            card_data_init();
            McActSave0Set(w->port, data_load_ptr, 0);
            break;
        case 0:
            mc_r_no_set(w, 9);
            break;
        }
    }
}

void CardOnsv1(w)
CARDW *w;
{
    switch (w->rno) {
    case 0:
        CardOnsv100(w);
        system_w[0x3C] = 1;
        break;
    case 1:
        CardOnsv101(w);
        system_w[0x3C] = 1;
        break;
    case 2:
        CardOnsv102(w);
        system_w[0x3C] = 1;
        break;
    case 3:
        CardOnsv103(w);
        system_w[0x3C] = 1;
        break;
    case 4:
        CardOnsv104(w);
        system_w[0x3C] = 1;
        break;
    case 5:
        CardOnsv105(w);
        system_w[0x3C] = 0;
        break;
    case 6:
        CardOnsv106(w);
        system_w[0x3C] = 0;
        break;
    case 7:
        CardOnsv107(w);
        system_w[0x3C] = 0;
        break;
    case 8:
    default:
        system_w[0x3C] = 0;
        w->done = 1;
        break;
    case 9:
        system_w[0x3C] = 0;
        w->done = 2;
        break;
    }
}

void CardOfsv000(w)
CARDW *w;
{
    if (w->op == 9) {
        mc_r_no_set(w, 2);
        w->sel = 0;
        if (w->x33 == 0) {
            w->port = 0;
            user_data_clr(0);
            user_data_clr(1);
            user_data_clr(2);
        }
    } else if (w->x33 == 0) {
        mc_r_no_set(w, 1);
        w->sel = 0;
        w->port = 0;
        user_data_clr(0);
        user_data_clr(1);
        user_data_clr(2);
    } else {
        w->op = 10;
        w->rno = 0;
    }
}

void CardOfsv012(w)
CARDW *w;
{
    switch (mc_yn_ck(w, 82, mc_mes_disp(100, 136, 29) + 0x24, &w->sel)) {
    case 1:
        mc_r_no_set(w, 14);
        break;
    case 0:
        if (w->x33 == 0) {
            mc_r_no_set(w, 1);
            w->sel = 0;
        } else {
            w->op = 10;
            w->rno = 0;
        }
        break;
    }
}

void CardOfsv001(w)
CARDW *w;
{
    int t;

    mc_mes_disp(40, 352, 8);
    switch (w->sub) {
    case 0:
        switch (mc_sel_ck(w, 221, 136, (u8 *)&w->port, 0)) {
        case 0:
        case 1:
            w->sub++;
            w->timer = 4;
            break;
        case 2:
            mc_r_no_set(w, 2);
            w->sel = 0;
            break;
        }
        break;
    case 1:
        mc_sel_ck(w, 221, 136, (u8 *)&w->port, 1);
        t = w->timer - 1;
        w->timer = t;
        if ((s16)t <= 0) {
            mc_r_no_set(w, 3);
            w->timer = 30;
            user_data_clr(0);
            user_data_clr(1);
            user_data_clr(2);
            card_data_init(w);
            McActSave0Set(w->port, data_load_ptr, 0);
        }
        break;
    }
}

void CardOfsv002(w)
CARDW *w;
{
    int r;

    mc_mes_disp(40, 352, 10);
    mc_sel_ck(w, 221, 136, (u8 *)&w->port, 1);
    if (w->timer <= 0) {
        r = McActResult();
        w->res[w->port] = r;
        switch (r) {
        case 0:
            mc_r_no_set(w, 4);
            w->timer = 30;
            McActNewClr();
            McActLoadSet(w->port, data_load_ptr);
            break;
        case -1:
            break;
        case -253:
        case -254:
            mc_r_no_set(w, 5);
            w->x02 = 0;
            edit_w[1] = 0;
            McActNewClr();
            McActCheckSet();
            break;
        case -252:
            mc_r_no_set(w, 12);
            w->msg = 12;
            break;
        case -255:
            mc_r_no_set(w, 12);
            w->msg = 11;
            break;
        default:
            mc_r_no_set(w, 12);
            w->msg = 20;
            break;
        }
    }
}

void CardOfsv003(w)
CARDW *w;
{
    int r;

    mc_sel_ck(w, 221, 136, (u8 *)&w->port, 1);
    mc_mes_disp(40, 352, 28);
    if (w->timer <= 0) {
        r = McActResult();
        w->res[w->port] = r;
        switch (r) {
        case 0:
            if (decode_to_ck(w) != 0) {
                mc_r_no_set(w, 12);
                w->msg = 20;
                return;
            }
            mc_r_no_set(w, 5);
            w->x02 = 1;
            w->sel = 1;
            edit_w[1] = 0;
            save_data_sub(0, 30);
            McActCheckSet();
            return;
        case -1:
            break;
        case -255:
            mc_r_no_set(w, 12);
            w->msg = 11;
            break;
        case -253:
        case -252:
        default:
            mc_r_no_set(w, 12);
            w->msg = 20;
            break;
        }
    }
}

void CardOfsv004(w)
CARDW *w;
{
    if (w->pad & 0x2000) {
        se_req(7, 22, 0);
        edit_w[1]--;
        if ((s8)edit_w[1] < 0) {
            edit_w[1] = 2;
        }
    }
    if (w->pad & 0x1000) {
        se_req(7, 22, 0);
        edit_w[1]++;
        if (edit_w[1] > 2) {
            edit_w[1] = 0;
        }
    }
    mc_mes_disp(40, 352, 42);
    if (mc_remove_ck(w->port) != 0) {
        mc_r_no_set(w, 12);
        w->msg = 38;
        return;
    }
    if (w->pad & 0x20) {
        se_req(7, 19, 0);
        mc_r_no_set(w, 6);
        w->sel = 1;
        if (w->x02 == 1) {
            if (option_w[0x10 + edit_w[1] * 0x480] == 0) {
                w->msg = 23;
            } else {
                w->msg = 22;
            }
        } else {
            w->msg = 43;
        }
    } else if (w->pad & 0x40) {
        se_req(7, 20, 0);
        mc_r_no_set(w, 1);
    }
    w->frame[1] = 6;
    disp_savesel_moji(edit_w, 2, 0);
}

void CardOfsv005(w)
CARDW *w;
{
    s16 y;

    y = mc_mes_disp(221, 136, w->msg);
    mc_mes_disp(40, 352, 42);
    if (mc_remove_ck(w->port) != 0) {
        mc_r_no_set(w, 12);
        w->msg = 38;
        return;
    }
    switch (mc_yn_ck(w, 203, y + 0x24, &w->sel)) {
    case 1:
        mc_r_no_set(w, 5);
        break;
    case 0:
        switch (w->res[w->port]) {
        case 0:
        case -253:
            mc_r_no_set(w, 9);
            break;
        case -254:
            mc_r_no_set(w, 7);
            w->sel = 1;
            break;
        default:
            mc_r_no_set(w, 12);
            w->msg = 19;
            break;
        }
        break;
    }
}

void CardOfsv006(w)
CARDW *w;
{
    s16 y;

    y = mc_mes_disp(40, 352, 13);
    mc_sel_ck(w, 221, 136, (u8 *)&w->port, 1);
    if (mc_remove_ck(w->port) != 0) {
        mc_r_no_set(w, 12);
        w->msg = 38;
        return;
    }
    switch (mc_yn_ck(w, 22, y + 0x12, &w->sel)) {
    case 1:
        mc_r_no_set(w, 1);
        break;
    case 0:
        mc_r_no_set(w, 8);
        w->timer = 30;
        McActFormatSet(w->port);
        break;
    }
}

void CardOfsv007(w)
CARDW *w;
{
    int r;

    mc_mes_disp(40, 352, 14);
    mc_sel_ck(w, 221, 136, (u8 *)&w->port, 1);
    if (w->timer <= 0) {
        r = McActResult();
        w->res[w->port] = r;
        switch (r) {
        case 0:
            mc_r_no_set(w, 9);
            break;
        case -1:
            break;
        case -256:
        default:
            mc_r_no_set(w, 12);
            w->msg = 15;
            break;
        }
    }
}

void CardOfsv008(w)
CARDW *w;
{
    mc_r_no_set(w, 10);
    w->timer = 30;
    Save_userdata(edit_w[1]);
    option_w[0xFCE] = edit_w[1];
    select_w[0xB6] = edit_w[1];
    save_data_sub(1, 31);
    encode_data_002814E0(data_load_ptr);
    McActSaveSet(w->port, data_load_ptr);
    mc_mes_disp(40, 352, 17);
    mc_sel_ck(w, 221, 136, (u8 *)&w->port, 1);
}

void CardOfsv009(w)
CARDW *w;
{
    int r;

    mc_mes_disp(40, 352, 17);
    mc_sel_ck(w, 221, 136, (u8 *)&w->port, 1);
    r = McActResult();
    w->res[w->port] = r;
    switch (r) {
    case 0:
        mc_r_no_set(w, 11);
        decode_to_ck(w);
        check_sum_set(w);
        w->x33 = 1;
        se_req(7, 25, 0);
        break;
    case -1:
        break;
    case -253:
    case -252:
    case -255:
    default:
        mc_r_no_set(w, 12);
        w->msg = 19;
        break;
    }
}

void CardOfsv010(w)
CARDW *w;
{
    mc_sel_ck(w, 221, 136, (u8 *)&w->port, 1);
    mc_mes_disp(40, 352, 24);
    if (mc_ok_ck(w, 562, 388, 0) != 0) {
        mc_r_no_set(w, 13);
    }
}

void CardOfsv011(w)
CARDW *w;
{
    mc_sel_ck(w, 221, 136, (u8 *)&w->port, 1);
    mc_mes_disp(40, 352, w->msg);
    if (mc_ok_ck(w, 562, 388, 1) != 0) {
        mc_r_no_set(w, 1);
    }
}

void CardOfsv0(w)
CARDW *w;
{
    switch (w->rno) {
    case 0:
        CardOfsv000(w);
        break;
    case 2:
        CardOfsv012(w);
        w->frame[1] = 3;
        break;
    case 1:
        CardOfsv001(w);
        w->frame[0] = 4;
        w->frame[1] = 3;
        break;
    case 3:
        CardOfsv002(w);
        w->frame[0] = 4;
        w->frame[1] = 3;
        break;
    case 4:
        CardOfsv003(w);
        w->frame[0] = 4;
        w->frame[1] = 3;
        break;
    case 5:
        CardOfsv004(w);
        w->frame[0] = 4;
        break;
    case 6:
        CardOfsv005(w);
        w->frame[0] = 4;
        w->frame[1] = 3;
        break;
    case 7:
        CardOfsv006(w);
        w->frame[0] = 4;
        w->frame[1] = 3;
        break;
    case 8:
        CardOfsv007(w);
        w->frame[0] = 4;
        w->frame[1] = 3;
        break;
    case 9:
        CardOfsv008(w);
        w->frame[0] = 4;
        w->frame[1] = 3;
        break;
    case 10:
        CardOfsv009(w);
        w->frame[0] = 4;
        w->frame[1] = 3;
        break;
    case 11:
        CardOfsv010(w);
        w->frame[0] = 4;
        w->frame[1] = 3;
        break;
    case 12:
        CardOfsv011(w);
        w->frame[0] = 4;
        w->frame[1] = 3;
        break;
    case 13:
        w->done = 1;
        break;
    case 14:
    default:
        w->done = 2;
        break;
    }
}

void CardEasysv00(w)
CARDW *w;
{
    mc_r_no_set(w, 1);
    w->port = 0;
    card_data_init();
    McActSave0Set(w->port, data_load_ptr, 0);
}

void CardEasysv01(w)
CARDW *w;
{
    int r;

    mc_mes_disp(40, 352, 10);
    r = McActResult();
    w->res[w->port] = r;
    switch (r) {
    case 0:
        mc_r_no_set(w, 2);
        mc_copy_opt_only(1);
        user_data_copy2(0, 1);
        user_data_copy2(1, 1);
        user_data_copy2(2, 1);
        encode_data_002814E0(data_load_ptr);
        McActSaveSet(w->port, data_load_ptr);
        break;
    case -1:
        break;
    case -253:
    case -254:
    case -252:
    case -255:
    default:
        mc_r_no_set(w, 3);
        w->msg = 19;
        w->timer = 120;
        break;
    }
}

void CardEasysv02(w)
CARDW *w;
{
    int r;

    mc_mes_disp(40, 352, 17);
    r = McActResult();
    w->res[w->port] = r;
    switch (r) {
    case 0:
        mc_r_no_set(w, 3);
        w->msg = 18;
        w->timer = 120;
        break;
    case -1:
        break;
    default:
        mc_r_no_set(w, 3);
        w->msg = 19;
        w->timer = 120;
        break;
    }
}

void CardEasysv03(w)
CARDW *w;
{
    mc_mes_disp(40, 352, w->msg);
    if (mc_ok_ck(w, 562, 388, 0) != 0 || w->timer <= 0) {
        mc_r_no_set(w, 4);
    }
}

void CardEasysv(w)
CARDW *w;
{
    switch (w->rno) {
    case 0:
        CardEasysv00(w);
        w->frame[1] = 2;
        break;
    case 1:
        CardEasysv01(w);
        break;
    case 2:
        CardEasysv02(w);
        break;
    case 3:
        CardEasysv03(w);
        break;
    case 4:
    default:
        w->done = 1;
        break;
    }
}

u8 McCardOperation(void)
{
    CARDW *w = &card_w;

    font_set_stack_no(4);
    *(s32 *)w->frame = 0;
    w->csr_on = 0;
    w->btn_on = 0;
    w->pad = Psw[2];
    w->held = Psw[0];
    if (w->timer > 0) {
        w->timer--;
    }
    switch (w->op) {
    case 3:
        CardCmsv(w);
        w->frame[0] = 1;
        add_prim2(ot8, card_prim, 0, 1);
        break;
    case 0:
        CardAtld(w);
        add_prim2(ot8, card_prim, 0, 1);
        break;
    case 1:
        CardOptsv(w);
        if (w->rno > 1 && w->rno != 12) {
            w->frame[0] = 1;
        }
        w->frame[1] = 2;
        add_prim2(ot8, card_prim, 0, 1);
        break;
    case 6:
    case 7:
    case 8:
    case 10:
        CardOnsv1(w);
        w->frame[0] = 3;
        add_prim2(ot8, card_prim, 0, 1);
        break;
    case 2:
        CardEasysv(w);
        break;
    case 5:
    case 9:
        CardOfsv0(w);
        add_prim2(ot8, card_prim, 0, 1);
        break;
    case 4:
        CardConld(w);
        add_prim2(ot8, card_prim, 0, 1);
        break;
    default:
        w->done = 1;
        break;
    }
    font_set_stack_no(0);
    McActMain();
    if (w->done != 0) {
        system_w[0x12] = 1;
    }
    return w->done;
}
