/* lb_sh1c - one translation unit 0x0053AB50-0x0053C23C (lbtu3). */
#include "lobby_s.h"
extern s8 r_no_process;
extern s32 armorIndex;
extern char User_data[];
extern s32 shop_process2_help[];
int Equip_ok_ck();
int Now_equip_ck();
void random_stack();
void lb_process_tag_decide01();
extern u8 D_3C7005[];
extern s16 D_3C7006[];
extern s16 D_3C7008[];
extern s8 armor_shop_r;
extern u8 buki_sei_tbl[];
extern u8 bou_sei_tbl[];
void armor_set_myArmor();
/* armor upgrade flags: bits 0-3 level, 4/5 = two toggles (exclusive pair), 6 = flag; op 0 = level up (max 4), 1-6 set/clear (near-match: 3 instructions off, the compare lands in at instead of v0) */
/* original bytes: build/raw/value_result.inc (config/c_rawfuncs.txt); the C below is a near-match, used by the PC build */
#ifdef __MWERKS__
#else
#endif
extern char kakou_tbl[];
extern char shopList2[];
extern char buki_sei_tbl_c4[];
extern char bou_sei_tbl_c4[];
extern char lit_1225_006555D0[];
extern char lb_shop_msg[];
extern char lit_1226_006555D8[];
extern char lit_1227_006555E0[];
/* original bytes: build/raw/lb_process_drawHelp.inc (config/c_rawfuncs.txt); the C below is a near-match, used by the PC build */
#ifdef __MWERKS__
#else
#endif
void Lb_put_armorIcon_a5(int x, int y, int z, s16 kind, s16 id);
extern char lit_1287_006555F0[];
extern char item_str[];
void Lb_put_job();
void Lb_put_icon_free2();
extern s32 shop_armor00_tag[1];
s32 shop_process_after();
s32 shop_armor2_stack(int kind, int id);
s32 shop_armor2_question();
asm s32 value_result(s32 v, s32 op);
s32 value_result(s32 v, s32 op);
asm void lb_process_drawHelp();
void lb_process_drawHelp();
void lb_armor2_listItem(int x, int y, int z, s16 n);
void Lb_put_materialItem();
void Lb_put_materialBase();
void Lb_put_armorIcon(int x, int y, int z, int kind, int id);
void armor_shop2_trans();
void lb_armor_init();
int Lb_put_armorIcon_k();
int shop_armor2_stack_k();
int value_result_k();
s32 shop_process_after() {
    struct { s8 x0; s8 id; s16 num; s32 x4; } st;
    PLW *pl;
    int id, num;
    u8 *em;

    pl = &player_work[*(u8 *)0x3F34C1];
    id = lbShop.tbl[lbShop.cur * 2];
    num = lbShop.tbl[lbShop.cur * 2 + 1];
    em = (u8 *)pl->x3B0;
    switch (r_no_process) {
    case 0:
        if (Online_ck() == 0) {
            r_no_process = 3;
            st.id = lbShop.tbl[lbShop.cur * 2];
            st.num = lbShop.tbl[lbShop.cur * 2 + 1];
            if (Equip_ok_ck(User_data, &st) == 0) {
                cnWrap_SoundRequest(0x11);
                lbShop.help = shop_process2_help[2];
                shop_armor2_stack_k(id, num);
                r_no_process = 0;
                lbShop.f38 = 0;
                lb_process_tag_decide01();
                Lb_put_set01(0xC);
                return 0;
            }
            if (lbShop.mode == 0 && lbShop.x1A == 1) {
                if (Now_equip_ck(User_data, armorIndex) == 1) {
                    shop_armor2_stack_k(id, num);
                    lbShop.f38 = 0;
                    lb_process_tag_decide01();
                    r_no_process = 0;
                    return 0;
                }
            } else {
                if (lbShop.tbl[lbShop.cur * 2 + 1] == 0x3E7) {
                    random_stack();
                }
                id = lbShop.tbl[lbShop.cur * 2];
            }
            lbShop.help = shop_process2_help[6];
            if (id == 7 || id == 6) {
                if (*(u8 *)0x3C738D != id) {
                    lbShop.help = shop_process2_help[8];
                    lbShop.x78 = 1;
                }
            }
            lbShop.x78 = 1;
            cnWrap_SoundRequest(0x11);
        } else {
            r_no_process++;
            if (pl->x3B0 != 0) {
                Lb_act_set(pl->x3B0, 0, 0x68);
            }
        }
        break;
    case 1:
        pNet[0x11] = 1;
        pl->work8ED = 0;
        if (*(u16 *)(em + 0x2DC) == 0x3FE) {
            r_no_process++;
        }
        break;
    case 2:
        pl->work8ED = 0;
        pNet[0x11] = 1;
        if (*(u16 *)(em + 0x2DC) == 0x3E9) {
            r_no_process++;
            lbShop.help = shop_process2_help[6];
            if (lbShop.tbl[lbShop.cur * 2 + 1] == 0x3E7) {
                random_stack();
            }
            st.id = lbShop.tbl[lbShop.cur * 2];
            st.num = lbShop.tbl[lbShop.cur * 2 + 1];
            if (Equip_ok_ck(User_data, &st) == 0) {
                lbShop.help = shop_process2_help[2];
                shop_armor2_stack_k(id, num);
                r_no_process = 0;
                lbShop.f38 = 0;
                lb_process_tag_decide01();
                Lb_put_set01(0xC);
                return 0;
            }
            if (lbShop.mode == 0 && lbShop.x1A == 1 && Now_equip_ck(User_data, armorIndex) == 1) {
                lbShop.help = shop_process2_help[2];
                shop_armor2_stack_k(id, num);
                r_no_process = 0;
                lbShop.f38 = 0;
                lb_process_tag_decide01();
                return 0;
            }
            if (id == 7 || id == 6) {
                if (*(u8 *)0x3C738D != id) {
                    lbShop.help = shop_process2_help[8];
                }
            }
            lbShop.x78 = 1;
        }
        break;
    case 3:
        if (shop_armor2_question() != 2) {
            r_no_process = 0;
            lb_process_tag_decide01();
            return 0;
        }
        break;
    }
    return 2;
}

