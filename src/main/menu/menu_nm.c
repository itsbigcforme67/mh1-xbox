/* menu_nm - f_menu (SLPM_654.95 0x00127440-0x00134950, main.bin): the whole pit menu file as near-match C, not built; matching runs are built from it as menuNN.c.
 * Field meanings are guesses. */
#include "menu.h"
#include "em.h"
#include "pl.h"
#include "fl.h"

extern u8 Psw[];
extern f32 map_size[][2];
extern u8 room_member_id[];

void *memset(void *, int, int);
void se_req(int, int, int);
extern u8 Item_data[327][16];
void PlayerStatusWindow(PLW *, u8);
void DispFrameMessage(void *, int);
void flfntLocate(int, int);
void font_print_sp(void *);
extern u8 frame_retire[];
extern int retire_str;
extern void *retire_yesno_str[2];
extern f32 wyvern_area_tbl[][4];
void SetFilterMode(int);
void SetTextureStage(int);
void reload_tex(int, int);
void disp_whole_map(int, f32, f32);
typedef struct PFLPS {
    s16 s[4];
    u32 a;
    u32 b;
    u32 c;
} PFLPS;
void flps0008(void *);
typedef struct PFLPS2 {
    s16 s[4];
    u32 col;
    s16 uv[4];
} PFLPS2;
void efct_circle(int, int, int, f32, f32);
typedef struct PFLP12 {
    s16 p[6];
    u32 col;
    s16 uv[6];
} PFLP12;
void flps000C(void *);
void flmatSetZYX33(f32, f32, f32, void *);
extern u8 enemy_icon_tbl[];
extern u32 enemy_icon_color[];
extern s16 enemy_icon_tex_u[];
void maru_disp_sub(int, f32, f32, f32);
void SetTrnslMode(int, int);
void flSetRenderState(int, u32);
void font_set_stack_no(int);
void disp_timer(void);
void disp_pl_vital(void);
void disp_slash_level(void);
void disp_pachinger(void);
void disp_cannon(void);
void disp_others_info(void);
void disp_name(void);
void Pit_disp_chat(void);
void disp_item_stock(void);
void func_5B4980(void);
void func_60CE50(void);
void Disp_NPC_message(void);
void Pit_disp_pit_effect(void);
void Pit_disp_receive_mes(void);
int SoftKeyboard_alive_check(void);
void DispSoftkeyboard(u8);
void func_63B470(void);
void disp_map(void);
void disp_menu(int, PIT_W *);
void Disp_menu_help(void);
void disp_item(void);
void disp_item_sub_select_ex(void);
void trans_box(void);
extern void (*disp_menu_jmp[])(int, PIT_W *);
f32 flSin(f32);
extern u16 System_timer;
void Chat_init(void);
void Chat_move(int);
void Join_pl_chk(void);
void Pit_effect_move(void);
void Receive_mess_move(void);
void add_prim2(void *, void *, int, int);
void func_5B3ED0(int);
int kb_chat_in_chk(void);
int softkey_ck(void);
extern int ot6;
extern int ot7;
int Game_clear_ck(int);
int Pit_shot_ok_chk(PLW *);
void Pl_box_select(PLW *);
int UseItemChk(PLW *, u16);
int func_63B0C0(int);
void menu_init(void);
void menu_move(int);
extern int ot5;
void Name_ID_change(void);
int Menu_chatlog_i(void);
extern int (*menu_mv_jmp[])(int);
void Chat_log_clear(void);
void pit_prim_init(void);
u16 pit_key_repeat(u16, u16);
void SoftKeyboard_exit(void);
int Quest_time_get(int);
int Online_ck();
int Pl_Skill_ck(PLW *, int);
void trans_pit_0(void);
void trans_pit_1(void);
void trans_pit_2(void);
void trans_pit_1_lb(void);
void trans_pit_2_lb(void);
void func_5B3D70();
void func_609750();
int Item_ok_chk(PLW *);
void ListSelect(u8 *, int, int);
void Menu_select_mv(u8 *, int, int);
void Pl_item_erase(PLW *, u8);
void func_5B3E60();
int func_5D8370(s8);
int Item_preparation_one_ck(s16);
void *Item_preparation_adrs(s16, s16, PLW *);
int Item_preparation_list_chk_0(void *);
int ItemPickingDeclaration(int, s16 *);
int Game_clear_ck(int);
void Quest_retire_set(void);
int func_5BD520(void);
void PageSelect(u8 *, int, int);
extern u8 quest_w[];
void font_print_uf(char *, int);
u8 *func_5B4D30(u8);
extern u8 User_data[];
int Item_preparation_list_search(s8 *, s8, u16 *, u16 *);
int Monster_list_search(s8, int);
int Get_weapon_job2(u8, u16);
void vib_set(int, int);
int Reibun_Edit_Core(u8);
int Reibun_Edit_Start(u8);
u8 Reibun_select_mv(u8);
extern u16 item_pick_declaration_code;
extern s16 item_pick_declaration_timer;
int Pl_master_ck(void);
int Check_hold_item(int);
void Pl_item_get_se(PLW *, int);
int Pl_item_stack(PLW *, int, int);
u8 set01_set(int, int, s16);
int item_stock_mv();
int Info_stack_ck(void);
void ItemCopy_Pl2Ud(PLW *);
void net_send_sys(int, u8);
f32 flSqrt(f32);
void menu_data_mix_sub(int);
void menu_data_monster_sub(int);
int menu_chcnfg_sendpl(int sw);
int menu_chcnfg_reibun(int sw);

void PitWork_init(void) {
    lpPit = &pit_work;
    memset(lpPit, 0, 0x90);
    GWS8(0xE) = 0;
    memset(&PitMenu, 0, 0x1764);
    lpPit->x3C = 10;
    Chat_log_clear();
}

void Pit_init(void) {
    PLW *p;
    u16 z = 0;

    pit_prim_init();
    pit_prim[0].trans = trans_pit_0;
    pit_prim[1].trans = trans_pit_1;
    pit_prim[2].trans = trans_pit_2;
    lpPit->pl = &player_work[game_w.master];
    *(s32 *)&lpPit->x04 = 0;
    pit_key_repeat(0, 0);
    lpPit->x40 = 0;
    GWS8(0xE) = 0;
    SoftKeyboard_exit();
    Chat_log_clear();
    PitMenu.open = 0;
    PitMenu.x0F = z;
    PitMenu.x0C = 0;
    lpPit->x80 = -1;
    PitMenu.x17 = 3;
    PitMenu.x15 = 1;
    PitMenu.x16 = 0;
    PitMenu.x18 = 0;
    PitMenu.x14 = 0;
    lpPit->map_sx = 1.0f / map_size[game_w.x2E][0];
    lpPit->map_sy = 1.0f / map_size[game_w.x2E][1];
    lpPit->x3A = -1;
    lpPit->x38 = -1;
    lpPit->x36 = -1;
    lpPit->x34 = -1;
    lpPit->x3E = 0;
    lpPit->x3F = 0;
    lpPit->x41 = 0;
    PitMenu.x10 = 0;
    lpPit->x84 = 0;
    lpPit->x52 = 0;
    lpPit->x8D = 0;
    lpPit->x28 = 4;
    lpPit->x29 = 0;
    lpPit->x2B = 0;
    p = lpPit->pl;
    lpPit->x5A = *(u16 *)((u8 *)p + 0x888);
    lpPit->x5E = *(u16 *)((u8 *)p + 0x88E);
    lpPit->lb = 0;
    lpPit->x8A = GW8(0x1DD);
    lpPit->x8B = game_w.x0F;
    lpPit->x8C = FLDS8(option_w, 3);
    PitMenu.x00 = 0;
    PitMenu.x08 = -1;
    PitMenu.x06 = 0;
    lpPit->x10 = -1;
    lpPit->x12 = -1;
    lpPit->x14 = -1;
    lpPit->x16 = -1;
    if (game_w.x1DC == 1) {
        func_5B3D70();
        func_609750();
        lpPit->lb = 1;
        pit_prim[1].trans = trans_pit_1_lb;
        pit_prim[2].trans = trans_pit_2_lb;
    } else {
        lpPit->lb = 0;
        lpPit->time0 = Quest_time_get(0);
        lpPit->time1 = Quest_time_get(1);
    }
}

