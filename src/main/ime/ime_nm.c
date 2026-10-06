/* "Ask" Japanese input method / dictionary engine, SLPM_654.95 main 0x23E500-0x24A240
 * (kana to kanji conversion for the name entry: roman input, bunsetu segmentation,
 * candidate lists, dictionary pages on disc, learning). Function and global names come
 * from the original symbols; struct fields are named by offset (guesses). Source of
 * truth for the whole range, in ADDRESS order, brace on its own line (tools/genruns.py
 * extracts the matching runs). */
#include "types.h"

typedef long long s64;
typedef struct NODE NODE;
typedef struct BS BS;
typedef struct KH KH;

typedef struct PW {
    s16 x00;
    u8 x02;
    u8 x03;
    s32 x04;
    s64 id;         /* 0x08 dictionary word id */
} PW;

struct KH {
    u8 flag;        /* 0x00 bit0 = continued, 0x80 = none */
    u8 str[5];      /* 0x01 */
    u8 x06;
    u8 x07;
    PW *pw;         /* 0x08 */
    u16 x0C;
    u16 x0E;
    KH *next;       /* 0x10 */
};

typedef struct CH CH;
struct CH {
    s16 len;        /* 0x00 */
    u8 x02;
    u8 x03;
    s32 x04;
    s64 id;         /* 0x08 */
    u16 x10;
    u16 x12;
    CH *next;       /* 0x14 */
};

typedef struct PWM PWM;
struct PWM {
    s16 len;        /* 0x00 */
    u8 x02;
    u8 x03;
    u8 x04;
    u8 x05;
    PWM *next;      /* 0x08 */
};

typedef struct KL KL;
struct KL {
    BS *bs;         /* 0x00 */
    s16 pri;        /* 0x04 */
    KH *kh;         /* 0x08 */
    KL *next;       /* 0x0C */
};

/* word record handed to the dictionary (learning) */
typedef struct WD {
    u8 *yomi;       /* 0x00 */
    s16 len;        /* 0x04 */
    s8 x06;
    u8 x07;
    u8 x08;
    u8 *tango;      /* 0x0C */
} WD;

/* bunsetu candidate (bsmem) */
struct BS {
    s16 len;        /* 0x00 */
    u8 x02;
    u8 x03;
    PW *pw;         /* 0x04 */
    s16 x08;
    s16 x0A;
    BS *next;       /* 0x0C */
};

/* 0x1C-byte edit character / bunsetu record, hchar[80] */
typedef struct HCHAR {
    s32 x00;
    void *ch;       /* 0x04 chmem list */
    BS *bs;         /* 0x08 bsmem list */
    KH *kh;         /* 0x0C candidate list */
    s32 x10;
    s8 x14;
    s8 x15;         /* bunsetu length */
    s8 x16;
    s8 x17;
    s8 x18;
    s8 x19;
    s8 x1A;
    s8 x1B;
} HCHAR;

extern HCHAR hchar[80];
extern u8 overlay_no[2];
extern u8 Arrangement[4];
extern int D_00386B00[];          /* 0x386B00 */
extern s16 *name_tbl[3];
extern int cur_len, cur_pos, func_mode, gun_nkh, sel_job, im_state, learn_on;
extern KH *top_kh;
extern u16 meanbuf[152];
extern u8 outbuf[152];
extern s64 wdsbuf[128];
extern u8 prev_yomi[80];
extern u8 prev_tango[80];
extern u8 yomi_buf[80];
extern u8 tango_buf[80];
extern u8 kana_ustr[39];
extern WD newwd_219;

typedef struct KANA {
    u16 ch;         /* 0x00 */
    u16 pad;
    s32 n;          /* 0x04 number of input chars */
} KANA;

typedef struct CNTAB {
    s16 a;
    s16 b;
} CNTAB;

typedef struct ASKROM {
    s64 rest;       /* 0x00 bytes left */
    u8 *cur;        /* 0x08 */
} ASKROM;

extern KANA kana_buf[39];
extern u16 inpc_buf[100];
extern u16 *einpc_buf, *cinpc_buf, *qinpc_buf, *pinpc_buf;
extern KANA *ekana_buf, *pkana_buf;
extern u8 *e_ustr, *p_ustr;
extern int kana_len, kana_buf_size;
extern u16 asc2jis[256];
extern u8 spec_key_19[];
extern u16 spec_tran_20[];
extern u8 rmtype[256];
extern u8 btoudata[180];
extern u8 bitpool[992];
extern u8 power[8];
extern CNTAB cntab[114];
extern u8 offsetmap[256];
extern ASKROM gAskRom;
extern u8 *ask_load_adrs;
extern int dic_rw, dic_fd;

int PatchExecCS();
int afs_file_length();
int load_bin();
int mwOverlayInit();
int bunsetu_len();
void change_kind();
int current_makedisp();
int first_makedisp();
int kouho_makedisp();
void free_khmemlist();
void free_mem();
KH *kh_followed();
KH *kh_skip();
int meantosjis();
int dic_learn();
int dic_newlearn();
struct WD *raw_newwd();
int dic_tmptouroku();
int strlen();
void strncpy();
void clear_prevwd();
void kh_learn();
void prev_learn();
void add_prevwd();
int api_funcent();
void free_hchar();
int ask_jis2sjis();
int ask_sjis2jis();
u16 to_zenkaku();
int to_hankaku();
int can_daku();
int can_handaku();
int to_ucode();
int setmean();
int dic_getgaku();
void init_univmem();
void init_hchar();
void init_edit0();
int g2jodo();
int getbit(s16);
int is_shift();
void *memcpy();
int close_dic();
void init_page();
void init_temp();
int open_dic();
int read_head();
int read_index();
void set_dicname();
int flush_head();
void flush_temp();
void flush_pages();
int main_snssyn();
int tmp_snssyn();
int main_getsyn();
int tmp_getsyn();
int set_entid_tab();
int set_synref();
int exist_synref();
u8 *next_wd();
int get_entid_tab();
u8 *load_page();
u8 *load_temp();
void get1wd();
void getallwd();
void set_wds();
u8 *ins_wds();
int getkbuflen();
void getkbuf();
void clear_allrtime();
void clear_rtime();
int get_maxtime();
void shift_temp();
void update_nowtmp();
void update_nowpage();
int update_entid_rtime();
int max_rtime();
u8 *end_page();
void shiftpage();
int newwdlen();
int updwdlen();
void set_record();
void upd_record();
void set_entry2();
void init_entid_tab();
int srch_page();
int ask_strncmp();
int calc_pulen();
int page_fix();
int prefix();
int free_entid_tab();
int chk_entry2();
int delwd();
extern int gaku_mode, suji_mode, henkan_mode, ikkatsu_mode;
int strcpy();
int d_open();
int d_close();
int d_read();
int d_write();
int d_seek();
int FAskRom_Open();
int FAskRom_Close();
int FAskRom_Read();
int FAskRom_Write();
int FAskRom_Seek();
int seek_dic();
u8 *set_num();
void init_page_tab();
int write_page();
int read_page();
int write_temp();
int read_temp();
void reset_temp();
void init_node_tab();
void init_hash_tab();
void page_gc();
int hashfunc();
void setkbuf();
int setkbuflen();
int iskanji();
void clear_hchar();
void free_hchar_one();
void free_chmemlist();
void free_bsmemlist();
BS *alloc_bsmem();
int bs_check();
int ch_check();
void fl_check();
int is_num();
int is_alpha();
int is_alphanum();
int is_paren();
int is_kata();
int is_jisknj();
int is_jiskig();
int is_kanji();
void add_dummy_chmem();
CH *make_chmem();
void hchar_addchmem();
int dic_freeentid();
int muhenkan();
void *srch_pword();
PWM *pword_list();
int not_bhead();
int is_kuten();
PWM *alloc_pwmem();
void free_pwmemlist();
CH *alloc_chmem();
KH *alloc_khmem();
KL *alloc_klmem();
void free_klmemlist();
BS *make_bsmem();
static BS *ins_bsmem();
void hchar_addbsmem();
int dic_get1num();
int dic_getallnum();
int dic_get1wd();
int dic_getallwd();
u16 kh_priority();
KH *null_kouho();
KH *create_kouho();
KH *get_kouholist();
void free_kouholists();
void all_kouho();
void disp_kouho();
int inc_gun();
int kstrncpy();
void kh_mergesort();
int kwin_length();
extern int kwin_len, gun_num;
extern u8 *e_khstr;
extern u8 kouho_head[5];
extern u8 kouho_rest[11];
extern u8 lit_485_0036E088[];
extern u8 mem[72000];
extern void *free_univ;
extern int first_init_5;
KH *raw_kouho();
KH *kh_endof();
void khmem_raw();
void kh_append_init();
void kh_append();
int kh_merge_getone();
static int exist_kouho();
int kh_length();
int kh_count();
KH *take_kouho();
void kouho_set_num();
int jiritu_makedisp();
int next_gun();
int back_gun();
int is_jis();
int shiftlen();
int sstrtom();
void *alloc_mem();
void free_mem();
int bs_point();
void bs_ctd();
int calc_point();
int ignore_syn();
int setu_point();
void bs_prefix();
int ktu_match();
int syn_match();
int josi_match();
int setu_match();
int to_roman();
int ToUpper();
u8 *getrda1();
u8 *getrda2();
int add_kana_buf();
int bytesin_kana_buf();
int count_byte_kana_buf();
int api_funcent();
int get_kouhostr();
int syn_2to3();
void wd_learn();
u8 *select_tostr();
u8 *select_subtostr();
void init_kouho();
extern u8 rmspec[39];
extern u8 rmtab[528];
extern u8 prmtab[108];
extern u8 tab_2to3[30];
extern int roman_japan, lock_mode;
extern u8 dic_name[128];
extern int (*D_0034ABEC[])();
extern CH null_chmem;
extern u16 pwordmap[96];
extern u8 pword[1532];
extern u8 pluswd[243];
int bs_prefer();
void bs_prefix();
void unify_bsmem();
void first_kouho();
int concat_bslen();
NODE **srch_node();
NODE *alloc_node();
void free_node();
u8 *alloc_record();
void clear_entid_tmp();
void clear_entid_tmpall();
int tmpoffset();

typedef struct PAGE PAGE;
struct PAGE {
    s32 id;         /* 0x00 page number or -1 */
    s32 dirty;      /* 0x04 */
    PAGE *next;     /* 0x08 */
    u8 data[0x400]; /* 0x0C */
};

typedef struct ENTID {
    u8 cnt;         /* 0x00 */
    u8 rtime;       /* 0x01 */
    u16 a;          /* 0x02 */
    s16 b;          /* 0x04 */
    s16 c;          /* 0x06 */
} ENTID;

struct NODE {
    u8 *rec;        /* 0x00 */
    NODE *next;     /* 0x04 */
};

extern PAGE page_tab[10];
extern PAGE *page_top;
extern ENTID entid_tab[128];
extern NODE node_tab[512];
extern NODE *hash_tab[80];
extern NODE *freelist;
extern u8 temp_pages[8][0x400];
extern u8 *temp_top, *temp_end;
extern int temp_page, old_temp, old_suji, entry2upd, mainlower, mainupper;
extern u8 temp_updated;
extern u8 header[0x400];
extern u8 title[];
extern u8 mainindex[0x1000];
extern u8 mydicname[];
extern u8 *entry2code;
extern u8 *num_chars[3];

#define ELEN(p) (((p)[0] + ((p)[1] << 8)) & 0xFFFF)

typedef struct SYN {
    u8 x00;
    u8 x01;
    u8 x02;
    u8 x03;
    u16 x04;
} SYN;

/* result of a synonym search (dic_snssyn / dic_getsyn) */
typedef struct SYNR {
    s16 x00;
    s16 x02;
    s16 x04;
    s16 x06;
    s64 id;         /* 0x08 */
    s16 x10;
    s16 x12;
    s16 x14;
    SYN syn[32];    /* 0x16 */
} SYNR;

/* result of one page search (main_snssyn / main_getsyn) */
typedef struct SRCH {
    s16 page;       /* 0x00 */
    s16 off;        /* 0x02 */
    s16 x04;
    s16 x06;
    s32 x08;
    u8 *ent;        /* 0x0C */
} SRCH;

extern SYNR entbuf;

/* learn the chosen candidate (pos, len unused) */

void Overlay_reset(void)
{
    overlay_no[0] = 0;
    overlay_no[1] = 0;
}

int Load_overlay(int arg0, int arg1)
{
    int len;
    int addr;

    if ((u8)arg1 != 2 && (u8)arg0 == Arrangement[(u8)arg1 + 3]) {
        return 1;
    }
    addr = D_00386B00[(u8)arg1];
    len = afs_file_length(name_tbl[(u8)arg1][(u8)arg0] | 0x20000);
    load_bin(name_tbl[(u8)arg1][(u8)arg0] | 0x20000, addr);
    if (len > 0) {
        mwOverlayInit(addr, len);
        Arrangement[(u8)arg1 + 3] = arg0;
        PatchExecCS(arg1, arg0);
        return 1;
    }
    return 0;
}

int disp_select(void)
{
    u16 *p;
    int k;
    int total;
    int pos;
    int len;

    total = 0;
    pos = 0;
    p = meanbuf;
    if ((len = bunsetu_len(0)) != 0) {
        do {
            if (pos == cur_pos) {
                k = current_makedisp(pos, len, p);
                cur_len = len;
                change_kind(p, k, 7);
            } else {
                k = first_makedisp(pos, len, p);
                change_kind(p, k, 6);
            }
            pos += len;
            p += k;
            total += k;
        } while ((len = bunsetu_len(pos)) != 0);
    }
    return total;
}

int current_makedisp(pos, len, buf)
int pos;
int len;
u16 *buf;
{
    KH *kh;

    if (sel_job == 1 || func_mode >= 3) {
        kh = take_kouho(top_kh, gun_nkh);
    } else {
        kh = take_kouho(hchar[pos].kh, 0);
    }
    return kouho_makedisp(pos, len, kh, buf);
}

int first_makedisp(pos, len, buf)
int pos;
int len;
u16 *buf;
{
    return kouho_makedisp(pos, len, hchar[pos].kh, buf);
}

void unify_khmem(int pos, int flag)
{
    KH **pk;
    KH *k;
    KH *top;
    KH *next;

    pk = &hchar[pos].kh;
    k = *pk;
    if (k != 0) {
        if (flag == 1 && (sel_job == 1 || func_mode >= 3)) {
            top = take_kouho(top_kh, gun_nkh);
            while (k != 0 && k != top) {
                next = kh_followed(k);
                free_mem(k);
                k = next;
            }
            *pk = k;
        }
        free_khmemlist(kh_skip(k));
    }
}

u8 *select_tostr(void)
{
    meantosjis(meanbuf, outbuf, disp_select());
    return outbuf;
}

u8 *select_subtostr(int arg0, int n)
{
    u16 *p;
    int len;
    int k;
    int pos;

    p = meanbuf;
    for (pos = arg0; pos < arg0 + n; pos += len) {
        len = bunsetu_len(pos);
        if (len == 0) {
            break;
        }
        if (pos == cur_pos) {
            k = current_makedisp(pos, len, p);
        } else {
            k = first_makedisp(pos, len, p);
        }
        p += k;
    }
    meantosjis(meanbuf, outbuf, p - meanbuf);
    return outbuf;
}

void kh_learn(int pos, int len, KH *kh, BS *list)
{
    s64 *out;
    PW *pw;
    s64 last;
    s64 first;
    int n;

    pw = 0;
    if (kh->x0C == 0xFFFF || (pw = kh->pw) != 0) {
        out = wdsbuf;
        if (list == 0 || list == (BS *)-1) {
            if (pw != 0) {
                wdsbuf[0] = pw->id;
                out = wdsbuf + 1;
            }
        } else {
            first = 0;
            last = first;
            do {
                if (list->pw != 0) {
                    if (list->pw->id != last && list->pw->id != first) {
                        *out = list->pw->id;
                        first = list->pw->id;
                        out++;
                    }
                }
                list = list->next;
            } while (list != 0);
        }
        if (pw == 0) {
            n = out - wdsbuf;
            dic_newlearn(raw_newwd(list), wdsbuf, n);
            return;
        }
        n = out - wdsbuf;
        dic_learn(pw->id, kh->x0C, wdsbuf, n);
    }
}

void prev_learn(KH *kh)
{
    WD wd;
    PW *pw;

    if (prev_yomi[0] != 0) {
        wd.yomi = prev_yomi;
        wd.len = strlen(prev_yomi);
        wd.x06 = 0;
        if (kh == 0 || (pw = kh->pw) == 0) {
            wd.x08 = 0;
            wd.x07 = 0x28;
        } else {
            wd.x07 = pw->x02;
            wd.x08 = pw->x03;
        }
        wd.tango = prev_tango;
        dic_tmptouroku(&wd);
    }
}

void wd_learn(int pos, int end)
{
    int len;
    KH *kh;
    HCHAR *h;

    while (pos < end) {
        len = bunsetu_len(pos);
        if (len == 0) {
            break;
        }
        h = &hchar[pos];
        if (pos == cur_pos && (sel_job == 1 || func_mode >= 3)) {
            kh = take_kouho(top_kh, gun_nkh);
        } else {
            kh = take_kouho(h->kh, 0);
        }
        if (im_state != 3 && func_mode < 3) {
            kh_learn(pos, len, kh, h->bs);
            pos += len;
            continue;
        }
        if (h->x16 != 0 && h->x16 != h->x15) {
            add_prevwd(pos, len, kh, 1);
            kh_learn(pos, len, kh, h->bs);
            pos += len;
            continue;
        }
        if (prev_yomi[0] != 0) {
            add_prevwd(pos, len, kh, 0);
            prev_learn(kh);
            clear_prevwd();
            pos += len;
        } else {
            kh_learn(pos, len, kh, h->bs);
            pos += len;
        }
    }
}

void clear_prevwd(void)
{
    prev_yomi[0] = 0;
}

