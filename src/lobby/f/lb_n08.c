/* lb_n08 - lobby misc 0x005C9F80-0x005CA078: lb_key_quest_ck. Whole file in lb_n.c. */
#include "lobby_f.h"













extern u8 D_3E55F0[], D_3E5FF0[], D_3E69F0[], D_3E73F0[], D_3E7DF0[], D_3E87F0[], D_3E91F0[];




int lb_key_quest_ck(int n) {
    switch (n & 0xFF) {
    case 0x27:
    case 9:
    case 0x65:
    case 0x4F:
    case 0x61:
    case 0x6B:
    case 0x83:
    case 0x88:
    case 0x89:
    case 0x9A:
    case 0x8B:
    case 0xAB:
    case 0xAA:
    case 0xAF:
    case 0xAE:
    case 0xAD:
    case 0xAC:
    case 0x67:
    case 0x68:
    case 0x69:
    case 0x6A:
        return 1;
    }
    return 0;
}
