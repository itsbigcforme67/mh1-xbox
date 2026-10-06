/* lb_gdr14 - browser tags/tables/http 0x005F3FE0-0x005F407C: BsFixPalInit. Whole file in lb_dr2.c. */
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


/* fixed browser font palettes 0x10 (white on black) and 0x11 (grey, dark outline) */
void BsFixPalInit(void) {
    BsGameSideFontPalInit();
    pal[1] = 0;
    pal[0] = 1;
    *(s32 *)(pal + 4) = -1;
    flfntSetPalData(0x10, 0, 0xFF000000, -1, -1);
    pal[9] = 0;
    pal[8] = 1;
    *(s32 *)(pal + 0xC) = 0xFF000001;
    flfntSetPalData(0x11, 0x808080, 0xFF222222, 0xFF111111, 0xFF010101);
}
