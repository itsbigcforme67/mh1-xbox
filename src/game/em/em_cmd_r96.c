/* em_cmd_r96 - near-match fixes: em_cmd_area_move_ck. Whole file in em_cmd_nm.c. 0x0055C440-0x0055C5B4: em_cmd_area_move_ck. Whole file in em_cmd_nm.c. */
#include "em_cmd.h"











































































































































u8 *em_cmd_area_move_ck(EMW *em, u8 *p) {
    u16 stg;
    u8 *q;
    u8 v;
    u8 *tbl;

    q = p;
    switch (*q) {
    case 0:
        tbl = em_area_mv_tbl[em->kind];
        stg = em->x73A;
        q += 1;
        if (em->stg == stg || stg == 0xFF) {
            goto clr;
        }
        v = tbl[stg];
        switch (v) {
        case 1:
            break;
        case 2:
            break;
        case 0:
        clr:
            em->x827 = 0;
            em->x828 = 0;
            em->x829 = 0;
            em->x881 = 0;
            em->x882 = 0;
            em->x883 = 0;
            CMD_SKIP(em, q, 3);
            break;
        }
        break;
    case 1:
        q = else_ck(em, q + 1, 3);
        break;
    case 2:
        q += 1;
        break;
    }
    return q;
}
