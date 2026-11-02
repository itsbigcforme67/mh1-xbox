/* em_cmd_r98 - monster command interpreter 0x00565DC0-0x00565E98: em_cdm_act_flag_ck. Whole file in em_cmd_nm.c. */
#include "em_cmd.h"

/* G: original bytes (build/mhg/raw/em_cdm_act_flag_ck.inc, config/mhg/c_rawfuncs.txt); the C below is the MH1 version */
#if defined(VERSION_MHG) && defined(__MWERKS__)
asm void em_cdm_act_flag_ck(EMW *em)
{
#include "em_cdm_act_flag_ck.inc"
}
#else
void em_cdm_act_flag_ck(EMW *em) {
    s32 i;
    s32 cnt;
    u8 n;
    u8 m;

    switch (em->x82B) {
    case 0:
        EM_FIELD(em, s8 *, 0x880) = 0;
        return;
    case 1:
        n = *(u8 *)MH2V(0x3F34C3, 0x6ADCEE);
        i = 0;
        cnt = 0;
        for (; i < n; i++) {
            if (em->x914 & (1 << i)) {
                cnt += 1;
            }
        }
        if (cnt == 0) {
            EM_FIELD(em, s8 *, 0x880) = 0;
            return;
        }
        EM_FIELD(em, s8 *, 0x880) = 1;
        em->x881 = 1;
        em->x882 = 0;
        m = *(u8 *)MH2V(0x3F34C3, 0x6ADCEE);
        i = 0;
        if (0 < m) {
            while (!(em->x914 & (1 << i)) && ++i < m) {
            }
        }
        em->x883 = i;
        break;
    }
}
#endif
