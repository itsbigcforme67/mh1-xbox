/* lb_rm01 - agent C 0x005C0EB0-0x005C112C: CallBack_Event_RecvMail (received lobby mail: shift 8-slot inbox down with 0x9A-byte struct copies; (char *)&m + n arguments avoid CSE into saved registers). */
#include "lobby_b.h"
typedef struct { u8 flag; char id[8]; char name[0x11]; char body[0x80]; } MAIL;
extern MAIL RecvMailInfo[8];
typedef struct { char id[8]; char name[0x14]; char body[0x80]; } RMSG;
void CallBack_Event_RecvMail(CNET_RES res) {
    RMSG m;
    s32 i;
    s32 n;
    MAIL *p;
    u8 *c;

    n = 0;
    i = 0;
    p = RecvMailInfo;
    do {
        if (p->flag != 0) {
            n++;
        }
        i++;
        p++;
    } while (i < 8);
    if (n == 0) {
        cnWrap_SoundRequest(5);
    }
    memset(&m, 0, 0x9C);
    cnLBS_Get_RecvMessage(&m, m.name, m.body);
    RecvMailInfo[7] = RecvMailInfo[6];
    RecvMailInfo[6] = RecvMailInfo[5];
    RecvMailInfo[5] = RecvMailInfo[4];
    RecvMailInfo[4] = RecvMailInfo[3];
    RecvMailInfo[3] = RecvMailInfo[2];
    RecvMailInfo[2] = RecvMailInfo[1];
    RecvMailInfo[1] = RecvMailInfo[0];
    strncpy(RecvMailInfo[0].id, m.id, 8);
    strncpy(RecvMailInfo[0].name, (char *)&m + 8, 0x10);
    strncpy(RecvMailInfo[0].body, (char *)&m + 0x1C, 0x80);
    KinshiYogo_chk(RecvMailInfo[0].body);
    RecvMailInfo[0].flag = 1;
    cw[0x2F7E]++;
    c = cw + 0x2F7E;
    if (cw[0x2F7E] > 8) {
        *c = 8;
    }
}