void add_prevwd(int pos, int len, KH *kh, int cont)
{
    u8 *yomi;
    u8 *tango;

    yomi = prev_yomi;
    tango = prev_tango;
    if (cont == 0) {
        yomi = prev_yomi + strlen(prev_yomi);
        len = kh->x06;
        tango = prev_tango + strlen(prev_tango);
    }
    strncpy(yomi, kana_ustr + pos, len);
    yomi[len] = 0;
    if (len != strlen(yomi)) {
        clear_prevwd();
        return;
    }
    meantosjis(meanbuf, tango, kouho_makedisp(pos, len, kh, meanbuf));
}

WD *raw_newwd(int pos, int len, KH *kh)
{
    strncpy(yomi_buf, kana_ustr + pos, len);
    yomi_buf[len] = 0;
    meantosjis(meanbuf, tango_buf, kouho_makedisp(pos, len, kh, meanbuf));
    newwd_219.len = len;
    newwd_219.yomi = yomi_buf;
    newwd_219.x07 = 0x19;
    newwd_219.tango = tango_buf;
    newwd_219.x06 = 0;
    newwd_219.x08 = 0;
    return &newwd_219;
}

void apiask_19_Henkan(int a, int b, int c)
{
    int req[4];

    req[1] = a;
    req[2] = b;
    req[3] = c;
    req[0] = 0x13;
    api_funcent(req);
}

void apiask_20_PrevKouho(int a, int b)
{
    int req[3];

    req[1] = a;
    req[2] = b;
    req[0] = 0x14;
    api_funcent(req);
}

void apiask_21_NextKouho(int a, int b)
{
    int req[3];

    req[1] = a;
    req[2] = b;
    req[0] = 0x15;
    api_funcent(req);
}

void apiask_24_AllKakutei(int a)
{
    int req[2];

    req[1] = a;
    req[0] = 0x18;
    api_funcent(req);
}

void apiask_25_FirstKakutei(int a, int b, int c, int d, int e)
{
    int req[6];

    req[1] = a;
    req[2] = b;
    req[3] = c;
    req[0] = 0x19;
    req[4] = d;
    req[5] = e;
    api_funcent(req);
}

void apiask_28_OpenDic(void)
{
    int req[1];

    req[0] = 0x1C;
    api_funcent(req);
}

void apiask_33_LongerKouho(int a, int b)
{
    int req[3];

    req[1] = a;
    req[2] = b;
    req[0] = 0x21;
    api_funcent(req);
}

void apiask_34_ShorterKouho(int a, int b)
{
    int req[3];

    req[1] = a;
    req[2] = b;
    req[0] = 0x22;
    api_funcent(req);
}

void apiask_35_PrevBunsetu(int a, int b)
{
    int req[3];

    req[1] = a;
    req[2] = b;
    req[0] = 0x23;
    api_funcent(req);
}

void apiask_36_NextBunsetu(int a, int b)
{
    int req[3];

    req[1] = a;
    req[2] = b;
    req[0] = 0x24;
    api_funcent(req);
}

void apiask_37_FirstHenkanToKata(int a, int b)
{
    int req[3];

    req[1] = a;
    req[2] = b;
    req[0] = 0x25;
    api_funcent(req);
}

void apiask_38_FirstHenkanToHira(int a, int b)
{
    int req[3];

    req[1] = a;
    req[2] = b;
    req[0] = 0x26;
    api_funcent(req);
}

int kwin_length()
{
    return 0x48;
}

int nwin_length()
{
    return 0x48;
}

void init_roman(void)
{
    kana_len = 0;
    einpc_buf = inpc_buf;
    cinpc_buf = inpc_buf;
    qinpc_buf = inpc_buf;
    pinpc_buf = inpc_buf;
    ekana_buf = kana_buf;
    pkana_buf = kana_buf;
    e_ustr = kana_ustr;
    p_ustr = kana_ustr;
}

void init_edit0(void)
{
    init_roman();
    free_hchar(0, 0x50, 0);
    clear_prevwd();
}

void trans_roman(u8 *out, int start, int cnt, int mode)
{
    KANA *k;
    KANA *p;
    KANA *end;
    u16 *ip;
    int i;
    int a;
    int b;
    int prev;
    int t;
    u16 c;

    if (mode >= 5) {
        k = kana_buf;
        a = 0;
        for (i = 0; i < start; i++) {
            a += k->n;
            k++;
        }
        b = a;
        for (i = 0; i < cnt; i++) {
            b += k->n;
            k++;
        }
        ip = inpc_buf + a;
        for (; a < b; a++) {
            if (mode == 5) {
                t = *ip | 0x100;
                ip++;
                t = ask_jis2sjis(to_zenkaku(t & 0xFFFF)) & 0xFFFF;
                out[0] = t >> 8;
                out[1] = t;
                out += 2;
            } else if (mode == 6) {
                out[0] = *ip;
                ip++;
                out++;
            }
        }
        *out = 0;
        return;
    }
    p = kana_buf + start;
    end = p + cnt;
    prev = 0;
    for (; p < end; p++) {
        c = p->ch;
        if (mode == 1) {
            if ((c & 0xFF00) == 0) {
                if (c == 0xDE && can_daku(prev)) {
                    out -= 2;
                    if ((prev & 0xFFFF) == 0x2426) {
                        c = 0x2474;
                    } else {
                        c = prev + 1;
                    }
                } else if (c == 0xDF && can_handaku(prev)) {
                    out -= 2;
                    c = (prev & 0xFFFF) + 2;
                } else {
                    c = to_zenkaku(c);
                }
            }
            if ((c & 0xFF00) == 0x2500) {
                c = c & 0x24FF;
            }
            goto emit;
        }
        if (mode == 2) {
            if ((c & 0xFF00) == 0) {
                if (c == 0xDE && can_daku(prev)) {
                    out -= 2;
                    if ((prev & 0xFFFF) == 0x2526) {
                        c = 0x2574;
                    } else {
                        c = prev + 1;
                    }
                } else if (c == 0xDF && can_handaku(prev)) {
                    out -= 2;
                    c = (prev & 0xFFFF) + 2;
                } else {
                    c = to_zenkaku(c | 0x100);
                }
            }
            if ((c & 0xFF00) == 0x2400) {
                c = c | 0x2500;
            }
            goto emit;
        }
        if (mode == 3) {
            out += to_hankaku(out, c);
        } else {
            if (mode == 4 && (c & 0xFF00) == 0) {
                c = to_zenkaku(c | 0x100, c);
            }
emit:
            prev = c;
            if (prev & 0xFF00) {
                c = ask_jis2sjis(c);
                *out = c >> 8;
                out++;
            }
            *out = c;
            out++;
        }
    }
    *out = 0;
}

void into_editing(int mode)
{
    int n;

    learn_on = dic_getgaku();
    init_univmem();
    init_hchar();
    init_edit0();
    if (mode == 1) {
        n = nwin_length() - 1;
        kana_buf_size = n / 2;
        im_state = 1;
    }
}

int roman_makedisp(int pos, int n, u16 *buf, int flag)
{
    KANA *p;
    KANA *end;
    u16 *ip;
    int first;
    int k;
    u16 c;
    u16 d;
    u16 *start;

    p = kana_buf + pos;
    end = kana_buf + pos + n;
    start = buf;
    first = 0;
    if (p < end) {
        for (;;) {
            if (p == pkana_buf) {
                for (ip = qinpc_buf; ip < pinpc_buf; ip += 2) {
                    c = *ip;
                    d = c & 0xFF;
                    if (c & 0x4000) {
                        d = to_zenkaku(c);
                    }
                    if (flag != 0 && ip == cinpc_buf) {
                        k = setmean(buf, d, 8);
                        flag = 0;
                    } else {
                        k = setmean(buf, d, 4);
                    }
                    buf += k;
                }
                first = 1;
            }
            if (p >= ekana_buf) {
                break;
            }
            if (flag != 0 && first != 0) {
                k = setmean(buf, p->ch, 8);
                flag = 0;
            } else {
                k = setmean(buf, p->ch, 4);
            }
            buf += k;
            p++;
            first = 0;
            if (p >= end) {
                break;
            }
        }
    }
    if (flag != 0) {
        buf += setmean(buf, 0x20, 8);
    }
    return buf - start;
}

int FAskRom_Open(void)
{
    gAskRom.rest = 0x97C00;
    gAskRom.cur = ask_load_adrs;
    return 1;
}

int FAskRom_Close(void)
{
    return 0;
}

int FAskRom_Read(fd, buf, n)
int fd;
void *buf;
int n;
{
    int len;

    len = gAskRom.rest;
    if (len >= n) {
        len = n;
    }
    memcpy(buf, gAskRom.cur, len);
    gAskRom.rest = gAskRom.rest - len;
    gAskRom.cur = gAskRom.cur + len;
    return len;
}

int FAskRom_Write(fd, buf, n)
int fd;
void *buf;
int n;
{
    int len;

    len = gAskRom.rest;
    if (n < len) {
        len = n;
    }
    memcpy(gAskRom.cur, buf, n);
    gAskRom.rest = gAskRom.rest - len;
    gAskRom.cur = gAskRom.cur + len;
    return len;
}

int FAskRom_Seek(fd, off, whence)
int fd;
int off;
int whence;
{
    s64 r;

    switch (whence) {
    case 0:
        if (off < 0) {
            return -1;
        }
        gAskRom.rest = 0x97C00 - off;
        gAskRom.cur = ask_load_adrs + off;
        break;
    case 1:
        r = gAskRom.rest - off;
        if (r < 0 || r > 0x97C00) {
            return -1;
        }
        gAskRom.rest = r;
        gAskRom.cur = gAskRom.cur + off;
        break;
    case 2:
        if (off > 0) {
            return -1;
        }
        gAskRom.rest = 0 - off;
        gAskRom.cur = ask_load_adrs + off + 0x97BFF;
        break;
    }
    return gAskRom.cur - ask_load_adrs;
}

u16 to_zenkaku(c)
u16 c;
{
    u16 r;

    r = asc2jis[c & 0xFF];
    if ((r & 0xFF00) == 0x2500 && !(c & 0x100)) {
        r = (r & 0xFF) | 0x2400;
        return r;
    }
    return r;
}

u16 to_zenkaku_spec(int c)
{
    u8 *k;
    u16 *t;
    u16 r;

    k = spec_key_19;
    t = spec_tran_20;
    while (*k != 0) {
        if ((*k & 0xFF) == (c & 0xFF)) {
            r = *t;
            if ((r & 0xFF00) == 0x2500 && !((u16)c & 0x100)) {
                return (r & 0xFF) | 0x2400;
            }
            return r;
        }
        k++;
        t++;
    }
    return 0;
}

int ext_jis(int c, u16 hi)
{
    return ((c & 0xFF) | ((hi & 0x100) + 0x2400)) & 0xFFFF;
}

int to_hankaku(u8 *out, int code)
{
    int c;
    int u;

    c = code;
    switch ((s8)c & 0xFF00 ? ((s8)c & 0xFF00) : 0) {
    default:
        break;
    }
    switch (c & 0xFF00) {
    case 0x0:
    case 0x2300:
        out[0] = c;
        return 1;
    case 0x2500:
        c = c & 0x24FF;
    case 0x2100:
    case 0x2400:
        u = (s8)(to_ucode(c, c) & 0xFF);
        if (u >= 0x98 && u < 0xF7) {
            u16 t = asc2jis[0x78 + u];
            out[0] = t;
            if (t & 0x8000) {
                out[1] = 0xDE;
                return 2;
            }
            if (t & 0x4000) {
                out[1] = 0xDF;
                return 2;
            }
            return 1;
        }
        out[0] = u;
        return 1;
    default:
        out[0] = 0x20;
        return 1;
    }
}

int can_daku(int c)
{
    u8 buf[4];

    if (is_shift() != 0) {
        c = ask_sjis2jis(c) & 0xFFFF;
    }
    if (to_hankaku(buf, c) != 1) {
        return 0;
    }
    return (rmtype[buf[0]] & 0xF) == 0xA;
}

int can_handaku(int c)
{
    u8 buf[4];

    if (is_shift() != 0) {
        c = ask_sjis2jis(c) & 0xFFFF;
    }
    if (to_hankaku(buf, c) != 1) {
        return 0;
    }
    return (rmtype[buf[0]] & 0xF) == 0xB;
}

int srch_ucode(int code)
{
    u8 *p;

    for (p = btoudata; p < btoudata + 180; p += 4) {
        if (*(u16 *)p == (u16)code) {
            return p[2];
        }
        if (*(u16 *)p > (u16)code) {
            break;
        }
    }
    return 0;
}

int getbit(s16 n)
{
    return bitpool[n >> 3] & power[n & 7];
}

int g2jodo(int c)
{
    c = c & 0xFF;
    if (c > 0 && c < 0xE) {
        return (c + 0x7F) & 0xFF;
    }
    return 0;
}

int goku_connect(int a, int b, int c)
{
    s16 om;
    CNTAB *cn;

    if ((u8)c < 0x80) {
        return 0;
    }
    if ((u8)a < 0x2D && b != 0) {
        a = g2jodo(a) & 0xFF;
        if (a == 0) {
            return 0;
        }
    }
    om = offsetmap[a & 0xFF];
    if (om == 0xFF) {
        return 0;
    }
    if ((u8)c >= 0xC0) {
        cn = &cntab[(u8)c - 0xC0];
    } else {
        cn = &cntab[(u8)c - 0x41];
    }
    if ((u8)a >= 0x80 && (u8)a < 0xC0) {
        return getbit(cn->b + om + (b & 0xFF) - 1);
    }
    return getbit(cn->a + om);
}

int setu_end(int a0, int flag)
{
    int b;
    int c;

    c = a0 & 0xFF;
    if (c >= 0xC0) {
        return 1;
    }
    if (c <= 0x7F) {
        if (c <= 0x2C && (flag & 0xFF)) {
            c = g2jodo(a0) & 0xFF;
            if (c == 0) {
                return 0;
            }
        } else {
            if (c == 0xD || (c >= 0x13 && c <= 0x34) || c == 0x38) {
                return 1;
            }
            return 0;
        }
    }
    b = flag & 0xFF;
    switch (c & 0xFF) {
    case 0x8B:
    case 0x8D:
    case 0x93:
    case 0x96:
    case 0x99:
    case 0x9A:
    case 0x9B:
    case 0x9E:
    case 0xA1:
    case 0xA2:
    case 0xA7:
    case 0xA8:
    case 0xB1:
        if ((b > 4 && b <= 8) || b >= 0xA) {
            return 1;
        }
        return 0;
    case 0x97:
        if (b == 8) {
            return 0;
        }
    case 0x8E:
    case 0x92:
    case 0x94:
    case 0x95:
    case 0x98:
    case 0xA0:
        if (b > 4) {
            return 1;
        }
        return 0;
    default:
        if (b == 4 || (u32)(b - 7) < 2 || b > 0x9) {
            return 1;
        }
        return 0;
    }
}

int dic_open(char *name)
{
    int r;

    if (*name == 0) {
        return -1;
    }
    set_dicname();
    r = open_dic();
    if (r != 0) {
        if (r == -1) {
            return -7;
        }
        return -8;
    }
    if (read_head() == -1) {
        close_dic();
        return -2;
    }
    if (read_index() == -1) {
        close_dic();
        return -2;
    }
    init_page();
    init_temp();
    if (dic_rw == 0x8000) {
        return -6;
    }
    return 3;
}

int dic_close(void)
{
    if (dic_fd == -1) {
        return -3;
    }
    flush_head();
    flush_temp();
    flush_pages();
    if (close_dic() == -1) {
        return -2;
    }
    return 3;
}

int dic_snssyn(u8 *key, int len, SYNR *r)
{
    int n;
    int m;
    int cnt;
    u8 buf[0x20];
    SRCH a;
    SRCH b;

    if (dic_fd == -1) {
        return -3;
    }
    strncpy(buf, key, (s16)len);
    buf[(s16)len] = 0;
    n = main_snssyn(buf, len, &a);
    if (n == 2) {
        return 2;
    }
    strncpy(buf, key, (s16)len);
    buf[(s16)len] = 0;
    m = tmp_snssyn(buf, len, &b);
    if (m == 2) {
        return 2;
    }
    if (n == 0 && m == 0) {
        return 0;
    }
    r->x12 = 1;
    cnt = 0;
    r->x10 = (a.x06 >= b.x06) ? a.x06 : b.x06;
    if (a.x04 < b.x04) {
        r->x00 = b.x04;
        r->x14 = set_synref(b.ent, r->syn, 0, &cnt);
        a.off = -1;
    } else if (b.x04 < a.x04) {
        r->x00 = a.x04;
        r->x14 = set_synref(a.ent, r->syn, 0, &cnt);
        b.off = -1;
    } else {
        r->x00 = b.x04;
        r->x14 = set_synref(a.ent, r->syn, set_synref(b.ent, r->syn, 0, &cnt), &cnt);
    }
    r->id = (int)set_entid_tab(a.page, b.page, b.off, cnt);
    return r->id != 0;
}

int main_snssyn(u8 *key, int len0, SRCH *r)
{
    s16 len;
    s16 page;
    s16 plen;
    s16 best;
    s16 maxp;
    int more;
    int c;
    u8 *e;
    u8 *hit;
    u8 *base;
    s16 klen;
    s16 pre;

    more = 1;
    len = len0;
    r->x06 = 0;
    r->x04 = 0;
    hit = 0;
    page = srch_page(key);
    if (page_fix(page, key) == 0) {
        return 2;
    }
    maxp = 0;
    for (;;) {
        base = load_page(page);
        e = base;
        plen = calc_pulen(page, key);
        best = 0;
        while (ELEN(e) != 0) {
            klen = e[2];
            pre = prefix(e + 3, key, klen);
            if (pre >= maxp) {
                maxp = pre + 1;
            }
            c = ask_strncmp(e + 3, key, klen);
            if (c == 0) {
                if (klen == len && more != 0) {
                    return 2;
                }
                best = klen;
                hit = e;
            } else if (c > 0) {
                if (ask_strncmp(e + 3, key, len) == 0 && more != 0) {
                    return 2;
                }
                break;
            }
            e += ELEN(e);
        }
        if (best > 0) {
            r->page = page;
            r->off = hit - base;
            r->x04 = best;
            r->x06 = maxp;
            r->ent = hit;
            return 1;
        }
        if (plen < 2) {
            r->off = -1;
            return 0;
        }
        len = plen - 1;
        key[len] = 0;
        page = srch_page(key);
        more = 0;
    }
}