void Pit_reset(void) {
    PLW *p;
    u16 z = 0;

    pit_prim_init();
    pit_prim[0].trans = trans_pit_0;
    pit_prim[1].trans = trans_pit_1;
    pit_prim[2].trans = trans_pit_2;
    lpPit->pl = &player_work[game_w.master];
    *(s32 *)&lpPit->x04 = 0;
    pit_key_repeat(0, 0);
    lpPit->x40 = 0;
    GWS8(0xE) = 0;
    SoftKeyboard_exit();
    PitMenu.open = 0;
    PitMenu.x0F = z;
    PitMenu.x0C = 0;
    lpPit->x3A = -1;
    lpPit->x38 = -1;
    lpPit->x36 = -1;
    lpPit->x34 = -1;
    lpPit->x3F = 0;
    lpPit->x41 = 0;
    PitMenu.x10 = 0;
    lpPit->x84 = 0;
    lpPit->x52 = 0;
    lpPit->x8D = 0;
    lpPit->x28 = 4;
    lpPit->x29 = 0;
    lpPit->x2B = 0;
    p = lpPit->pl;
    lpPit->x5A = *(u16 *)((u8 *)p + 0x888);
    lpPit->x5E = *(u16 *)((u8 *)p + 0x88E);
    PitMenu.x00 = 0;
    PitMenu.x08 = -1;
    PitMenu.x06 = 0;
    lpPit->x10 = -1;
    lpPit->x12 = -1;
    lpPit->x14 = -1;
    lpPit->x16 = -1;
    if (game_w.x1DC == 1) {
        func_5B3D70();
        func_609750();
        lpPit->lb = 1;
        pit_prim[1].trans = trans_pit_1_lb;
        pit_prim[2].trans = trans_pit_2_lb;
    } else {
        lpPit->lb = 0;
    }
}

void select_yes_no(int unused, u16 mask) {
    u16 k = FLD16(Psw, 4) & mask;

    if (lpPit->yn == 0) {
        if (k & 0x1400) {
            lpPit->yn = 1;
            se_req(7, 0x16, 0);
        }
    } else if (k & 0x2800) {
        lpPit->yn = 0;
        se_req(7, 0x16, 0);
    }
}

int Cockpit_menu_chk(void) {
    if (GW8(0xE) != 0) {
        return 1;
    }
    if (PitMenu.open != 0) {
        return 1;
    }
    return (lpPit->x07 != 0) ? 1 : 0;
}

int Cockpit_menu_chk_lobby(void) {
    if (Online_ck() == 1 && game_w.x1DC == 1) {
        return *(int *)0x6EAEBC;
    }
    return 0;
}

int enemy_mark_chk(PLW *pl, EMW *em) {
    u8 c;

    if (em->be_flag == 0) {
        return 0;
    }
    if (em->x56A != 0) {
        return 1;
    }
    c = enemy_icon_tbl[em->kind];
    if (c == 0xFF) {
        return 0;
    }
    if ((c & 0xC0) == 0x80) {
        return 0;
    }
    if (pl->work930 != 0) {
        return 1;
    }
    return Pl_Skill_ck(pl, 0x29);
}

void menu_exit(void) {
    lpPit->x05 = 0;
    lpPit->x40 = 0;
    GWS8(0xE) = 0;
    PitMenu.x10 = 0;
    lpPit->x84 = 0;
}

/* Prints a player name, at most 11 characters, ending with a cut mark. */
void player_name_print(char *name) {
    char buf[16];
    int n = 11;
    char *d = buf;

    do {
        *d = *name;
        if (*name == 0) {
            font_print_uf(buf, n);
            return;
        }
        d++;
        name++;
    } while (--n != 0);
    buf[9] = -0x5B;
    buf[10] = 0;
    font_print_uf(buf, n);
}

void player_name_id_print(PLW *pl) {
    char *name = (char *)pl + 0x8D4;

    if (Online_ck() == 1 && PitMenu.x14 != 0) {
        if (game_w.x1DC == 0) {
            name = (char *)room_member_id + *(u16 *)((u8 *)pl + 0xC) * 8;
        } else {
            name = (char *)func_5B4D30(*(u16 *)((u8 *)pl + 0xC));
        }
    }
    player_name_print(name);
}

u16 pit_key_repeat(u16 now, u16 hold) {
    u16 k = now & 0x3C00;
    u16 h;

    if (k != 0) {
        lpPit->key = k;
        lpPit->rep = 8;
        return (s16)k;
    }
    h = hold & 0x3C00;
    if (h == 0) {
        return lpPit->key = 0;
    }
    lpPit->rep--;
    if (lpPit->rep <= 0) {
        lpPit->rep = 3;
        return lpPit->key &= h;
    }
    return 0;
}

void menu_init(void) {
    lpPit->x05 = 1;
    lpPit->x40 = 0;
    GWS8(0xE) = 1;
    PitMenu.x10 = 1;
    PitMenu.x11 = 0;
    PitMenu.x12 = lpPit->x41;
    if (Online_ck() == 1 && lpPit->x41 == 9) {
        PitMenu.x12 = 10;
    }
    lpPit->x84 = 0;
}

int menu_retire_i(void) {
    if (Game_clear_ck(0) == 1) {
        return 1;
    }
    lpPit->x42 = 0;
    lpPit->yn = 1;
    PitMenu.x10 = 0;
    return 0;
}

int menu_retire_mv(int sw) {
    if (Game_clear_ck(0) == 1) {
        return 0x8000;
    }
    if (lpPit->x42 == 0) {
        select_yes_no(sw, 0xC00);
        if ((u16)sw & 0x20) {
            if (lpPit->yn == 0) {
                Quest_retire_set();
                lpPit->x42++;
                return 0;
            }
            return 0x40;
        }
    } else {
        return 0;
    }
    return sw;
}

int juchu_chk(void) {
    if (game_w.x1DC == 1 && func_5BD520() == 0) {
        lpPit->x46 = 0;
        return 0;
    }
    lpPit->x46 = 1;
    return 1;
}

void Menu_quest_i(void) {
    if (FLDS16(quest_w, 0x24) == 0x63) {
        lpPit->x47 = 4;
    } else if (FLDS16(quest_w, 8) == 0x65 || FLDS16(quest_w, 8) == 0x6B || FLDS16(quest_w, 8) == 0xCE) {
        lpPit->x47 = 3;
    } else if (FLD32(quest_w, 0x40) & 1) {
        lpPit->x47 = 0;
    } else if (FLD32(quest_w, 0x40) & 2) {
        lpPit->x47 = 1;
    } else if (FLD32(quest_w, 0x40) & 4) {
        lpPit->x47 = 2;
    }
    juchu_chk();
    lpPit->x43 = 0;
    PitMenu.x10 = 0;
}

int Menu_quest_mv(int sw) {
    PitMenu.x10 = 0;
    if (juchu_chk() == 1) {
        PageSelect(&lpPit->x43, sw, 4);
    }
    return sw;
}

s16 get_pl_item_type(u8 no) {
    return ((s16 *)((u8 *)lpPit->pl + 0x828))[no * 2];
}

void menu_item_back_sub(void) {
    lpPit->x48--;
    se_req(7, 0x14, 0);
    PitMenu.x12 = get_pl_item_type(lpPit->x49) + 0x18;
}

int item_present_chk(void) {
    if (Online_ck() == 0) {
        return 0;
    }
    if (Item_data[get_pl_item_type(lpPit->x49)][2] >= 3) {
        return 0;
    }
    if (Item_data[get_pl_item_type(lpPit->x49)][3] == 0xFF) {
        return 0;
    }
    if (game_w.x1DC != 0 && game_w.stage != 0x4C && game_w.stage != 0x4D) {
        return 0;
    }
    return 1;
}

void Menu_item_i(void) {
    lpPit->x48 = 0;
    lpPit->x49 = 0;
    lpPit->x4C = 0;
    if (game_w.master == 0) {
        lpPit->x4C = 1;
    }
    PitMenu.x10 = 1;
    PitMenu.x11 = 1;
    PitMenu.x12 = get_pl_item_type(lpPit->x49) + 0x18;
}

int mix_item_chk(u8 no, int id) {
    if (no == 0xFF) {
        return 0;
    }
    if (lpPit->pl->item[no].id != (id & 0xFFFF)) {
        return 0;
    }
    return Item_preparation_one_ck((s16)id) != 0 ? 1 : 0;
}

int mix_item_2_chk(u8 a, u8 b) {
    PLW *pl = lpPit->pl;

    if (a == 0xFF) {
        return 0;
    }
    if (b == a) {
        return 0;
    }
    lpPit->x68 = (s32)Item_preparation_adrs(pl->item[b].id, pl->item[a].id, pl);
    if (lpPit->x68 == 0) {
        return 0;
    }
    if (Item_preparation_list_chk_0((void *)lpPit->x68) != 0) {
        lpPit->x74 = *(s16 *)(lpPit->x68 + 2);
    } else {
        lpPit->x74 = -1;
    }
    return 1;
}

