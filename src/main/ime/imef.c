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
BS *ins_bsmem();
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
int exist_kouho();
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
s16 bs_prefer();
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

extern SYNR entbuf;

/* learn the chosen candidate (pos, len unused) */

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
