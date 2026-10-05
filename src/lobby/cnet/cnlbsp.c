/* cnlbs, run 16: __cnetSub_Run_BgProcess .. __cnet_RecvFromLbs (lobby.bin 0x005AD440-0x005AD61C): the matching functions of cnlbs_nm.c. */
#include "lbnet_proto.h"
#pragma readonly_strings on

void __cnetSub_Run_BgProcess(void) {
    int i;

    for (i = 0; i < 0x80; i++) {
        if (CnetSys_w.bg[i].state == 2) {
            if (CnetSys_w.bg[i].cb != 0) CnetSys_w.bg[i].cb(i);
        }
    }
    for (i = 0; i < 12; i++) {
        if (CnetSys_w.burst[i].state == 1) {
            if (CnetSys_w.burst[i].run != 0) CnetSys_w.burst[i].run(i);
        }
    }
}

int __cnetSub_Get_RestBgWork(void) {
    int n = 0;
    int i;

    for (i = 0; i < 0x80; i++) {
        if (CnetSys_w.bg[i].state == 0) n++;
    }
    return n;
}

int __cnet_RecvFromLbs(int cmd, int from, int cat, int x) {
    int i;
    int c16;
    int c8;
    u8 *h;
    u8 *l;
    u8 *ft;
    u8 *ct;
    void (**jmp)();
    int hi;
    int full;

    c16 = cmd & 0xFFFF;
    c8 = cat & 0xFF;
    i = 0;
    h = lbs_command_tbl_h;
    l = lbs_command_tbl_l;
    ft = lbs_fromto_tbl;
    ct = lbs_category_tbl;
    jmp = lbs_command_jmp;
    for (; i < 0x102; i++, h++, l++, ft++, ct++, jmp++) {
        hi = (*h << 8) & 0xFFFF;
        full = (hi | *l) & 0xFFFF;
        if (*ft != 8 && c16 == (full & 0xFFFF) && *ct == c8 && *jmp != 0) {
            lbs_command_jmp[i](full, hi, c8, c16);
            return 1;
        }
    }
    return 0;
}