int set_synref(u8 *ent, SYN *out, int n, int *maxv)
{
    u8 *p;
    u8 *end;
    u8 *q;
    SYN *o;

    o = out + n;
    end = ent + ELEN(ent);
    p = ent + ent[2] + 3;
    while (p < end) {
        o->x00 = p[0];
        o->x04 = p[1];
        q = p + 2;
        if (*maxv < p[1]) {
            *maxv = o->x04;
        }
        if (p[2] < 0xC) {
            o->x01 = p[2];
            q++;
        } else {
            o->x01 = 0;
        }
        o->x02 = 0;
        if (exist_synref(out, o) == 0) {
            n++;
            o++;
            if (n >= 0x20) {
                break;
            }
        }
        p = next_wd(q, end);
    }
    return n;
}

int exist_synref(SYN *p, SYN *e)
{
    for (; p < e; p++) {
        if (p->x00 == e->x00 && p->x01 == e->x01) {
            if (p->x04 < e->x04) {
                p->x04 = e->x04;
            }
            return 1;
        }
    }
    return 0;
}

u8 *next_wd(p, end)
u8 *p;
u8 *end;
{
    if (p < end) {
        do {
            if ((int)(*p) <= 0x38) {
                break;
            }
            p += 2;
        } while (p < end);
    }
    return p;
}

int dic_getsyn(u8 *key, int len, SYNR *r)
{
    int n;
    int m;
    int cnt;
    u8 buf[0x20];
    SRCH a;
    SRCH b;

    if (dic_fd == -1) {
        return -3;
    }
    strncpy(buf, key, (s16)len);
    buf[(s16)len] = 0;
    n = main_getsyn(buf, len, &a);
    m = tmp_getsyn(buf, len, &b);
    if (n == 0 && m == 0) {
        return 0;
    }
    r->x00 = len;
    r->x10 = len;
    r->x12 = 1;
    r->x14 = 0;
    cnt = 0;
    if (n == 1) {
        r->x14 = set_synref(a.ent, r->syn, 0, &cnt);
    }
    if (m == 1) {
        r->x14 = set_synref(b.ent, r->syn, r->x14, &cnt);
    }
    r->id = (int)set_entid_tab(a.page, a.off, b.off, cnt);
    if (r->id == 0) {
        return 0;
    }
    return 1;
}

int main_getsyn(u8 *key, int len0, SRCH *r)
{
    s16 len;
    int page;
    s16 klen;
    u8 *base;
    u8 *e;
    int c;

    len = len0;
    if (len < 3 && chk_entry2() != 0) {
        r->off = -1;
        return 0;
    }
    page = srch_page(key);
    e = base = load_page(page);
    while (ELEN(e) != 0) {
        klen = e[2];
        c = ask_strncmp(e + 3, key, klen);
        if (c == 0) {
            if ((s16)klen == len) {
                break;
            }
        } else if (c > 0) {
            r->off = -1;
            return 0;
        }
        e += ELEN(e);
    }
    if (ELEN(e) == 0) {
        r->off = -1;
        return 0;
    }
    r->page = page;
    r->off = e - base;
    r->ent = e;
    return 1;
}

int dic_get1wd(int id, int a, int b, u8 *out)
{
    int off;
    int tmp;
    int unused;
    int best;
    int page;

    if (dic_fd == -1) {
        return -3;
    }
    page = get_entid_tab(id, &off, &tmp, &unused);
    if (page == -1) {
        return 0;
    }
    best = -1;
    if (off != -1) {
        get1wd(load_page(page) + off, a & 0xFF, b & 0xFF, &best);
    }
    if (tmp != -1) {
        get1wd(load_temp(tmp), a & 0xFF, b & 0xFF, &best);
    }
    if (best == -1) {
        return 0;
    }
    return 1;
}

void get1wd(u8 *ent, int a, int b, int *best, int out, int tag)
{
    int old;
    u8 *end;
    u8 *p;
    u8 *q;
    u8 *hit;
    int rt;
    int k;

    old = *best;
    end = ent + ELEN(ent);
    p = ent + ent[2] + 3;
    hit = 0;
    while (p < end) {
        rt = p[1];
        q = p + 2;
        k = p[2];
        if (k < 0xC) {
            q++;
        } else {
            k = 0;
        }
        if (a == p[0] && b == k && *best < rt) {
            *best = rt;
            hit = p;
        }
        p = next_wd(q, end, p);
    }
    if (old < *best) {
        set_wds(out, hit, end, tag | (hit - ent));
    }
}

void set_wds(w0, rec, end, tag)
void *w0;
u8 *rec;
u8 *end;
int tag;
{
    s16 *w = w0;

    w[0] = tag;
    w[1] = rec[1];
    rec += 2;
    ((u8 *)w)[4] = 0;
    if (rec[0] < 0xC) {
        rec++;
    }
    getkbuf((u8 *)w + 5, rec);
}

int dic_getallwd(int id, int a, int b, u8 *out)
{
    int off;
    int tmp;
    int cnt;
    int page;
    int n;

    if (dic_fd == -1) {
        return -3;
    }
    page = get_entid_tab(id, &off, &tmp, &cnt);
    if (page == -1) {
        return 0;
    }
    cnt = 0;
    n = 0;
    if (off != -1) {
        getallwd(load_page(page) + off, a & 0xFF, b & 0xFF, &cnt);
    }
    if (tmp != -1) {
        getallwd(load_temp(tmp), a & 0xFF, b & 0xFF, &cnt);
    }
    *(int *)out = cnt;
    if (cnt == 0) {
        return 0;
    }
    return 1;
}

void getallwd(u8 *ent, int a, int b, int *cnt, int *pos, int buf, int tag)
{
    u8 *end;
    u8 *p;
    u8 *q;
    int rt;
    int k;
    int len;

    end = ent + ELEN(ent);
    p = ent + ent[2] + 3;
    while (p < end) {
        rt = p[1];
        q = p + 2;
        k = p[2];
        if (k < 0xC) {
            q++;
        } else {
            k = 0;
        }
        if (a == p[0] && b == k) {
            len = getkbuflen(q, end) + 6;
            if (len & 1) {
                len++;
            }
            set_wds(ins_wds(buf, rt, len, *pos), p, end, tag | (p - ent));
            *pos += len;
            (*cnt)++;
        }
        p = next_wd(q, end);
    }
}

u8 *ins_wds(u8 *p, int rt, int len, int total)
{
    u8 *end;
    u8 *s;

    end = p + total;
    while (p < end) {
        if (*(s16 *)(p + 2) >= rt) {
            break;
        }
        p += 5;
        while (*p++ != 0) {
        }
        if ((u32)p & 1) {
            p++;
        }
    }
    s = end - 1;
    if (p < end) {
        while (s >= p) {
            s[len] = *s;
            s--;
        }
    }
    return p;
}

int dic_learn(s64 id, int code, s64 *list, int n)
{
    int off;
    int tmp;
    int rt;
    int page;
    int c;
    int istmp;
    u8 *base;
    u8 *e;

    if (gaku_mode == 0) {
        return 3;
    }
    if (dic_fd == -1) {
        return -3;
    }
    if (id == 0) {
        c = code & 0xFFFF;
        if (c >= 0 && c < 4) {
            suji_mode = c;
            clear_allrtime(list, n);
            update_entid_rtime(list, n, 0);
            return 3;
        }
    }
    page = get_entid_tab(&off, &tmp, &rt);
    if (page == -1) {
        return 0;
    }
    c = code & 0xFFFF;
    istmp = c & 0x8000;
    if (istmp != 0) {
        if (tmp == -1) {
            return 0;
        }
        base = load_temp(tmp, -1);
    } else {
        if (off == -1) {
            return 0;
        }
        base = load_page(page, -1) + off;
    }
    rt = get_maxtime(list, n);
    e = base + (c & 0xFFFF7FFF);
    if (e[1] != 0 && e[1] == rt) {
        if (istmp != 0) {
            shift_temp(tmp);
            update_nowtmp();
        }
        return 3;
    }
    rt = rt + 1;
    if (rt == 0xFF) {
        clear_allrtime(list, n);
        rt = 1;
    }
    e[1] = rt;
    if (istmp != 0) {
        shift_temp(tmp);
        update_nowtmp();
    } else {
        update_nowpage();
    }
    update_entid_rtime(list, n, rt);
    return 3;
}

void clear_rtime(u8 *ent)
{
    u8 *end;
    u8 *p;

    end = ent + ELEN(ent);
    p = ent + ent[2] + 3;
    if (p < end) {
        do {
            p[1] = 0;
            p += 2;
            if (*p < 0xC) {
                p++;
            }
            p = next_wd(p, end);
        } while (p < end);
    }
}

void clear_allrtime(s64 *list, int n)
{
    int off;
    int tmp;
    int unused;
    int page;
    int i;

    for (i = 0; i < n; i++) {
        page = get_entid_tab(list[i], &off, &tmp, &unused);
        if (page != -1) {
            if (off != -1) {
                clear_rtime(load_page(page, -1) + off);
                update_nowpage();
            }
            if (tmp != -1) {
                clear_rtime(load_temp(tmp));
                update_nowtmp();
            }
        }
    }
}

int max_rtime(u8 *ent)
{
    int m;
    u8 *end;
    u8 *p;

    end = ent + ELEN(ent);
    p = ent + ent[2] + 3;
    m = 0;
    if (p < end) {
        do {
            if (m < p[1]) {
                m = p[1];
            }
            p += 2;
            if (*p < 0xC) {
                p++;
            }
            p = next_wd(p, end);
        } while (p < end);
    }
    return m;
}

int dic_touroku(WD *w)
{
    int isnew;
    int need;
    int c;
    s16 klen;
    u8 buf[0x30];
    u8 *base;
    u8 *end;
    u8 *e;

    isnew = 1;
    if (dic_fd == -1) {
        return -3;
    }
    if (dic_rw == 0x8000) {
        return -6;
    }
    strncpy(buf, *(u8 **)w, ((s16 *)w)[2]);
    buf[((s16 *)w)[2]] = 0;
    base = load_page(srch_page(buf));
    end = end_page(base);
    e = base;
    while (e < end) {
        klen = e[2];
        c = ask_strncmp(e + 3, buf, klen);
        if (c == 0) {
            if (klen == ((s16 *)w)[2]) {
                isnew = 0;
                break;
            }
        } else if (c > 0) {
            break;
        }
        e += ELEN(e);
    }
    if (isnew != 0) {
        need = newwdlen(w);
    } else {
        need = updwdlen(w);
    }
    if ((u32)(base + 0x3FE - need) < (u32)end) {
        return -5;
    }
    if (isnew != 0) {
        shiftpage(e, end + 2, need);
        set_record(e, need, w, 0);
        if (((s16 *)w)[2] < 3) {
            set_entry2(buf, ((s16 *)w)[2]);
        }
    } else {
        shiftpage(e + ELEN(e), end + 2, need);
        upd_record(e, need, w, max_rtime(e) + 1);
    }
    init_entid_tab();
    update_nowpage();
    return 3;
}

u8 *end_page(u8 *p)
{
    int n;

    n = ELEN(p);
    while (n != 0) {
        p += n;
        n = ELEN(p);
    }
    return p;
}

void shiftpage(u8 *from, u8 *end, int d)
{
    u8 *s;

    if (d > 0) {
        s = end - 1;
        while (s >= from) {
            s[d] = *s;
            s--;
        }
    } else if (d < 0) {
        while (from < end) {
            from[d] = *from;
            from++;
        }
    }
}

int dic_delete(WD *w)
{
    int none;
    int c;
    int d;
    int r;
    s16 klen;
    u8 buf[0x30];
    u8 *e;
    u8 *end;

    none = 1;
    if (dic_fd == -1) {
        return -3;
    }
    if (dic_rw == 0x8000) {
        return -6;
    }
    strncpy(buf, w->yomi, w->len);
    buf[w->len] = 0;
    e = load_page(srch_page(buf));
    end = end_page(e);
    while (e < end) {
        klen = e[2];
        c = ask_strncmp(e + 3, buf, klen);
        if (c == 0) {
            if (klen == w->len) {
                none = 0;
                break;
            }
        } else if (c > 0) {
            break;
        }
        e += ELEN(e);
    }
    if (none != 0) {
        return 0;
    }
    r = delwd(e, w);
    if (r == 0) {
        return 0;
    }
    if (r == -1) {
        d = ELEN(e);
        shiftpage(e + d, end + 2, -d);
    } else {
        shiftpage(e + ELEN(e) + r, end + 2, -r);
    }
    init_entid_tab();
    update_nowpage();
    return 3;
}

int delwd(u8 *ent, WD *w)
{
    u8 buf[0x30];
    int len;
    int tot;
    int n;
    int cl;
    u8 *end;
    u8 *p;
    u8 *q;
    u8 *start;

    setkbuf(w->tango, buf);
    len = setkbuflen(w->tango);
    buf[len] = 0;
    tot = ELEN(ent);
    end = ent + tot;
    p = ent + ent[2] + 3;
    start = p;
    while (p < end) {
        q = p + 2;
        if (p[2] < 0xC) {
            q++;
        }
        p = next_wd(q, end);
        cl = p - q;
        if (*start == w->x07 && cl == len && ask_strncmp(q, buf, cl) == 0) {
            break;
        }
        start = p;
    }
    n = p - start;
    if (n > 0) {
        if (start == ent + ent[2] + 3 && p == end) {
            return -1;
        }
        shiftpage(p, end, -n);
        tot -= n;
        ent[0] = tot % 256;
        ent[1] = tot / 256;
    }
    return n;
}

int dic_tmptouroku(WD *w)
{
    u8 buf[0x50];

    if (gaku_mode == 0) {
        return 3;
    }
    if (dic_fd == -1) {
        return -3;
    }
    strncpy(buf, w->yomi, w->len);
    if (w->x07 == 0x28 || w->x07 == 0x29) {
        w->x07 = 0x27;
    }
    buf[w->len] = 0;
    tmp_touroku(buf, w, 1);
    return 3;
}

int dic_newlearn(WD *w, s64 *list, int n)
{
    u8 buf[0x50];
    int rt;

    if (gaku_mode == 0) {
        return 3;
    }
    if (dic_fd == -1) {
        return -3;
    }
    strncpy(buf, w->yomi, w->len);
    buf[w->len] = 0;
    if (isnum(buf) != 0) {
        w->x07 = 0x1F;
    }
    rt = get_maxtime(list, n);
    rt++;
    if (rt == 0xFF) {
        clear_allrtime(list, n);
        rt = 1;
    }
    tmp_touroku(buf, w, rt);
    update_entid_rtime(list, n, rt);
    return 3;
}

int isnum(u8 *p)
{
    u8 v;

    for (v = *p; v != 0; v = *++p) {
        if ((v & 0xFF) < 0x30 || (v & 0xFF) > 0x39) {
            return 0;
        }
    }
    return 1;
}

int dic_freeentid()
{
    free_entid_tab();
    return 3;
}

int dic_getgaku(void)
{
    return gaku_mode;
}

int dic_get1num(u8 *s, int len, u8 *out)
{
    if (dic_fd == -1) {
        return -3;
    }
    if (set_num(s, len, out, suji_mode) == 0) {
        set_num(s, len, out, 0);
    }
    return 1;
}

u8 *set_num(s, n, out, kind)
u8 *s;
s16 n;
u8 *out;
int kind;
{
    u8 *p;
    u8 *q;
    int i;
    int r;
    int d;
    int idx;
    int k;
    int g;

    if (kind >= 2 && n > 13) {
        return 0;
    }
    *(s16 *)out = kind;
    *(s16 *)(out + 2) = 0;
    out[4] = 0;
    p = out + 5;
    q = p;
    for (i = 0; i < n; i++) {
        d = s[i] - 0x30;
        if (kind < 2) {
            p[0] = num_chars[kind][d * 2];
            p[1] = num_chars[kind][d * 2 + 1];
            p += 2;
        } else {
            r = n - i - 1;
            g = r % 4;
            if (d != 0 && (kind != 2 || d != 1 || (u32)(g - 1) > 1)) {
                if (d <= 0) {
                    k = 1;
                } else if (d < 4) {
                    k = kind - 1;
                } else {
                    k = 1;
                }
                p[0] = num_chars[k][d * 2];
                p[1] = num_chars[k][d * 2 + 1];
                p += 2;
            }
            if (r != 0 && (g != 0 || q != p) && (g == 0 || d != 0)) {
                idx = r / 4;
                if (g == 0) {
                    idx = idx + 6;
                } else if (g == 1) {
                    idx = 0;
                    if (kind == 2) {
                        idx = 4;
                    }
                } else {
                    idx = g + 3;
                }
                p[0] = num_chars[2][idx * 2];
                p[1] = num_chars[2][idx * 2 + 1];
                p += 2;
                if (g == 0) {
                    q = p;
                }
            }
        }
    }
    if (p == out + 5) {
        return 0;
    }
    *p = 0;
    return p + 1;
}

int dic_getallnum(u8 *s, int len, u8 *out, int *cnt)
{
    int k;
    u8 *r;

    if (dic_fd == -1) {
        return -3;
    }
    *cnt = 0;
    r = set_num(s, len, out, suji_mode);
    if (r != 0) {
        out = r;
        (*cnt)++;
    }
    for (k = 0; k < 4; k++) {
        if (k != suji_mode) {
            r = set_num(s, len, out, k);
            if (r != 0) {
                out = r;
                (*cnt)++;
            }
        }
    }
    return 1;
}