s32 shop_armor2_stack(int kind, int id) {
    s32 ai;
    s32 idx;
    int c;
    u16 *lp;

    if (lbShop.mode == 0 && lbShop.x1A == 1) {
        ai = armorIndex;
        idx = ai & 0xFF;
        D_3C7005[idx * 6] = kind;
        if (kind == 6) {
            *(s16 *)((u8 *)D_3C7006 + idx * 6) = id;
            *(s16 *)((u8 *)D_3C7008 + idx * 6) = 0;
            if (idx == *(u8 *)0x3C7416) {
                Set_equip_idx(User_data);
                Set_userdata((u8 *)player_work + game_w.master * 0xA00);
            }
        } else {
            lp = (u16 *)lbShop.list;
            c = lbShop.cur;
            switch (lp[c * 20 + 0x13]) {
            case 0:
                Gun_level_up(User_data, (s16)ai, 1);
                break;
            case 1:
                Gun_Silencer_set(User_data, (s16)ai, 0);
                break;
            case 2:
                Gun_Silencer_set(User_data, (s16)ai, 1);
                break;
            case 3:
                Gun_barrel_set(User_data, (s16)ai, 0);
                break;
            case 4:
                Gun_barrel_set(User_data, (s16)ai, 1);
                break;
            case 5:
                Gun_Scope_set(User_data, (s16)ai, 0);
                break;
            case 6:
                Gun_Scope_set(User_data, (s16)ai, 1);
                break;
            }
            if (Now_equip_ck(User_data, armorIndex) == 1) {
                Set_equip_idx(User_data);
                Set_userdata((u8 *)player_work + game_w.master * 0xA00);
            }
        }
    } else {
#ifdef __MWERKS__
        idx = item_to_stack() & 0xFF;
#else
        idx = item_to_stack(kind, id) & 0xFF;  /* PC: a0/a1 are still kind/id in the asm (argregs.py) */
#endif
    }
    return idx;
}

