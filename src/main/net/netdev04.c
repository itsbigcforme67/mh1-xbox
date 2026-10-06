/* netdev04 - _reset_recognize (SLPM_654.95 0x00238070-0x00238104): advances Ppp_connection_param through 1 -> 3 -> 2 -> 0 -> 1 and returns -1
   once it is back at PppRecognize (a try-the-next-setting rotation; guess from use). Written new in this pass. */
#include "types.h"
extern u8 PppRecognize;
extern s16 Ppp_connection_param[8];
int _reset_recognize(void) {
    switch (Ppp_connection_param[0]) {
    case 1:
        Ppp_connection_param[0] = 3;
        break;
    case 3:
        Ppp_connection_param[0] = 2;
        break;
    case 2:
        Ppp_connection_param[0] = 0;
        break;
    case 0:
        Ppp_connection_param[0] = 1;
        break;
    }
    return -(PppRecognize == Ppp_connection_param[0]);
}
