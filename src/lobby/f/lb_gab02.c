/* lb_gab02 - near-match fixes 0x005CD7D0-0x005CD924: Lb_set_player. Whole file in lb_ab.c. */
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
        *(u8 **)(pl + 0x564) = (u8 *)get_prim_ptr(s);
        *(s32 *)(*(u8 **)(pl + 0x564) + 0x18) = *(u16 *)(pl + 0xC);
        *(void **)(*(u8 **)(pl + 0x564) + 0x14) = Lb_trans_pl;
    }
    *(s8 *)(pl + 0x8F0) = 1;
    Lb_pl_to_normal(pl, 0, 0, 0);
    *(u8 **)(pl + 0x3CC) = pl01_adr_tbl;
    (**(void (**)(u8 *))(*(u8 **)(pl + 0x3CC) + 0xC))(pl);
}