int ask_strncmp(u8 *a, u8 *b, int n)
{
    int d;
    u8 c;

    n--;
    while (n != -1) {
        c = *a;
        d = c - *b;
        if (d != 0) {
            return d;
        }
        if (c == 0) {
            return 0;
        }
        a++;
        b++;
        n--;
    }
    return 0;
}

int d_open()
{
    return FAskRom_Open();
}

int d_read()
{
    return FAskRom_Read();
}

int d_write()
{
    return FAskRom_Write();
}

int d_close()
{
    return FAskRom_Close();
}

int d_seek()
{
    return FAskRom_Seek();
}

void set_dicname(u8 *name)
{
    strcpy(mydicname, name);
}

int open_dic(void)
{
    int fd;

    fd = d_open(mydicname, dic_rw);
    if (fd == -1) {
        dic_fd = -1;
        return -1;
    }
    dic_fd = fd;
    return 0;
}

int close_dic(void)
{
    if (d_close(dic_fd) == -1) {
        dic_fd = -1;
        return -1;
    }
    dic_fd = -1;
    return 0;
}

int seek_dic(pos)
int pos;
{
    if (d_seek(dic_fd, pos, 0) == -1) {
        if (open_dic() != 0) {
            return -1;
        }
        if (d_seek(dic_fd, pos, 0) == -1) {
            return -1;
        }
    }
    return 0;
}

void init_page(void)
{
    init_page_tab();
    init_entid_tab();
}

int read_head(void)
{
    if (seek_dic(0) == -1) {
        return -1;
    }
    if (d_read(dic_fd, header, 0x400) != 0x400) {
        return -1;
    }
    if (ask_strncmp(header, title, strlen(title)) != 0) {
        return -1;
    }
    mainlower = 0;
    mainupper = header[0x2E] + (header[0x2F] << 8);
    temp_page = header[0x30] + (header[0x31] << 8);
    old_temp = temp_page;
    entry2upd = 0;
    suji_mode = header[0x34] + (header[0x35] << 8);
    old_suji = suji_mode;
    return 0;
}

int flush_head(void)
{
    u8 *p;

    p = header + 0x30;
    if (temp_page == old_temp && suji_mode == old_suji && entry2upd == 0) {
        return 0;
    }
    *p++ = temp_page % 256;
    *p++ = temp_page / 256;
    *p++ = gaku_mode % 256;
    *p++ = gaku_mode / 256;
    *p++ = suji_mode % 256;
    *p = suji_mode / 256;
    if (seek_dic(0) == -1) {
        return -1;
    }
    if (d_write(dic_fd, header, 0x400) != 0x400) {
        return -1;
    }
    return 0;
}

int read_index(void)
{
    if (seek_dic(0x400) == -1) {
        return -1;
    }
    if (d_read(dic_fd, mainindex, 0x1000) != 0x1000) {
        return -1;
    }
    return 0;
}

int chk_entry2(u8 *key, int len)
{
    int b;
    int r;

    if (key[0] < 0xA1) {
        return 0;
    }
    if ((s16)len == 1) {
        b = 0;
    } else {
        if (key[1] < 0xA1) {
            return 0;
        }
        b = key[1] - 0xA0;
    }
    r = 1;
    if ((1 << (b & 7)) & entry2code[(b >> 3) + (key[0] - 0xA1) * 0xB]) {
        r = 0;
    }
    return r;
}

void set_entry2(u8 *key, int len)
{
    int b;

    if (key[0] >= 0xA1) {
        if ((s16)len == 1) {
            b = 0;
        } else {
            if (key[1] < 0xA1) {
                return;
            }
            b = key[1] - 0xA0;
        }
        entry2code[(b >> 3) + (key[0] - 0xA1) * 0xB] |= (1 << (b & 7)) & 0xFF;
        entry2upd = 1;
    }
}

int srch_page(u8 *key)
{
    int hi;
    int lo;
    int mid;
    int c;

    lo = mainlower;
    hi = mainupper;
    while (lo + 1 < hi) {
        mid = (hi + lo) / 2;
        c = ask_strncmp(key, mainindex + mid * 4, 4);
        if (c == 0) {
            return mid;
        }
        if (c > 0) {
            lo = mid;
        } else {
            hi = mid;
        }
    }
    if (ask_strncmp(key, mainindex + lo * 4, 4) < 0) {
        lo--;
    }
    return lo;
}

int page_fix(int page, u8 *key)
{
    return key[prefix(mainindex + (page + 1) * 4, key, 4)] != 0;
}

int calc_pulen(int page, u8 *key)
{
    int n;

    n = prefix(mainindex + page * 4, key, 4);
    if (n >= 4) {
        return n;
    }
    if (key[n] == 0) {
        return n;
    }
    return n + 1;
}

int prefix(u8 *a, u8 *b, int n)
{
    int i;

    i = 0;
    while (i < n) {
        if (*a != *b || *a == 0) {
            break;
        }
        i++;
        a++;
        b++;
    }
    return i;
}

void init_page_tab(void)
{
    PAGE *p;

    page_top = page_tab;
    for (p = page_tab; p < page_tab + 9; p++) {
        p->id = -1;
        p->dirty = 0;
        p->next = p + 1;
    }
    p->id = -1;
    p->dirty = 0;
    p->next = 0;
}

int write_page(PAGE *p)
{
    if (seek_dic(((s64)p->id << 10) + 0x3400) == -1) {
        return -1;
    }
    if (d_write(dic_fd, p->data, 0x400) != 0x400) {
        return -1;
    }
    return 0;
}

int read_page(PAGE *p)
{
    if (seek_dic(((s64)p->id << 10) + 0x3400) == -1) {
        return -1;
    }
    d_read(dic_fd, p->data, 0x400);
    return 0;
}

u8 *load_page(int id)
{
    PAGE *p;
    PAGE *prev;

    prev = 0;
    p = page_top;
    for (;;) {
        if (p->id == id) {
            if (prev != 0) {
                prev->next = p->next;
                p->next = page_top;
                page_top = p;
            }
            return p->data;
        }
        if (p->next == 0) {
            break;
        }
        prev = p;
        p = p->next;
    }
    prev->next = 0;
    p->next = page_top;
    page_top = p;
    if (p->dirty == 1) {
        write_page(p);
    }
    p->id = id;
    p->dirty = 0;
    read_page(p);
    return p->data;
}

void update_nowpage(void)
{
    page_top->dirty = 1;
}

void flush_pages(void)
{
    PAGE *p;

    for (p = page_top; p != 0; p = p->next) {
        if (p->dirty == 1) {
            write_page(p);
        }
    }
}

void init_entid_tab(void)
{
    int i;

    for (i = 0; i < 128; i++) {
        entid_tab[i].cnt = 0;
    }
}

int set_entid_tab(int a, int b, int c, int rt)
{
    int free;
    int i;

    free = -1;
    for (i = 0; i < 128; i++) {
        if (entid_tab[i].cnt == 0) {
            if (free == -1) {
                free = i;
            }
        } else if (entid_tab[i].a == (u16)a && entid_tab[i].b == (s16)b && entid_tab[i].c == (s16)c) {
            entid_tab[i].cnt++;
            return i;
        }
    }
    if (free == -1) {
        return -1;
    }
    entid_tab[free].cnt = 1;
    entid_tab[free].a = a;
    entid_tab[free].b = b;
    entid_tab[free].c = c;
    entid_tab[free].rtime = rt;
    return free;
}

int get_entid_tab(unsigned long id, int *b, int *c, int *rt)
{
    ENTID *e;

    if (id >= 0x80) {
        return -1;
    }
    e = &entid_tab[id];
    if (e->cnt == 0) {
        return -1;
    }
    *b = e->b;
    *c = e->c;
    *rt = e->rtime;
    return e->a;
}

int get_maxtime(int *ids, int n)
{
    int i;
    int m;
    int id;

    m = 0;
    for (i = 0; i < n; i++) {
        id = ids[i * 2];
        if (id >= 0 && id < 0x80) {
            if (entid_tab[id].cnt > 0) {
                if (m < entid_tab[id].rtime) {
                    m = entid_tab[id].rtime;
                }
            }
        }
    }
    return m;
}

int update_entid_rtime(int *ids, int n, int rt)
{
    int i;
    int id;

    for (i = 0; i < n; i++) {
        id = ids[i * 2];
        if (id >= 0 && id < 0x80) {
            if (entid_tab[id].cnt > 0) {
                entid_tab[id].rtime = rt;
            }
        }
    }
    return 0;
}

int free_entid_tab(unsigned int id)
{
    ENTID *e;
    s64 i;

    if (id >= 0x80) {
        return -1;
    }
    i = (int)id;
    e = &entid_tab[i];
    if (e->cnt == 0) {
        return -1;
    }
    e->cnt--;
    return 0;
}

void clear_entid_tmpall(int pg)
{
    int i;

    for (i = 0; i < 128; i++) {
        if (pg == (s16)(entid_tab[i].c >> 12)) {
            entid_tab[i].c = -1;
        }
    }
}

void clear_entid_tmp(int v)
{
    int i;

    for (i = 0; i < 128; i++) {
        if (v == entid_tab[i].c) {
            entid_tab[i].c = -1;
        }
    }
}

void init_temp(void)
{
    init_node_tab();
    init_hash_tab();
    if (read_temp() == -1) {
        reset_temp();
    }
    temp_updated = 0;
}

void flush_temp(void)
{
    if (temp_updated != 0) {
        write_temp();
    }
}

void init_node_tab(void)
{
    NODE *n;

    for (n = node_tab; n < node_tab + 511; n++) {
        n->next = n + 1;
    }
    n->next = 0;
    freelist = node_tab;
}

void init_hash_tab(void)
{
    int i;

    for (i = 0; i < 80; i++) {
        hash_tab[i] = 0;
    }
}

void reset_temp(void)
{
    int i;

    for (i = 0; i < 8; i++) {
        temp_pages[i][0] = 0;
        temp_pages[i][1] = 0;
    }
    temp_top = temp_pages[0];
    temp_end = temp_pages[1];
    temp_page = 0;
}

int hashfunc(u8 *key)
{
    if (key[0] >= 0xA0 && key[0] < 0xF0) {
        return key[0] - 0xA0;
    }
    return 0;
}

NODE *alloc_node(void)
{
    NODE *n;

    if (freelist == 0) {
        page_gc();
    }
    n = freelist;
    freelist = n->next;
    return n;
}

void free_node(NODE *n)
{
    n->next = freelist;
    freelist = n;
}

u8 *alloc_record(int len)
{
    u8 *r;

    if ((u32)temp_top > (u32)(temp_end - len - 2)) {
        page_gc();
    }
    r = temp_top;
    temp_top += len;
    temp_top[0] = 0;
    temp_top[1] = 0;
    return r;
}

NODE **srch_node(u8 *key, int len, NODE **found)
{
    NODE *prev;
    NODE *n;
    int h;
    int klen;
    int c;
    u8 *e;

    prev = 0;
    h = hashfunc(key);
    n = hash_tab[h];
    while (n != 0) {
        e = n->rec;
        klen = (s16)e[2];
        c = ask_strncmp(e + 3, key, klen);
        if (c == 0) {
            if ((s16)len == (s16)klen) {
                break;
            }
        } else if (c > 0) {
            break;
        }
        prev = n;
        n = n->next;
    }
    *found = n;
    if (prev == 0) {
        return &hash_tab[h];
    }
    return &prev->next;
}

void page_gc(void)
{
    NODE *nd;
    NODE **link;
    u8 *p;
    s16 klen;
    u8 key[0x50];

    *temp_top = 0;
    temp_top++;
    *temp_top = 0;
    temp_page = (temp_page + 1) % 8;
    temp_top = temp_pages[temp_page];
    p = temp_top;
    temp_end = p + 0x400;
    while (ELEN(p) != 0) {
        klen = p[2];
        if (klen != 0) {
            strncpy(key, p + 3, klen);
            key[klen] = 0;
            link = srch_node(key, klen, &nd);
            if (nd->rec == p) {
                *link = nd->next;
                free_node(nd);
            }
        }
        p += ELEN(p);
    }
    clear_entid_tmpall(temp_page);
}

int tmpoffset(u8 *p)
{
    int d;

    d = p - temp_pages[0];
    return ((d / 1024) << 12) | (d % 1024);
}

u8 *load_temp(int off)
{
    return temp_pages[(s16)(off >> 12)] + (s16)(off & 0xFFF);
}

int read_temp(void)
{
    NODE *nd;
    NODE **link;
    NODE *n;
    u8 *pg;
    u8 *p;
    int i;
    s16 klen;
    u8 key[0x50];

    if (seek_dic(0x1400) == -1) {
        return -1;
    }
    if (d_read(dic_fd, temp_pages, 0x2000) != 0x2000) {
        return -1;
    }
    pg = temp_pages[0];
    for (i = 0; i < 8; i++) {
        p = pg;
        while (ELEN(p) != 0) {
            klen = p[2];
            if (klen != 0) {
                strncpy(key, p + 3, klen);
                key[klen] = 0;
                link = srch_node(key, klen, &nd);
                n = alloc_node();
                n->rec = p;
                n->next = nd;
                *link = n;
            }
            p += ELEN(p);
        }
        if (temp_page == i) {
            temp_top = p;
            temp_end = pg + 0x400;
        }
        pg += 0x400;
    }
    return 0;
}

int write_temp(void)
{
    if (seek_dic(0x1400) == -1) {
        return -1;
    }
    if (d_write(dic_fd, temp_pages, 0x2000) != 0x2000) {
        return -1;
    }
    return 0;
}

int newwdlen(WD *w)
{
    int extra;

    if (w->x08 != 0 || w->x07 >= 0x2D) {
        extra = 3;
    } else {
        extra = 2;
    }
    return w->len + 3 + extra + setkbuflen(w->tango);
}

int updwdlen(WD *w)
{
    int extra;

    if (w->x08 != 0 || w->x07 >= 0x2D) {
        extra = 3;
    } else {
        extra = 2;
    }
    return extra + setkbuflen(w->tango);
}

void set_record(u8 *r, int len, WD *w, int rt)
{
    u8 *p;
    u8 *q;

    r[0] = len % 256;
    r[1] = len / 256;
    r[2] = w->len;
    p = r + 3;
    strncpy(p, w->yomi, w->len);
    q = p + w->len;
    q[0] = w->x07;
    q[1] = rt;
    p = q + 2;
    if (w->x08 != 0 || w->x07 >= 0x2D) {
        q[2] = w->x08;
        p++;
    }
    setkbuf(w->tango, p);
}

void upd_record(u8 *r, int add, WD *w, int rt)
{
    int old;
    int total;
    u8 *q;

    old = ELEN(r);
    total = add + old;
    r[0] = total % 256;
    r[1] = total / 256;
    q = r + old;
    q[0] = w->x07;
    q[1] = rt;
    if (w->x08 != 0 || w->x07 >= 0x2D) {
        q[2] = w->x08;
    }
    setkbuf(w->tango, q + 2 + ((w->x08 != 0 || w->x07 >= 0x2D) ? 1 : 0));
}

int tmp_touroku(u8 *key, WD *w, int rt)
{
    NODE *nd;
    NODE **link;
    NODE *n;
    u8 *rec;
    int need;
    int len;

    temp_updated = 1;
    link = srch_node(key, w->len, &nd);
    len = w->len;
    rec = nd->rec;
    if (rec[2] == len && ask_strncmp(key, rec + 3, len) == 0) {
        rec[2] = 0;
        *link = nd->next;
        clear_entid_tmp(tmpoffset(nd->rec));
        free_node(nd);
    }
    n = alloc_node();
    rec = alloc_record(need = newwdlen(w));
    set_record(rec, need, w, rt);
    link = srch_node(key, w->len, &nd);
    n->rec = rec;
    n->next = nd;
    *link = n;
    return 0;
}

int tmp_snssyn(u8 *key, int len0, SRCH *r)
{
    s16 len;
    NODE *n;
    u8 *e;
    u8 *hit;
    s16 best;
    s16 maxp;
    s16 klen;
    s16 pre;
    int c;

    hit = 0;
    maxp = 0;
    best = 0;
    len = len0;
    r->x06 = 0;
    r->x04 = 0;
    n = hash_tab[hashfunc(key)];
    while (n != 0) {
        e = n->rec;
        klen = e[2];
        pre = prefix(e + 3, key, klen);
        if (pre >= maxp) {
            maxp = pre + 1;
        }
        c = ask_strncmp(e + 3, key, klen);
        if (c == 0) {
            best = klen;
            if (best == len) {
                return 2;
            }
            hit = e;
        } else if (c > 0) {
            if (ask_strncmp(e + 3, key, len) == 0) {
                return 2;
            }
            break;
        }
        n = n->next;
    }
    if (best == 0) {
        r->off = -1;
        return 0;
    }
    r->page = 0;
    r->off = tmpoffset(hit);
    r->x04 = best;
    r->x06 = maxp;
    r->ent = hit;
    return 1;
}

int tmp_getsyn(u8 *key, int len, SRCH *r)
{
    NODE *n;
    u8 *e;
    s16 klen;
    int c;

    e = 0;
    n = hash_tab[hashfunc(key)];
    while (n != 0) {
        e = n->rec;
        klen = e[2];
        c = ask_strncmp(e + 3, key, klen);
        if (c == 0) {
            if (klen == (s16)len) {
                break;
            }
        } else if (c > 0) {
            r->off = -1;
            return 0;
        }
        n = n->next;
    }
    if (n == 0) {
        r->off = -1;
        return 0;
    }
    r->page = 0;
    r->off = tmpoffset(e);
    r->ent = e;
    return 1;
}

