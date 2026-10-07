/* return_mc_save (0x00271EA0-0x00271FB0): step machine that resets the game, runs a memory card operation (8) and fades out;
 * returns 1 when done (COM_R_No_1 == 3). */
#include "types.h"

extern u8 COM_R_No_1;
extern u8 Disp_back[];
extern u8 D_6B2A20[];

int all_reset();
int Net_all_reset();
int func_5B7460();
int fade_set();
int func_5B2620();
int McOperationSet();
int McCardOperation();
int Fade_busy_ck();
int func_5B78F0();

int return_mc_save() {
    int r = 0;

    switch (COM_R_No_1) {
    case 0:
        all_reset();
        Net_all_reset(0);
        func_5B7460(0, 0, Disp_back);
        fade_set(2);
        func_5B2620();
        McOperationSet(8);
        COM_R_No_1 += 1;
        break;
    case 1:
        if ((u8)McCardOperation() != 0) {
            COM_R_No_1 += 1;
            fade_set(1);
        }
        break;
    case 2:
        if ((u8)Fade_busy_ck() != 1) {
            COM_R_No_1 += 1;
        }
        break;
    case 3:
        r = 1;
        break;
    }
    func_5B78F0(D_6B2A20);
    return r;
}
