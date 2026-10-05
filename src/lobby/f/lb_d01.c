/* lb_d01 - lobby send functions 0x005D5BA0-0x005D5F34: check_sender1, Lb_send_pl_pos, Lb_send_pl_warp, Lb_send_pl_chidori_off, check_sender0, Lb_send_pl_status, Lb_send_stage, Lb_send_commer, Lb_send_chair_req, Lb_send_chair_release. Whole file in lb_d.c. */
#include "lobby_f.h"
















static s8 check_sender1() {
    return 0;
}

void Lb_send_pl_pos(PLW *pl) {
    LBPKPOS p;
    s8 t = lb_sys.x85 - 1;
    s8 m;
    lb_sys.x85 = t;
    if (t <= 0 && (m = lb_sys.x03) == 4) {
        lb_sys.x85 = lbSendInterval;
        p.x = pl->pos[0];
        p.z = pl->pos[2];
        p.ang = *(u16 *)&pl->ang_y;
        p.stg = pl->stg;
        if (check_sender1(&p) == 0) {
            lastSend.x = p.x;
            lastSend.z = p.z;
            lastSend.ang = p.ang;
            lastSend.stg = p.stg;
            lb_send_data(0xC, 0, &p);
        }
    }
}

void Lb_send_pl_warp(PLW *pl) {
    LBPKPOS p;
    p.x = pl->pos[0];
    p.z = pl->pos[2];
    p.ang = *(u16 *)&pl->ang_y;
    lb_send_data(0xA, 2, &p);
}

void Lb_send_pl_chidori_off(void) {
    lb_send_data(0, 6, 0);
}

static s8 check_sender0() {
    return 0;
}

void Lb_send_pl_status(PLW *pl) {
    LBSTAT p;
    u8 c;
    lb_sys.x85 = lbSendInterval;
    p.x = pl->pos[0];
    p.z = pl->pos[2];
    p.act14 = pl->flag14;
    p.act15 = pl->flag15;
    p.ang = *(u16 *)&pl->ang_y;
    p.x0D = PLU8(pl, 0x8EC);
    c = pl->flag15;
    switch (c) {
    case 0x4C:
    case 0x54:
    case 0x59:
    case 0x5A:
    case 0x5C:
    case 0x5D:
    case 0x5E:
    case 0x60:
    case 0x4E:
    case 0x2A:
    case 0x29:
    case 0x2B:
        p.chair = lb_sys.x66;
        break;
    case 0x4D:
        p.chair = **(u16 **)((u8 *)pl + 0x878);
        c = p.chair;
        lb_sys.chair_mask = lb_sys.chair_mask & ~(1 << c);
        break;
    }
    if (check_sender0(&p) == 0) {
        lastSend.x = p.x;
        lastSend.z = p.z;
        lastSend.x0F = p.act14;
        lastSend.x10 = p.act15;
        lastSend.ang = p.ang;
        lastSend.x11 = p.x0D;
        lastSend.stat = p.chair;
        lb_send_data(0x10, 1, &p);
    }
}

void Lb_send_stage(void) {
    lb_send_data(2, 3, D_3F3404);
}

void Lb_send_commer(void) {
    u8 buf[0x18];
    memcpy(buf, my_user_handle, 0x10);
    lb_send_data(0x10, 0x10, buf);
}

void Lb_send_chair_req(u8 *p) {
    lb_send_data(2, 4, *(s32 *)(p + 0x878));
}

void Lb_send_chair_release(void) {
    lb_send_data(1, 9, &lb_sys.x66);
}