void shift_temp(int off)
{
    NODE *nd;
    u8 key[0x50];
    u8 *src;
    u8 *dst;
    u8 *old;
    int len;
    s16 pg;
    s16 klen;

    pg = off >> 12;
    if (temp_page != pg) {
        old = load_temp(temp_page);
        src = old;
        len = ELEN(old);
        dst = alloc_record(len);
        if (pg != temp_page) {
            klen = src[2];
            strncpy(key, src + 3, klen);
            key[klen] = 0;
            srch_node(key, klen, &nd);
            if (nd->rec == src) {
                nd->rec = dst;
                len--;
                while (len-- != 0) {
                    *dst++ = *src++;
                }
                old[2] = 0;
                clear_entid_tmp(off);
            }
        }
    }
}

void update_nowtmp(void)
{
    temp_updated = 1;
}

int setkbuflen(u8 *p)
{
    int n;

    n = 0;
    while (*p != 0) {
        if (iskanji(*p) != 0) {
            p += 2;
        } else {
            p += 1;
        }
        n += 2;
    }
    return n;
}

void setkbuf(u8 *src, u8 *dst)
{
    while (*src != 0) {
        if (iskanji(*src) != 0) {
            *dst = *src;
            src++;
            dst++;
        } else {
            *dst = 0xFF;
            dst++;
        }
        *dst = *src;
        src++;
        dst++;
    }
}

int getkbuflen(u8 *p, u8 *end)
{
    int n;

    n = 0;
    while (p < end && *p >= 0x39) {
        if (*p == 0xFF) {
            n++;
        } else {
            n += 2;
        }
        p += 2;
    }
    return n;
}

void getkbuf(u8 *dst, u8 *src, u8 *end)
{
    while (src < end && *src >= 0x39) {
        if (*src == 0xFF) {
            src++;
        } else {
            *dst = *src;
            src++;
            dst++;
        }
        *dst = *src;
        src++;
        dst++;
    }
    *dst = 0;
}

int iskanji(int c)
{
    c = c & 0xFF;
    if ((c >= 0x80 && c <= 0x9F) || (c >= 0xE0 && c <= 0xFC)) {
        return 1;
    }
    return 0;
}

void init_hchar(void)
{
    HCHAR *h;

    for (h = hchar; (u8 *)h < (u8 *)wdsbuf; h++) {
        clear_hchar(h);
    }
}

void clear_hchar(HCHAR *h)
{
    h->x00 = -1;
    h->ch = 0;
    h->bs = 0;
    h->kh = 0;
    h->x10 = 0;
    h->x14 = 0;
    h->x15 = 0;
    h->x16 = 0;
    h->x17 = -1;
    h->x18 = -1;
    h->x19 = -1;
}

void free_hchar(int from, int to, int keep)
{
    HCHAR *h;
    HCHAR *end;

    end = hchar + to;
    for (h = hchar + from; h < end; h++) {
        free_hchar_one(h, keep);
    }
}

void free_hchar_one(HCHAR *h, int keep)
{
    if (keep == 0) {
        h->x00 = -1;
        h->x18 = -1;
        if (h->ch != (void *)-1) {
            free_chmemlist(h->ch);
        }
        h->ch = 0;
        h->x17 = -1;
        h->x19 = -1;
        h->x16 = 0;
    }
    if (h->bs != 0 && h->bs != (BS *)-1) {
        free_bsmemlist(h->bs);
    }
    h->bs = 0;
    if (h->kh != 0) {
        free_khmemlist(h->kh);
    }
    h->kh = 0;
    h->x10 = 0;
    h->x14 = 0;
    h->x15 = 0;
}

void henkan(int start, int end, int mode, int pref)
{
    int pos;
    int p;
    int total;
    int a;
    int len;
    int top;
    HCHAR *h;
    HCHAR *hs;
    BS *b;
    int sel;
    int cur;

    henkan_mode = mode;
    fl_check();
    if (henkan_mode != 3 || ikkatsu_mode == 0) {
        pos = start;
        if (start < end) {
            top = start + pref;
            do {
                h = &hchar[pos];
                if (h->x15 > 0) {
                    pos += h->x15;
                    continue;
                }
                if (h->ch == 0 && ch_check(pos, end) == 0) {
                    break;
                }
                if (h->bs == 0 && bs_check(pos, end) == 0) {
                    break;
                }
                for (b = h->bs; b != 0; b = b->next) {
                    p = pos + b->len;
                    if (p >= end) {
                        if (henkan_mode >= 3) {
                            if (b->x02 == 0xFF) {
                                henkan_mode = 0;
                            }
                        }
                        continue;
                    }
                    if (hchar[p].ch == 0 && ch_check(p, end) == 0) {
                        break;
                    }
                    if (hchar[p].bs == 0 && bs_check(p, end) == 0) {
                        break;
                    }
                }
                if (pos == start) {
                    if (henkan_mode == 1) {
                        bs_prefer(pos, end, pref);
                        sel = pref;
                        hchar[start].x14 = 1;
                    } else if (henkan_mode == 2) {
                        hchar[start].x14 = 1;
                        sel = pref;
                    } else {
                        goto prefer;
                    }
                } else {
                    if (pos == top && henkan_mode == 2) {
                        bs_prefix(pos);
                    }
prefer:
                    a = bs_prefer(pos, end, -1);
                    if (a == -1) {
                        break;
                    }
                    sel = a;
                }
                unify_bsmem(pos, sel);
                first_kouho(pos, sel);
                h->x15 = sel;
                pos += sel;
            } while (pos < end);
        }
        pos = start;
        while (pos < end) {
            hs = &hchar[pos];
            h = hs;
            cur = hs->x15;
            if (hs->x14 != 0) {
                len = cur;
            } else {
                len = concat_bslen(pos, end);
                if (len <= -1) {
                    break;
                }
                if (len == 0) {
                    len = cur;
                }
                if (len == cur) {
                    hs->x14 = 1;
                } else {
                    while (cur < len) {
                        if (h->bs != (BS *)-1) {
                            free_bsmemlist(h->bs);
                        }
                        h->bs = 0;
                        h->x15 = 0;
                        free_khmemlist(h->kh);
                        h->kh = 0;
                        h = &hchar[pos + cur];
                        cur += h->x15;
                    }
                    if (h->bs != (BS *)-1) {
                        free_bsmemlist(h->bs);
                    }
                    h->bs = 0;
                    h->x15 = 0;
                    free_khmemlist(h->kh);
                    h->kh = 0;
                    hs->x15 = len;
                    b = alloc_bsmem();
                    if (b != 0) {
                        b->len = len;
                        b->x02 = 0x28;
                        b->x03 = 0;
                        b->pw = 0;
                        b->x08 = 0;
                        b->x0A = 0;
                        b->next = 0;
                        hs->bs = b;
                        hs->x14 = 1;
                        first_kouho(pos, len);
                    }
                }
            }
            pos += len;
        }
    }
}

int concat_bslen(int pos, int end)
{
    int n;
    s8 c;
    HCHAR *h;

    c = 0;
    n = 0;
    h = &hchar[pos];
    while (pos < end) {
        c = h->x15;
        if (c == 0) {
            return -1;
        }
        if (h->bs != 0 && h->bs != (BS *)-1 && h->bs->x02 != 0x28) {
            break;
        }
        pos += c;
        n += c;
        h += c;
    }
    if (n == 0 || pos >= end) {
        return n;
    }
    return n + c;
}

int muhenkan(int pos, int end)
{
    int k;
    u8 *p;

    k = pos + 1;
    if (k < end) {
        p = kana_ustr + k;
        do {
            if (not_bhead(*p) == 0) {
                break;
            }
            k++;
            p++;
        } while (k < end);
    }
    return k - pos;
}

void fl_check(int pos, int end)
{
    int n;
    void *found;
    int hit;
    u8 *p;
    HCHAR *h;

    n = end - pos;
    p = kana_ustr + pos;
    h = &hchar[pos];
    while (n > 0) {
        if (h->x00 == -1) {
            found = srch_pword(p, n, &hit);
            if (found != (void *)-1) {
                h->x18 = hit;
                h->x00 = (int)found;
            }
        }
        n--;
        p++;
        h++;
    }
}

int ch_check(int pos, int end)
{
    int n;
    int e;
    int k;
    int kind;
    int r;
    u8 *p;
    HCHAR *h;
    u8 c;
    KANA *kb;

    n = end - pos;
    if (n == 0) {
        return 0;
    }
    kind = 0;
    p = kana_ustr + pos;
    h = &hchar[pos];
    if (is_num(*p)) {
        kind = 0x1F;
        for (k = 1; k < n; k++) {
            if (!is_num(p[k])) {
                break;
            }
        }
        if (henkan_mode >= 3 && k >= n) {
            return 0;
        }
        goto dic;
    }
    if (is_alpha(*p)) {
        kind = 0x19;
        for (k = 1; k < n; k++) {
            if (!is_alphanum(p[k])) {
                break;
            }
        }
        if (henkan_mode >= 3 && k >= n) {
            return 0;
        }
        goto dic;
    }
    if (is_paren(*p)) {
        if (henkan_mode >= 3 && n < 2) {
            return 0;
        }
        end = 1;
        k = 1;
        kind = 0x19;
        goto loop;
    }
    c = *p;
    if ((s8)c != 0 && c < 0xA0) {
        if (henkan_mode >= 3 && n < 2) {
            return 0;
        }
        end = 1;
        k = 1;
        kind = 0x29;
        goto loop;
    }
    if ((s8)c == 0 && (kana_buf[pos].ch & 0xFF00) == 0) {
        k = pos + 1;
        kb = kana_buf + k;
        while (k < end && (kb->ch & 0xFF00) == 0) {
            k++;
            kb++;
        }
        kind = 0x19;
        goto dummy;
    }
    if ((s8)c == 0 && is_kata(kana_buf[pos].ch, 0)) {
        k = pos + 1;
        kb = kana_buf + k;
        while (k < end && is_kata(kb->ch, 1)) {
            k++;
            kb++;
        }
        kind = 0x19;
        goto dummy;
    }
    if ((s8)c == 0) {
        k = pos + 1;
        p++;
        while (k < end && *p == 0) {
            k++;
            p++;
        }
        kb = kana_buf + k - 1;
        if (is_jisknj(kb->ch)) {
            kind = 0x28;
        } else if (is_jiskig(kb->ch)) {
            kind = 0x29;
        } else {
            kind = 0x19;
        }
dummy:
        if (henkan_mode >= 3 && k == end) {
            return 0;
        }
        add_dummy_chmem(pos, k - pos, kind);
        return 1;
    }
    k = 1;
dic:
    for (e = 0; e < n; e++) {
        if (p[e] == 0) {
            break;
        }
    }
    r = dic_snssyn(p, e, &entbuf);
    switch (r) {
    case 2:
        if (henkan_mode < 3) {
            goto loop;
        }
        return 0;
    case 1:
        if (kind != 0 && k >= entbuf.x00) {
            dic_freeentid(entbuf.id);
            e = k;
            h->x17 = entbuf.x10;
            goto loop;
        }
        hchar_addchmem(pos, make_chmem(pos, &entbuf));
        h->x17 = entbuf.x10;
        e = entbuf.x00 - 1;
        goto loop;
    default:
        if (kind != 0) {
            e = k;
            goto loop;
        }
        h->ch = (void *)-1;
        return 0;
    }
loop:
    while (e >= k) {
        if (kind != 0 && e == k) {
            add_dummy_chmem(pos, k, kind);
        }
        if (dic_getsyn(p, e, &entbuf) == 1) {
            if (h->x17 == -1) {
                h->x17 = entbuf.x10;
            }
            hchar_addchmem(pos, make_chmem(pos, &entbuf));
        }
        e--;
    }
    return 1;
}

CH *make_chmem(int pos, SYNR *r)
{
    SYN *s;
    CH *first;
    CH *prev;
    CH *c;
    s16 len;
    s64 id;
    int n;

    s = r->syn;
    first = 0;
    prev = 0;
    len = r->x00;
    id = r->id;
    n = r->x14 - 1;
    if (r->x14 != 0) {
        do {
            c = alloc_chmem();
            if (c == 0) {
                break;
            }
            if (first == 0) {
                first = c;
            }
            c->len = len;
            c->x02 = s->x00;
            c->x03 = s->x01;
            c->id = id;
            c->x10 = s->x04;
            c->next = 0;
            if (prev != 0) {
                prev->next = c;
            }
            prev = c;
            s++;
            n--;
        } while (n != 0);
    }
    return first;
}

void hchar_addchmem(pos, c)
int pos;
CH *c;
{
    void **pp;
    CH *p;
    HCHAR *h;

    h = &hchar[pos];
    pp = &h->ch;
    p = h->ch;
    while (p != 0) {
        pp = (void **)&p->next;
        p = p->next;
    }
    *pp = c;
}

void add_dummy_chmem(int pos, int len, int kind)
{
    CH *c;
    s8 k;

    c = alloc_chmem();
    if (c != 0) {
        c->len = len;
        c->x02 = kind;
        c->x03 = 0;
        k = len + 1;
        c->id = 0;
        c->x10 = 0;
        c->next = 0;
        hchar[pos].x17 = k;
        hchar_addchmem(pos, c, k);
    }
}

int bs_check(int pos, int end)
{
    HCHAR *h;
    CH *c;
    BS *r;
    BS *r2;
    BS *b;

    h = &hchar[pos];
    c = h->ch;
    if (c != (CH *)-1 && c != 0) {
        do {
            r = make_bsmem(pos, end, c);
            if (r == (BS *)-1) {
                if (h->bs != 0) {
                    free_bsmemlist(h->bs);
                    h->bs = 0;
                }
                return 0;
            }
            if (r != 0) {
                hchar_addbsmem(pos, r);
            }
            c = c->next;
        } while (c != 0);
    }
    r2 = make_bsmem(pos, end, &null_chmem);
    if (r2 == (BS *)-1) {
        if (h->bs != 0) {
            free_bsmemlist(h->bs);
            h->bs = 0;
        }
        return 0;
    }
    if (r2 != 0) {
        hchar_addbsmem(pos, r2);
    }
    if (h->bs == 0) {
        if ((b = alloc_bsmem()) == 0) {
            return -1;
        }
        b->len = muhenkan(pos, end);
        b->x02 = 0x28;
        b->x03 = 0;
        b->pw = 0;
        b->x08 = 0;
        b->x0A = 0;
        b->next = 0;
        h->bs = b;
        return 1;
    }
    return 1;
}

BS *make_bsmem(int pos, int end, CH *ch)
{
    s16 clen;
    int p;
    PWM *list;
    PWM *l;
    BS *first;
    BS *prev;
    BS *b;

    first = 0;
    clen = ch->len;
    prev = 0;
    p = pos + clen;
    list = pword_list(p, ch->x02, ch->x03);
    if (list == (PWM *)-1) {
        return (BS *)-1;
    }
    l = list;
    while (l != 0) {
        b = alloc_bsmem();
        if (b == 0) {
            break;
        }
        if (first == 0) {
            first = b;
        }
        b->len = clen + l->len;
        b->x02 = l->x04;
        b->x03 = l->x05;
        b->pw = (PW *)ch;
        b->x08 = 0;
        b->x0A = 0;
        b->next = 0;
        if (prev != 0) {
            prev->next = b;
        }
        l = l->next;
        prev = b;
    }
    free_pwmemlist(list);
    if (clen > 0 && setu_end(ch->x02, ch->x03) != 0) {
        if (p >= end || not_bhead(kana_ustr[p]) == 0) {
            b = alloc_bsmem();
            if (b != 0) {
                if (first == 0) {
                    first = b;
                }
                b->len = clen;
                b->x02 = ch->x02;
                b->x03 = ch->x03;
                b->pw = (PW *)ch;
                b->x08 = 0;
                b->x0A = 0;
                b->next = 0;
                if (prev != 0) {
                    prev->next = b;
                }
            }
        }
    }
    return first;
}

static BS *ins_bsmem(BS *list, BS *n)
{
    s16 len;
    BS *prev;
    BS *cur;

    len = n->len;
    if (list == 0 || list->len < len) {
        n->next = list;
        return n;
    }
    cur = list->next;
    prev = list;
    while (cur != 0 && cur->len >= len) {
        prev = cur;
        cur = cur->next;
    }
    prev->next = n;
    n->next = cur;
    return list;
}

void hchar_addbsmem(int pos, BS *list)
{
    BS *l;
    BS *next;
    BS *head;

    head = hchar[pos].bs;
    l = list;
    while (l != 0) {
        next = l->next;
        head = ins_bsmem(head, l);
        l = next;
    }
    hchar[pos].bs = head;
}

void unify_bsmem(int pos, int len)
{
    BS **pp;
    BS *b;

    pp = &hchar[pos].bs;
    b = *pp;
    while (b != 0) {
        if (b->len == len) {
            pp = &b->next;
        } else {
            *pp = b->next;
            free_mem(b);
        }
        b = *pp;
    }
}

int bunsetu_len(pos)
int pos;
{
    HCHAR *h;

    if (pos >= kana_len) {
        return 0;
    }
    h = &hchar[pos];
    if (im_state == 2 && h->x14 == 0) {
        return 0;
    }
    return h->x15;
}

void save_fst_bslen(int pos)
{
    HCHAR *h;

    h = &hchar[pos];
    if (h->x16 == 0 && h->x14 != 0) {
        h->x16 = h->x15;
    }
}

void *srch_pword(u8 *key, int n, int *hit)
{
    u8 *top;
    u8 *e;
    u8 *s;
    u8 *a;
    u8 *b;
    int best;
    int run;
    int ka;
    int kb;
    int idx;

    best = 1;
    if (key[0] < 0xA0) {
        *hit = 1;
        return 0;
    }
    idx = ((key[0] - 0xA0) & 0xFF) * 2;
    top = pword + pwordmap[idx / 2] * 4;
    e = pword + pwordmap[idx / 2 + 1] * 4 - 4;
    while (e >= top) {
        a = key + 1;
        ka = n - 1;
        run = 1;
        s = pluswd + e[2];
        kb = *s - 1;
        b = s + 1;
        for (;;) {
            if (kb == 0) {
                if (best < run) {
                    best = run;
                }
                *hit = best;
                return e;
            }
            if (ka == 0) {
                if (henkan_mode >= 3) {
                    return (void *)-1;
                }
                break;
            }
            run++;
            if (*b != *a) {
                if (best < run) {
                    best = run;
                }
                break;
            }
            a++;
            b++;
            ka--;
            kb--;
        }
        e -= 4;
    }
    *hit = best;
    return 0;
}

