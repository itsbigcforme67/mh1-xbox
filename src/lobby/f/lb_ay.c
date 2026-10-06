/* Lobby item box (storage) small helpers: slot checks, count select, yes/no, hand-written from m2c drafts. */
#include "lobby_f.h"
typedef struct ITEMSLOT { u16 id; s16 num; } ITEMSLOT;
extern u8 *ib;
extern u8 User_data[];
extern ITEMSLOT D_3C7184[];
extern u8 D_3C7186[];
extern u8 D_3396D3[];
extern u8 D_3396D5[];
void se_req();
u8 *Get_equip_data_ptr();

int u_item_chk(int a) {
    u16 id;
    id = D_3C7184[a & 0xFF].id;
    if (id == 0) {
        return 0;
    }
    return D_3396D5[id * 0x10] != 0xFF;
}
int u_equip_chk(int a) {
    int v;
    u8 *u;
    v = a & 0xFF;
    u = User_data;
    if (u[v * 6 + 0x44] == 0) {
        return -1;
    }
    if (v == u[0x457] || v == u[0x458] || v == u[0x459] || v == u[0x45A] || v == u[0x45B] || v == u[0x456]) {
        return 0;
    }
    return 1;
}
int item_kosuu_sel_chk() {
    int a0;
    u8 *p;
    a0 = ib[8] * 4;
    if (D_3396D3[*(u16 *)((u8 *)D_3C7184 + a0) * 0x10] == 0xFF) {
        return 0;
    }
    p = a0 + User_data;
    if (D_3396D3[*(u16 *)(p + 0x1C4) * 0x10] == 1) {
        return 0;
    }
    return *(s16 *)(p + 0x1C6) != 1;
}
int pick_kosuu_sel_chk(int a0, int a1, int a2, u8 *a3) {
    u8 *v;
    if (item_kosuu_sel_chk(User_data) == 0) {
        return 0;
    }
    v = a3 + ib[0xB] * 4;
    return (D_3396D3[*(u16 *)(v + 0x37C) * 0x10] - *(s16 *)(v + 0x37E)) >= 2;
}
u8 *sortup_idx_chk(int a) {
    int v;
    u8 *r;
    v = a & 0xFF;
    if (v == User_data[0x457]) {
        return User_data + 0x457;
    }
    if (v == User_data[0x458]) {
        return User_data + 0x458;
    }
    if (v == User_data[0x459]) {
        return User_data + 0x459;
    }
    if (v == User_data[0x45A]) {
        return User_data + 0x45A;
    }
    if (v == User_data[0x45B]) {
        return User_data + 0x45B;
    }
    r = 0;
    if (v == User_data[0x456]) {
        r = User_data + 0x456;
    }
    return r;
}
int equip_ok_chk(u8 *e) {
    int r;
    u8 a1;
    u8 t;
    t = e[1];
    if (t != 6) {
        if (t == 7) {
            goto b3;
        }
        a1 = Get_equip_data_ptr()[2];
        if (!(a1 & (((*(u8 *)0x3C6FC1 != 0) ? 2 : 1) & 0xFF))) {
            return 0;
        }
        r = 8;
        if (User_data[User_data[0x456] * 6 + 0x45] == 6) {
            r = 4;
        }
        if (!(a1 & (r & 0xFF))) {
            return 0;
        }
        return 1;
    }
b3:
    return 1;
}
void yes_no_select(u16 pad) {
    u8 *t;
    u8 *q;
    t = ib;
    q = t + 0x21;
    if (*q == 0) {
        if (pad & 0x400) {
            *q = 1;
            se_req(7, 0x16, 0);
        }
    } else if (pad & 0x800) {
        *q = 0;
        se_req(7, 0x16, 0);
    }
}
void kosuu_select(int pad, int c) {
    s16 a3;
    s16 a0;
    int a1;
    int t;
    u8 *t0;
    u8 *p;
    u8 mx;
    t0 = ib;
    mx = D_3C7186[t0[8] * 4];
    if (!(c & 0xFF)) {
        p = User_data + t0[0xB] * 4;
        a1 = D_3396D3[*(u16 *)(p + 0x37C) * 0x10] - *(s16 *)(p + 0x37E);
        if (a1 < (mx & 0xFF)) {
            mx = a1 & 0xFF;
        }
    }
    t = pad & 0xFFFF;
    t0[0x1C] = 0;
    p = ib;
    a3 = *(s16 *)(p + 0x1A);
    if (t & 0x800) {
        *(s16 *)(p + 0x1A) = 1;
    } else if (t & 0x400) {
        *(s16 *)(p + 0x1A) = mx & 0xFF;
    } else {
        a0 = mx & 0xFF;
        if (t & 0x2000) {
            if (a3 >= a0) {
                *(s16 *)(p + 0x1A) = a0;
                a3 = -1;
            } else {
                *(s16 *)(p + 0x1A) = a3 + 1;
            }
        } else if (t & 0x1000) {
            if (a3 >= 2) {
                *(s16 *)(p + 0x1A) = a3 - 1;
            } else {
                a3 = -1;
            }
        }
    }
    a1 = a3;
    if (a1 < 0) {
        se_req(7, 0x15, 0, a3);
    } else if (a1 != *(s16 *)(ib + 0x1A)) {
        se_req(7, 0x16, 0, a3);
    }
    p = ib;
    if (*(s16 *)(p + 0x1A) >= (mx & 0xFF)) {
        p[0x1C] = 1;
    }
}