void menu_mix_clear(void) {
    lpPit->x7A = 0;
    lpPit->x49 = 0;
    lpPit->x7B = 0;
    lpPit->x70 = 0xFFFF;
    lpPit->x6E = 0xFFFF;
    lpPit->x6C = 0xFFFF;
    lpPit->x74 = -1;
    lpPit->x68 = 0;
    lpPit->x84 = 0;
}

int Menu_mix_i(void) {
    if (Item_ok_chk(lpPit->pl) == 0) {
        return -1;
    }
    lpPit->x76 = -1;
    if ((s16)ItemPickingDeclaration(0, &lpPit->x76) < 0) {
        return -1;
    }
    menu_mix_clear();
    PitMenu.x10 = 0;
    PitMenu.x11 = 1;
    PitMenu.x12 = 0;
    return 0;
}

/* Item list in the pit menu: step x48 0 pick slot, 1 pick action, 2 details. */
int Menu_item_mv(int sw) {
    PLW *pl = lpPit->pl;
    int ok;
    int add;
    int n;
    int max;
    u8 step = lpPit->x48;

    switch (step) {
    case 0:
        if (!((u16)sw & 0x40)) {
            Menu_select_mv(&lpPit->x49, sw, 0x14);
            PitMenu.x12 = get_pl_item_type(lpPit->x49) + 0x18;
            if ((u16)sw & 0x20) {
                if (pl->item[lpPit->x49].id != 0) {
                    lpPit->x48++;
                    se_req(7, 0x13, 0);
                } else {
                    se_req(7, 0x15, 0);
                }
            }
        }
        break;
    case 1:
        if (pl->item[lpPit->x49].id == 0) {
            lpPit->x48 = 0;
            sw = (u16)(sw & 0xFFBF);
        } else {
            ListSelect(&lpPit->x4A, sw, 2);
            if ((u16)sw & 0x40) {
                sw = (u16)(sw & 0xFFBF);
                lpPit->x48--;
                se_req(7, 0x14, 0);
            } else if ((u16)sw & 0x20) {
                if (Item_ok_chk(pl) == 0) {
                    se_req(7, 0x15, 0);
                } else {
                    switch (lpPit->x4A) {
                    case 0:
                        lpPit->yn = 1;
                        lpPit->x48++;
                        lpPit->x4B = 0;
                        se_req(7, 0x13, 0);
                        break;
                    case 1:
                        if (item_present_chk() == 1) {
                            lpPit->x4D = 1;
                            lpPit->x48++;
                            if (pl->item[lpPit->x49].num != 1) {
                                lpPit->x4E = 0;
                                lpPit->x4B = 0;
                            } else {
                                lpPit->x4B = 1;
                            }
                            se_req(7, 0x13, 0);
                        } else {
                            se_req(7, 0x15, 0);
                        }
                        break;
                    }
                }
            }
        }
        break;
    case 2:
        if (pl->item[lpPit->x49].id == 0) {
            lpPit->x48 = 0;
            sw = (u16)(sw & 0xFFBF);
            PitMenu.x12 = get_pl_item_type(lpPit->x49) + 0x18;
        } else {
            switch (lpPit->x4A) {
            case 0:
                if ((u16)sw & 0x40) {
                    sw = (u16)(sw & 0xFFBF);
                    menu_item_back_sub();
                } else {
                    PitMenu.x12 = 0;
                    select_yes_no(sw, 0x3000);
                    if ((u16)sw & 0x20) {
                        if (lpPit->yn == 0) {
                            Pl_item_erase(pl, lpPit->x49);
                            lpPit->x48 = 0;
                            lpPit->x4A = 0;
                            se_req(7, 0x13, 0);
                        } else {
                            menu_item_back_sub();
                        }
                    }
                }
                break;
            case 1:
                switch (lpPit->x4B) {
                case 0:
                    if ((u16)sw & 0x40) {
                        sw = (u16)(sw & 0xFFBF);
                        menu_item_back_sub();
                    } else {
                        PitMenu.x12 = 3;
                        add = 0;
                        if ((u16)sw & 0x800) {
                            add--;
                        }
                        if ((u16)sw & 0x400) {
                            add += 1;
                        }
                        if ((u16)sw & 0x2000) {
                            add += 10;
                        }
                        if ((u16)sw & 0x1000) {
                            add -= 10;
                        }
                        if (add != 0) {
                            lpPit->x4E = 0;
                            n = add + lpPit->x4D;
                            if (n <= 0) {
                                n = 1;
                            }
                            if (n >= pl->item[lpPit->x49].num) {
                                n = pl->item[lpPit->x49].num;
                                lpPit->x4E = 1;
                            }
                            if (lpPit->x4D != n) {
                                lpPit->x4D = n;
                                se_req(7, 0x16, 0);
                            }
                        }
                        if ((u16)sw & 0x20) {
                            PitMenu.x12 = 1;
                            lpPit->x4B++;
                            se_req(7, 0x13, 0);
                        }
                    }
                    break;
                case 1:
                    if ((u16)sw & 0x40) {
                        sw = (u16)(sw & 0xFFBF);
                        if (pl->item[lpPit->x49].num != 1 && Item_data[pl->item[lpPit->x49].id][3] != 0xFF) {
                            lpPit->x4B = 0;
                        } else {
                            menu_item_back_sub();
                        }
                    } else {
                        PitMenu.x12 = 1;
                        max = game_w.x1DC == 0 ? 3 : 7;
                        if ((u16)sw & 0x2000) {
                            FLDS8(*lpPit, 0x4C)--;
                            if (FLDS8(*lpPit, 0x4C) < 0) {
                                FLDS8(*lpPit, 0x4C) = max;
                            }
                            if (FLDS8(*lpPit, 0x4C) == game_w.master) {
                                FLDS8(*lpPit, 0x4C)--;
                            }
                            if (FLDS8(*lpPit, 0x4C) < 0) {
                                FLDS8(*lpPit, 0x4C) = max;
                            }
                            se_req(7, 0x16, 0);
                        } else if ((u16)sw & 0x1000) {
                            FLDS8(*lpPit, 0x4C)++;
                            if (max < FLDS8(*lpPit, 0x4C)) {
                                FLDS8(*lpPit, 0x4C) = 0;
                            }
                            if (FLDS8(*lpPit, 0x4C) == game_w.master) {
                                FLDS8(*lpPit, 0x4C)++;
                            }
                            if (max < FLDS8(*lpPit, 0x4C)) {
                                FLDS8(*lpPit, 0x4C) = 0;
                            }
                            se_req(7, 0x16, 0);
                        }
                        if ((u16)sw & 0x20) {
                            if (game_w.x1DC == 0) {
                                ok = game_w.pl_state[FLDS8(*lpPit, 0x4C)] == 1;
                            } else {
                                switch (func_5D8370(FLDS8(*lpPit, 0x4C))) {
                                case 0:
                                    ok = 1;
                                    break;
                                case 1:
                                case 2:
                                    ok = 0;
                                    break;
                                }
                            }
                            if (ok == 1) {
                                sw = 0;
                                FLDS8(*pl, 0x909) = FLDS8(*lpPit, 0x4C);
                                FLD16(*pl, 0x904) = pl->item[lpPit->x49].id;
                                FLDS16(*pl, 0x906) = lpPit->x4D;
                                FLDS8(*pl, 0x908) = 1;
                                lpPit->x4B++;
                            } else {
                                se_req(7, 0x15, 0);
                            }
                        }
                    }
                    break;
                case 2:
                    if (FLD8(*pl, 0x90B) != 0) {
                        if (game_w.x1DC == 0) {
                            menu_exit();
                        } else {
                            func_5B3E60();
                        }
                        se_req(7, 0x13, 0);
                    } else {
                        lpPit->x4B = 1;
                        se_req(7, 0x15, 0);
                    }
                    sw = 0;
                    break;
                }
                break;
            default:
                lpPit->x48 = 0;
                break;
            }
        }
        break;
    }
    return sw;
}

void Menu_data_i(void) {
    lpPit->x42 = 0;
    lpPit->x43 = 0;
    lpPit->x81 = 0;
    lpPit->x68 = 0;
    lpPit->x82 = 0;
    PitMenu.x10 = 1;
    PitMenu.x11 = 2;
    PitMenu.x12 = 0;
}