PWM *pword_list(int pos, int end, int a2, int a3)
{
    PWM *res;
    PWM *n;
    PWM *sub;
    PWM *t;
    u8 *e;
    u8 *e2;
    u8 *src;
    HCHAR *h;
    int len;
    int kind;
    int a2b;

    res = 0;
    if (pos >= end) {
        if (henkan_mode < 3) {
            return 0;
        }
        return (PWM *)-1;
    }
    h = &hchar[pos];
    e = (u8 *)h->x00;
    if (e == (u8 *)-1) {
        return (PWM *)-1;
    }
    if (e == 0 && is_kuten(kana_ustr[pos]) != 0) {
        if ((a2 & 0xFF) == 0 || setu_end(a2, a3) != 0) {
            n = alloc_pwmem();
            if (n != 0) {
                n->len = 1;
                n->x04 = 0xFF;
                n->x02 = 0xFF;
                n->x05 = 0;
                n->x03 = 0;
                n->next = 0;
            }
            return n;
        }
    }
    kind = a2 & 0xFF;
    src = kana_ustr + pos;
    while (e != 0) {
        if (setu_end(e[0], e[1]) != 0) {
            if (kind == 0 || goku_connect(a2, a3, e[0]) != 0) {
                len = pluswd[e[2]];
                if (pos + len >= end || not_bhead(src[len]) == 0) {
                    n = alloc_pwmem();
                    if (n != 0) {
                        n->len = len;
                        n->x04 = e[0];
                        n->x02 = e[0];
                        n->x05 = e[1];
                        n->x03 = e[1];
                        n->next = res;
                        res = n;
                    }
                }
            }
        }
        if ((s8)e[3] == 0) {
            break;
        }
        e -= (s8)e[3] * 4;
    }
    e2 = (u8 *)h->x00;
    while (e2 != 0) {
        if (kind == 0 || goku_connect(a2, a3, e2[0]) != 0) {
            len = pluswd[e2[2]];
            sub = pword_list(pos + len, end, e2[0], e2[1]);
            if (sub == (PWM *)-1) {
                free_pwmemlist(res);
                return (PWM *)-1;
            }
            t = sub;
            if (sub != 0) {
                for (;;) {
                    t->len += len;
                    t->x02 = e2[0];
                    t->x03 = e2[1];
                    if (t->next == 0) {
                        break;
                    }
                    t = t->next;
                }
                t->next = res;
                res = sub;
            }
        }
        if ((s8)e2[3] == 0) {
            break;
        }
        e2 -= (s8)e2[3] * 4;
        if (e2 == 0) {
            break;
        }
    }
    return res;
}

int not_bhead(int c)
{
    if (henkan_mode == 1 || henkan_mode == 2) {
        return 0;
    }
    switch (c & 0xFF) {
    case 0x9D:
    case 0xA1:
    case 0xA3:
    case 0xA5:
    case 0xA7:
    case 0xA9:
    case 0xC3:
    case 0xE3:
    case 0xE5:
    case 0xE7:
    case 0xEE:
    case 0xF2:
    case 0xF3:
        return 1;
    }
    return 0;
}

int is_kuten(int c)
{
    c = c & 0xFF;
    if (c >= 0xA0) {
        return 0;
    }
    switch (c) {
    case 0x20:
    case 0x21:
    case 0x2C:
    case 0x2E:
    case 0x3A:
    case 0x3B:
    case 0x3F:
    case 0x98:
    case 0x9B:
    case 0x9C:
        return 1;
    default:
        return 0;
    }
}

void first_kouho(int pos, int len)
{
    HCHAR *h;
    BS *b;
    BS *best;
    PW *pw;
    int pri;
    int p;
    KH *kh;
    KH *out;

    best = 0;
    pri = 0;
    h = &hchar[pos];
    if (h->kh == 0) {
        b = h->bs;
        if (b != (BS *)-1) {
            while (b != 0) {
                if (b->len == len) {
                    if (b->pw == 0) {
                        p = 0;
                    } else {
                        p = kh_priority(b, ((CH *)b->pw)->x10) & 0xFFFF;
                    }
                    if ((pri & 0xFFFF) < p || best == 0) {
                        pri = p & 0xFFFF;
                        best = b;
                    }
                }
                b = b->next;
            }
            pw = best != 0 ? best->pw : 0;
            if (best != 0 && pw != 0 && pw->x00 != 0) {
                if (pw->id == 0) {
                    if (pw->x02 == 0x1F && dic_get1num(kana_ustr + pos, pw->x00, (u8 *)wdsbuf) > 0) {
                        kh = create_kouho(wdsbuf, pw, pw->x00, &out);
                    } else {
                        kh = null_kouho(len);
                    }
                } else if (dic_get1wd(pw->id, pw->x02, pw->x03, (u8 *)wdsbuf) > 0) {
                    kh = create_kouho(wdsbuf, pw, pw->x00, &out);
                } else {
                    kh = null_kouho(len);
                }
            } else {
                kh = null_kouho(len);
            }
        } else {
            kh = null_kouho(len);
        }
        h->kh = kh;
    }
}

void init_kouho(int idx, int flag)
{
    KH *k;
    int n;
    int i;

    if (flag == 1) {
        all_kouho();
    }
    k = hchar[cur_pos].kh;
    if (func_mode > 0) {
        kwin_len = 0x50;
    } else {
        kwin_len = kwin_length(cur_pos * 0x1C, cur_pos);
    }
    n = inc_gun(k);
    if (idx >= n) {
        for (;;) {
            idx -= n;
            for (i = n; i != 0; i--) {
                k = kh_followed(k);
            }
            n = inc_gun(k);
            if (n == 0) {
                init_kouho(0, 0);
                break;
            }
            if (idx < n) {
                goto set;
            }
        }
    } else {
set:
        top_kh = k;
        gun_nkh = idx;
        gun_num = n;
    }
    if (flag == 1 && func_mode == 0) {
        disp_kouho();
    }
}

void all_kouho(void)
{
    BS *b;
    KL *head;
    KL *tail;
    KL *n;
    KH *kh;

    b = hchar[cur_pos].bs;
    if (b != (BS *)-1) {
        head = 0;
        tail = 0;
        while (b != 0) {
            if (b->len == cur_len) {
                n = alloc_klmem();
                if (n == 0) {
                    free_kouholists(head);
                    head = 0;
                } else {
                    kh = get_kouholist(b);
                    n->kh = kh;
                    n->bs = b;
                    if (kh == 0) {
                        n->pri = 0;
                    } else {
                        n->pri = kh_priority(b, kh->x0E) & 0xFFFF;
                    }
                    n->next = 0;
                    if (head == 0) {
                        tail = n;
                        head = n;
                    } else {
                        tail->next = n;
                        tail = n;
                    }
                }
            }
            b = b->next;
        }
        kh_mergesort(cur_pos, head);
        free_klmemlist(head);
    }
}

KH *get_kouholist(BS *b)
{
    int cnt;
    KH *out;
    KH *last;
    KH *first;
    KH *k;
    PW *pw;
    u8 *p;

    pw = b->pw;
    if (pw == 0 || pw->x00 == 0) {
        goto none;
    }
    if (pw->id == 0) {
        if (pw->x02 != 0x1F || dic_getallnum(kana_ustr + cur_pos, pw->x00, (u8 *)wdsbuf, &cnt) <= 0) {
            goto none;
        }
    } else if (dic_getallwd(pw->id, pw->x02, pw->x03, (u8 *)wdsbuf) <= 0) {
        goto none;
    }
    last = 0;
    first = 0;
    p = (u8 *)wdsbuf;
    while (cnt > 0) {
        if (create_kouho(p, pw, pw->x00, &out) == 0) {
            free_khmemlist(first);
            return 0;
        }
        if (first == 0) {
            last = k;
            first = out;
        } else {
            last->next = out;
            last = k;
        }
        p += 5;
        while (*p++ != 0) {
        }
        if ((u32)p & 1) {
            p++;
        }
        cnt--;
    }
    return first;
none:
    return null_kouho(b->len);
}

void free_kouholists(KL *l)
{
    while (l != 0) {
        free_mem(l->kh);
        l = l->next;
    }
}

KH *null_kouho(int len)
{
    KH *k;

    k = alloc_khmem();
    if (k != 0) {
        k->flag = 0x80;
        k->str[0] = 0;
        k->x06 = len;
        k->x07 = 0;
        k->pw = 0;
        k->x0C = 0xFFFF;
        k->next = 0;
    }
    return k;
}

KH *create_kouho(u8 *buf, PW *pw, int len, KH **out)
{
    u8 *s;
    int n;
    int m;
    KH *k;
    KH *k2;
    KH *last;

    s = buf + 5;
    n = strlen(s);
    k = alloc_khmem();
    last = k;
    if (k == 0) {
        return 0;
    }
    k->flag = 0;
    m = kstrncpy(k->str, s, 4);
    k->x06 = len;
    n -= m;
    s += m;
    k->x07 = buf[4];
    k->pw = pw;
    k->x0C = *(u16 *)buf;
    k->x0E = *(u16 *)(buf + 2);
    k->next = 0;
    *out = k;
    while (n > 0) {
        k2 = alloc_khmem();
        if (k2 == 0) {
            free_khmemlist(*out);
            return 0;
        }
        last->flag |= 1;
        k2->flag = 2;
        m = kstrncpy(k2->str, s, 0xE);
        k2->next = 0;
        n -= m;
        last->next = k2;
        s += m;
        last = k2;
    }
    return *out;
}

int kstrncpy(u8 *dst, u8 *src, int n)
{
    int total;
    int c;

    total = n;
    while ((c = *src) && n > 0) {
        if (is_kanji(c) != 0) {
            if (n <= 1) {
                break;
            }
            n--;
            *dst++ = *src++;
        }
        n--;
        *dst++ = *src++;
    }
    *dst = 0;
    return total - n;
}

KH *raw_kouho(int pos, int len, int mode)
{
    KH *out;

    ((u16 *)wdsbuf)[0] = 0xFFFF;
    ((u16 *)wdsbuf)[1] = 0;
    ((u8 *)wdsbuf)[4] = 0;
    trans_roman((u8 *)wdsbuf + 5, pos, len, mode);
    return create_kouho((u8 *)wdsbuf, 0, len, &out);
}

void khmem_raw(mode)
int mode;
{
    HCHAR *h;

    h = &hchar[cur_pos];
    free_khmemlist(h->kh);
    h->kh = raw_kouho(cur_pos, cur_len, mode);
}

void kh_mergesort(int pos, KL *list)
{
    KH *head;
    KH *tail;
    KH *k;
    HCHAR *h;

    h = &hchar[pos];
    head = h->kh;
    tail = kh_endof(head);
    kh_append_init(pos, head);
    while ((k = (KH *)kh_merge_getone(list)) != 0) {
        kh_append(pos, &head, &tail, k);
    }
    if ((k = null_kouho(cur_len)) != 0) {
        kh_append(pos, &head, &tail, k);
    }
    h->kh = head;
}

int kh_merge_getone(KL *list)
{
    u16 best;
    KL *sel;
    KH *r;
    KL *l;

    best = 0;
    sel = 0;
    for (l = list; l != 0; l = l->next) {
        if (l->kh != 0 && (sel == 0 || best < (u16)l->pri)) {
            best = l->pri;
            sel = l;
        }
    }
    if (sel == 0) {
        return 0;
    }
    r = sel->kh;
    sel->kh = kh_skip(r, best);
    if (sel->kh == 0) {
        sel->pri = 0;
    } else {
        sel->pri = kh_priority(sel->bs, sel->kh->x0E) & 0xFFFF;
    }
    return (int)r;
}

void kh_append_init(int pos, KH *k)
{
    int n;

    e_khstr = (u8 *)wdsbuf;
    while (k != 0) {
        n = meantosjis(meanbuf, e_khstr + 1, kouho_makedisp(pos, cur_len, k, meanbuf));
        *e_khstr = n;
        e_khstr++;
        e_khstr += n;
        k = kh_followed(k);
    }
}

void kh_append(pos, head, tail, k)
int pos;
KH **head;
KH **tail;
KH *k;
{
    int n;

    n = meantosjis(meanbuf, outbuf, kouho_makedisp(pos, cur_len, k, meanbuf));
    if (exist_kouho(outbuf, n) != 0) {
        free_khmemlist(k);
        return;
    }
    if ((u32)(e_khstr + n + 1) <= (u32)mem) {
        *e_khstr = n;
        e_khstr++;
        strncpy(e_khstr, outbuf, n);
        e_khstr += n;
    }
    if (*head == 0) {
        *tail = k;
        *head = k;
    } else {
        (*tail)->next = k;
    }
    *tail = kh_endof(k);
}

static int exist_kouho(u8 *s, int n)
{
    u8 *p;
    int len;

    for (p = (u8 *)wdsbuf; p < e_khstr;) {
        len = *p;
        p++;
        if (len == n && ask_strncmp(p, s, len) == 0) {
            return 1;
        }
        p += len;
    }
    return 0;
}

KH *kh_skip(KH *k)
{
    KH *r;

    while (k->flag & 1) {
        k = k->next;
    }
    r = k->next;
    k->next = 0;
    return r;
}

KH *kh_followed(KH *k)
{
    while (k->flag & 1) {
        k = k->next;
    }
    return k->next;
}

KH *kh_endof(KH *k)
{
    KH *n;

    if (k == 0) {
        return 0;
    }
    for (;;) {
        n = k->next;
        if (n == 0) {
            break;
        }
        k = n;
    }
    return k;
}

int kh_count(KH *k)
{
    int n;

    n = 0;
    while (k != 0) {
        k = kh_followed(k);
        n++;
    }
    return n;
}

int kh_length(KH *k)
{
    int n;
    int len;

    if (k->flag == 0x80) {
        return cur_len * 2;
    }
    n = (cur_len - k->x06) * 2;
    while (k->flag & 1) {
        len = strlen(k->str);
        k = k->next;
        n += len;
    }
    return n + strlen(k->str);
}

KH *take_kouho(KH *k, int n)
{
    while (n-- != 0) {
        k = kh_followed(k);
    }
    return k;
}

void disp_kouho()
{
    u16 *p;
    KH *k;
    int pos;
    int n;
    int i;
    int len;
    u16 *q;

    kouho_set_num(kh_count(top_kh) - gun_nkh - 1, kouho_rest + 5);
    sstrtom(meanbuf, kouho_rest, 0);
    pos = 0xA;
    k = top_kh;
    i = 0;
    while (i < gun_num) {
        kouho_head[2] = i + 0x31;
        pos += sstrtom(meanbuf + pos, kouho_head, 0);
        q = meanbuf + pos;
        len = kouho_makedisp(cur_pos, cur_len, k, q);
        if (i == gun_nkh) {
            change_kind(q, len, 7);
        } else {
            change_kind(q, len, 0);
        }
        pos += len;
        k = kh_followed(k);
        if (k == 0) {
            break;
        }
        i++;
    }
    p = meanbuf + pos;
    while (pos < kwin_len) {
        sstrtom(p, lit_485_0036E088, 0);
        p++;
        pos++;
    }
}

void kouho_set_num(int n, u8 *out)
{
    int h;
    int t;
    int r;

    if (n < 0) {
        n = 0;
    }
    h = n / 100;
    if (h != 0) {
        *out = h + 0x30;
    } else {
        *out = 0x20;
    }
    out++;
    r = n % 100;
    t = r / 10;
    if (t == 0 && out[-1] == 0x20) {
        *out = 0x20;
    } else {
        *out = t + 0x30;
    }
    out[1] = r % 10 + 0x30;
}

int kouho_makedisp(int pos, int len, KH *k, u16 *buf)
{
    int n;

    if (k != 0 && !(k->flag & 0x80)) {
        n = jiritu_makedisp(k, buf);
        return n + roman_makedisp(pos + k->x06, len - k->x06, buf + n, 0);
    }
    return roman_makedisp(pos, len, buf, 0);
}

int jiritu_makedisp(KH *k, u16 *buf)
{
    int n;

    n = 0;
    for (;;) {
        n += sstrtom(buf + n, k->str, 6);
        if (!(k->flag & 1)) {
            break;
        }
        k = k->next;
    }
    return n;
}

int inc_gun(KH *k)
{
    int n;
    int w;
    int room;
    KH *p;

    if (k == 0) {
        return 0;
    }
    p = k;
    w = 0;
    n = 0;
    room = kwin_len - 0xA;
    while (n <= 8) {
        if (p == 0) {
            break;
        }
        w += kh_length(p) + 4;
        if (room < w) {
            break;
        }
        n++;
        p = kh_followed(p);
    }
    if (n == 0) {
        n = 1;
    }
    return n;
}

int next_gun(int disp, int wrap)
{
    KH *old;
    int n;

    old = top_kh;
    top_kh = take_kouho(old, gun_num);
    n = inc_gun(top_kh);
    if (n == 0) {
        if (wrap == 0) {
            top_kh = old;
            return 0;
        }
        init_kouho(0, 0);
    } else {
        gun_num = n;
    }
    gun_nkh = 0;
    if (disp == 1) {
        disp_kouho();
    }
    return 1;
}

int back_gun(int disp, int wrap)
{
    KH *old;
    int num;

    old = top_kh;
    num = gun_num;
    init_kouho(0, 0);
    if (old == top_kh) {
        if (wrap == 0) {
            top_kh = old;
            gun_num = num;
            gun_nkh = 0;
            return 0;
        }
        old = 0;
    }
    for (;;) {
        if (take_kouho(top_kh, gun_num) == old) {
            break;
        }
        next_gun(0, 1);
    }
    if (disp == 1) {
        disp_kouho();
    }
    return 1;
}

