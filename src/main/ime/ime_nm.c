/* "Ask" Japanese input method / dictionary engine, SLPM_654.95 main 0x23E500-0x24A240
 * (kana to kanji conversion for the name entry: roman input, bunsetu segmentation,
 * candidate lists, dictionary pages on disc, learning). Function and global names come
 * from the original symbols; struct fields are named by offset (guesses). Source of
 * truth for the whole range, in ADDRESS order, brace on its own line (tools/genruns.py
 * extracts the matching runs). */
#include "types.h"

typedef long long s64;

typedef struct PW {
    s32 x00;
    u8 x04;
    u8 x05;
    s32 x08pad;
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

/* 0x1C-byte edit character / bunsetu record, hchar[80] */
typedef struct HCHAR {
    s32 x00;
    s32 x04;
    void *x08;      /* 0x08 */
    KH *kh;         /* 0x0C candidate list */
    s32 x10;
    s32 x14;
    s32 x18;
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
void dic_learn();
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
void close_dic();
void init_page();
void init_temp();
int open_dic();
int read_head();
int read_index();
void set_dicname();
void flush_head();
void flush_temp();
void flush_pages();

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
void kh_learn(int pos, int len, KH *kh, void **list)
{
    s64 *out;
    PW *pw;
    s64 last;
    s64 first;
    int n;
    void **node;

    pw = 0;
    if (kh->x0C == 0xFFFF || (pw = kh->pw) != 0) {
        out = wdsbuf;
        if (list == 0 || list == (void **)-1) {
            if (pw != 0) {
                wdsbuf[0] = *(s64 *)((u8 *)pw + 8);
                out = wdsbuf + 1;
            }
        } else {
            first = 0;
            last = first;
            do {
                node = *(void ***)((u8 *)list + 4);
                if (node != 0) {
                    s64 id = *(s64 *)((u8 *)node + 8);
                    if (id != last && id != first) {
                        *out = id;
                        first = id;
                        out++;
                    }
                }
                list = *(void ***)((u8 *)list + 0xC);
            } while (list != 0);
        }
        if (pw == 0) {
            n = out - wdsbuf;
            dic_newlearn(raw_newwd(list), wdsbuf, n);
            return;
        }
        n = out - wdsbuf;
        dic_learn(*(s64 *)((u8 *)pw + 8), kh->x0C, wdsbuf, n);
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
            wd.x07 = pw->x04;
            wd.x08 = pw->x05;
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
            kh_learn(pos, len, kh, h->x08);
            pos += len;
            continue;
        }
        if (((s8 *)h)[0x16] != 0 && ((s8 *)h)[0x16] != ((s8 *)h)[0x15]) {
            add_prevwd(pos, len, kh, 1);
            kh_learn(pos, len, kh, h->x08);
            pos += len;
            continue;
        }
        if (prev_yomi[0] != 0) {
            add_prevwd(pos, len, kh, 0);
            prev_learn(kh);
            clear_prevwd();
            pos += len;
        } else {
            kh_learn(pos, len, kh, h->x08);
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

int FAskRom_Read(int fd, void *buf, int n)
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

int FAskRom_Write(int fd, void *buf, int n)
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

int FAskRom_Seek(int fd, int off, int whence)
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
