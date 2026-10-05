/* lb_z34 - auto-drafted 0x005FDAA0-0x005FDB70: BsMakeErrorHtml (first drafted by tools/lbauto.py). */
#include "lobby_f.h"
extern char HtmlHeadErrorTbl[];
extern char HtmlErrorMessageTbl[];
extern char HtmlEndErrorTbl2[];
extern char HtmlEndErrorTbl1[];

void BsMakeErrorHtml(s32 arg0, int *arg1, int *arg2, s32 arg3) {
    s32 sp4C;

    sp4C = arg0;
    BsHtmlMemcpy2(&sp4C, &HtmlHeadErrorTbl, 0x14C);
    BsHtmlMemcpy2(&sp4C, arg1, strlen(arg1) + 1);
    BsHtmlMemcpy2(&sp4C, &HtmlErrorMessageTbl, 0x18);
    BsHtmlMemcpy2(&sp4C, arg2, strlen(arg2) + 1);
    if (arg3 != 0) {
        BsHtmlMemcpy2(&sp4C, &HtmlEndErrorTbl2, 0x69);
        return;
    }
    BsHtmlMemcpy2(&sp4C, &HtmlEndErrorTbl1, 0x32);
}
