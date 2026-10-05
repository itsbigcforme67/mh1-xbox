/* cnlbs, run 19: cnLBS_Read_RoomExplain .. cnLBS_Get_RoomExplain (lobby.bin 0x005A4F10-0x005A4FD0): the matching functions of cnlbs_nm.c. */
#include "lbnet_proto.h"

int cnLBS_Read_RoomExplain(int arg0, int arg1) {
    int slot = __cnetSub_Set_BgProcess(1, 0, arg1);

    if (slot != -1) {
        CnetSys_w.bg[slot].cmd = __cnet_SendReq_PieceExplain(2, arg0);
        return slot;
    }
    return -1;
}

int cnLBS_Get_RoomExplain(idx, d)
int idx;
char *d;
{
    strcpy(d, (u8 *)&CnetSys_w + (((u16)idx - 1) * 0x164) + 0x6224);
    return 0;
}