int is_jis(c)
int c;
{
    int a;
    int b;
    int r;

    a = 0;
    r = 0;
    if (((c & 0xFFFF) >> 8 & 0xFF) > 0x20 && ((c & 0xFFFF) >> 8 & 0xFF) < 0x7F) {
        a = 1;
    }
    if (a != 0) {
        b = 0;
        if ((c & 0xFF) >= 0x21 && (c & 0xFF) < 0x7F) {
            b = 1;
        }
        if (b != 0) {
            r = 1;
        }
    }
    return r;
}

int is_kanji(int c)
{
    c = c & 0xFF;
    if (c < 0x81 || c >= 0xFD || (c >= 0xA0 && c < 0xE0)) {
        return 0;
    }
    return 1;
}

int is_shift(int c)
{
    u8 lo;

    lo = c;
    if (is_kanji((c & 0xFFFF) >> 8 & 0xFF) == 0) {
        return 0;
    }
    if (lo < 0x40 || lo >= 0xFD || lo == 0x7F) {
        return 0;
    }
    return 1;
}

int setmean(u16 *out, int c, int kind)
{
    int k;
    int f;

    k = ((kind & 0xFFFF) << 12) & 0xFFFF;
    if (is_shift(c) != 0) {
        if (shiftlen(c) == 1) {
            f = (k | 0x300) & 0xFFFF;
        } else {
            f = (k | 0x700) & 0xFFFF;
        }
        goto two;
    }
    if (is_jis(c) != 0) {
        f = (k | 0x500) & 0xFFFF;
        if ((c & 0xFFFF) == 0x2474) {
            c = 0x2574;
        }
        if ((c & 0xFFFF) == 0x2475) {
            c = 0x2575;
        }
        if ((c & 0xFFFF) == 0x2476) {
            c = 0x2576;
        }
two:
        out[0] = (f & 0xFFFF) | ((c & 0xFFFF) >> 8);
        out[1] = ((f & 0xFFFF) + 0x100) | (c & 0xFF);
        return 2;
    }
    out[0] = (k & 0xFFFF) | (c & 0xFF);
    return 1;
}

void change_kind(u16 *p, int n, int kind)
{
    u16 k;

    k = (kind & 0xFFFF) << 12;
    while (n-- != 0) {
        *p = (*p & 0xFFF) | k;
        p++;
    }
}

int shiftlen(int x)
{
    int c;
    int h;

    c = x & 0xFFFF;
    h = c & 0xFF00;
    switch (h) {
    case 0x8000:
    case 0x8500:
        return 1;
    case 0x8600:
        if ((c & 0xFF) < 0x9E) {
            return 1;
        }
    default:
        return 2;
    }
}

int sstrtom(u16 *out, u8 *s, int kind)
{
    u16 *start;
    int c;
    int n;

    start = out;
    while (*s != 0) {
        c = *s;
        if (c >= 0x80 && !(c >= 0xA0 && c < 0xE0)) {
            n = setmean(out, ((c << 8) | s[1]) & 0xFFFF, kind);
            s += 2;
        } else {
            c = *s;
            s++;
            n = setmean(out, c, kind);
        }
        out += n;
    }
    return out - start;
}

int to_ucode(int x)
{
    int c;

    c = x & 0xFFFF;
    if (c > 0x20 && c < 0x7F) {
        return 0;
    }
    switch (c & 0xFF00) {
    case 0x2300:
        return c & 0x7F;
    case 0x2400:
        return ((c & 0x7F) | 0x80) & 0xFF;
    case 0x2500:
        return 0;
    default:
        return srch_ucode(x);
    }
}

int is_kata(c, flag)
u16 c;
int flag;
{
    if (flag != 0 && c == 0x213C) {
        return 1;
    }
    if ((c & 0xFF00) == 0x2500) {
        return 1;
    }
    return 0;
}

int is_jisknj(int c)
{
    return (c & 0xFFFF) >= 0x3020;
}

int is_jiskig(int x)
{
    int c;

    c = x & 0xFFFF;
    if (c >= 0x2120 && c < 0x3020) {
        return is_kata(x, 0) ? 0 : 1;
    }
    return 0;
}

int meantosjis(u16 *src, u8 *dst, int n)
{
    int cnt;
    u8 *d;
    u16 c;
    u16 c2;
    int hi;
    int v;

    cnt = 0;
    d = dst;
    while (cnt < n) {
        c = *src;
        hi = (c >> 8) & 0xF;
        if (hi == 0) {
            d[0] = c;
            src++;
            d++;
            cnt++;
        } else if (hi & 1) {
            src++;
            c2 = *src;
            v = (c << 8) & 0xFFFF;
            cnt++;
            if (((c2 >> 8) & 0xF) == hi + 1) {
                src++;
                cnt++;
                v = (v | (c2 & 0xFF)) & 0xFFFF;
                if (is_jis(v, v) != 0) {
                    v = ask_jis2sjis(v) & 0xFFFF;
                }
                d[0] = v >> 8;
                d[1] = v;
                d += 2;
            }
        }
    }
    *d = 0;
    return d - dst;
}

int ask_sjis2jis(int c)
{
    int lo;
    int hi;
    int r;
    int s;

    lo = c & 0xFF;
    hi = ((c & 0xFFFF) >> 8) & 0xFF;
    if (lo > 0x3F && lo < 0xFD) {
        if (lo == 0x7F) {
            return 0;
        }
        if (hi >= 0xE0) {
            hi = (hi - 0x40) & 0xFF;
        }
        s = (((hi - 0x81) * 2) + 0x21) & 0xFF;
        if (lo >= 0x9F) {
            r = lo - 0x7E;
            s = (s + 1) & 0xFF;
        } else {
            r = lo - 0x20;
            if (lo < 0x80) {
                r = lo - 0x1F;
            }
        }
        return ((s << 8) | (r & 0xFF)) & 0xFFFF;
    }
    return 0;
}

int ask_jis2sjis(int c)
{
    int lo;
    unsigned int hi;
    int a;
    unsigned int b;
    int t;
    int r;

    c = c & 0xFFFF;
    lo = c & 0xFF;
    hi = (c >> 8) & 0xFF;
    if (c >= 0x2121 && c < 0x7E7F) {
        if (lo < 0x21 || lo >= 0x7F) {
            return 0;
        }
        a = (lo + 0x1F) & 0xFF;
        b = hi >> 1;
        if (!(hi & 1)) {
            a = (a + 0x5E) & 0xFF;
            b = ((hi >> 1) & 0xFF) - 1;
        }
        t = b & 0xFF;
        if ((a & 0xFF) >= 0x7F) {
            a = (a + 1) & 0xFF;
        }
        r = t + 0x71;
        if (t > 0x2E) {
            r = t + 0xB1;
        }
        return (((r & 0xFF) << 8) | (a & 0xFF)) & 0xFFFF;
    }
    return 0;
}

void init_univmem(void)
{
    u8 *p;

    free_univ = mem;
    for (p = mem; p < mem + 0x11928; p += 0x18) {
        *(u8 **)p = p + 0x18;
    }
    *(u8 **)p = 0;
    first_init_5 = 0;
}

void *alloc_mem(void)
{
    void *r;

    r = free_univ;
    if (r == 0) {
        return 0;
    }
    free_univ = *(void **)r;
    return r;
}

void free_mem(void *p)
{
    if (p != 0) {
        *(void **)p = free_univ;
        free_univ = p;
    }
}

CH *alloc_chmem(void)
{
    void *r;

    r = alloc_mem();
    if (r != 0) {
        return r;
    }
    return 0;
}

BS *alloc_bsmem(void)
{
    void *r;

    r = alloc_mem();
    if (r != 0) {
        return r;
    }
    return 0;
}

PWM *alloc_pwmem(void)
{
    void *r;

    r = alloc_mem();
    if (r != 0) {
        return r;
    }
    return 0;
}

KH *alloc_khmem(void)
{
    void *r;

    r = alloc_mem();
    if (r != 0) {
        return r;
    }
    return 0;
}

KL *alloc_klmem(void)
{
    void *r;

    r = alloc_mem();
    if (r != 0) {
        return r;
    }
    return 0;
}

void free_pwmemlist(PWM *p)
{
    PWM *n;

    while (p != 0) {
        n = p->next;
        free_mem(p);
        p = n;
    }
}

void free_chmemlist(CH *c)
{
    CH *n;
    s64 prev;

    prev = 0;
    while (c != 0) {
        n = c->next;
        if (prev == 0 || c->id != prev) {
            if (c->id != 0) {
                dic_freeentid(c->id);
            }
        }
        prev = c->id;
        free_mem(c);
        c = n;
    }
}

void free_bsmemlist(BS *b)
{
    BS *n;

    while (b != 0) {
        n = b->next;
        free_mem(b);
        b = n;
    }
}

void free_khmemlist(KH *k)
{
    KH *n;

    while (k != 0) {
        n = k->next;
        free_mem(k);
        k = n;
    }
}

void free_klmemlist(KL *l)
{
    KL *n;

    while (l != 0) {
        n = l->next;
        free_mem(l);
        l = n;
    }
}

int bs_prefer(int pos, int end, int len)
{
    BS *b;
    BS *best;
    BS *p;
    HCHAR *h;

    h = &hchar[pos];
    for (b = h->bs; b != 0; b = b->next) {
        if (len < 0 || b->len == len) {
            if (bs_point(b, pos, end) == -1) {
                return -1;
            }
        }
    }
    if (h != 0) {
        best = h->bs;
        if (best == 0) {
            return -1;
        }
        for (p = best->next; p != 0; p = p->next) {
            if (best->x08 < p->x08) {
                best = p;
            }
        }
        bs_ctd(best, pos, end);
        return best->len;
    }
    return -1;
}

int calc_point(int pos, BS *b, BS *next)
{
    s16 a;
    s16 c;
    int f;
    u16 pri;

    if (next == 0) {
        a = b->len;
        c = 0;
        f = 1;
    } else {
        c = b->len;
        a = next->len;
        f = 0;
    }
    pri = b->x0A;
    return f * 0x32 + (pri + (c * 0x10 + a * 0x11) + setu_point(b, next, pri));
}

int bs_point(BS *b, int pos, int end)
{
    BS *n;
    s16 best;
    s16 v;
    int p;

    if (ignore_syn() != 0) {
        b->x08 = 0;
        return 0;
    }
    if (b->x02 == 0xFF || (p = pos + b->len) >= end) {
        best = calc_point(pos, b, 0) & 0xFFFF;
    } else {
        n = hchar[p].bs;
        if (n == 0 && henkan_mode >= 3) {
            return -1;
        }
        best = 0;
        while (n != 0) {
            v = calc_point(pos, b, n) & 0xFFFF;
            if ((best & 0xFFFF) < v) {
                best = v;
            }
            n = n->next;
        }
    }
    b->x08 = best;
    return 0;
}

void bs_prefix(int pos)
{
    BS *b;
    PW *pw;
    HCHAR *h;

    h = &hchar[pos];
    for (b = h->bs; b != 0; b = b->next) {
        b->x0A = 0;
        pw = b->pw;
        if (pw != 0 && pw->x02 == 0x19 && pw->x00 == 0) {
            b->x0A = 0xA;
        }
    }
}

void bs_ctd(BS *b, int pos, int end)
{
    BS *n;
    int pt;
    int p;
    int len;

    len = b->len;
    if (b->x02 == 0xFF || (p = pos + len) >= end) {
        return;
    }
    n = hchar[p].bs;
    if (n == 0) {
        return;
    }
    while (n != 0) {
        n->x0A = 0;
        if (ignore_syn(n) == 0) {
            pt = setu_point(b, n);
            if (pt > 0) {
                n->x0A = pt;
            }
        }
        n = n->next;
    }
}

int ignore_syn(BS *b)
{
    PW *pw;

    pw = b->pw;
    if (pw != 0 && (pw->x02 == 0x28 || pw->x02 == 0x29)) {
        return 1;
    }
    return 0;
}

int setu_point(BS *b, BS *n)
{
    int pt;
    int a;
    int c;
    int t;
    int u;
    int k;
    PW *p;

    pt = 0;
    u = 0;
    a = b->x02;
    c = 0;
    if (a >= 0x2D && a < 0x39) {
        u = b->x03;
    } else {
        c = b->x03;
    }
    t = 0;
    if (n != 0) {
        p = n->pw;
        t = 0;
        if (p != 0) {
            t = p->x02;
            if (t == 0x28) {
                return 0;
            }
            if (t == 0x29 || (t == 0x19 && p->x00 > 0) || (t == 0x1F && p->id == 0xFFFFFFFF)) {
                pt += 0x14;
            }
        }
    }
    p = b->pw;
    if (p != 0) {
        k = p->x02;
        if (k == 0x19 && p->x00 == 0) {
            pt += 0xA;
        } else if (k == 0x1B || k == 0x1C) {
            pt += 3;
        } else if (k == 0x1A) {
            pt += 2;
        } else if (k <= 0x19 || k >= 0x1F) {
            pt += 1;
        }
    }
    if ((c & 0xFF) != 0 || a == 0xD) {
        return ktu_match(a, c, t, pt);
    }
    if (a == 0x2D) {
        return setu_match(u, t, 0xFF, pt, 0);
    }
    k = t & 0xFF;
    if (k >= 0x32 && k < 0x39) {
        p = n->pw;
        if (p != 0) {
            u = p->x03;
        } else {
            u = 0;
        }
        return setu_match(u & 0xFF, a, c, pt, 0);
    }
    if (a >= 0x14 && a < 0xC0) {
        return syn_match(a, t, pt, pt);
    }
    if (a >= 0xC0) {
        return josi_match(a, t, pt);
    }
    return pt;
}

int ktu_match(a, b, c, base)
int a;
int b;
int c;
int base;
{
    a = a & 0xFF;
    if (a == 0xD && !(b & 0xFF)) {
        c = c & 0xFF;
        if (c > 0 && c < 0x18) {
            return base + 3;
        }
        return base;
    }
    b = b & 0xFF;
    if (b >= 4 && b < 7 && (c & 0xFF) > 0 && (c & 0xFF) < 0x18) {
        switch (a) {
        case 0x8E:
            if (b == 6) {
                return base + 0xA;
            }
            return base;
        case 0x8D:
            return base + 0xA;
        default:
            return base + 3;
        }
    }
    if (b == 8) {
        c = c & 0xFF;
        if (c >= 0x14 && c < 0x19) {
            if (a >= 0x80 && a < 0x8F) {
                return base + 0xF;
            }
            if (a == 0x92 || a == 0x94 || a == 0xA0 || a == 0xA2 || (u32)(a - 0xAE) <= 1 || a == 0xB1 || a == 1) {
                return base + 0xF;
            }
            if (a >= 0xA && a < 0xD) {
                return base + 0xF;
            }
            return base + 3;
        }
    }
    return base;
}

int syn_match(a, b, base)
int a;
int b;
int base;
{
    a = a & 0xFF;
    b = b & 0xFF;
    if (a >= 0x14 && a < 0x19) {
        if (b >= 0x14 && b < 0x19) {
            return base + 3;
        }
        return base;
    }
    switch (a) {
    case 0x1F:
        if (b == 0x1F) {
            return base + 0xF;
        }
        break;
    case 0x21:
    case 0x26:
    case 0x27:
        return base + 0x14;
    case 0x20:
    case 0x22:
        return base + 0xF;
    case 0x1B:
        if (b == 0x1C) {
            return base + 0x14;
        }
        break;
    case 0x1A:
        if (b == 0x1A) {
            return base + 5;
        }
        break;
    }
    return base;
}

int josi_match(b, c, d, base)
BS *b;
int c;
int d;
int base;
{
    PW *p;
    int k;
    int t;
    int v;

    k = 0;
    p = b->pw;
    if (p != 0) {
        t = p->x02;
        if ((t >= 0x14 && t <= 0x19) || t == 0x32) {
            k = 1;
            if (p->x00 + 1 == b->len) {
                k = 2;
            }
        }
    }
    c = c & 0xFF;
    d = d & 0xFF;
    switch (c) {
    case 0xC7:
        v = 0;
        if (d > 0 && d < 0xE) {
            v = 1;
        }
        if (v != 0) {
            return base + 0x1E;
        }
        return base + 0x19;
    case 0xC8:
        if (d == 0) {
            return base;
        }
        if (k == 2) {
            return base + 0x14;
        }
        return base + 5;
    case 0xE1:
    case 0xC1:
        if (k == 2) {
            base += 5;
        }
        return base;
    default:
        if ((u32)(c - 0xC4) <= 1 || (c >= 0xCE && c < 0xD2)) {
            if ((d > 0 && d <= 0xD) || (d >= 0x16 && d < 0x18)) {
                return base + 5;
            }
            return base;
        }
        if (c == 0xC2) {
            if (d == 0) {
                return base;
            }
            if (k == 2) {
                base += 3;
            }
            return base;
        }
        if (c >= 0xFC && c < 0xFE && d > 0 && d < 0x18) {
            return base + 0x12;
        }
        switch (c) {
        case 0xFE:
        case 0xFA:
            if (d >= 0x14 && d < 0x19) {
                return base + 0x12;
            }
        default:
            return base;
        }
    }
}

int setu_match(a, b, c, base, extra)
int a;
int b;
int c;
int base;
int extra;
{
    int r;

    r = 0;
    b = b & 0xFF;
    switch (a & 0xFF) {
    case 0:
        if ((b >= 0x14 && b <= 0x19) || b == 0x32) {
            r = 0xF;
        }
        break;
    case 1:
        if (b == 0x1A) {
            r = 0xF;
        }
        break;
    case 2:
        if (b == 0x1B || b == 0x1C) {
            r = 0x14;
        }
        break;
    case 3:
        if (b == 0x1F || b == 0x38) {
            r = 0x14;
        }
        break;
    case 4:
        if (b == 0x16) {
            r = 0x14;
        }
        break;
    case 5:
        c = c & 0xFF;
        if (c == 0xFF) {
            if (b > 0 && b < 0xE) {
                r = 0xF;
            }
        } else if ((b >= 0x80 && b < 0x8C && c == 4) || (b == 0xD && c == 0)) {
            r = 0xF;
        }
        break;
    case 6:
        if (b >= 0x14 && b < 0x1A) {
            r = 0xF;
        } else if (b == 0x1F || b == 0x38) {
            r = 0x14;
        }
        break;
    case 7:
        if (b == 0x1D) {
            r = 0xF;
        }
        break;
    case 8:
        if ((b >= 0x14 && b <= 0x19) || (b > 0 && b < 0xE)) {
            r = 0x14;
        }
        break;
    }
    if (r == 0) {
        return base;
    }
    return extra + (base + r);
}

