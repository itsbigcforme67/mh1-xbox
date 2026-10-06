/* lb_tk01 - NPC talk input, default handler 0x005D7F40-0x005D8160: Lb_talk_check_default(flags): flags bit 0 = ignore input, bit 1 = silent.
   lb_pit+0 (s32) counts frames (-1 = abort), +4 = script entry pointer (8-byte entries), +8 block, +9 yes/no cursor, +0xB talk step (0 = running).
   Layout lessons: the `return 0` after the confirm/cancel if-else sits after the outer else; the final return 0 is reached by fall-through. Whole file in lb_village_nm.c. */
#include "lobby_f.h"
extern u8 lb_pit[0xC];
int Get_sw2();

s32 Lb_talk_check_default(s32 a) {
    u16 pad;
    int f;
    int p;
    pad = Get_sw2(0);
    if (*(s32 *)lb_pit >= 0) {
        *(s32 *)lb_pit = *(s32 *)lb_pit + 1;
    }
    if (*(s8 *)(lb_pit + 0xB) == 0) {
        f = (s8)a;
        if (!(f & 1)) {
            p = pad & 0xFFFF;
            if (p & 0x20) {
                *(s32 *)lb_pit = 0;
                if (*(s8 *)(lb_pit + 9) != 1) {
                    if (!(f & 2)) {
                        cnWrap_SoundRequest(0);
                    }
                } else if (!(f & 2)) {
                    cnWrap_SoundRequest(3);
                }
                return 1;
            }
            if (p & 0x40) {
                if (*(u16 *)(*(u8 **)(lb_pit + 4) + *(s8 *)(lb_pit + 8) * 8) == 1) {
                    if (*(s8 *)(lb_pit + 9) != 1) {
                        *(s8 *)(lb_pit + 9) = 1;
                        cnWrap_SoundRequest(3);
                    } else {
                        *(s32 *)lb_pit = 0;
                        if (!(f & 2)) {
                            cnWrap_SoundRequest(3);
                        }
                        return 1;
                    }
                } else {
                    *(s32 *)lb_pit = 0;
                    if (!(f & 2)) {
                        cnWrap_SoundRequest(3);
                    }
                    return 1;
                }
                return 0;
            }
            if (*(u16 *)(*(u8 **)(lb_pit + 4) + *(s8 *)(lb_pit + 8) * 8) == 1) {
                if (p & 0x800) {
                    if (*(s8 *)(lb_pit + 9) != 0) {
                        cnWrap_SoundRequest(1);
                        *(s8 *)(lb_pit + 9) = 0;
                    }
                } else if (p & 0x400) {
                    if (*(s8 *)(lb_pit + 9) != 1) {
                        cnWrap_SoundRequest(1);
                        *(s8 *)(lb_pit + 9) = 1;
                    }
                }
            }
        }
    } else {
        if (!(((s8)a) & 1) && ((pad & 0xFFFF) & 0x60)) {
            *(s32 *)lb_pit = -1;
            return 0;
        }
    }
    return 0;
}
