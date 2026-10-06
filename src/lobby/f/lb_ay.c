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
    return D_3396D5[id * 0x10] != 0xFF ? 1 : 0;
}
int u_equip_chk(int a) {
    u8 *u;
    int v;
    u = User_data;
    v = a & 0xFF;
    if (*(u8 *)(v * 6 + u + 0x44) == 0) {
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
    u8 *u = User_data;
    u8 *new_var2;
    unsigned long long new_var;
    a0 = ib[8] * 4;
    new_var2 = D_3396D3;
    new_var = new_var2[(*((u16 *)(((u8 *)D_3C7184) + a0))) * 0x10];
    if (new_var == 0xFF) {
        return 0;
    }
    p = (u8 *)(a0 + (int)u);
    if (new_var2[(*((u16 *)(p + 0x1C4))) * 0x10] == 1) {
        return 0;
    }
    return *(s16 *)(p + 0x1C6) != 1 ? 1 : 0;
}
int pick_kosuu_sel_chk() {
    u8 *v;
    int i;
    u8 *u = User_data;
    if (item_kosuu_sel_chk() == 0) {
        return 0;
    }
    i = ib[0xB] * 4;
    v = (u8 *)(i + (int)u);
    return (D_3396D3[*(u16 *)(v + 0x37C) * 0x10] - *(s16 *)(v + 0x37E)) >= 2;
}
u8 *sortup_idx_chk(int a) {
    u8 *u;
    int v;
    v = a & 0xFF;
    u = User_data;
    if (v == User_data[0x457]) {
        return u + 0x457;
    }
    if (v == u[0x458]) {
        return u + 0x458;
    }
    if (v == u[0x459]) {
        return u + 0x459;
    }
    if (v == u[0x45A]) {
        return u + 0x45A;
    }
    if (v == u[0x45B]) {
        return u + 0x45B;
    }
    if (v == u[0x456]) {
        return u + 0x456;
    }
    return 0;
}
/* permuter form: r holds the constant 6 shared with the compares */
int equip_ok_chk(u8 *e) {
  int r;
  int new_var[2];
  u8 *u;
  new_var[1] = e[1];
  r = 6;
  if ((e[1] == r) || (new_var[1] == 7))
  {
    return 1;
  }
  u = User_data;
  new_var[0] = Get_equip_data_ptr()[2];
  if (!(new_var[0] & ((((*((u8 *) 0x3C6FC1)) == 0) ? (1) : (2)) & 0xFF)))
  {
    return 0;
  }
  r = (u[(u[0x456] * r) + 0x45] == 6) ? (4) : (8);
  if (!(new_var[0] & (r & 0xFF)))
  {
    return 0;
  }
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
    int a3;
    int a0;
    int a1;
    s16 *q;
    int k;
    u8 *t0;
    int t;
    u8 *u;
    u8 mx;
    t0 = ib;
    u = User_data;
    mx = D_3C7186[t0[8] * 4];
    if (!(c & 0xFF)) {
        k = t0[0xB] * 4;
        a1 = D_3396D3[*(u16 *)(k + (int)u + 0x37C) * 0x10] - *(s16 *)(k + (int)u + 0x37E);
        if (a1 < (mx & 0xFF)) {
            mx = a1;
        }
    }
    t = pad & 0xFFFF;
    t0[0x1C] = 0;
    q = (s16 *)(ib + 0x1A);
    a3 = *q;
    if (t & 0x800) {
        *q = 1;
    } else if (t & 0x400) {
        *q = mx & 0xFF;
    } else {
        a0 = mx & 0xFF;
        if (t & 0x2000) {
            if (a3 >= a0) {
                *q = a0;
                a3 = -1;
            } else {
                *q = a3 + 1;
            }
        } else if (t & 0x1000) {
            if (a3 > 1) {
                *q = a3 - 1;
            } else {
                a3 = -1;
            }
        }
    }
    a1 = (s16)a3;
    if (a1 < 0) {
        se_req(7, 0x15, 0);
    } else if (a1 != *(s16 *)(ib + 0x1A)) {
        se_req(7, 0x16, 0);
    }
    t0 = ib;
    if (*(s16 *)(t0 + 0x1A) >= (mx & 0xFF)) {
        t0[0x1C] = 1;
    }
}
