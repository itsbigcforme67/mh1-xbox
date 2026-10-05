/* Lobby: small browser (Bs*) helpers, hand-fixed from lbauto drafts (SLPM_654.95 lobby overlay). Whole file; runs split into lb_mNN.c */
#include "lobby.h"
void BsWorkTrans();
void To_RetVal();
void flSetRenderState();
int strncmp();

void BsBgImgTrans(void) {
    BsWorkTrans(0);
}

void BsPageObjTrans(void) {
    BsWorkTrans(1);
}

void BsHScrlBarTrans(void) {
    BsWorkTrans(2);
}

void BsVScrlBarTrans(void) {
    BsWorkTrans(2);
}

void BsTitleBarTrans(void) {
    BsWorkTrans(2);
}

void BsToolMenuTrans(void) {
    BsWorkTrans(3);
}

void BsSoftKbdTrans(void) {
    BsWorkTrans(4);
}

void BsDialogTrans(void) {
    BsWorkTrans(6);
}

void BsCursorTrans(void) {
    BsWorkTrans(8);
}

void BsPosterQuit(void) {
    To_RetVal(1);
}

void BsSetRenderState(u8 a, int b) {
    flSetRenderState(a, b);
}

int BsUrlCompare_SS(char *a, char *b) {
    return strncmp(a, b, 0x100);
}

void eoo_cb(int a, u8 b, u8 *p) {
    *p = b;
}

void eot_cb(int a, u8 b, u8 *p) {
    *p = b;
}
