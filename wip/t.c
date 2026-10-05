#include "em.h"
#include "game.h"
void em21_to_normal(EMW *em);
typedef struct EML { s32 v; u8 _q[0x50-4]; } EML;
s32 em21_act_sub(EMW *em, s32 arg1) {
    if ((*(EML (*)[])&em->x194)[arg1].v == 0 || arg1 == 0xFF) {
        em21_to_normal(em);
        return 1;
    }
    return 0;
}
