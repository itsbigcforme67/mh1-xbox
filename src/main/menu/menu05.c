/* f_menu run (SLPM_654.95 0x00128900-0x00129470): juchu_chk .. Menu_item_mv. Whole file in menu_nm.c. */
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
