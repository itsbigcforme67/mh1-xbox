/* Lobby: room member in/out/change checks, newcomer handling, player load/set, hand-written from m2c drafts. */
#include "lobby_f.h"
extern PLW player_work[];
extern char lit_236_00664CC0[];
extern u8 D_3E4C05[];
extern u8 pl01_adr_tbl[];
void armor_model_free();
void release_prim();
void Chat_log_add(int, void *);
int lb_check_mini_data();
void Lb_set_mini_data_to_pl();
void Lb_set_player();
void flCompact();
void Lb_send_myChair();
void Lb_player_load();
void com_motion_load();
void Lbc_connect();
void npc_create_model();
void Lb_trans_pl();
char *strcpy();
int lb_member_outCheck(void) {
    char buf[0x120];
    int sp[0x40 / 4];
    int s5;
    s8 i;
    u8 *c;
    u8 *pl;
    u8 *lp;
    int off;
    s5 = 0;
    i = 0;
    c = (u8 *)lbCommer;
    off = 0;
    pl = (u8 *)player_work;
    lp = (u8 *)lb_player;
    do {
        if (*(s8 *)c == 0 && *(s8 *)(cw + off + 0x132C) != 0) {
            pl[1] = 0;
            s5 = 1;
            pl[0] = 0;
            armor_model_free(pl);
            if (*(s32 *)(pl + 0x564) != 0) {
                release_prim(*(s16 *)(pl + 0x568));
                *(s32 *)(pl + 0x564) = 0;
            }
            if (cw[0x35D5] != 0 && (cw + off + 0x1334) != 0) {
                memset(buf, 0, 0x120);
                buf[0x11F] = 6;
                buf[0x11E] = 6;
                buf[0x11D] = 6;
                sprintf(buf + 0x1C, lit_236_00664CC0, cw + off + 0x1334);
                Chat_log_add(0, buf);
            }
            memset(pl, 0, 0xA00);
            memset(cw + off + 0x132C, 0, 8);
            memset(cw + off + 0x1334, 0, 8);
            memset(cw + off + 0x1346, 0, 8);
            *(s8 *)(lp + 0x24) = 0;
        }
        c += 0x5C;
        i = i + 1;
        off += 0x2FC;
        pl += 0xA00;
        lp += 0x38;
    } while (i < 8);
    return s5;
}
int lb_member_changeCheck(void) {
    u8 *c;
    int s1;
    s8 i;
    int r;
    u8 *pl;
    r = 0;
    i = 0;
    pl = (u8 *)player_work;
    s1 = 0;
    c = (u8 *)lbCommer;
    do {
        if (*pl != 0 && memcmp(c + 0x1C, cw + s1 + 0x1346, 0x40) != 0 && lb_check_mini_data(i, c) == 1) {
            r = 1;
        }
        pl += 0xA00;
        i = i + 1;
        s1 += 0x2FC;
        c += 0x5C;
    } while (i < 8);
    return r;
}
void lb_member_inCheck(void) {
    u8 *pl;
    int off;
    s8 i;
    u8 *c;
    u8 *c2;
    u8 *base;
    i = 0;
    c = (u8 *)lbCommer;
    off = 0;
    c2 = (u8 *)lbCommer;
    pl = (u8 *)player_work;
    do {
        if (*(s8 *)c != 0) {
            base = cw + off;
            if (*(s8 *)(base + 0x132C) == 0) {
                strcpy((char *)(base + 0x132C), (char *)c2);
                strcpy((char *)(cw + off + 0x1334), (char *)c + 8);
                memcpy(cw + off + 0x1346, c + 0x1C, 0x40);
                Lb_set_mini_data_to_pl(i, c + 0x1C);
                Lb_set_player(i & 0xFF, c, c + 8);
                *(s8 *)(cw + i + 0x2BFE) = 0;
                pl[1] = 1;
            }
        }
        c += 0x5C;
        i = i + 1;
        off += 0x2FC;
        c2 += 0x5C;
        pl += 0xA00;
    } while (i < 8);
}
void Lb_check_newCommer(int a) {
    int i;
    u8 *pl;
    u8 st;
    if (*(s8 *)(cw + 0x2C08) != 0) {
        if (lb_member_outCheck() == 1) {
            st = game_w.stage;
            if (st != 0x4C) {
                if (st == 0x4D) {
                    goto b5;
                }
            } else {
b5:
                flCompact(st);
                lb_sys.x8D = 3;
            }
            lb_sys.chair_mask = 0;
            st = D_3E4C05[game_w.master * 0xA00];
            if (st != 0x2B && st != 0x29 && st != 0x2A && st != 0x4E && st != 0x60 && st != 0x5E && st != 0x5D && st != 0x5C && st != 0x5A && st != 0x59 && st != 0x54 && st != 0x4C) {
            } else {
                Lb_send_myChair();
            }
            return;
        }
        if (lb_sys.x8D == 0) {
            if (lb_member_changeCheck() == 1) {
                st = game_w.stage;
                if (st != 0x4C) {
                    if (st == 0x4D) {
                        goto b27;
                    }
                } else {
b27:
                    flCompact(st);
                    lb_sys.x8D = 3;
                }
                return;
            }
            if (lb_sys.x8D == 0) {
                lb_member_inCheck();
                st = game_w.stage;
                if (st != 0x4D) {
                    if (st == 0x4C) {
                        goto b33;
                    }
                } else {
b33:
                    i = 0;
                    pl = (u8 *)player_work;
                    do {
                        if (*pl != 0 && *(s8 *)(cw + (s8)i + 0x2BFE) == 0) {
                            Lb_player_load(pl);
                        }
                        i = (s8)(i + 1);
                        pl += 0xA00;
                    } while (i < 8);
                }
            }
        }
    }
}
void Lb_load_player_all(a, b)
int a;
int b;
{
    u32 i;
    u8 *pl;
    u8 *c;
    if (*(u8 *)((u8 *)&lb_sys + 0x70) == 0) {
        c = cw;
        if (*(s8 *)(c + 0x2C06) == 0) {
            *(s8 *)(c + 0x2C06) = 1;
            com_motion_load(1);
        }
        i = 0;
        pl = (u8 *)player_work;
        do {
            if (*pl != 0) {
                c = cw + i;
                if (*(s8 *)(c + 0x2BFE) == 0) {
                    *(s8 *)(c + 0x2BFE) = 1;
                    Lb_player_load(pl);
                    Lbc_connect(a);
                }
            }
            i += 1;
            pl += 0xA00;
        } while (i < 8U);
        npc_create_model();
        Lbc_connect(a);
        npc_create_model();
        Lbc_connect(a);
        npc_create_model();
        Lbc_connect(a);
        npc_create_model();
        Lbc_connect(a);
        *(u8 *)((u8 *)&lb_sys + 0x70) = 1;
    }
}
void Lb_set_player(a, b, c)
int a;
u8 *b;
u8 *c;
{
    u16 id;
    u8 *lp;
    u8 *pl;
    u8 *pr;
    s16 s;
    id = a & 0xFF;
    lp = (u8 *)lb_player + id * 0x38;
    pl = (u8 *)player_work + id * 0xA00;
    *(u8 **)lp = pl;
    pl[0] = 1;
    *(u16 *)(pl + 0xC) = id;
    memcpy(lp + 4, c, 0x11);
    memcpy(lp + 0x24, b, 8);
    memcpy(pl + 0x8D4, c, 0x11);
    *(f32 *)(pl + 0xC0) = 1.0f;
    *(f32 *)(pl + 0xBC) = 1.0f;
    *(f32 *)(pl + 0xB8) = 1.0f;
    *(s16 *)(pl + 0x300) = 2;
    *(f32 *)(pl + 0x798) = 1.0f;
    *(f32 *)(pl + 0x1A0) = 1.0f;
    *(f32 *)(pl + 0x1F0) = 1.0f;
    *(s16 *)(pl + 0x568) = get_prim();
    *(s8 *)(pl + 0x4D4) = 1;
    s = *(s16 *)(pl + 0x568);
    if (s != -1) {
        pr = (u8 *)get_prim_ptr(s);
        *(u8 **)(pl + 0x564) = pr;
        *(s32 *)(pr + 0x18) = *(u16 *)(pl + 0xC);
        *(void **)(pr + 0x14) = Lb_trans_pl;
    }
    *(s8 *)(pl + 0x8F0) = 1;
    Lb_pl_to_normal(pl, 0, 0, 0);
    *(u8 **)(pl + 0x3CC) = pl01_adr_tbl;
    (**(void (**)(u8 *))(*(u8 **)(pl + 0x3CC) + 0xC))(pl);
}