int Menu_data_mv(int sw) {
    int a;

    switch (lpPit->x42) {
    case 0:
        PitMenu.x10 = 1;
        ListSelect(&lpPit->x43, sw, 2);
        PitMenu.x12 = lpPit->x43;
        a = sw & 0xFFFF;
        if (!(a & 0x40) && (a & 0x20)) {
            switch (lpPit->x43) {
            case 0:
                lpPit->x68 = Item_preparation_list_search(&lpPit->x81, 0, &lpPit->x6C, &lpPit->x6E);
                break;
            case 1:
                lpPit->x82 = Monster_list_search(lpPit->x82, 0);
                break;
            }
            sw = (u16)(sw & 0x8000);
            lpPit->x42++;
            se_req(7, 0x13, 0);
        } else {
            break;
        }
    case 1:
        if ((u16)sw & 0x40) {
            sw = sw & 0xFFBF & 0xFFFF;
            PitMenu.x10 = 1;
            PitMenu.x11 = 2;
            PitMenu.x12 = lpPit->x43;
            lpPit->x42 = 0;
            se_req(7, 0x14, 0);
        } else {
            switch (lpPit->x43) {
            case 0:
                menu_data_mix_sub(sw);
                break;
            case 1:
                menu_data_monster_sub(sw);
                break;
            }
        }
    }
    return (u16)((u16)sw & 0x8040);
}

void menu_data_mix_sub(int sw) {
    int a = sw & 0xFFFF;
    s8 d;
    int p;

    if (a & 0xC00) {
        d = (a & 0x800) ? -1 : 1;
        p = Item_preparation_list_search(&lpPit->x81, d, &lpPit->x6C, &lpPit->x6E);
        if (lpPit->x68 != p) {
            lpPit->x68 = p;
            se_req(7, 0x16, 0);
        }
    }
    PitMenu.x10 = 1;
    PitMenu.x11 = 1;
    if (lpPit->x68 != 0) {
        PitMenu.x12 = *(s16 *)(lpPit->x68 + 2) + 0x18;
        return;
    }
    PitMenu.x12 = 0xFFFF;
}

void menu_data_monster_sub(int sw) {
    int a = sw & 0xFFFF;
    s8 m;
    int d;

    PitMenu.x10 = 0;
    if (FLD32(*User_data, 0x3F0) != 0) {
        if (a & 0xC00) {
            m = Monster_list_search(FLDS8(*lpPit, 0x82), !(a & 0x800) ? 1 : -1);
            if (FLDS8(*lpPit, 0x82) != m) {
                FLDS8(*lpPit, 0x82) = m;
                se_req(7, 0x16, 0);
            }
        }
    } else {
        FLDS8(*lpPit, 0x82) = -1;
    }
}

void Menu_status_i(void) {
    lpPit->x43 = 0;
    PitMenu.x10 = 0;
}

int Menu_status_mv(int sw) {
    if ((u16)sw & 0xC00) {
        lpPit->x43 = (lpPit->x43 + 1) & 1;
        se_req(7, 0x11, 0);
    }
    return (u16)((u16)sw & 0x8040);
}

u8 *menu_equip_get_equip(u8 no) {
    u16 idx;
    u8 *u = User_data;

    switch (no) {
    case 0:
        idx = u[0x456];
        break;
    case 1:
        idx = u[0x458];
        break;
    case 2:
        idx = u[0x459];
        break;
    case 3:
        idx = u[0x45A];
        break;
    case 4:
        idx = u[0x45B];
        break;
    case 5:
        idx = u[0x457];
        break;
    default:
        return 0;
    }
    if (idx == 0xFF) {
        return 0;
    }
    return &u[0x44 + idx * 6];
}

void Menu_equipment_i(void) {
    lpPit->x43 = 0;
    lpPit->x44 = 0;
    PitMenu.x10 = 0;
}

int Menu_equipment_mv(int sw) {
    PLW *pl = lpPit->pl;
    u8 sel;
    int job;

    sel = lpPit->x43;
    ListSelect(&sel, sw, 6);
    if (sel != lpPit->x43) {
        lpPit->x43 = sel;
        lpPit->x44 = 0;
    }
    if (menu_equip_get_equip(lpPit->x43) != 0) {
        lpPit->x45 = 2;
        if (lpPit->x43 == 0 && ((job = Get_weapon_job2(FLD8(*pl, 0x35F), FLD16(*pl, 0x360)) & 0xFF) == 1 || job == 5)) {
            lpPit->x45 = 4;
        }
        PageSelect(&lpPit->x44, sw, lpPit->x45);
    }
    return sw;
}

void menu_option_i(void) {
    lpPit->x43 = 0;
    PitMenu.x10 = 0;
}

#define OPT(i) (((i) + (u8 *)lpPit)[0x88])
int menu_option_mv(int sw) {
    u8 c;

    ListSelect(&lpPit->x43, sw, 5);
    if ((u16)sw & 0xC00) {
        c = lpPit->x43;
        switch (c) {
        case 1:
            if (lpPit->x88 != 0) {
                se_req(7, 0x15, 0);
            } else {
        case 0:
                OPT(c) ^= 1;
                se_req(7, 0x16, 0);
            }
            break;
        case 2:
        case 3:
            if ((u16)sw & 0x800) {
                if (OPT(c) == 0) {
                    OPT(c) = 2;
                } else {
                    OPT(c)--;
                }
            } else {
                OPT(c)++;
                if (OPT(lpPit->x43) >= 3) {
                    OPT(lpPit->x43) = 0;
                }
            }
            se_req(7, 0x16, 0);
            break;
        case 4:
            if (FLD16(Psw, 4) & 0xC00) {
                lpPit->x8C ^= 1;
                se_req(7, 0x16, 0);
            }
            break;
        }
    }
    c = lpPit->x8A;
    FLD8(option_w, 4) = c;
    GW8(0x1DD) = c;
    c = lpPit->x8B;
    FLD8(option_w, 7) = c;
    GW8(0xF) = c;
    c = lpPit->x8C;
    if (c != FLDS8(option_w, 3)) {
        FLDS8(option_w, 3) = c;
        if (FLDS8(option_w, 3) != 0) {
            vib_set(0, 1);
            vib_set(1, 1);
        }
    }
    return (u16)((u16)sw & 0x8040);
}

#undef OPT
int ItemPickingDeclaration(int arg, s16 *code) {
    u16 c;

    if (arg != 0 && Pl_master_ck() == 0) {
        return -1;
    }
    if (lpPit->x07 != 0) {
        return -1;
    }
    if (item_pick_declaration_timer <= 0) {
        c = item_pick_declaration_code + 1;
        item_pick_declaration_code = c;
        *code = c;
        item_pick_declaration_timer = 4;
        return 0;
    }
    if ((s16)*code == item_pick_declaration_code) {
        item_pick_declaration_timer = 4;
        return 0;
    }
    return -1;
}

int ItemStockRequest(PLW *pl, int id, int code, int flags) {
    int r;
    int t;

    if (lpPit->x07 != 0) {
        return -1;
    }
    if (item_pick_declaration_timer > 0 && (s16)code != item_pick_declaration_code) {
        return -1;
    }
    r = Pl_item_stack(pl, id, 1) & 0xFFFF;
    lpPit->x52 = 0;
    switch (r) {
    case 0:
    case 1:
        t = flags & 0xFF;
        if (t & 2) {
            Pl_item_get_se(pl, id);
        }
        if (t & 1) {
            set01_set(1, 0, id);
        }
        break;
    case 2:
    case 3:
        if ((u8)flags & 1) {
            set01_set(1, 3, id);
        }
        break;
    case 5:
        if (Check_hold_item(id) & 0xFF) {
            if ((u8)flags & 1) {
                set01_set(0, 12, 0);
            }
            return 5;
        }
        lpPit->x52 = id;
        lpPit->x07 = 1;
        lpPit->x50 = id;
        if ((u8)flags & 2) {
            Pl_item_get_se(pl, id);
        }
        lpPit->x54 = 0xFF;
        break;
    }
    return (s16)r;
}

/* Item box / stock request window: state x07 (0 idle, 1 wait for the info
 * message, 2-3 open, 4 pick, 5 erase confirm, 6 close). */
