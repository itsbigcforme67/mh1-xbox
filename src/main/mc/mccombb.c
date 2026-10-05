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
void mc_r_no_set();
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

int decode_to_ck(out)
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
