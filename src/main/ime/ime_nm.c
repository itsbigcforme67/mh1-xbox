/* "Ask" Japanese input method / dictionary engine, SLPM_654.95 main 0x23E500-0x24A240
 * (kana to kanji conversion for the name entry: roman input, bunsetu segmentation,
 * candidate lists, dictionary pages on disc, learning). Function and global names come
 * from the original symbols; struct fields are named by offset (guesses). Source of
 * truth for the whole range, in ADDRESS order, brace on its own line (tools/genruns.py
 * extracts the matching runs). */
#include "types.h"

typedef long long s64;
typedef struct NODE NODE;

typedef struct PW {
    u16 x00;
    u8 x02;
    u8 x03;
    s32 x04;
    s64 id;         /* 0x08 dictionary word id */
} PW;

typedef struct KH {
    s32 x00;
    s32 x04;
    PW *pw;         /* 0x08 */
    u16 x0C;
    u8 x0E;
    u8 x0F;
} KH;

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
typedef struct BS BS;
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
extern int overlay_addr[];          /* 0x386B00 */
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
KH *take_kouho();
void free_khmemlist();
void free_mem();
KH *kh_followed();
KH *kh_skip();
int meantosjis();
int dic_learn();
void dic_newlearn();
struct WD *raw_newwd();
void dic_tmptouroku();
int strlen();
void strncpy();
void clear_prevwd();
void kh_learn();
void prev_learn();
void add_prevwd();
void api_funcent();
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
u16 get_entid_tab();
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
s16 tmpoffset();

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
    addr = overlay_addr[(u8)arg1];
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