int item_stock_mv(int sw) {
    PLW *pl;
    PIT_W *p = lpPit;
    u8 *st = &lpPit->x07;

    switch (*st) {
    case 0:
        if (item_pick_declaration_timer != 0) {
            item_pick_declaration_timer--;
        }
        return 0;
    case 1:
        if (p->x54 == 0xFF) {
            lpPit->x54 = set01_set(1, 0, p->x52);
            return 0;
        }
        if (p->x54 == game_w.info_now) {
            (*st)++;
            lpPit->x55 = 6;
            return 1;
        }
        if (Info_stack_ck() == 0) {
            lpPit->x07++;
            return 1;
        }
        return 0;
    case 2:
        if (p->x55 != 0) {
            p->x55--;
        }
        if (lpPit->x54 == game_w.info_now) {
            return 1;
        }
        lpPit->x55 = 0;
        lpPit->x4F = 0;
        lpPit->x07++;
    case 3:
        lpPit->x55 = (lpPit->x55 + 1) & 0x7F;
        if (lpPit->x55 < 0x40) {
            lpPit->x50 = 0x14;
        } else {
            lpPit->x50 = (u16)lpPit->x52 + 0x18;
        }
        sw = sw & 0xFFFF;
        Menu_select_mv(&lpPit->x49, sw & 0xC00, 0x14);
        if (sw & 0x3000) {
            lpPit->x4F = (lpPit->x4F + 1) & 1;
            se_req(7, 0x16, 0);
        }
        if (sw & 0x20) {
            if (lpPit->x4F == 0) {
                lpPit->x50 = 0x15;
                lpPit->x56 = 0;
                lpPit->x07++;
            } else if (lpPit->x4F == 1) {
                lpPit->x07 = 6;
            }
            se_req(7, 0x13, 0);
        }
        break;
    case 4:
        if (lpPit->x56 != 0) {
            if ((u16)sw & 0x240) {
                lpPit->x56 = 0;
                se_req(7, 0x14, 0);
            }
            sw = sw & 0xFFBF & 0xFFFF;
        } else if ((u16)sw & 0x200) {
            lpPit->x56 = 1;
            se_req(7, 9, 0);
        }
        Menu_select_mv(&lpPit->x49, sw, 0x14);
        if ((u16)sw & 0x20) {
            lpPit->x50 = 0x16;
            lpPit->x56 = 0;
            lpPit->yn = 1;
            lpPit->x07++;
            se_req(7, 0x13, 0);
        } else if ((u16)sw & 0x40) {
            lpPit->x50 = 0x14;
            lpPit->x55 = 0;
            lpPit->x07 = 3;
            se_req(7, 0x14, 0);
        } else if (lpPit->x56 == 0) {
            lpPit->x50 = 0x15;
        } else {
            lpPit->x50 = get_pl_item_type(lpPit->x49) + 0x18;
        }
        break;
    case 5:
        lpPit->x50 = 0x16;
        select_yes_no(sw, 0x3000);
        if ((u16)sw & 0x20) {
            if (lpPit->yn == 0) {
                pl = lpPit->pl;
                Pl_item_erase(pl, lpPit->x49);
                Pl_item_stack(pl, lpPit->x52, 1);
                lpPit->x07 = 6;
                se_req(7, 0x13, 0);
            } else {
                lpPit->x07 = 4;
                se_req(7, 0x14, 0);
            }
        } else if ((u16)sw & 0x40) {
            lpPit->x07 = 4;
            se_req(7, 0x14, 0);
        }
        break;
    case 6:
        *st = 0;
        return 0;
    }
    return 1;
}

int lb_item_stock_mv(int sw) {
    int r = item_stock_mv(sw);

    if (r != 0) {
        ItemCopy_Pl2Ud(lpPit->pl);
    }
    return r;
}

void map_move(int sw, u16 hold) {
    if (lpPit->lb == 0 && lpPit->x83 == 0 && ((u16)sw & 0x4000)) {
        lpPit->x3E ^= 1;
    }
}

#define SIGN(o) (*(s16 *)((u8 *)lpPit + (o) + 0x34))
void map_sign_move(int sw) {
    int i;
    int o;

    o = 0;
    for (i = 0; i < 4; i++, o += 2) {
        if (SIGN(o) < 100 && SIGN(o) % 20 == 0) {
            se_req(1, SIGN(o) / 20 + 0x7B, 0);
        }
        if (SIGN(o) >= 0) {
            SIGN(o)++;
        }
        if (SIGN(o) > 105) {
            SIGN(o) = -1;
        }
    }
    if ((((s16 *)((u8 *)lpPit + 0x34))[game_w.master]) < 0 && ((u16)sw & 2)) {
        lpPit->x3C--;
        if (lpPit->x3C <= 0) {
            (((s16 *)((u8 *)lpPit + 0x34))[game_w.master]) = 0;
            net_send_sys(9, game_w.master);
        }
        return;
    }
    lpPit->x3C = 10;
}
#undef SIGN

void MapSignRequest(int no) {
    (&lpPit->x34)[no] = 0;
}

void Item_box_get_efct(EMW *em) {
    lpPit->x60 = 0x1F;
    lpPit->x62 = FLD8(*em, 0x8C3);
}

void Item_box_get_item(s16 id, u8 slot) {
    f32 dx;
    f32 dy;
    u8 d;

    lpPit->x66 = id;
    lpPit->x65 = slot;
    dx = 0.8f * (313.0f + 36.0f * (lpPit->x65 & 7)) - 252.0f;
    dy = (lpPit->x65 >> 3) * 32 - 162;
    d = (u32)(0.1f * flSqrt(dx * dx + dy * dy));
    lpPit->x64 = d;
    lpPit->x63 = d;
}

int Menu_chatcnfg_i(void) {
    if (Online_ck() == 0) {
        return -1;
    }
    lpPit->x7D = 0;
    lpPit->x7E = 0;
    PitMenu.x10 = 1;
    PitMenu.x11 = 3;
    PitMenu.x12 = lpPit->x7D;
    PitMenu.x1B = 0;
    return 0;
}

int Menu_chatcnfg_mv(int sw) {
    int r;
    int t;

    switch (lpPit->x7E) {
    case 0:
        t = sw & 0xFFFF;
        if (t & 0x3000) {
            lpPit->x7D = (lpPit->x7D + 1) & 1;
            se_req(7, 0x16, 0);
        }
        r = sw & 0xFFFF;
        PitMenu.x12 = lpPit->x7D;
        if (t & 0x20) {
            lpPit->x7E++;
            lpPit->x7F = 0;
            lpPit->x80 = -1;
            se_req(7, 0x13, 0);
        }
        break;
    case 1:
        switch (lpPit->x7D) {
        case 0:
            r = menu_chcnfg_sendpl(sw) & 0xFFFF;
            break;
        case 1:
            r = menu_chcnfg_reibun(sw) & 0xFFFF;
            break;
        }
        if (r & 0x40) {
            r = r & 0xFFBF & 0xFFFF;
            lpPit->x7E = 0;
            se_req(7, 0x14, 0);
        }
        break;
    }
    return r;
}

int menu_chcnfg_sendpl(int sw) {
    int a = sw & 0xFFFF;
    s8 n;
    int mask;
    int i;
    int bit;
    int v;

    PitMenu.x18 = 0;
    if (a & 0x40) {
        return sw;
    }
    n = 4;
    if (game_w.x1DC == 0) {
        mask = 0xF;
    } else {
        n = 8;
        mask = 0xFF;
    }
    PitMenu.x12 = 2;
    if (a & 0x2000) {
        if (lpPit->x80 < 0) {
            lpPit->x80 = n;
        }
        lpPit->x80--;
        if (game_w.master == lpPit->x80) {
            lpPit->x80--;
        }
        se_req(7, 0x16, 0);
    } else if (a & 0x1000) {
        lpPit->x80++;
        if (game_w.master == lpPit->x80) {
            lpPit->x80++;
        }
        if ((u32)lpPit->x80 >= (u32)n) {
            lpPit->x80 = -1;
        }
        se_req(7, 0x16, 0);
    }
    if (a & 0x20) {
        s8 sel = lpPit->x80;

        if (sel < 0) {
            PitMenu.x16 = 0;
            PitMenu.x15 = 1;
            se_req(7, 0x13, 0);
        } else {
            if (game_w.x1DC == 0) {
                v = game_w.pl_state[sel] ^ 1;
            } else {
                v = func_5D8370(sel) ^ 0;
            }
            if ((v == 0) == 1) {
                PitMenu.x15 = 0;
                PitMenu.x16 ^= (1 << lpPit->x80) & 0xFF;
                PitMenu.x16 &= ((mask & 0xFF) - (1 << game_w.master)) & 0xFF;
                if (PitMenu.x16 == 0) {
                    PitMenu.x15 = 1;
                }
                se_req(7, 0x13, 0);
            } else {
                se_req(7, 0x15, 0);
            }
        }
    }
    if (n != 0) {
        for (i = 0; i < (u32)n; i++) {
            bit = 1 << i;
            if (PitMenu.x16 & bit) {
                if (game_w.x1DC == 0) {
                    if (game_w.pl_state[i] == 1) {
                        continue;
                    }
                } else if (func_5D8370((s8)i) == 0) {
                    continue;
                }
                PitMenu.x16 &= ((mask & 0xFF) - bit) & 0xFF;
            }
        }
    }
    if (PitMenu.x16 == 0) {
        PitMenu.x15 = 1;
    }
    if (PitMenu.x15 != 0) {
        PitMenu.x17 = 3;
    } else {
        v = ((PitMenu.x16 & 0x55) + ((PitMenu.x16 & 0xAA) >> 1)) & 0xFF;
        v = ((v & 0x33) + ((v & 0xCC) >> 2)) & 0xFF;
        if ((((v & 0xF) + ((v & 0xF0) >> 4)) & 0xFF) >= 2) {
            PitMenu.x17 = 2;
        } else {
            PitMenu.x17 = 1;
        }
    }
    return sw;
}