s32 shop_armor2_question() {
    s32 key;
    s32 k;
    s32 r;
    int kind;
    int id;
    u16 *lp;
    int c;
    u8 *e;
    u8 *f;
    u8 m;

    c = lbShop.cur;
    e = (u8 *)lbShop.tbl + c * 8;
    key = lbShop.key;
    if (lbShop.mode == 0) {
        if (lbShop.x1A == 1) {
            kind = *(u16 *)e;
            id = *(u16 *)(e + 4);
        } else {
            f = buki_sei_tbl + ((u16 *)&((u8 *)shopList)[0x26])[c * 20] * 0x18;
            kind = f[0];
            id = *(u16 *)(f + 2);
            if (id == 0x3E7) {
                kind = lbShop.x5A[1];
                id = *(u16 *)&lbShop.x5A[2];
            }
        }
    } else {
        f = bou_sei_tbl + ((u16 *)&((u8 *)shopList)[0x26])[c * 20] * 0x18;
        kind = f[0];
        id = *(u16 *)(f + 2);
    }
    if (armor_shop_r == 0) {
        k = key & 0xFFFF;
        if (k & 0x20) {
            r = shop_armor2_stack_k(kind & 0xFFFF, id & 0xFFFF) & 0xFF;
            if (lbShop.x78 == 0) {
                cnWrap_SoundRequest(0x10);
                cnWrap_SoundRequest(0);
                Warehouse_equip(User_data, r);
                armor_shop_r++;
            } else {
            cnWrap_SoundRequest(3);
            Lb_put_set01(0xC);
            return 3;
            }
        } else if (k & 0x40) {
            if (lbShop.x78 != 1) {
                cnWrap_SoundRequest(3);
                lbShop.x78 = 1;
            } else {
            shop_armor2_stack_k(kind & 0xFFFF, id & 0xFFFF);
            cnWrap_SoundRequest(3);
            Lb_put_set01(0xC);
            return 3;
            }
        } else if (k & 0x800) {
            if (lbShop.x78 != 0) {
                lbShop.x78 = 0;
                cnWrap_SoundRequest(1);
            }
        } else if ((k & 0x400) && lbShop.x78 != 1) {
            lbShop.x78 = 1;
            cnWrap_SoundRequest(1);
        }
    } else {
    if ((u16)kind != 7 && (u16)kind != 6) {
        armor_set_myArmor(kind, id);
    } else if (*(u8 *)0x3C738D != (u16)kind) {
        armor_set_myArmor(kind, id);
    } else {
        Set_equip_idx(User_data, id);
    }
    lb_sys.x78 = 1;
    Set_userdata((u8 *)player_work + game_w.master * 0xA00);
    Lb_set_mini_data(cw + game_w.master * 0x2FC + 0x1346);
    m = game_w.master;
    memcpy((u8 *)lbCommer + m * 0x5C + 0x1C, cw + m * 0x2FC + 0x1346, 0x40);
    return 0;
    }
    return 2;
}

#ifdef __MWERKS__
asm s32 value_result(s32 v, s32 op)
{
#include "value_result.inc"
}
#endif

#ifdef __MWERKS__
asm void lb_process_drawHelp()
{
#include "lb_process_drawHelp.inc"
}
#endif

