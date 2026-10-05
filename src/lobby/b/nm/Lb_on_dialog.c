#include "lobby_a.h"
extern char Friend_data[];
void Lb_on_dialog(void) {
    s32 temp_v0;
    s32 temp_v0_2;
    u8 temp_a0;
    u8 temp_a0_2;
    int temp_a1;
    int temp_a2;
    int temp_v0_3;

    if ((F(u8, (u8 *)cw, 0x35D5) == 0) || (F(s32, &lb_sys, 0x68) == 0xF)) {
        temp_a1 = (int)pNet;
        temp_a0 = F(u8, temp_a1, 7);
        switch (temp_a0) {                          /* switch 1; irregular */
        case 5:                                     /* switch 1 */
            if (F(u8, temp_a1, 5) == 1) {
                temp_v0 = (int)SearchResult + ((F(u8, temp_a1, 0xA) + (F(s16, temp_a1, 0x24) * 7)) * 0x5C);
                DispNameAndIDonDialog(0x98, temp_v0 + 0xC, temp_v0 + 4);
                return;
            }
            break;
        case 6:                                     /* switch 1 */
            if ((F(u8, temp_a1, 3) == 8) && (F(u8, temp_a1, 5) == 1)) {
                temp_v0_2 = (int)SearchResult + ((F(u8, temp_a1, 0xA) + (F(s16, temp_a1, 0x24) * 7)) * 0x5C);
                DispNameAndIDonDialog(0x98, temp_v0_2 + 0xC, temp_v0_2 + 4);
                return;
            }
            break;
        case 3:                                     /* switch 1 */
            temp_a0_2 = F(u8, temp_a1, 3);
            switch (temp_a0_2) {                    /* switch 2; irregular */
            case 10:                                /* switch 2 */
                temp_a2 = (int)&Friend_data + ((F(u8, temp_a1, 0xA) + (F(s16, temp_a1, 0x24) * 7)) * 0x30);
                DispNameAndIDonDialog(0x98, temp_a2 + 8, temp_a2);
                return;
            case 13:                                /* switch 2 */
                temp_v0_3 = (int)cw;
                DispNameAndIDonDialog(0x98, temp_v0_3 + 0x2F88, temp_v0_3 + 0x2F80);
                break;
            }
            break;
        }
    }
}