u16 kh_priority(BS *b, int v)
{
    v = v & 0xFFFF;
    if (v != 0) {
        return (v + 0x3E8) & 0xFFFF;
    }
    return b->x08;
}

int is_alphanum(int c)
{
    return rmtype[c & 0xFF] & 0xC0;
}

int is_num(int c)
{
    return rmtype[c & 0xFF] & 0x80;
}

int is_alpha(int c)
{
    return rmtype[c & 0xFF] & 0x40;
}

int is_paren(int c)
{
    return rmtype[c & 0xFF] & 0x20;
}

int to_roman(u16 *src, int n, u16 *out, int *cnt)
{
    int kind;
    int step;
    int sub;
    int t;
    int v;
    u16 *p;
    u8 *e;
    int c;
    u8 *r;

    step = 1;
    kind = rmtype[src[0] & 0xFF] & 0xF;
    *cnt = 1;
    if (kind < 0xA) {
        if (kind < 6) {
            p = src;
            e = rmtab + 1;
            goto got;
        }
        if (kind <= 8 || roman_japan == 0) {
            out[0] = to_zenkaku(src[0] & 0xFF);
        } else {
            out[0] = to_zenkaku_spec(src[0] & 0xFF);
        }
        return 1;
    }
    v = 0;
    if (kind < 0xD) {
        if (kind == 0xC) {
            out[0] = ext_jis(0x73, src[0] & 0xFF);
            v = 1;
        } else {
            out[0] = to_zenkaku(src[0] & 0xFF);
            v = 2;
            if (kind != 0xA) {
                v = 3;
            }
        }
    }
    if (n == 1) {
        return -((v & 0xFFFF) != 0);
    }
    t = src[1];
    c = t & 0xFF;
    p = src + 1;
    if (src[0] == c) {
        if (kind == 0xC) {
            return 2;
        }
        if (kind >= 0xD) {
            out[0] = ext_jis(0x43, t);
            return 1;
        }
    }
    sub = v & 0xFFFF;
    if (sub == 1 && c == 0x27) {
        return 2;
    }
    kind = rmtype[c & 0xFF] & 0xF;
    if (kind == 7 && sub == 3) {
        out[0] = out[0] + 2;
        return 2;
    }
    if (kind == 6 && sub >= 2) {
        out[0] = out[0] + 1;
        if (src[0] == 0xB3) {
            out[0] = ext_jis(0x74, src[1]);
        }
        return 2;
    }
    if (sub >= 2 || (kind > 5 && kind < 0xF && sub == 1)) {
        return 1;
    }
    step = 2;
    for (;;) {
        if (kind < 6) {
            r = getrda2(src, p + 1);
            e = r;
            if (r == 0) {
                r = getrda1(src, p);
                e = r;
                if (r == 0) {
                    return -2;
                }
got:
                e = e + (kind - 1) * 2;
            }
            out[0] = ext_jis(e[0], *p);
            if (e[1] != 0) {
                out[1] = ext_jis(e[1], *p);
                *cnt = 2;
            }
            return step;
        }
        step++;
        if (step >= n) {
            return 0;
        }
        p++;
        kind = rmtype[*p] & 0xF;
        if (kind >= 6 && kind < 0xD) {
            return -2;
        }
        if (step >= 5) {
            return -2;
        }
    }
}

int ToUpper(int c)
{
    int u;

    u = c & 0xFF;
    if (u >= 0x61 && u < 0x7B) {
        return (u - 0x20) & 0xFF;
    }
    return c;
}

u8 *getrda2(u16 *a, u16 *b)
{
    int n;
    u8 *p;
    u16 *q;
    int k;
    int len;
    u8 key;

    n = b - a;
    p = rmspec;
    while (*p != 0) {
        len = *p;
        p++;
        if (n == len) {
            q = a;
            k = n;
            while (k > 0) {
                key = *p;
                if (key != (ToUpper(*q) & 0xFF)) {
                    break;
                }
                q++;
                k--;
                p++;
            }
            if (k == 0) {
                return p;
            }
            p += k;
        } else {
            p += len;
        }
        while (*p++ != 0) {
        }
    }
    return 0;
}

u8 *getrda1(u16 *a, u16 *b)
{
    int n;
    int c;
    int idx;
    u32 *tp;
    u8 *e;
    u8 *end;
    int up;

    n = b - a;
    switch (n) {
    case 0:
        return rmtab + 1;
    case 2:
    case 1:
        c = a[0] & 0xFF;
        if (c >= 0x61 && c < 0x7B) {
            idx = c - 0x61;
        } else if (c >= 0x41 && c < 0x5B) {
            idx = c - 0x41;
        } else {
            return 0;
        }
        tp = (u32 *)prmtab + idx;
        e = (u8 *)*tp;
        if (n == 1) {
            return e + 1;
        }
        do {
            tp++;
            end = (u8 *)*tp;
        } while (end == 0);
        e += 0xB;
        up = ToUpper(a[1]) & 0xFF;
        while (e < end) {
            if (*e == up) {
                return e + 1;
            }
            e += 0xB;
        }
        return 0;
    default:
        return 0;
    }
}

int add_kana_buf(u8 *s)
{
    KANA *kb;
    u8 *us;
    int len;
    int bytes;
    int lead;
    int c;
    int code;

    kb = pkana_buf;
    lead = 0;
    us = p_ustr;
    len = kana_len;
    c = *s;
    bytes = bytesin_kana_buf(kana_buf, kb);
    while (c != 0) {
        if (lead != 0) {
            code = ask_sjis2jis(((lead & 0xFFFF) << 8) | (c & 0xFF)) & 0xFFFF;
            if (code != 0) {
                if (len >= 0x24 || bytes >= 0x4E) {
                    return -1;
                }
                kb->ch = code;
                kb->n = 2;
                kb++;
                *us = to_ucode(code);
                len++;
                us++;
                bytes += 2;
            }
            lead = 0;
        } else if (is_kanji(c) != 0) {
            lead = *s;
        } else {
            c = *s;
            if ((c >= 0x20 && c < 0x7F) || (c >= 0xA0 && c < 0xE0)) {
                if (len >= 0x24 || bytes >= 0x4F) {
                    return -1;
                }
                kb->ch = c;
                len++;
                bytes++;
                kb->n = 1;
                lead = 0;
                *us = 0;
                kb++;
                us++;
            }
        }
        s++;
        c = *s;
    }
    pkana_buf = kb;
    ekana_buf = kb;
    kana_len = len;
    p_ustr = us;
    e_ustr = us;
    return len;
}

int bytesin_kana_buf(KANA *a, KANA *b)
{
    int r = 0;

    for (; a < b; a++) {
        if (a->ch & 0xFF00) {
            r += 2;
        } else {
            r += 1;
        }
    }
    return r;
}

int count_byte_kana_buf(int a, int n)
{
    KANA *p = &kana_buf[a];
    int r = 0;

    while (n > 0) {
        r += p->n;
        n--;
        p++;
    }
    return r;
}

int api_funcent(int *req)
{
    int cmd;

    cmd = *req;
    if (cmd <= 0 || (u32)cmd > 0x3F) {
        return -1;
    }
    return D_0034ABEC[cmd]((u8 *)req + 4);
}

int api_Rstrconv(int *a)
{
    u16 out[2];
    int cnt;
    u16 *src;
    u16 *p;
    u16 *q;
    u8 *in;
    u8 *dst;
    int n;
    int kind;
    int r;
    int d;
    u16 v;
    int j;

    dst = (u8 *)a[1];
    in = (u8 *)a[0];
    if (a[-1] == 0xF) {
        kind = 0x4100;
    } else {
        kind = 0x4000;
    }
    p = (u16 *)wdsbuf;
    n = 0;
    while (*in != 0 && (u32)p < (u32)mem) {
        *p = *in | (kind & 0xFFFF);
        in++;
        n++;
        p++;
    }
    src = (u16 *)wdsbuf;
    while (n > 0) {
        r = to_roman(src, n, out, &cnt);
        if (r <= 0) {
            if (r != -1) {
                return (src - (u16 *)wdsbuf) + 1;
            }
            r = 1;
        }
        src += r;
        n -= r;
        q = out;
        for (j = cnt; j != 0; j--) {
            d = ask_jis2sjis(*q);
            q++;
            if (d == 0x82F2) {
                d = 0x8394;
            }
            dst[0] = d >> 8;
            dst[1] = d;
            dst += 2;
        }
    }
    *dst = 0;
    return 0;
}

int api_henkan(int *a)
{
    u8 *in;
    u8 *kana;
    u8 *kj;

    if (func_mode == 0) {
        return -1;
    }
    in = (u8 *)a[0];
    kana = (u8 *)a[1];
    kj = (u8 *)a[2];
    if (in != 0) {
        if (func_mode >= 2) {
            init_edit0();
        }
        if (add_kana_buf(in) < 0) {
            return -1;
        }
    }
    if (kana_len <= 0) {
        *kana = 0;
        *kj = 0;
        return 0;
    }
    henkan(0, kana_len, 0, -1);
    cur_pos = 0;
    cur_len = bunsetu_len(0);
    init_kouho(0, 1);
    get_kouhostr(kana, kj);
    func_mode = 3;
    return kh_count(hchar[0].kh);
}

int get_kouhostr(u8 *a, u8 *b)
{
    strcpy(a, select_subtostr(cur_pos, cur_len));
    strcpy(b, select_subtostr(cur_pos + cur_len, kana_len - cur_pos - cur_len));
}

int api_movekh(int *a)
{
    int cnt;
    u8 *p;
    u8 *q;
    int n;

    if (func_mode != 3) {
        return 0;
    }
    p = (u8 *)a[0];
    q = (u8 *)a[1];
    switch (a[-1]) {
    case 20:
        if (gun_nkh > 0) {
            gun_nkh--;
        } else if (back_gun(0, 0) == 0) {
            init_kouho(0, 1);
            n = gun_num;
            while (next_gun(0, 0) != 0) {
                n += gun_num;
            }
            init_kouho(n - 1, 1);
        } else {
            gun_nkh = gun_num - 1;
        }
        break;
    case 21:
        if (gun_nkh < gun_num - 1) {
            gun_nkh++;
        } else if (next_gun(0, 0) == 0) {
            init_kouho(0, 1);
        }
        break;
    }
    get_kouhostr(p, q);
    return gun_nkh + 1;
}

int api_moveblk(int *a)
{
    int fail;
    u8 *p;
    u8 *q;

    fail = 0;
    if (func_mode != 3) {
        return 0;
    }
    p = (u8 *)a[0];
    q = (u8 *)a[1];
    switch (a[-1]) {
    case 22:
        if (back_gun(0, 0) == 0) {
            fail = 1;
        }
        break;
    case 23:
        if (next_gun(0, 0) == 0) {
            fail = 1;
        }
        break;
    }
    get_kouhostr(p, q);
    if (fail != 0) {
        return 0;
    }
    return gun_num;
}

int api_allfix(int *a)
{
    u8 *out;

    if (func_mode != 3) {
        return -1;
    }
    out = (u8 *)a[0];
    wd_learn(0, kana_len);
    strcpy(out, select_tostr());
    init_edit0();
    func_mode = 1;
    return 0;
}

int api_select(int *a)
{
    s16 *cnt;
    u8 *k1;
    u8 *k2;
    u8 *out;
    int r;
    int n;
    int pos;

    if (func_mode != 3) {
        return -1;
    }
    n = a[0];
    if (n > 0 && gun_num >= n) {
        out = (u8 *)a[1];
        k1 = (u8 *)a[2];
        k2 = (u8 *)a[3];
        cnt = (s16 *)a[4];
        gun_nkh = n - 1;
        strcpy(out, select_subtostr(cur_pos, cur_len));
        unify_khmem(cur_pos, 1);
        r = count_byte_kana_buf(cur_pos, cur_len);
        pos = cur_pos;
        cur_pos = pos + cur_len;
        if (cur_pos >= kana_len) {
            *k1 = 0;
            *k2 = 0;
            *cnt = 0;
            return r;
        }
        cur_len = bunsetu_len(cur_pos, pos);
        init_kouho(0, 1);
        get_kouhostr(k1, k2);
        *cnt = kh_count(hchar[cur_pos].kh);
        return r;
    }
    return 0;
}

int api_dicopen(void)
{
    if (lock_mode == 0) {
        return -1;
    }
    if (dic_open((char *)dic_name) == -7) {
        return 1;
    }
    into_editing(0);
    func_mode = 1;
    return 0;
}

int api_dicclose(void)
{
    if (lock_mode == 0) {
        return -1;
    }
    init_edit0();
    dic_close();
    func_mode = 0;
    return 0;
}

int api_touroku(int *a)
{
    WD wd;
    u8 *s;
    u8 *d;
    int cmd;
    int kind;
    int pri;
    int len;
    int c;
    int u;
    int r;

    if (im_state > 1) {
        return 4;
    }
    if (func_mode >= 2) {
        return 4;
    }
    kind = a[0];
    cmd = a[-1];
    if (kind != 1 && kind != 0) {
        return 1;
    }
    s = (u8 *)a[1];
    pri = a[2];
    u = syn_2to3(a[3]);
    if ((s8)u < 0) {
        return 2;
    }
    len = 0;
    d = yomi_buf;
    while (*s != 0) {
        c = ((*s << 8) | s[1]) & 0xFFFF;
        if (is_shift(c) == 0) {
            return 3;
        }
        u = to_ucode(ask_sjis2jis(c) & 0xFFFF) & 0xFF;
        if (u == 0) {
            return 3;
        }
        *d = u;
        s += 2;
        d++;
        len++;
    }
    *d = 0;
    wd.yomi = yomi_buf;
    wd.len = len;
    wd.x07 = u;
    wd.tango = (u8 *)pri;
    wd.x06 = 0;
    wd.x08 = 0;
    switch (cmd) {
    case 30:
        if (dic_touroku(&wd) == 3) {
            return 0;
        }
        return 4;
    case 31:
        r = dic_delete(&wd);
        if (r == 3) {
            return 0;
        }
        if (r == 0) {
            return -1;
        }
    default:
        return 4;
    }
}

int syn_2to3(int n)
{
    n = n - 1;
    if (n < 0 || n >= 0x1E) {
        return -1;
    }
    return tab_2to3[n];
}

int apis_dicname(int *a)
{
    strcpy(dic_name, a[0]);
    return 0;
}

int api_khlong(int *a)
{
    u8 *p;
    u8 *q;

    if (func_mode != 3) {
        return -1;
    }
    p = (u8 *)a[0];
    q = (u8 *)a[1];
    if (cur_pos + cur_len >= kana_len) {
        return 0;
    }
    save_fst_bslen(cur_pos);
    free_hchar(cur_pos, kana_len, 1);
    cur_len++;
    henkan(cur_pos, kana_len, 1, cur_len);
    init_kouho(0, 1);
    get_kouhostr(p, q);
    return kh_count(hchar[cur_pos].kh);
}

int api_khshort(int *a)
{
    u8 *p;
    u8 *q;

    if (func_mode != 3) {
        return -1;
    }
    p = (u8 *)a[0];
    q = (u8 *)a[1];
    if (cur_len < 2) {
        return 0;
    }
    save_fst_bslen(cur_pos);
    free_hchar(cur_pos, kana_len, 1);
    cur_len--;
    henkan(cur_pos, kana_len, 1, cur_len);
    init_kouho(0, 1);
    get_kouhostr(p, q);
    return kh_count(hchar[cur_pos].kh);
}

int api_backbunsetu(int *a)
{
    u8 *p;
    u8 *q;
    int pos;
    int len;

    len = 0;
    if (func_mode != 3) {
        return -1;
    }
    p = (u8 *)a[0];
    q = (u8 *)a[1];
    if (cur_pos == 0) {
        return 0;
    }
    unify_khmem(cur_pos, 0);
    pos = 0;
    while (pos < cur_pos) {
        len = bunsetu_len(pos);
        if (pos + len >= cur_pos) {
            break;
        }
        pos += len;
    }
    cur_pos = pos;
    cur_len = len;
    init_kouho(0, 1);
    get_kouhostr(p, q);
    return kh_count(hchar[cur_pos].kh);
}

int api_nextbunsetu(int *a)
{
    u8 *p;
    u8 *q;

    if (func_mode != 3) {
        return -1;
    }
    p = (u8 *)a[0];
    q = (u8 *)a[1];
    if (cur_pos + cur_len >= kana_len) {
        return 0;
    }
    unify_khmem(cur_pos, 0);
    cur_pos += cur_len;
    cur_len = bunsetu_len(cur_pos);
    init_kouho(0, 1);
    get_kouhostr(p, q);
    return kh_count(hchar[cur_pos].kh);
}

int api_khhenkan(int *a)
{
    int mode;
    u8 *p;
    u8 *q;

    if (func_mode != 3) {
        return -1;
    }
    p = (u8 *)a[0];
    q = (u8 *)a[1];
    switch (a[-1]) {
    case 37:
        mode = 2;
        break;
    case 38:
        mode = 1;
        break;
    case 39:
        mode = 3;
        break;
    case 40:
        mode = 4;
        break;
    default:
        mode = 4;
        break;
    }
    khmem_raw(mode);
    if (hchar[cur_pos].kh == 0) {
        return -1;
    }
    init_kouho(0, 1);
    get_kouhostr(p, q);
    return 0;
}

int api_none(void)
{
    return -1;
}
