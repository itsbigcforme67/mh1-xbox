/* G online (PC): lobby_func_0057DDC0, written from Monster Hunter G's code at lobby 0x57DDC0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 SetDialogData();
s32 cnWrap_SoundRequest();
s32 lobby_func_0056AF20();
s32 lobby_func_0059F740();
s32 lobby_func_0059F750();
extern u8 BsLbsCount;
extern u8 cw;
extern u8 textLobbyTbl;
s32 Get_sw();                                /* extern */
s32 func_0012E1C0();            /* extern */
extern s8 ClassInfo;
extern u8 PlazaInfo;

s32 lobby_func_0057DDC0(void) {
    u8 *var_a2;   /* (s32 *) */
    s32 temp_a2;
    s32 var_a3;
    u8 temp_a0;
    u8 var_v0;
    u8 *temp_a1;
    u8 *temp_v0;
    u8 *temp_v1;

    temp_v1 = (*(void **)&cw);
    temp_a2 = Get_sw(0) & 0xFFFF;
    temp_a0 = (*(u8 *)((u8 *)(temp_v1) + 0xBA1));
    temp_a1 = temp_v1 + 0xBA1;
    if (temp_a2 & 0x20) {
        if (lobby_func_0056AF20(0, 0, ((s8) (((temp_a0 & 0xFF) + 0x31)))) == 0) {
            SetDialogData(0xD);
            cnWrap_SoundRequest(7);
            return 1;
        }
        var_a3 = 0;
        if ((s32) (*(u16 *)((u8 *)&ClassInfo + 0x2)) > 0) {
            var_a2 = &PlazaInfo;
loop_5:
            if ((((*(u8 *)((u8 *)((*(void **)&cw)) + 0xBA1)) + 0x31) == (*(s8 *)((u8 *)(var_a2) + 0x5B))) && ((*(u8 *)((u8 *)(var_a2) + 0x16)) == 3)) {
                ClassInfo = var_a3 + 1;
                goto block_14;
            }
            var_a3 += 1;
            var_a2 += 0x15C;
            if (var_a3 == (*(u16 *)((u8 *)&ClassInfo + 0x2))) {
                SetDialogData(0x12);
                cnWrap_SoundRequest(3);
                return 1;
            }
            if (var_a3 >= (s32) (*(u16 *)((u8 *)&ClassInfo + 0x2))) {
                goto block_14;
            }
            goto loop_5;
        }
block_14:
        cnWrap_SoundRequest(6);
        return 0;
    }
    if (temp_a2 & 0x40) {
        if ((s32) (*(u8 *)&BsLbsCount) >= 2) {
            cnWrap_SoundRequest(3);
            return 3;
        }
        goto block_39;
    }
    if (temp_a2 & 0x2000) {
        if ((temp_a0 & 0xFF) == 4) {
            (*(u8 *)((u8 *)(temp_v1) + 0xBA1)) = 0U;
        } else {
            var_v0 = 3;
            goto block_38;
        }
    } else if (temp_a2 & 0x1000) {
        if ((temp_a0 & 0xFF) == 3) {
            (*(u8 *)((u8 *)(temp_v1) + 0xBA1)) = 0U;
        } else {
            var_v0 = 4;
            goto block_38;
        }
    } else if (temp_a2 & 0x800) {
        if ((temp_a0 & 0xFF) == 2) {
            (*(u8 *)((u8 *)(temp_v1) + 0xBA1)) = 0U;
        } else {
            var_v0 = 1;
            goto block_38;
        }
    } else if (temp_a2 & 0x400) {
        if ((temp_a0 & 0xFF) == 1) {
            (*(u8 *)((u8 *)(temp_v1) + 0xBA1)) = 0U;
        } else {
            var_v0 = 2;
block_38:
            (*(u8 *)((u8 *)(temp_v1) + 0xBA1)) = var_v0;
        }
    }
block_39:
    temp_v0 = (*(void **)&cw);
    if ((temp_a0 & 0xFF) != (*(u8 *)((u8 *)(temp_v0) + 0xBA1))) {
        (*(s32 *)((u8 *)&(*(u8 *)((u8 *)&textLobbyTbl + 0x58)) + ((*(u8 *)((u8 *)(temp_v0) + 0xBA1)) * 0x14))) = func_0012E1C0(1, temp_a1, temp_a2);
        lobby_func_0059F740();
        lobby_func_0059F750(0xFA, 0x16A);
        cnWrap_SoundRequest(1);
    } else {
        (*(s32 *)((u8 *)&(*(u8 *)((u8 *)&textLobbyTbl + 0x58)) + ((*(u8 *)((u8 *)((*(void **)&cw)) + 0xBA1)) * 0x14))) = func_0012E1C0(0, temp_a1, temp_a2);
    }
    return 2;
}

