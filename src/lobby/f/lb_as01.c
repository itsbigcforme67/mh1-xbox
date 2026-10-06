/* lb_as01 - browser mode dispatchers 0x005F2750-0x005F2814: MainBrowser, BsNormalMode, BsPosterMode. Whole file in lb_as.c. */
#include "lobby_f.h"
extern BSSYS *bsSys;
extern u8 *bsCur;
extern s8 bsMainRetVal;
extern u8 BS_MODE_R_NO;
extern void (*main_browser_jmp_378[3])();
extern void (*normal_browser_jmp_383[4])();
extern void (*poster_browser_jmp_388[4])();
extern void (*bs_poster_jmp_397[7])();
extern void (*bs_init_jmp_503[2])();
extern void (*bs_body_jmp_560[16])();
extern void (*bs_quit_jmp_2477[4])();
extern void (*cs_main_jmp_2535[3])();
extern void (*bsCur_body_jmp_2544[9])();
void BsKeySet();
void BsRequestTask();
void BsCheckInetProblem();
void TransReset();
void BsCsMain();
void BsSetRenderState();

s8 MainBrowser(void) {
    BsKeySet();
    BsRequestTask();
    BsCheckInetProblem();
    main_browser_jmp_378[BS_MODE_R_NO]();
    return bsMainRetVal;
}

void BsNormalMode(void) {
    TransReset();
    normal_browser_jmp_383[bsSys->x01]();
    BsCsMain();
}

void BsPosterMode(void) {
    poster_browser_jmp_388[bsSys->x01]();
}
