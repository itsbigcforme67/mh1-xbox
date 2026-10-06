/* lb_gdr12 - browser tags/tables/http 0x005E9210-0x005E92B8: BsUrlBadHeaderGet. Whole file in lb_dr2.c. */
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


/* index of the first "bad header" prefix the url starts with, -1 if none */
int BsUrlBadHeaderGet(char *url) {
    int i;
    char **p;
    char *cur;
    i = 0;
    p = BadHeaderList;
    BsUrlCopy_SS(BsCacheTmpUrlstr, url);
    _my_tolower(BsCacheTmpUrlstr);
    cur = BadHeaderList[0];
    if (cur != 0) {
        do {
            if (strncmp(BsCacheTmpUrlstr, *p, strlen(cur)) == 0) {
                break;
            }
            p += 1;
            cur = *p;
            i += 1;
        } while (cur != 0);
    }
    if (*p != 0) {
        return i;
    }
    i = -1;
    return i;
}
