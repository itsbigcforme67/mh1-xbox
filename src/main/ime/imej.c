/* "Ask" Japanese input method / dictionary engine, SLPM_654.95 main 0x23E500-0x24A240
 * (kana to kanji conversion for the name entry: roman input, bunsetu segmentation,
 * candidate lists, dictionary pages on disc, learning). Function and global names come
 * from the original symbols; struct fields are named by offset (guesses). Source of
 * truth for the whole range, in ADDRESS order, brace on its own line (tools/genruns.py
 * extracts the matching runs). */
#include "types.h"

typedef long long s64;
typedef unsigned long long u64;
typedef struct NODE NODE;
typedef struct BS BS;
typedef struct KH KH;

typedef struct PW {
    s16 x00;
    u8 x02;
    u8 x03;
    s32 x04;
    u64 id;         /* 0x08 dictionary word id */
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
    u64 id;         /* 0x08 */
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
    u16 pri;        /* 0x04 */
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
    u16 x08;
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
extern u64 wdsbuf[128];
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
static void add_prevwd();
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
static int g2jodo();
static int getbit(s16);
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
static void kh_append_init();
static void kh_append();
static int kh_merge_getone();
static int exist_kouho();
int kh_length();
int kh_count();
KH *take_kouho();
static void kouho_set_num();
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
static int bytesin_kana_buf();
int count_byte_kana_buf();
int api_funcent();
static int get_kouhostr();
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
extern s16 pwordmap[96];
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

int dic_newlearn(WD *w, u64 *list, int n)
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
                k = (d > 0 && d <= 3) ? kind - 1 : 1;
                p[0] = num_chars[k][d * 2];
                p[1] = num_chars[k][d * 2 + 1];
                p += 2;
            }
            if (r != 0 && (g != 0 || q != p) && (g == 0 || d != 0)) {
                if (g == 0) {
                    idx = r / 4 + 6;
                } else if (g == 1) {
                    idx = (kind == 2) ? 4 : 0;
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
    s8 c;

    while (n-- != 0) {
        c = *(s8 *)a;
        d = (u8)c - *b;
        if (!!d) {
            return d;
        }
        if (c == 0) {
            return 0;
        }
        a++;
        b++;
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
    u8 *h;

    if (seek_dic(0) == -1) {
        return -1;
    }
    if (d_read(dic_fd, header, 0x400) != 0x400) {
        return -1;
    }
    h = header;
    if (ask_strncmp(h, title, strlen(title)) != 0) {
        return -1;
    }
    mainlower = 0;
    mainupper = h[0x2E] + (h[0x2F] << 8);
    old_temp = temp_page = h[0x30] + (h[0x31] << 8);
    old_suji = suji_mode = h[0x34] + (h[0x35] << 8);
    entry2upd = 0;
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
    int row;
    u8 *q;

    if (key[0] < 0xA1) {
        return 0;
    }
    row = key[0] - 0xA1;
    if ((s16)len == 1) {
        b = 0;
    } else {
        if (key[1] < 0xA1) {
            return 0;
        }
        b = key[1] - 0xA0;
    }
    q = &entry2code[(b >> 3) + row * 0xB];
    if ((1 << (b & 7)) & *q) {
        return 0;
    }
    return 1;
}

void set_entry2(u8 *key, int len)
{
    int b;
    int row;
    u8 *q;

    if (key[0] >= 0xA1) {
        row = key[0] - 0xA1;
        if ((s16)len == 1) {
            b = 0;
        } else {
            if (key[1] < 0xA1) {
                return;
            }
            b = key[1] - 0xA0;
        }
        q = &entry2code[(b >> 3) + row * 0xB];
        *q |= (1 << (b & 7)) & 0xFF;
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
    int i;
    ENTID *e;
    int free;

    free = -1;
    for (e = entid_tab, i = 0; i < 128; i++, e++) {
        if (e->cnt == 0) {
            if (free == -1) {
                free = i;
            }
        } else if (e->a == a && e->b == b && e->c == c) {
            e->cnt++;
            return i;
        }
    }
    if (free == -1) {
        return -1;
    }
    e = &entid_tab[free];
    e->cnt = 1;
    e->a = a;
    e->b = b;
    e->c = c;
    e->rtime = rt;
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
    u8 *p;

    for (i = 0; i < 8; i++) {
        p = temp_pages[i];
        p[1] = 0;
        p[0] = 0;
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

NODE **srch_node(key, len, found)
u8 *key;
int len;
NODE **found;
{
    NODE *n;
    NODE *prev;
    u8 *e;
    int c;
    int klen;
    int h;

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
        return &hash_tab[(u32)h];
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
            strncpy(key, p + 3, (int)klen);
            key[(s16)klen] = 0;
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
    u8 *p;
    int i;
    u8 *pg;
    s16 klen;
    u8 key[0x50];

    if (seek_dic(0x1400) == -1) {
        return -1;
    }
    if (d_read(dic_fd, temp_pages, 0x2000) != 0x2000) {
        return -1;
    }
    for (i = 0, pg = temp_pages[0]; i < 8; i++) {
        p = pg;
        while (ELEN(p) != 0) {
            klen = p[2];
            if (klen != 0) {
                strncpy(key, p + 3, (int)klen);
                key[(s16)klen] = 0;
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
    *r++ = len % 256;
    *r++ = len / 256;
    *r++ = w->len;
    strncpy(r, w->yomi, w->len);
    r += w->len;
    *r++ = w->x07;
    *r++ = rt;
    if (w->x08 != 0 || w->x07 >= 0x2D) {
        *r++ = w->x08;
    }
    setkbuf(w->tango, r);
}

void upd_record(u8 *r, int add, WD *w, int rt)
{
    int old;

    old = ELEN(r);
    add += old;
    r[0] = add % 256;
    r[1] = add / 256;
    r += old;
    r[0] = w->x07;
    r[1] = rt;
    r += 2;
    if (w->x08 != 0 || w->x07 >= 0x2D) {
        *r = w->x08;
        r++;
    }
    setkbuf(w->tango, r);
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
