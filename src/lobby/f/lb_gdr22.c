/* lb_gdr22 - browser tags/tables/http 0x00605810-0x006058D0: set_TH_TD_data_1st. Whole file in lb_dr2.c. */
#include "lobby_f.h"
extern u8 *bsw;
extern u8 BFontSizeN[];
extern u8 BFontSizeH[];


extern u8 HttpWork[];
extern void (*http_test_proc_jmp_178[])(u8 *);
extern BSSYS *bsSys;
extern u8 *bsCur;
extern s16 BsTimer1;





extern s32 ContentType_encoded;
extern s32 ContentLength_0;
extern s32 CacheControl_nocache;
extern s32 Pragma_nocache;
extern char verisign_root_ca[];
typedef struct CAINFO { char *data; int size; } CAINFO;
void CpInetHttpInitialize();
void BsMemAllocInitialize();
int BsMemAlloc();
int BsMemRealloc();
int BsMemFree();
void sceHTTPSetMallocFunction();
void sceHTTPSetReallocFunction();
void sceHTTPSetFreeFunction();
int sceHTTPInit();
int get_CA_size();
int sceHTTPSInitCertFromMemory();


extern u8 Hn_Size[8];
extern char lit_155_006676E0[];
char *strstr(const char *, const char *);
void font_data_clear();
void tagoutprintf4();
void font_data_set();




extern int strlen();
void tagoutprintf3();


void font_data_clear();
void font_data_set();



void stockTableOutline(int x, int y, int x2, int y2, int t0, int t1, int t2, int t3, u8 *p);


extern u8 pal[];
void BsGameSideFontPalInit();
void flfntSetPalData(int, int, int, int, int);


extern char *BadHeaderList[2];
extern char BsCacheTmpUrlstr[];
void BsUrlCopy_SS();
void _my_tolower();
int strncmp(const char *, const char *, int);


extern BSNODE BcRequest_head;


extern char *bsCsv;
extern char *bsUrl;
extern s8 bsIsOnRequesting;
extern char lit_429_00667370[];
extern char lit_430_00667378[];
int BsRequestPostAdd();
void BsRequestHtmlPost();
int strcmp(const char *, const char *);


extern u8 BsLbsErrNum;
void To_ReqCancelWait();
void To_QuitMain_Init();


extern char lit_444_00667798[];
extern char *special_character_tbl[5];
extern char change_character_tbl[5];
char *strstr(const char *, const char *);


extern u8 INTCTYPE_MAP_006535E0[];


extern u8 *bsOW[500];




void init_td_data();
int SetTableData();
void set_TH_TD_data_1st();
int check_rowspan_sub();





extern u8 INTCTYPE_MAP_006535E0[];


/* copy the cell settings of a table cell record (TH/TD) to the new cell and count the use of the source */
void set_TH_TD_data_1st(u8 *d, u8 *s) {
    u8 v;
    u8 v2;
    if (*(s8 *)(bsw + 0x186) == -0xA) {
        d[0x4C] = s[0x4C];
        d[0x4D] = s[0x4D];
        d[0x45] = s[0x45];
        *(u16 *)(d + 0x32) = *(u16 *)(s + 0x32);
        *(u16 *)(d + 0x30) = *(u16 *)(s + 0x30);
        d[0x50] = d[0x50] | bsw[0x18A];
        *(s32 *)(d + 0x54) = *(s32 *)(bsw + 0xF18);
        v = bsw[0xF16];
        if (v != 0) {
            d[0x4A] = v;
        } else {
            d[0x4A] = s[0x4A];
        }
        v2 = bsw[0xF17];
        if (v2 != 0) {
            d[0x4B] = v2;
        } else {
            d[0x4B] = s[0x4B];
        }
        d[0x44] = 0;
        *(u16 *)(d + 0x20) = *(u16 *)(bsw + 0xF10);
    }
    s[0x4C] = s[0x4C] + 1;
}