int menu_chcnfg_reibun(int sw) {
    int a;
    int r = sw;

    switch (lpPit->x7F) {
    case 0:
        a = r & 0xFFFF;
        if (!(a & 0x40)) {
            PitMenu.x12 = 3;
            PitMenu.x1B = Reibun_select_mv(PitMenu.x1B);
            if ((a & 0x20) && Reibun_Edit_Start(PitMenu.x1B) == 1) {
                lpPit->x7F++;
            }
        }
        break;
    case 1:
        PitMenu.x12 = 4;
        r = r & 0x7FBF & 0xFFFF;
        if ((s8)Reibun_Edit_Core(PitMenu.x1B) != 0) {
            lpPit->x7F = 0;
        }
        break;
    }
    return r;
}

void Pit_disp_menu_status(void) {
    PlayerStatusWindow(lpPit->pl, lpPit->x43);
}

void disp_retire(void) {
    DispFrameMessage(frame_retire, retire_str);
    flfntLocate(0x18F, 0x106);
    font_print_sp(retire_yesno_str[lpPit->yn]);
}

void wyvern_area(f32 *x, f32 *y, f32 *z, int no) {
    *x = wyvern_area_tbl[no][0];
    *y = wyvern_area_tbl[no][1];
    *z = wyvern_area_tbl[no][2];
}

void DispWholeMap(void) {
    PFLPS q;

    SetFilterMode(1);
    reload_tex(1, 0x156);
    SetTextureStage(0x156);
    q.s[2] = 0x200;
    q.s[0] = 0;
    q.s[3] = 0x1C0;
    q.s[1] = 0;
    q.b = 0;
    q.c = 0xE000FF;
    q.a = 0xFF606060;
    flps0008(&q);
    disp_whole_map(0x40, 160.0f, 1.0f);
}

void WyvernAreaMove(PLW *em) {
    if (enemy_mark_chk(lpPit->pl, (EMW *)em) != 0 && lpPit->x05 == 0 && lpPit->lb == 0 && lpPit->x83 == 0) {
        lpPit->x3F = 0x1A;
    }
}

void trans_pit_0(void) {
    flSetRenderState(0x60, 0);
    SetTrnslMode(4, 5);
    font_set_stack_no(0);
    if (lpPit->x83 == 0) {
        disp_timer();
        disp_pl_vital();
        disp_slash_level();
        disp_pachinger();
        disp_cannon();
        disp_others_info();
        disp_name();
        return;
    }
    disp_pachinger();
    if (lpPit->x2A == 0) {
        disp_cannon();
    }
}

void trans_pit_1_lb(void) {
    flSetRenderState(0x60, 0);
    SetTrnslMode(4, 5);
    font_set_stack_no(1);
    if (PitMenu.open != 0) {
        Pit_disp_chat();
        return;
    }
    if (lpPit->x07 != 0) {
        disp_item_stock();
    } else if (GW8(0xE) != 0) {
        func_5B4980();
    }
    func_60CE50();
}

void trans_pit_2_lb(void) {
    font_set_stack_no(2);
    if (PitMenu.x06 != 0) {
        Disp_NPC_message();
        PitMenu.x06 = 0;
    } else {
        Pit_disp_receive_mes();
    }
    if (PitMenu.open == 0) {
        Pit_disp_pit_effect();
    }
    if ((s8)SoftKeyboard_alive_check() != 0) {
        SetTrnslMode(4, 5);
        DispSoftkeyboard(FLD8(system_w, 0x31));
    }
}

void trans_pit_1(void) {
    u8 c;

    SetTrnslMode(4, 5);
    font_set_stack_no(1);
    if (game_w.x1E7 != 0) {
        func_63B470();
    }
    if (PitMenu.open != 0) {
        if (game_w.x1E7 == 0) {
            Pit_disp_chat();
            disp_map();
        }
    } else {
        if (lpPit->x07 != 0) {
            disp_item_stock();
            return;
        }
        if (lpPit->x05 != 0) {
            c = lpPit->x40;
            switch (c) {
            case 0:
                disp_menu(0, lpPit);
                break;
            case 1:
                disp_menu_jmp[lpPit->x41](c, lpPit);
                break;
            }
            if (PitMenu.x10 != 0) {
                Disp_menu_help();
            }
        } else {
            disp_map();
            if (FLD8(*lpPit->pl, 0x8C2) == 0) {
                disp_item();
            }
        }
    }
}

void trans_pit_2(void) {
    font_set_stack_no(2);
    disp_item_sub_select_ex();
    if (PitMenu.open == 0 && lpPit->x07 == 0 && lpPit->x05 == 0) {
        trans_box();
    }
    if (PitMenu.x06 != 0) {
        Disp_NPC_message();
        PitMenu.x06 = 0;
    } else {
        Pit_disp_receive_mes();
    }
    if (PitMenu.open == 0) {
        Pit_disp_pit_effect();
    }
    if ((s8)SoftKeyboard_alive_check() != 0) {
        SetTrnslMode(4, 5);
        DispSoftkeyboard(FLD8(system_w, 0x31));
    }
}

/* Colour of a boss icon on the map: pulses with System_timer. */
u32 boss_icon_color(EMW *em) {
    u32 t;
    int c;
    u32 r;

    if (em->mode == 5) {
        t = (*(u8 *)&System_timer << 8) & 0xFFFF;
        c = ((s8)(int)(32.0f * flSin(0.0000958738f * (f32)t)) + 0x80) & 0xFF;
        r = c | ((c << 16) | 0xC0000000 | (c << 8));
    } else if (FLD8(*em, 0x888) == 1) {
        t = ((System_timer & 0xF) << 12) & 0xFFFF;
        r = (((s8)(int)(64.0f * flSin(0.0000958738f * (f32)t)) + 0x48) << 8) | 0xFFF00000;
    } else {
        t = ((System_timer & 0x3F) << 10) & 0xFFFF;
        r = (((s8)(int)(48.0f * flSin(0.0000958738f * (f32)t)) + 0x80) << 8) | 0xFF1000E0;
    }
    return r;
}

void maru_disp_sub(int col, f32 x, f32 y, f32 r) {
    PFLPS q;

    SetFilterMode(1);
    q.a = col;
    q.c = 0x1000020;
    q.b = 0xF00010;
    q.s[0] = 0.8f * (x - r);
    q.s[1] = y - r;
    q.s[2] = 0.8f * (2.0f * r);
    q.s[3] = 2.0f * r;
    flps0008(&q);
}

void camp_disp_sub(f32 x, f32 y) {
    PFLPS q;

    reload_tex(1, 0x119);
    SetTextureStage(0x119);
    SetFilterMode(0);
    q.b = 0xF00020;
    q.a = -1;
    q.c = 0xFF002F;
    q.s[0] = 0.8f * (x - 8.0f);
    q.s[1] = y - 8.0f;
    q.s[2] = 12.8f;
    q.s[3] = 16.0f;
    flps0008(&q);
}

void Pit_mv_lb(void) {
    int sw;
    int r;
    int now;

    now = FLD16(Psw, 4);
    sw = (now | pit_key_repeat(now, FLD16(Psw, 0))) & 0xFFFF;
    switch (lpPit->x04) {
    case 0:
        lpPit->x04++;
        FLDS8(*lpPit, 0) = 1;
        GWS8(0xE) = 0;
    case 1:
        if (Online_ck() == 1) {
            Join_pl_chk();
            FLDS8(PitMenu, 0x22) = 0;
        }
        switch (PitMenu.open) {
        case 0:
            if (PitMenu.x06 == 0) {
                if (Online_ck() == 1 && softkey_ck() == 1 && ((r = kb_chat_in_chk(), ((u16)sw & 0x100) != 0) || r == 1)) {
                    Chat_init();
                } else {
                    if (lb_item_stock_mv(sw) == 0) {
                        func_5B3ED0(sw);
                    }
                    Pit_effect_move();
                }
            }
            break;
        case 1:
            Chat_move(sw);
            break;
        }
        break;
    }
    Receive_mess_move();
    add_prim2(&ot6, &pit_prim[1], 0, 1);
    add_prim2(&ot7, &pit_prim[2], 0, 1);
}

