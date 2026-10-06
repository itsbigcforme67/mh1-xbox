/* lb_at01 - browser body wait-cancel states 0x005F3A20-0x005F3DD4: BsBody07_WaitCancel2, BsBody08_WaitCancel3, BsBody09_WaitCancel4, BsBody10_WaitCancel5, BsBody11_WaitCancel6, BsBody12_WaitCancel7, BsBody13_WaitCancel8, BsBody14_WaitCancel9, BsBody15_WaitCancel10. Whole file in lb_at.c. */
#include "lobby_f.h"
extern BSSYS *bsSys;
extern u8 *bsCur;
extern s8 bsIsOnRequesting;
extern s8 bs_end_type;
extern u8 bsUrl[4];
void MoveAndTransSet();
void CheckAllImages();
void historyBack();
void historyForward();
void To_BodyMain_ReqSrc();
void To_BodyMain_RcvSrc();
void BsUrlSet();
void BsRequestHtmlPost();
void refreshPage();
void To_QuitMain_Init();
int BsRequestCheck();
void To_NetErrorDialog();

void BsBody07_WaitCancel2(void) {
    u8 a0;
    u8 a1;
    BSSYS *b;
    MoveAndTransSet();
    b = bsSys;
    a0 = b->x2E;
    switch (a0) {
    case 8:
        CheckAllImages();
        return;
    case 10:
        a1 = b->x2C;
        switch (a1) {
        case 1:
            historyBack();
            return;
        case 2:
            historyForward(a1, b);
        }
        break;
    }
}

void BsBody08_WaitCancel3(void) {
    u8 a0;
    MoveAndTransSet();
    a0 = bsSys->x2E;
    switch (a0) {
    case 8:
        CheckAllImages();
        return;
    case 10:
        To_BodyMain_ReqSrc(a0);
    }
}

void BsBody09_WaitCancel4(void) {
    u8 a0;
    BSSYS *b;
    MoveAndTransSet();
    b = bsSys;
    a0 = b->x2E;
    switch (a0) {
    case 8:
        CheckAllImages();
        return;
    case 10:
        BsUrlSet(bsUrl, b->meta);
        BsRequestHtmlPost(bsUrl);
        bsIsOnRequesting = 1;
        To_BodyMain_RcvSrc();
    }
}

void BsBody10_WaitCancel5(void) {
    u8 a0;
    MoveAndTransSet();
    a0 = bsSys->x2E;
    switch (a0) {
    case 8:
        CheckAllImages();
        return;
    case 10:
        refreshPage(a0);
    }
}

void BsBody11_WaitCancel6(void) {
    u8 a0;
    MoveAndTransSet();
    a0 = bsSys->x2E;
    switch (a0) {
    case 8:
        CheckAllImages();
        return;
    case 10:
        bs_end_type = 1;
        To_QuitMain_Init(0);
    }
}

void BsBody12_WaitCancel7(void) {
    u8 a0;
    MoveAndTransSet();
    a0 = bsSys->x2E;
    switch (a0) {
    case 8:
        CheckAllImages();
        return;
    case 10:
        bs_end_type = 2;
        To_QuitMain_Init(0);
    }
}

void BsBody13_WaitCancel8(void) {
    if (BsRequestCheck(bsUrl) != 0) {
        bsIsOnRequesting = 0;
        To_NetErrorDialog(0);
        bsCur[2] = 1;
        bsSys->x02 = 5;
    }
}

void BsBody14_WaitCancel9(void) {
    u8 a0;
    a0 = bsSys->x2E;
    switch (a0) {
    case 8:
        CheckAllImages();
        return;
    case 10:
        To_NetErrorDialog(0);
        bsCur[2] = 1;
        bsSys->x02 = 5;
    }
}

void BsBody15_WaitCancel10(void) {
    if (BsRequestCheck(bsUrl) != 0) {
        bsIsOnRequesting = 0;
        To_QuitMain_Init(0);
        bsSys->x01 = 2;
        bsSys->x02 = 0;
    }
}