void lb_armor2_listItem(int x, int y, int z, s16 n) {
    u16 kind;
    u16 id;
    u8 *e;
    int i;
    u8 *f;

    i = (s16)n + lbShop.x6C * 7;
    e = (u8 *)lbShop.tbl + i * 8;
    if (lbShop.mode == 0) {
        if (lbShop.x1A == 1) {
            kind = *(u16 *)e;
            id = *(u16 *)(e + 4);
            if (kind == 7) {
                id = *(u16 *)&lbShop.x54[2];
            }
        } else {
            f = buki_sei_tbl + ((u16 *)&((u8 *)shopList)[0x26])[i * 20] * 0x18;
            kind = f[0];
            id = *(u16 *)(f + 2);
        }
    } else {
        f = bou_sei_tbl + ((u16 *)&((u8 *)shopList)[0x26])[i * 20] * 0x18;
        kind = f[0];
        id = *(u16 *)(f + 2);
    }
    Lb_put_armorIcon_a5(x, y, z, kind, id);
}

void Lb_put_materialItem(y, id, need)
int y;
int id;
int need;
{
    int num;
    int r;
    int u;

    u = id & 0xFFFF;
    num = (s16)Ud_item_num_ck(u);
    r = Ud_stock_item_num_ck3(u);
    u = (s16)id;
    id = (s16)r;
    if (u != 0) {
        if ((s16)id > 99) {
            id = 99;
        }
        if ((s16)need <= (s16)num) {
            font_set_palette(0);
        } else if ((s16)need <= (s16)num + (s16)id) {
            font_set_palette(6);
        } else {
            font_set_palette(10);
        }
        flfntLocate(0x12C, y);
        font_print(&lit_1287_006555F0, ((s32 *)&item_str)[u], (s16)num, (s16)id, (s16)need);
    }
}

void Lb_put_materialBase() {
    LB_SHOPITEM *it;

    it = (LB_SHOPITEM *)lbShop.list + lbShop.cur;
    Paint_square(0x120, 0x4C, 0x140, 0xFC, 0xA0202020);
    Draw_menu_square(0x120, 0xC4, 0x140, 0x84, 0, 0);
    flfntSetSize(0x14, 0x14);
    flfntLocate(0x160, 0xD6);
    font_print(lit_1225_006555D0, it->name);
}

void Lb_put_armorIcon(int x, int y, int z, int kind, int id) {
    int icon;
    int k;
    int uid;
    int col;

    k = (s16)kind;
    if (k == 7 || k == 6) {
        if ((s16)id == 0x3E7) {
            Lb_put_job(x, y, z, -1, 5, 1);
            return;
        }
        uid = id & 0xFFFF;
        col = Equip_icon_color_rare(Get_equip_rare(kind & 0xFF, uid) & 0xFF, 0xFF, 0);
        Lb_put_job(x, y, z, col, Get_weapon_job2((u8)kind, uid) & 0xFF, 1);
        return;
    }
    reload_tex(1, 0x118);
    SetTextureStage(0x118);
    switch (k) {
    case 0:
        icon = 0x17;
        break;
    case 2:
        icon = 0x13;
        break;
    case 3:
        icon = 0x14;
        break;
    case 4:
        icon = 0x16;
        break;
    case 5:
        icon = 0x15;
        break;
    }
    Lb_put_icon_free2(x, y, z, Equip_icon_color_rare(Get_equip_rare(kind & 0xFF, id & 0xFFFF) & 0xFF, 0xFF, 0), (s16)icon);
    reload_tex(1, 0x157);
    SetTextureStage(0x157);
}

void armor_shop2_trans() {
    int temp_a1;

    temp_a1 = F(s32, &lb_pit, 4) + (F(s8, &lb_pit, 8) * 8);
    switch (lbShop.step) {
    case 0:
    case 2:
        break;
    default:
            F(s8, &lb_pit, 0xB) = NPC_Message(F(s32, temp_a1, 4), F(s32, &lb_pit, 0), F(u16, temp_a1, 0), F(s8, &lb_pit, 9));
        break;
    }
}

void lb_armor_init() {
    lbShop.tag = shop_armor00_tag;
    lbShop.x17 = 2;
}

