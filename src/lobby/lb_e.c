/* Lobby: room members, friend list, status set, cockpit (SLPM_654.95 lobby overlay 0x5CB310-0x5CCEE0). Whole file; matching runs split into lb_eNN.c */
#include "lobby.h"

void Lobby_quest_print(void) {
    font_set_palette(5);
    flfntLocate(0x167, 0x8E);
    font_print_uf(lit_254_00664B00);
    font_set_palette(0);
    lb_put_room_member_005CB220();
}

u8 *Lb_room_member(int id, int sel) {
    u8 *m = CWPLAYER(id & 0xFF);
    u8 *r;
    if (*(s8 *)(m + 0x73C) == 0) {
        return 0;
    }
    r = m + 0x744;
    if (sel & 0xFF) {
        r = m + 0x73C;
    }
    return r;
}

void Lb_frendlist_entry(int a0, int a1) {
    pNet[7] = 5;
    pNet[5] = 0;
    cnLBS_Get_ConditionSearchUser(&SearchResult);
    memcpy(SearchResult + 4, (void *)a0, 8);
    memcpy(SearchResult + 0xC, (void *)a1, 0x12);
    cnWrap_SoundRequest(6);
}

void Lb_PlStatusSet(int id) {
    s16 *mini;
    u8 *pl;
    u8 uid;
    s16 a2, a1;
    mini = (s16 *)GetAdrsMiniData();
    uid = id;
    a2 = mini[4];
    a1 = mini[5];
    pl = (u8 *)&player_work[uid];
    *(s16 *)(pl + 0x35E) = a2;
    *(s16 *)(pl + 0x360) = a1;
    *(s16 *)(pl + 0x362) = mini[6];
    pl[0x352] = ((u8 *)mini)[0xE];
    pl[0x353] = ((u8 *)mini)[0xF];
    pl[0x354] = ((u8 *)mini)[0x10];
    pl[0x355] = ((u8 *)mini)[0x11];
    pl[0x356] = ((u8 *)mini)[0x12];
    pl[0x357] = ((u8 *)mini)[0x13];
    Skill_set_PL(pl, a1, a2, uid * 0xA00);
    Lb_get_comment((u8 *)&lb_player[uid] + 0x24);
}

void Lb_equip_set(u16 *p, int a1) {
    Set_equip_idx(a1);
    Set_userdata(p);
    ((u8 *)&lb_sys)[0x78] = 1;
    Lb_set_mini_data((u8 *)&lbCommer[p[6]] + 0x1C);
    Lb_set_mini_data(CWPLAYER(p[6]) + 0x1346);
}

void Lb_cockpit_move(void) {
    flSetRenderState(0x6D, 7);
    flSetRenderState(0x6C, 1);
    flSetRenderState(1, 1);
    InitRenderState(1);
    switch (lb_sys.x68) {
    case 0:
    case 0xF:
        lb_sys.x6C = 0;
        if (lb_sys.x68 == 0) {
            Lbc_set_prim(lb_disp_name, Lb_put_help, 0);
        } else {
            Lbc_set_prim(lb_disp_name, 0, 0);
        }
        break;
    }
    flSetRenderState(0x6D, 3);
}

void Lb_put_msg(s16 *p) {
    flfntLocate(p[0], p[1]);
    font_print(lit_429_00664C38, p + 2);
    strlen_sp(p + 2);
}

void Lb_put_status(int a0, int a1, int a2, int a3, int no) {
    switch ((s16)no) {
    case 0:
        return;
    case 1:
        no = 0x10;
        break;
    case 2:
        no = 0x11;
        break;
    case 3:
        no = 8;
        break;
    case 4:
        no = 0x12;
        break;
    }
    Lb_put_icon_free(a0, a1, a2, a3, no);
}

void Lb_put_icon_free(a0, a1, a2, a3, no)
int a0;
s16 a1;
s16 a2;
int a3;
s16 no;
{
    struct { s16 w, x, y, z; s32 u; LBS16x2 e; LBS16x2 f; } pk;
    LBS16x2 *t = (LBS16x2 *)&lb_icon_tbl[no * 4];
    pk.x = a1;
    pk.w = 0.8f * a0;
    pk.y = 0.8f * a2;
    if (pk.y < 0) {
        pk.y = -pk.y;
    }
    pk.z = a2;
    pk.u = a3;
    pk.e = t[0];
    pk.f = t[1];
    flps0008(&pk, &pk.f, &pk.e);
}

void Lb_put_icon_free2(a0, a1, a2, a3, no)
int a0;
s16 a1;
s16 a2;
int a3;
s16 no;
{
    struct { s16 w, x, y, z; s32 u; LBS16x2 e; LBS16x2 f; } pk;
    LBS16x2 *t;
    pk.x = a1;
    pk.u = a3;
    t = (LBS16x2 *)&lb_icon_tbl[no * 4];
    pk.y = a2;
    pk.z = a2;
    pk.w = 0.8f * a0;
    pk.e = t[0];
    pk.f = t[1];
    flps0008(&pk, &pk.f, &pk.e, t);
}

void Lb_num_to_str(int n, char *out) {
    char buf[0x20];
    char *p;
    char c;
    sprintf(buf, lit_688_00664CB0, n);
    *out = 0;
    c = buf[0];
    p = buf;
    if (c != 0) {
        do {
            strcat(out, *(char **)((u8 *)lb_num_str + (c - 0x30) * 4));
            p++;
            c = *p;
        } while (c != 0);
    }
}

int Lb_check_pl_load(int id) {
    s8 i = id;
    if (player_work[i].be_flag != 0 && ((s8 *)(i + (int)cw))[0x2BFE] == 0) {
        return 0;
    }
    return 1;
}
