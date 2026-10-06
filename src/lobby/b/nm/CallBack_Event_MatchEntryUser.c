/* CallBack_Event_MatchEntryUser (0x5C1690): logic complete, 39/66 differ (flag=0 init lands in the bne delay slot in ours; original keeps it before the cw load). Not built. */
#include "lobby_b.h"
typedef struct { u8 pad[8]; u8 x8; } CI8;
extern CI8 ClassInfo;
typedef struct { u8 pad0[0x32C5]; u8 x32C5; u16 x32C6; } CWS_me;
#define CWX ((CWS_me *)cw)
void CallBack_Event_MatchEntryUser(CNET_RES res) {
    s32 flag;
    CWS_me *c;
    s32 r;
    s32 t;
    u16 cls;

    cls = ClassInfo.x8;
    flag = 0;
    c = CWX;
    if (c->x32C6 + 1 == mhRule.x00 + 1) {
        flag = 1;
    }
    cnLBS_Get_MatchEntryJoinUser(cls, &c->x32C6, (u8 *)c + 0x32C8);
    if (lb_sys.x68 != 7 && lb_sys.x68 != 8) {
        r = mhRule.x00;
        if (r + 1 >= 2) {
            if (CWX->x32C6 == r) {
                if (CWX->x32C5 != 0) {
                    Lb_put_chat(0x10);
                    cnWrap_SoundRequest(0x12);
                }
            }
        }
    }
    if (flag == 1) {
        t = mhRule.x00 + 1;
        if (t >= 2) {
            if (CWX->x32C6 + 1 < t) {
                if (CWX->x32C5 != 0) {
                    Lb_put_chat(0x11);
                }
            }
        }
    }
}