/* Per-frame pit menu: HP/stamina bars, item stock window, chat, main menu. */
void Pit_mv(void) {
    PLW *pl;
    int now;
    int hold;
    int sw;
    u32 i;
    int r;

    if (GW8(0x21F) != 0) {
        return;
    }
    pl = lpPit->pl;
    if (Game_clear_ck(1) == 1) {
        if (lpPit->x8D != 0) {
            for (i = 0; i < 2; i++) {
                u16 id = (&lpPit->x6C)[i];

                if (id != 0xFFFF) {
                    Pl_item_stack(pl, id, 1);
                }
            }
            lpPit->x8D = 0;
        }
        lpPit->x05 = 0;
        lpPit->x06 = 0;
        lpPit->x07 = 0;
        lpPit->x40 = 0;
        GWS8(0xE) = 0;
        PitMenu.open = 0;
        PitMenu.x18 = 0;
        return;
    }
    now = FLD16(Psw, 4);
    hold = FLD16(Psw, 0);
    sw = (now | pit_key_repeat(now, hold)) & 0xFFFF;
    switch (lpPit->x04) {
    case 0:
        lpPit->x04++;
        FLDS8(*lpPit, 0) = 1;
        GWS8(0xE) = 0;
        lpPit->x24 = pl->vital;
        lpPit->x26 = pl->vital_red;
    case 1:
        if (lpPit->x24 != pl->vital) {
            if (lpPit->x24 < pl->vital) {
                lpPit->x24 = lpPit->x24 + 1;
            } else {
                lpPit->x24 = lpPit->x24 - 1;
            }
        }
        lpPit->x26 = pl->vital_red;
        lpPit->x57 = 0;
        if (lpPit->x5A != pl->work888) {
            lpPit->x5A = pl->work888;
            if (UseItemChk(pl, pl->work888) == 1) {
                lpPit->x58 = 2;
                if (pl->work8F2 & 4) {
                    lpPit->x59 = -1;
                } else {
                    lpPit->x59 = 1;
                }
            }
        }
        if (pl->kind == 1 || pl->kind == 5) {
            if (lpPit->x5E != pl->work88E) {
                lpPit->x5E = pl->work88E;
                if (pl->work88E != 0xFF) {
                    lpPit->x5C = 2;
                    if (pl->work8F2 & 0x10) {
                        lpPit->x5D = -1;
                    } else {
                        lpPit->x5D = 1;
                    }
                }
            }
        }
        lpPit->x2B = 0;
        if (Pit_shot_ok_chk(pl) == 1) {
            if (lpPit->x28 >= 4) {
                lpPit->x2B = 1;
            }
            if (lpPit->x28 != 0) {
                lpPit->x28--;
            }
        } else {
            lpPit->x28 = 4;
        }
        lpPit->x83 = 0;
        if (pl->work88C == 0) {
            lpPit->x83 = lpPit->x88;
        }
        if (lpPit->x3F != 0) {
            lpPit->x3F--;
        }
        map_sign_move(hold);
        if (lpPit->x60 > 0) {
            lpPit->x60--;
        }
        if (lpPit->x64 > 0) {
            lpPit->x64--;
        }
        if (Online_ck() == 1) {
            Join_pl_chk();
            FLDS8(PitMenu, 0x22) = 0;
        }
        switch (PitMenu.open) {
        case 0:
            if (game_w.x1E7 == 0) {
                if (pl->work88C == 0 && pl->x8C6 == 0 && Online_ck() == 1 && softkey_ck() == 1 &&
                    ((r = kb_chat_in_chk(), ((u16)sw & 0x100) != 0) || r == 1)) {
                    Chat_init();
                    map_move(sw, hold);
                    break;
                }
            } else if (func_63B0C0(sw) & 0xFF) {
                PitMenu.open++;
                break;
            }
            if (item_stock_mv(sw) == 0) {
                switch (lpPit->x05) {
                case 0:
                    if (pl->x8C6 == 0 && ((u16)sw & 0x8000)) {
                        menu_init();
                        se_req(7, 0x11, 0);
                    } else {
                        if (lpPit->x06 != 0) {
                            Pl_box_select(pl);
                        } else {
                            map_move(sw, hold);
                        }
                        lpPit->x06 = pl->work8C2;
                    }
                    break;
                case 1:
                    menu_move(sw);
                    break;
                }
            }
            Pit_effect_move();
            break;
        case 1:
            if (game_w.x1E7 == 0) {
                Chat_move(sw);
                map_move(sw, hold);
            } else if (!(func_63B0C0(sw) & 0xFF)) {
                PitMenu.open = 0;
            }
            break;
        }
        break;
    }
    Receive_mess_move();
    add_prim2(&ot5, &pit_prim[0], 0, 1);
    add_prim2(&ot6, &pit_prim[1], 0, 1);
    add_prim2(&ot7, &pit_prim[2], 0, 1);
}

#define MIX70 FLD8(*lpPit, 0x70)
#define MIX71 FLD8(*lpPit, 0x71)
/* Item mixing window: state x7A 0 pick first item, 1 pick second, 2 confirm,
 * 3 waiting, 4 result, 5 show result, 6 re-check items. */
int Menu_mix_mv(int sw) {
    int r = (u16)sw;
    PLW *pl = lpPit->pl;
    int n;

    if (lpPit->x49 >= 0x14) {
        lpPit->x49 = 0;
    }
    switch (lpPit->x7A) {
    case 0:
        PitMenu.x10 = 0;
        Menu_select_mv(&lpPit->x49, sw, 0x14);
        if ((u16)sw & 0x20) {
            if (mix_item_chk(lpPit->x49, pl->item[lpPit->x49].id) == 1) {
                MIX70 = lpPit->x49;
                lpPit->x6C = pl->item[MIX70].id;
                lpPit->x6E = 0xFFFF;
                se_req(7, 0x13, 0);
                lpPit->x7A++;
            } else {
                se_req(7, 0x15, 0);
            }
        }
        break;
    case 1:
        PitMenu.x10 = 0;
        r = (u16)(r & 0xFFBF);
        if (mix_item_chk(MIX70, lpPit->x6C) == 1) {
            Menu_select_mv(&lpPit->x49, sw, 0x14);
            if ((u16)sw & 0x40) {
                se_req(7, 0x14, 0);
            } else {
                if ((u16)sw & 0x20) {
                    if (mix_item_2_chk(lpPit->x49, MIX70) == 1) {
                        se_req(7, 0x13, 0);
                        MIX71 = lpPit->x49;
                        lpPit->x6E = pl->item[MIX71].id;
                        lpPit->x7B = 0;
                        lpPit->x7A++;
                    } else {
                        se_req(7, 0x15, 0);
                    }
                }
                break;
            }
        }
        MIX70 = 0xFF;
        lpPit->x6C = 0xFFFF;
        lpPit->x7A = 0;
        break;
    case 2:
        PitMenu.x10 = 1;
        ListSelect(&lpPit->x7B, sw, 2);
        sw = (u16)sw;
        Menu_select_mv(&lpPit->x49, sw & 0xC00, 0x14);
        lpPit->x7C = 0;
        if (mix_item_chk(MIX70, lpPit->x6C) == 0) {
            MIX70 = 0xFF;
            lpPit->x7C |= 4;
        }
        if (mix_item_chk(MIX71, lpPit->x6E) == 0) {
            MIX71 = 0xFF;
            lpPit->x7C |= 4;
        }
        if (lpPit->x7C == 0) {
            if (Item_preparation_list_chk(lpPit->x6C, lpPit->x6E)) {
                n = (s16)Pl_item_num_ck3(pl, lpPit->x74);
                if (n == 0) {
                    lpPit->x7C |= 1;
                    PitMenu.x12 = 12;
                } else if (n < 0) {
                    if (pl->item[MIX70].num >= 2 && pl->item[MIX71].num >= 2) {
                        lpPit->x7C |= 2;
                        PitMenu.x12 = 13;
                    }
                }
            }
        } else {
            PitMenu.x12 = 11;
        }
        if (lpPit->x7C == 0) {
            if (lpPit->x74 == -1) {
                PitMenu.x12 = 10;
            } else {
                PitMenu.x12 = lpPit->x74 + 0x18;
            }
        } else {
            lpPit->x7B = 1;
        }
        if (sw & 0x20) {
            r = (u16)(r & 0x7FBF);
            switch (lpPit->x7B) {
            case 0:
                if (lpPit->x7C == 0) {
                    se_req(7, 0xF, 0);
                    lpPit->x7A++;
                    lpPit->x72 = 30;
                    lpPit->x78 = Item_preparation(pl, pl->item[MIX70].id, pl->item[MIX71].id, 0);
                    mix_effect_set(0);
                    lpPit->x8D = 1;
                } else {
                    se_req(7, 0x15, 0);
                }
                break;
            case 1:
                menu_mix_clear();
                return 0x40;
            }
        } else if (sw & 0x8000) {
            menu_mix_clear();
            se_req(7, 0x14, 0);
            return 0x8000;
        } else if (sw & 0x40) {
            r = (u16)(r & 0xFFBF);
            if (MIX71 < 0x14) {
                lpPit->x49 = MIX71;
            } else {
                lpPit->x49 = 0;
            }
            MIX71 = 0xFF;
            lpPit->x6E = 0xFFFF;
            lpPit->x74 = -1;
            lpPit->x7A = 1;
            se_req(7, 0x14, 0);
        }
        break;
    case 3:
        r = (u16)(r & 0x7FBF);
        lpPit->x72--;
        if (lpPit->x72 > 0) {
            break;
        }
        if (lpPit->x78 > 0) {
            Add_to_Item_preparation_list_0(lpPit->x68);
            lpPit->x74 = lpPit->x78;
            mix_effect_set(1);
            se_req(7, 0xE, 0);
        } else {
            lpPit->x78 = 0x8F;
            mix_effect_set(2);
            se_req(7, 0xD, 0);
        }
        if (pl->item[MIX70].id == 0) {
            MIX70 = 0xFF;
        }
        if (pl->item[MIX71].id == 0) {
            MIX71 = 0xFF;
        }
        PitMenu.x12 = lpPit->x78 + 0x18;
        lpPit->x7A++;
    case 4:
        switch ((s16)ItemStockRequest(pl, (u16)lpPit->x78, lpPit->x76, 1)) {
        case 0:
        case 1:
            if (lpPit->x78 == 0x8F) {
                adx_se_set(pl, 8);
            } else {
                Pl_item_get_se(pl, (u16)lpPit->x78);
            }
        case 2:
        case 3:
            lpPit->x72 = 30;
            lpPit->x7A++;
            lpPit->x8D = 0;
            break;
        case 5:
            lpPit->x49 = 0;
            lpPit->x7A = 6;
            lpPit->x8D = 0;
            break;
        }
        break;
    case 5:
        r = (u16)(r & 0xFFBF);
        if (lpPit->x72 != 0) {
            lpPit->x72--;
            break;
        }
        if ((u16)sw & 0x3FFF) {
            lpPit->x7A = 2;
        }
        break;
    case 6:
        r = (u16)(r & 0xFFBF);
        if (lpPit->x07 == 0) {
            if (lpPit->x6C != pl->item[MIX70].id) {
                MIX70 = 0xFF;
            } else if (lpPit->x6E != pl->item[MIX71].id) {
                MIX71 = 0xFF;
            }
            lpPit->x7A = 2;
        }
        break;
    }
    return r;
}
#undef MIX70
#undef MIX71

