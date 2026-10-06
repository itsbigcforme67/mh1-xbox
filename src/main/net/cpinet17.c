/* cpinet17 - CpInetPppGetDns (SLPM_654.95 0x00235C50-0x00235CC8): copies the PPP status words (Ave_PppStatus, 11 words) into six
   result ints: [0]=st[1], [1]=st[0], [2..5] = the DNS part of the status (names are guesses from use). */
#include "types.h"
typedef struct PPPSTI { s32 v[11]; } PPPSTI;
int Ave_PppStatus(PPPSTI *);
int CpInetPppGetDns(int *out) {
    PPPSTI st;
    s32 *q;
    int r;

    r = (s16)Ave_PppStatus(&st);
    if (r >= 0) {
    } else {
        return r;
    }
    out[0] = st.v[1];
    out[1] = st.v[0];
    q = &st.v[5];
    out[2] = q[4];
    out[3] = q[5];
    out[4] = q[2];
    out[5] = q[3];
    return r;
}
