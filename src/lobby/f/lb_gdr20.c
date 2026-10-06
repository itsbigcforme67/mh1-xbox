/* lb_gdr20 - browser tags/tables/http 0x00604670-0x00604784: tagoutprintf2. Whole file in lb_dr2.c. */
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


/* print a tag text that may end in a blank-like character (kept back for the next call) */
void tagoutprintf2(u8 *arg0) {
    int temp_a0;
    int temp_a1;
    int var_a3;
    u8 temp_v1;
    u8 var_s0;
    u8 *temp_a0_2;
    u8 *temp_v0;
    s32 *np;

    var_a3 = 0;
    temp_v0 = bsw;
    var_s0 = 0;
    temp_a0 = *(s32 *)(temp_v0 + 4);
    temp_a1 = temp_a0 - 1;
    np = (s32 *)(temp_v0 + 4);
    if (0 < temp_a1) {
        do {
            if (INTCTYPE_MAP_006535E0[arg0[var_a3 & 0xFF]] & 1) {
                var_a3 = (var_a3 + 1) & 0xFF;
            }
            var_a3 = (var_a3 + 1) & 0xFF;
        } while (var_a3 < (*(volatile s32 *)np - 1));
    }
    if ((var_a3 & 0xFF) == temp_a1) {
        temp_a0_2 = (u8 *)(temp_a0 + (int)arg0);
        temp_v1 = temp_a0_2[-1];
        if (INTCTYPE_MAP_006535E0[temp_v1] & 1) {
            temp_a0_2[-1] = 0;
            var_s0 = temp_v1;
        }
    }
    tagoutprintf3(arg0, temp_a1, np, var_a3);
    if (!(var_s0 & 0xFF)) {
        *(s32 *)(bsw + 4) = 0;
        arg0[0] = 0;
        return;
    }
    *(s32 *)(bsw + 4) = 1;
    arg0[0] = var_s0;
    arg0[1] = 0;
}
