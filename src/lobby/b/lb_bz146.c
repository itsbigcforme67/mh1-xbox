/* lb_bz146 - lobby UI/client 0x005B4ED0-0x005B4EF0: connecting_proc (first drafted by tools/lbauto.py). */
#include "lobby_b.h"
extern u8 COM_R_No_2;
extern int connect_jmp_tbl_270[];
extern int dial_jmp_tbl_540[2];
extern int dial_jmp_tbl_558[];
extern int lobby_client_login_jmp_324[];
extern int lbc_user_regist_jmp_939[];
extern int lobby_client_top_menu_jmp_1131[1];
extern int lobby_client_top_menu_jmp_1136[];
extern int lobby_client_matching_failed_jmp_2996[];
extern int lobby_client_logout_jmp_3078[];
extern int lbc_in_lobby_03_jmp_2430[];

void connecting_proc(void) {
    ((int (**)())connect_jmp_tbl_270)[COM_R_No_2]();
}