/* Main pit menu: x40 0 = choose entry, 1 = run the entry's move function. */
void menu_move(int sw) {
    int r;
    int a;

    switch (lpPit->x40) {
    case 0:
        Menu_select_mv(&lpPit->x41, sw, 10);
        PitMenu.x12 = lpPit->x41;
        if (Online_ck() == 1) {
            if (((u16)sw & 0x200) != 0) {
                Name_ID_change();
            }
            if (lpPit->x41 == 9) {
                PitMenu.x12 = 10;
            }
        }
        a = (u16)sw;
        if (a & 0x20) {
            r = 0;
            switch (lpPit->x41) {
            case 0:
                Menu_item_i();
                break;
            case 1:
                r = Menu_mix_i();
                break;
            case 2:
                Menu_data_i();
                break;
            case 3:
                Menu_quest_i();
                break;
            case 4:
                menu_option_i();
                break;
            case 5:
                Menu_status_i();
                break;
            case 6:
                Menu_equipment_i();
                break;
            case 7:
                r = Menu_chatcnfg_i();
                break;
            case 8:
                r = Menu_chatlog_i();
                break;
            case 9:
                r = menu_retire_i();
                break;
            default:
                r = 1;
                break;
            }
            if (r == 0) {
                lpPit->x40++;
                lpPit->x48 = 0;
                se_req(7, 0x13, 0);
            } else {
                se_req(7, 0x15, 0);
            }
        } else if (a & 0x8040) {
            menu_exit();
            se_req(7, 0x14, 0);
        }
        break;
    case 1:
        r = (u16)menu_mv_jmp[lpPit->x41]((u16)sw);
        if (r & 0x8000) {
            menu_exit();
            se_req(7, 0x14, 0);
        } else if (r & 0x40) {
            menu_init();
            se_req(7, 0x14, 0);
        }
        break;
    }
}

/* Ripple ring of a map sign: circle at (x, y) fading with t. */
void efct_circle(int x, int y, int color, f32 scale, f32 t) {
    PFLPS q;
    u8 alpha;
    f32 r;
    f32 w;

    alpha = (u32)(51.0f * flSqrt(t));
    r = flSqrt(25.0f - t);
    q.a = (color & 0xFFFFFF) | (alpha << 24);
    w = scale * (0.291667f + 0.1180555f * r);
    q.s[2] = 0.8f * w;
    q.s[3] = w;
    q.s[0] = x;
    q.s[1] = y;
    q.b = 0xE100E1;
    q.c = 0x1000100;
    flps0008(&q);
    q.s[1] = (s16)y - q.s[3];
    ((s16 *)&q.b)[1] = 0x100;
    ((s16 *)&q.c)[1] = 0xE1;
    flps0008(&q);
    q.s[0] = (s16)x - q.s[2];
    ((s16 *)&q.b)[0] = 0x100;
    ((s16 *)&q.c)[0] = 0xE1;
    flps0008(&q);
    q.s[1] = y;
    ((s16 *)&q.b)[1] = 0xE1;
    ((s16 *)&q.c)[1] = 0x100;
    flps0008(&q);
}

void disp_map_sign(int x, int y, s16 timer, int color) {
    int m;

    if (timer < 20) {
        efct_circle(x, y, color, 40.0f, 25 - timer);
    } else if (timer < 85) {
        m = (s16)(timer % 20);
        efct_circle(x, y, color, 40.0f, 25 - m);
        if (5 - m >= 0) {
            efct_circle(x, y, color, 40.0f, 5 - m);
        }
    } else {
        efct_circle(x, y, color, 40.0f, 105 - timer);
    }
}

/* Draws one monster icon on the map at (x, y); bosses and small icons are
 * filled circles, the rest a rotated textured quad. */
void enemy_on_map(EMW *em, f32 x, f32 y, f32 scale) {
    u8 c;
    f32 r;
    u32 col;
    s16 u;
    s16 u2;
    f32 v[3];
    PFLP12 q;
    FLMAT m;
    f32 out[4][3];
    f32 step;
    f32 lo;
    f32 t;
    s16 i;
    u32 ang;

    c = enemy_icon_tbl[em->kind];
    if (c == 0xFF) {
        return;
    }
    if (c & 0x80) {
        if (c & 0x40) {
            r = 5.0f * scale;
            col = boss_icon_color(em);
        } else {
            r = 3.0f;
            col = enemy_icon_color[c & 0xF];
        }
        maru_disp_sub(col, x, y, r);
        return;
    }
    x *= 0.8f;
    SetFilterMode(0);
    if (FLD8(*em, 0x388) != 2) {
        c |= 4;
    }
    u = enemy_icon_tex_u[c];
    u2 = u + 0x10;
    flmatInit(&m);
    ang = (0x18000 - em->ang[1]) & 0xFFFF;
    flmatSetZYX33(0.0f, 0.0f, 2.0f * (3.1415927f * (((360.0f * (f32)ang) / 65536.0f) / 360.0f)), &m);
    v[2] = 0.0f;
    t = 12.8f * scale;
    lo = -t;
    step = 2.0f * t;
    for (i = 0; i < 4; i++) {
        v[0] = lo + step * (f32)(i / 2);
        v[1] = lo + step * (f32)(i & 1);
        flvecApplyMat33(out[i], v, &m);
        out[i][0] = x + 0.8f * out[i][0];
        out[i][1] = out[i][1] + y;
    }
    q.p[0] = out[0][0];
    q.p[1] = out[0][1];
    q.p[2] = out[1][0];
    q.p[3] = out[1][1];
    q.p[4] = out[2][0];
    q.p[5] = out[2][1];
    q.uv[0] = u;
    q.uv[1] = 0xF0;
    q.uv[3] = 0x100;
    q.uv[5] = 0xF0;
    q.uv[2] = u;
    q.uv[4] = u2;
    q.col = boss_icon_color(em);
    flps000C(&q);
    q.p[0] = out[3][0];
    q.p[1] = out[3][1];
    q.uv[0] = u2;
    q.uv[1] = 0x100;
    flps000C(&q);
}
