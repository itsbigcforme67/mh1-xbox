/* cpinet16 - CpInetTcpGetStatus (SLPM_654.95 0x00235720-0x00235850): asks the IOP for a socket's state (Ave_TcpStat) and maps the IOP state
   code (-1, 0..10) to the game's 0..11 (the mapping is the identity except -1 -> 11 and 7 -> 7 via default). Written new in this
   pass (no near-match copy). Jump table in main:rodata. */
#include "types.h"
int Ave_TcpStat();
int common_error(int);
int CpInetTcpGetStatus(int sock, s16 *st) {
    int r;

    r = common_error((s16)Ave_TcpStat((s16)sock, st, st + 2, st + 3));
    if (r >= 0) {
    } else {
        return r;
    }
    switch (*st) {
    case 0:
        *st = 0;
        break;
    case 1:
        *st = 1;
        break;
    case 2:
        *st = 2;
        break;
    case 3:
        *st = 3;
        break;
    case 4:
        *st = 4;
        break;
    case 5:
        *st = 5;
        break;
    case 6:
        *st = 6;
        break;
    default:
        *st = 7;
        break;
    case 8:
        *st = 8;
        break;
    case 9:
        *st = 9;
        break;
    case 10:
        *st = 10;
        break;
    case -1:
        *st = 11;
        break;
    }
    return r;
}
