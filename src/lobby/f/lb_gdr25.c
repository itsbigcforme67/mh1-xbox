/* lb_gdr25 - browser tags/tables/http 0x005DA880-0x005DA99C: http_test_06. Whole file in lb_dr2.c. */
#include "lobby_f.h"
extern u8 *bsw;
extern u8 BFontSizeN[];
extern u8 BFontSizeH[];


extern u8 HttpWork[];
void http_test_proc(u8 *p);
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


void *sceHTTPGetResponse();
void KanjiEuc2SjisEx();
void *memcpy(void *, const void *, unsigned int);


void CpInetInterfaceProblemEnable();
int sceHTTPParseURI();


extern s16 ParseCk_ret;
extern s32 ParseArg;
extern u8 ParseReq;
int parsetag();


/* http state 6: pick up the response body into the task buffer (converted to Shift-JIS when the page is EUC) */
void http_test_06(u8 *arg0) {
    s32 temp_a1;
    s32 temp_v0_2;
    s32 var_s1;
    u8 *temp_v0;
    u8 *temp_v1;

    if (*(s8 *)(arg0 + 0x35) != 0) {
        *(s8 *)(arg0 + 0x3D) = 0x11;
        *(s8 *)(arg0 + 0x3C) = 3;
        *(u8 *)(arg0 + 0x40) = 0xB;
        return;
    }
    temp_v0 = sceHTTPGetResponse(*(s32 *)(arg0 + 0x6C));
    *(u8 **)(arg0 + 0x7C) = temp_v0;
    if (temp_v0 == 0) {
        *(s8 *)(arg0 + 0x3D) = 7;
        *(s8 *)(arg0 + 0x3C) = 3;
        *(u8 *)(arg0 + 0x40) = 0xB;
        return;
    }
    temp_v1 = *(u8 **)(arg0 + 0x7C);
    temp_a1 = *(s32 *)(temp_v1 + 0x18);
    if (temp_a1 != 0) {
        var_s1 = *(s32 *)(temp_v1 + 0x20);
        temp_v0_2 = *(s32 *)(arg0 + 0x14);
        if (temp_v0_2 <= var_s1) {
            var_s1 = temp_v0_2 - 1;
        }
        memcpy(*(char **)(arg0 + 0x10), (void *)temp_a1, var_s1);
        (*(char **)(arg0 + 0x10))[var_s1] = 0;
        *(s32 *)(arg0 + 0x38) = var_s1;
        if (*(s8 *)(arg0 + 0x34) != 0) {
            if (is_sjis(arg0, *(u8 **)(arg0 + 0x10), var_s1) == 0) {
                KanjiEuc2SjisEx(*(char **)(arg0 + 0x10));
            }
            *(s32 *)(arg0 + 0x38) = strlen(*(char **)(arg0 + 0x10));
        }
    } else {
        **(char **)(arg0 + 0x10) = 0;
        *(s32 *)(arg0 + 0x38) = 0;
    }
    *(u8 *)(arg0 + 0x40) = *(u8 *)(arg0 + 0x40) + 1;
}
