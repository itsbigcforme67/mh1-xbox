/* G online (PC): CallBack_Result_Plaza_ReadAllocation2, written from Monster Hunter G's code at lobby 0x57E540 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 ClassInfo;
extern u8 cw;
s32 CallBackWaitInit();                      /* extern */
s32 Check_CallBackWait();                    /* extern */
s32 To_LogOut();                         /* extern */
s32 cnLBS_Get_PlazaCount();                /* extern */
s32 cnLBS_Get_PlazaName();           /* extern */
s32 cnLBS_Get_PlazaStatus();         /* extern */
s32 cnLBS_Get_mhPlazaJoinUser(); /* extern */
s32 lobby_func_005AB8B0(); /* extern */
s32 lobby_func_005AC230();           /* extern */
s32 lobby_func_005AC580();        /* extern */
extern u8 CallBack_Result_Plaza_PlazaExit2;
extern u8 LobbyInfo;

s32 CallBack_Result_Plaza_ReadAllocation2(void) {
    u8 *var_s0;   /* (s32 *) */
    s16 temp_v0;
    s32 temp_a0;
    u32 var_s1;
    u8 temp_v1_2;
    u8 *temp_v1;

    temp_v1 = (*(void **)&cw);
    temp_a0 = temp_v1 + 0xB9D;
    temp_v1_2 = (*(u8 *)((u8 *)(temp_v1) + 0xB9D));
    switch (temp_v1_2) {                            /* irregular */
    case 0:
        (*(u8 *)((u8 *)(temp_v1) + 0xB9D)) = (u8) (temp_v1_2 + 1);
        CallBackWaitInit(temp_a0);
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xBA9)) = 0x18;
        lobby_func_005AB8B0(0, 0x87, &CallBack_Result_Plaza_PlazaExit2);
block_12:
        return 2;
    case 1:
        Check_CallBackWait(temp_a0);
        goto block_12;
    case 2:
        cnLBS_Get_PlazaCount(&(*(u16 *)((u8 *)&ClassInfo + 0x6)));
        var_s1 = 0;
        if ((*(u16 *)((u8 *)&ClassInfo + 0x6)) != 0) {
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
            } while (var_s1 < (u16) (*(u16 *)((u8 *)&ClassInfo + 0x6)));
        }
        (*(u8 *)((u8 *)((*(void **)&cw)) + 0xB9D)) = 0U;
        return 0;
    default:
        To_LogOut(4);
        goto block_12;
    }
}

