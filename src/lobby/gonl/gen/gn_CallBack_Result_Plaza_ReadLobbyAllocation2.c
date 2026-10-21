/* G online (PC): CallBack_Result_Plaza_ReadLobbyAllocation2, written from Monster Hunter G's code at lobby 0x57D010 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 To_LogOut();
s32 lobby_func_005AC230();
s32 lobby_func_005AC580();
extern u8 ClassInfo;
extern u8 cw;
s32 cnLBS_Get_PlazaCount();            /* extern */
s32 cnLBS_Get_PlazaName();           /* extern */
s32 cnLBS_Get_PlazaStatus();         /* extern */
s32 cnLBS_Get_mhPlazaJoinUser(); /* extern */
extern u8 LobbyInfo;

void CallBack_Result_Plaza_ReadLobbyAllocation2(s64 arg0) {
    u8 g_frame[0x40] __attribute__((aligned(16)));   /* G's stack frame */
#define sp38 (*(s64 *)(g_frame + 0x38))
    u8 *var_s0;   /* (s32 *) */
    s16 temp_v0;
    s32 var_s1;
    u8 *temp_a0;

    sp38 = arg0;
    if ((*(u8 *)((u8 *)((*(void **)&cw)) + 0xB99)) != 5) {
        if ((s8) sp38 == 2) {
            if ((((s8) ((s32) (((s8 *)&sp38)[1])))) != 0xB) {

            }
        } else {
            if ((s8) sp38 == 0) {
                cnLBS_Get_PlazaCount(&(*(u16 *)((u8 *)&ClassInfo + 0x6)), (((s8 *)&sp38)[1]));
                var_s1 = 0;
                if ((s32) (*(u16 *)((u8 *)&ClassInfo + 0x6)) > 0) {
                    var_s0 = &LobbyInfo;
                    do {
                        temp_v0 = var_s1 + 1;
                        (*(s16 *)((u8 *)(var_s0) + 4)) = temp_v0;
                        cnLBS_Get_PlazaStatus(temp_v0 & 0xFFFF, var_s0 + 0x16);
                        lobby_func_005AC580((var_s1 + 1) & 0xFFFF, var_s0);
                        cnLBS_Get_mhPlazaJoinUser((var_s1 + 1) & 0xFFFF, var_s0 + 6, var_s0 + 0x12);
                        cnLBS_Get_PlazaName((var_s1 + 1) & 0xFFFF, var_s0 + 0x1A);
                        lobby_func_005AC230((var_s1 + 1) & 0xFFFF, var_s0 + 8);
                        var_s1 += 1;
                        var_s0 += 0x15C;
                    } while (var_s1 < (s32) (*(u16 *)((u8 *)&ClassInfo + 0x6)));
                }
                temp_a0 = (*(void **)&cw);
                (*(u8 *)((u8 *)(temp_a0) + 0xB9D)) = (u8) ((*(u8 *)((u8 *)(temp_a0) + 0xB9D)) + 1);
                return;
            }
            To_LogOut(4);
        }
    }
}

