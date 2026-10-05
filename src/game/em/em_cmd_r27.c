/* em_cmd_r27 - monster command 0x00566230-0x0056648C: cancel_prog_ck. Whole file in em_cmd_nm.c. */
#include "em_cmd.h"











































































































































u8 *cancel_prog_ck(EMW *em) {
    u8 *r;
    u8 f;

    f = em->x917;
    if (f == 0) {
        return 0;
    }
    r = 0;
    if (((u8)(f & 0xFF) & 0x40) && em_cancel_act_ck(em, 0x40) == 0) {
        r = unko_ptr_set(em);
        em->x917 = 0;
        em->x83B = 0x40;
    }
    if ((em->x917 & 0x80) && em_cancel_act_ck(em, 0x80) == 0 && r == 0) {
        r = no_floor_ptr_set(em);
        em->x917 = 0;
        em->x83B = 0x80;
    }
    if ((em->x917 & 0x20) && em_cancel_act_ck(em, 0x20) == 0) {
        em->cmd_find = find_ptr_set(em, 0);
        if (r == 0) {
            r = em->cmd_find;
        }
        em->x917 &= 0xCE;
        em->x83B |= 0x20;
    }
    if ((em->x917 & 0x10) && em_cancel_act_ck(em, 0x10) == 0) {
        em->cmd_kehai = kehai_ptr_set(em, 0);
        if (r == 0) {
            r = em->cmd_kehai;
        }
        em->x917 &= 0xEF;
        em->x83B |= 0x10;
    }
    if ((em->x917 & 8) && em_cancel_act_ck(em, 8) == 0) {
        em->cmd_ikari = ikari_ptr_set(em, 0);
        if (r == 0) {
            r = em->cmd_ikari;
        }
        em->x917 &= 0xF7;
        em->x83B |= 8;
    }
    if ((em->x917 & 4) && em_cancel_act_ck(em, 4) == 0) {
        em->cmd_yobi = yobi_ptr_set(em, 0);
        if (r == 0) {
            r = em->cmd_yobi;
        }
        em->x917 &= 0xC8;
        em->x83B |= 4;
    }
    if ((em->x917 & 2) && em_cancel_act_ck(em, 2) == 0) {
        em->cmd_smell = smell_ptr_set(em, 0);
        if (r == 0) {
            r = em->cmd_smell;
        }
        em->x917 &= 0xCC;
        em->x83B |= 2;
    }
    if (r != 0) {
        em->cmd_top = r;
    }
    return r;
}
