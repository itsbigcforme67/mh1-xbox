/* G online (PC): lobby_func_0057ECE0, written from Monster Hunter G's code at lobby 0x57ECE0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 To_LogOut();
s32 lobby_func_005E3420();
s32 lobby_func_005E36A0();
s32 lobby_func_005E36D0();
s32 lobby_func_005E3900();
extern u8 ClassInfo;
extern u8 cw;
s32 cnLBS_Get_LobbyName();           /* extern */
s32 gonl_Get_PlazaStatus();          /* extern */
s32 gonl_Get_PlazaJoinUser(); /* extern */
extern u8 PlazaInfo;

void lobby_func_0057ECE0(s64 arg0) {
    u8 g_frame[0x40] __attribute__((aligned(16)));   /* G's stack frame */
#define sp38 (*(s64 *)(g_frame + 0x38))
    u8 *var_s0;   /* (s32 *) */
    s16 temp_v0;
    s32 var_s1;
    u8 *temp_a0;

    sp38 = arg0;
    if (((*(u8 *)((u8 *)((*(void **)&cw)) + 0xB99)) != 5) && ((s8) sp38 != 2)) {
        if ((s8) sp38 == 0) {
            lobby_func_005E3420(&(*(u16 *)((u8 *)&ClassInfo + 0x2)));
            var_s1 = 0;
            if ((s32) (*(u16 *)((u8 *)&ClassInfo + 0x2)) > 0) {
                var_s0 = &PlazaInfo;
                do {
                    temp_v0 = var_s1 + 1;
                    (*(s16 *)((u8 *)(var_s0) + 4)) = temp_v0;
                    gonl_Get_PlazaStatus(temp_v0 & 0xFFFF, var_s0 + 0x16);
                    gonl_Get_PlazaJoinUser((var_s1 + 1) & 0xFFFF, var_s0 + 6, var_s0 + 0x12);
                    cnLBS_Get_LobbyName((var_s1 + 1) & 0xFFFF, var_s0 + 0x1A);
                    lobby_func_005E3900((var_s1 + 1) & 0xFFFF, (s32) (var_s0 + 0x5B));
                    lobby_func_005E36A0((var_s1 + 1) & 0xFFFF, var_s0 + 8);
                    lobby_func_005E36D0((var_s1 + 1) & 0xFFFF, var_s0 + 0x14);
                    var_s1 += 1;
                    var_s0 += 0x15C;
                } while (var_s1 < (s32) (*(u16 *)((u8 *)&ClassInfo + 0x2)));
            }
            temp_a0 = (*(void **)&cw);
            (*(u8 *)((u8 *)(temp_a0) + 0xB9D)) = (u8) ((*(u8 *)((u8 *)(temp_a0) + 0xB9D)) + 1);
            return;
        }
        To_LogOut(4);
    }
}