/* learn the chosen candidate (pos, len unused) */
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
        if (kh != 0 && (pw = kh->pw) != 0) {
            wd.x07 = pw->x02;
            wd.x08 = pw->x03;
        } else {
            wd.x08 = 0;
            wd.x07 = 0x28;
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
        len = kh->x0E;
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

void apiask_25_FirstKakutei(int a, int b, int c, int d)
{
    int req[6];

    req[1] = a;
    req[2] = b;
    req[3] = c;
    req[0] = 0x19;
    req[4] = d;
    req[5] = 0;
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

int kwin_length(void)
{
    return 0x48;
}

int nwin_length(void)
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
    memcpy(gAskRom.cur, buf, len);
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
        if (r < 0 || r >= 0x97C01) {
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

u16 to_zenkaku(int c)
{
    u16 r;

    r = asc2jis[c & 0xFF];
    if ((r & 0xFF00) == 0x2500 && !(c & 0x100)) {
        r = (r & 0xFF) | 0x2400;
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
        if (*k == (c & 0xFF)) {
            r = *t;
            if ((r & 0xFF00) == 0x2500 && !(c & 0x100)) {
                return (r & 0xFF) | 0x2400;
            }
            return r;
        }
        k++;
        t++;
    }
    return 0;
}

int ext_jis(int c, int hi)
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
    int c;

    c = code & 0xFFFF;
    for (p = btoudata; p < btoudata + 180; p += 4) {
        if (*(u16 *)p == c) {
            return p[2];
        }
        if (c < *(u16 *)p) {
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
    if (c < 0x80) {
        if (c < 0x2D && (flag & 0xFF)) {
            c = g2jodo(a0) & 0xFF;
            if (c == 0) {
                return 0;
            }
        } else {
            if (c == 0xD || (c >= 0x13 && c < 0x35) || c == 0x38) {
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
        if ((b >= 5 && b < 9) || b >= 0xA) {
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
        if (b >= 5) {
            return 1;
        }
        return 0;
    default:
        if (b == 4 || (u32)(b - 7) < 2 || b >= 0xA) {
            return 1;
        }
        return 0;
    }
}

int dic_open(u8 *name)
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
    r = 3;
    if (dic_rw == 0x8000) {
        r = -6;
    }
    return r;
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
    while (p < end && *p >= 0x39) {
        p += 2;
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
    s16 page;
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
    base = load_page(page);
    e = base;
    while (ELEN(e) != 0) {
        klen = e[2];
        c = ask_strncmp(e + 3, key, klen);
        if (c == 0) {
            if (klen == len) {
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

int dic_get1wd(int a, int b, int out)
{
    int off;
    int tmp;
    int unused;
    int best;
    int page;

    if (dic_fd == -1) {
        return -3;
    }
    page = get_entid_tab(&off, &tmp, &unused);
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
    u8 *p;

    w[0] = tag;
    w[1] = rec[1];
    p = rec + 2;
    ((u8 *)w)[4] = 0;
    if (rec[2] < 0xC) {
        p++;
    }
    getkbuf(p);
}

int dic_getallwd(int a, int b, int out)
{
    int off;
    int tmp;
    int cnt;
    int page;
    int n;

    if (dic_fd == -1) {
        return -3;
    }
    page = get_entid_tab(&off, &tmp, &cnt);
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
    u8 *q;

    end = ent + ELEN(ent);
    p = ent + ent[2] + 3;
    while (p < end) {
        p[1] = 0;
        q = p + 2;
        if (p[2] < 0xC) {
            q++;
        }
        p = next_wd(q, end);
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
    u8 *q;

    end = ent + ELEN(ent);
    p = ent + ent[2] + 3;
    m = 0;
    while (p < end) {
        if (m < p[1]) {
            m = p[1];
        }
        q = p + 2;
        if (p[2] < 0xC) {
            q++;
        }
        p = next_wd(q, end);
    }
    return (u8)m;
}

int dic_touroku(SYNR *w)
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

int isnum(u8 *p)
{
    while (*p != 0) {
        if (*p < 0x30 || *p >= 0x3A) {
            return 0;
        }
        p++;
    }
    return 1;
}

int dic_freeentid(void)
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

u8 *set_num(u8 *s, int n0, u8 *out, int kind)
{
    s16 n;
    u8 *p;
    u8 *q;
    u8 *t;
    int i;
    int d;
    int r;
    int g;
    int k;
    int idx;

    n = n0;
    if (kind >= 2 && n >= 0xE) {
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
            t = num_chars[kind] + d * 2;
            p[0] = t[0];
            p[1] = t[1];
            p += 2;
        } else {
            r = n - i - 1;
            g = r % 4;
            if (d != 0 && (kind != 2 || d != 1 || (u32)(g - 1) >= 2)) {
                k = 1;
                if (d > 0) {
                    k = kind - 1;
                    if (d >= 4) {
                        k = 1;
                    }
                }
                t = num_chars[k] + d * 2;
                p[0] = t[0];
                p[1] = t[1];
                p += 2;
            }
            if (r != 0 && (g != 0 || q != p) && (g == 0 || d != 0)) {
                idx = r >> 2;
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
                t = num_chars[2] + idx * 2;
                p[0] = t[0];
                p[1] = t[1];
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

int seek_dic(int pos)
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
    if (temp_page == old_temp && suji_mode == old_suji && entry2upd == 0) {
        return 0;
    }
    header[0x30] = temp_page % 256;
    header[0x31] = temp_page / 256;
    header[0x32] = gaku_mode % 256;
    header[0x33] = gaku_mode / 256;
    header[0x34] = suji_mode % 256;
    header[0x35] = suji_mode / 256;
    if (seek_dic(0) == -1) {
        return -1;
    }
    return -(d_write(dic_fd, header, 0x400) != 0x400);
}

int read_index(void)
{
    if (seek_dic(0x400) == -1) {
        return -1;
    }
    return -(d_read(dic_fd, mainindex, 0x1000) != 0x1000);
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
    int lo;
    int hi;
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
    if (key[n] != 0) {
        n++;
    }
    return n;
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
    if (seek_dic((p->id << 10) + 0x3400) == -1) {
        return -1;
    }
    return -(d_write(dic_fd, p->data, 0x400) != 0x400);
}

int read_page(PAGE *p)
{
    if (seek_dic((p->id << 10) + 0x3400) == -1) {
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

u16 get_entid_tab(unsigned int id, int *b, int *c, int *rt)
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

    if (id >= 0x80) {
        return -1;
    }
    e = &entid_tab[id];
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
        if (pg == (entid_tab[i].c >> 12)) {
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

    if ((u32)(temp_end - len - 2) < (u32)temp_top) {
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

s16 tmpoffset(u8 *p)
{
    int d;

    d = p - temp_pages[0];
    return ((d >> 10) << 12) | (d % 1024);
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
    return -(d_write(dic_fd, temp_pages, 0x2000) != 0x2000);
}

int newwdlen(WD *w)
{
    int extra;

    if (w->x08 == 0 && w->x07 < 0x2D) {
        extra = 2;
    } else {
        extra = 3;
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
    need = newwdlen(w);
    rec = alloc_record(need);
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
    if (c >= 0x80 && c < 0xA0) {
        return 1;
    }
    if (c >= 0xE0 && c < 0xFD) {
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
    s8 sel;
    s8 cur;

    henkan_mode = mode;
    fl_check();
    if (henkan_mode != 3 || ikkatsu_mode != 0) {
        pos = start;
        if (start < end) {
            top = start + pref;
            while (pos < end) {
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
                    sel = bs_prefer(pos, end, -1);
                    if (sel == -1) {
                        break;
                    }
                }
                unify_bsmem(pos, sel);
                first_kouho(pos, sel);
                h->x15 = sel;
                pos += sel;
            }
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
                if (len < 0) {
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
