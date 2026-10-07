/* lb_pz14_nm - lobby.bin 0x00597DA0-0x00598638: plaza_mailBox(), mail box menu. NEAR-MATCH (4 insns: after the sw tests the original loads x26 into a1 and keeps pNet in v0; mine v0/v1). NOT linked. */
#pragma readonly_strings on
#include "lbui_proto.h"
int mail_input();
int Lb_select();
int Lbc_SendMail();
void SetDialogYesNo();
int Lb_cursorUD();
int KinshiYogo_chk();
extern u8 RecvMailInfo[][0x9A];
extern char seekStr[];
extern u8 my_user_id[];
typedef struct { u8 b[0x9A]; } BLK9A;
#define XA (*(u8 *)&pNet->x0A)
#define XE (*(u8 *)&pNet->x0E)
#define X6 (*(u8 *)&pNet->x06)
int plaza_mailBox()
{
    u16 sw = Get_sw2(0);
    LB_NETW *a = pNet;
    u8 *st = &a->step;
    int i;
    int r;
    LB_NETW *w;
    s16 t;
    u8 *p;

    switch (*st) {
    case 0:
        (*st)++;
        XA = 0;
        XE = 0;
        break;
    case 1:
        pNet->x26 = 0;
        i = 0;
        p = (u8 *)RecvMailInfo;
        do {
            if (((s8 *)p)[1] == 0) {
                break;
            }
            p += 0x9A;
            i = (s16)(i + 1);
            pNet->x26++;
        } while (i < 8);
        pNet->x28 = Get_sw_on2(0);
        if (sw & 0x20) {
            if (pNet->x26 != 0) {
                *(BLK9A *)(cw + 0x2F7F) = *(BLK9A *)RecvMailInfo[XA];
                XE = XA;
                XA = 0;
                RecvMailInfo[XE][0] = 0;
                pNet->step++;
                cnWrap_SoundRequest(0);
            } else {
                cnWrap_SoundRequest(7);
            }
            break;
        }
        if (sw & 0x40) {
            return 3;
        }
        if (sw & 0x80) {
            pNet->step = 8;
            XA = 0;
            cw[0x2F80] = 0;
            cw[0x2F88] = 0;
            cw[0x2F99] = 0;
            cnWrap_SoundRequest(6);
        } else {
            w = pNet;
            t = w->x26;
            if (t > 1) {
                XA = Lb_cursorUD(*(u8 *)&w->x0A);
            }
        }
        break;
    case 2:
        pNet->x28 = Get_sw_on2(0);
        if (sw & 0x40) {
            XA = XE;
            pNet->step--;
            cnWrap_SoundRequest(3);
        } else if (sw & 0x200) {
            pNet->step++;
            cw[0x2F99] = 0;
            cnWrap_SoundRequest(6);
        }
        break;
    case 3:
        pNet->x28 = Get_sw_on2(0);
        if (sw & 0x40) {
            pNet->step = 1;
            XA = XE;
            cnWrap_SoundRequest(3);
        } else if (sw & 0x20) {
            if (XA == 0) {
                pNet->step++;
            } else {
                X6 = 3;
                *(BLK9A *)(cw + 0x3019) = *(BLK9A *)(cw + 0x2F7F);
                pNet->step = 5;
                SetDialogData(0x2A, 2);
                SetDialogYesNo(1);
            }
            cnWrap_SoundRequest(0);
        } else if ((sw & 0x3000) && *(s8 *)(cw + 0x2F99) != 0) {
            XA = XA ^ 1;
            cnWrap_SoundRequest(1);
        }
        break;
    case 4:
        if (mail_input(a, cw + 0x2F99, st) == 1) {
            KinshiYogo_chk(cw + 0x2F99);
            if (*(s8 *)(cw + 0x2F99) != 0) {
                XA = 1;
            } else {
                XA = 0;
            }
            pNet->step--;
        }
        break;
    case 5:
        a->x0C = 1;
        switch (Lb_select()) {
        case 0:
            SetDialogData(0x2D, 5);
            pNet->step++;
            break;
        case 3:
            pNet->step = X6;
            if (X6 == 1) {
                XA = X6;
            }
            break;
        }
        break;
    case 6:
        a->x0C = 1;
        switch (Lbc_SendMail(a)) {
        case 0:
            SetDialogData(0x26, 3);
            pNet->step++;
            break;
        case 1:
            SetDialogData_HTML(cw + 0x32D1);
            pNet->step++;
            break;
        }
        break;
    case 7:
        a->x0C = 1;
        if (sw & 0x20) {
            pNet->step = 1;
            XA = XE;
            cw[0x2F99] = 0;
            cnWrap_SoundRequest(0);
        }
        break;
    case 8:
        pNet->x28 = Get_sw_on2(0);
        if (sw & 0x40) {
            pNet->step = 1;
            XA = XE;
            cnWrap_SoundRequest(3);
        } else if (sw & 0x20) {
            X6 = XA;
            a = pNet;
            switch (XA) {
            case 0:
                a->step++;
                pNet->x04 = 1;
                seekStr[0] = 0;
                cw[0x2F80] = 0;
                cnWrap_SoundRequest(0);
                break;
            case 1:
                a->step = 0xB;
                cnWrap_SoundRequest(0);
                break;
            case 2:
                X6 = 8;
                *(BLK9A *)(cw + 0x3019) = *(BLK9A *)(cw + 0x2F7F);
                pNet->step = 5;
                SetDialogData(0x2A, 2);
                SetDialogYesNo(1);
                cnWrap_SoundRequest(0);
                break;
            }
        } else if (*(s8 *)(cw + 0x2F99) != 0) {
            XA = Lb_cursorUD(XA, 3);
        } else {
            XA = Lb_cursorUD(XA, 2);
        }
        break;
    case 9:
        if (plaza_req_input(a, seekStr, st) == 1) {
            strcpy((char *)cw + 0x2F80, seekStr);
            if (strlen(seekStr) < 6) {
                SetDialogData(0x1B, 3);
                pNet->step = 0xC;
                cw[0x2F80] = 0;
                cw[0x2F88] = 0;
            } else if (memcmp(my_user_id, cw + 0x2F80, 6) == 0) {
                SetDialogData(0x2E, 3);
                pNet->step = 0xC;
                cw[0x2F80] = 0;
                cw[0x2F88] = 0;
            } else {
                pNet->step++;
            }
        }
        break;
    case 10:
        r = getHandleFromID();
        switch (r) {
        case 0:
        case 1:
            pNet->step = 8;
            XA++;
            seekStr[0] = 0;
            break;
        }
        break;
    case 11:
        if (mail_input(a, cw + 0x2F99, st) == 1) {
            KinshiYogo_chk(cw + 0x2F99);
            if (*(s8 *)(cw + 0x2F99) != 0) {
                XA++;
            } else {
                XA = 1;
            }
            pNet->step = 8;
        }
        break;
    case 12:
        a->x0C = 1;
        if (sw & 0x20) {
            pNet->step = 8;
            XA = 0;
            cnWrap_SoundRequest(0);
        }
        break;
    }
    return 2;
}
