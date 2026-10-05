/* em_cmd_r25 - monster command interpreter 0x00565F20-0x00566224: set_cmd, ret_cmd, route_ptr_set, kehai_ptr_set, find_ptr_set, smell_ptr_set, yobi_ptr_set, ikari_ptr_set, action_ptr_set, area_route_ptr_set, area_move_ptr_set, option_route_ptr_set, ground_area_move_ptr_set, no_floor_ptr_set, unko_ptr_set. Whole file in em_cmd_nm.c. */
#include "em_cmd.h"











































































































































u8 *set_cmd(EMW *em) {
    u8 f = em->x83B;
    if (f & 0x20) {
        return em->cmd_find;
    }
    if (f & 0x10) {
        return em->cmd_kehai;
    }
    if (f & 1) {
        return em->cmd_route;
    }
    return em->cmd_pc;
}

void ret_cmd(EMW *em, u8 *p) {
    u8 f = em->x83B;
    if (f & 0x20) {
        em->cmd_find = p;
        return;
    }
    if (f & 0x10) {
        em->cmd_kehai = p;
        return;
    }
    if (f & 1) {
        em->cmd_route = p;
        return;
    }
    em->cmd_pc = p;
}

u8 *route_ptr_set(EMW *em, u8 n) {
    u8 **t = em->cmd_tbl[2];
    if (t == 0) {
        return 0;
    }
    return t[n];
}

u8 *kehai_ptr_set(EMW *em, u8 n) {
    u8 **t = em->cmd_tbl[3];
    if (t == 0) {
        return 0;
    }
    return t[n];
}

u8 *find_ptr_set(EMW *em, u8 n) {
    u8 **t = em->cmd_tbl[4];
    if (t == 0) {
        return 0;
    }
    return t[n];
}

u8 *smell_ptr_set(EMW *em, u8 n) {
    u8 **t = em->cmd_tbl[8];
    if (t == 0) {
        return 0;
    }
    return t[n];
}

u8 *yobi_ptr_set(EMW *em, u8 n) {
    u8 **t = em->cmd_tbl[10];
    if (t == 0) {
        return 0;
    }
    return t[n];
}

u8 *ikari_ptr_set(EMW *em, u8 n) {
    u8 **t = em->cmd_tbl[11];
    if (t == 0) {
        return 0;
    }
    return t[n];
}

u8 *action_ptr_set(EMW *em, u8 n) {
    u8 **t = em->cmd_tbl[9];
    if (t == 0) {
        return 0;
    }
    return t[n];
}

u8 *area_route_ptr_set(EMW *em, u8 n) {
    u8 **t = em->cmd_tbl[5];
    if (t == 0) {
        return 0;
    }
    return t[n];
}

u8 *area_move_ptr_set(EMW *em, u8 n) {
    return em->cmd_tbl[6][n];
}

u8 *option_route_ptr_set(EMW *em, u8 n) {
    u8 **t = em->cmd_tbl[2];
    if (t == 0) {
        return 0;
    }
    return t[n];
}

u8 *ground_area_move_ptr_set(EMW *em, u8 n) {
    u8 **t = em->cmd_tbl[12];
    if (t == 0) {
        return 0;
    }
    return t[n];
}

u8 *no_floor_ptr_set(EMW *em) {
    u8 **t = em->cmd_tbl[13];
    if (t == 0) {
        return 0;
    }
    return *t;
}

u8 *unko_ptr_set(EMW *em) {
    u8 **t = em->cmd_tbl[14];
    if (t == 0) {
        return 0;
    }
    return *t;
}
