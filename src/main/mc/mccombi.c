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
