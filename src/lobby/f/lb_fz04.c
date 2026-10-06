/* lb_fz04 0x005CF700-0x005CFA44: Lb_check_target */
/* Lobby: target selection (lock-on list), unique-action lookup, receive dispatcher, hand-written from m2c drafts. */
#include "lobby_f.h"
extern PLW player_work[];
extern u8 sw_flag_1260;
extern u8 em_work[];
f32 flSqrt(f32);
f32 flvecCalcDistance(f32 *, f32 *);
int Lb_get_angle();
void lb_target_angle();
void lb_insert_target_list();
u8 *Stage_unique_data_get();
void Lb_put_unique_act_hint();
int lb_ck_unique_act();
int Lb_Pl_stg_ck();
int SoftKeyboard_alive_check();
int lb_check_target(PLW *pl, u8 *tgt, u8 **list, int ang, int x, f32 range);
void Lb_check_target(void) {
    u8 *em;
    PLW *pl;
    u16 cnt;
    int i;
    u8 *tgt0;
    u8 *list;
    f32 range;
    u8 st;
    u16 trg;
    list = 0;
    em = em_work;
    pl = &player_work[game_w.master];
    tgt0 = (u8 *)pl->x3B0;
    if (pl->flag15 == 0x58) {
        return;
    }
    if (lb_sys.x6C == 1 || lb_sys.x68 != 0) {
        if (*(u8 *)0x3F33FE != 0 || SoftKeyboard_alive_check() != 0 || lb_sys.x68 != 0) {
            *((u8 *)&lb_sys + 0x86) = 1;
        }
        return;
    }
    trg = pl->sw.trg;
    if (trg & 0x800) {
        sw_flag_1260 = 2;
    } else if (trg & 0x400) {
        sw_flag_1260 = 1;
    }
    st = game_w.stage;
    cnt = 0;
    if (st >= 0x51 && st < 0x57) {
        range = 200.0f;
    } else {
        range = 400.0f;
    }
    i = 0;
    do {
        if (em[0] != 0) {
            st = game_w.stage;
            if (st >= 0x51 && st < 0x56 && em[2] == 3 && *((u8 *)&lb_sys + st + 0x37) != 0) {
            } else {
                cnt += (u8)lb_check_target(pl, em, &list, 0x2AAB, sw_flag_1260, range);
            }
        }
        i += 1;
        em += 0xA10;
    } while (i < 0x14);
    pl = &player_work[game_w.master];
    em = (u8 *)player_work;
    i = 0;
    do {
        if (em[0] != 0 && *(u16 *)(em + 0xC) != game_w.master && (Lb_Pl_stg_ck(em) & 0xFF)) {
            cnt += (u8)lb_check_target(pl, em, &list, 0x238E, sw_flag_1260, 300.0f);
        }
        i += 1;
        em += 0xA00;
    } while (i < 8);
    if (list == 0) {
        if (pl->x3B0 != 0) {
            *(s8 *)((u8 *)pl->x3B0 + 0x3D0) = 0;
        }
        pl->x3B0 = 0;
        return;
    }
    if (pl->x3B0 != 0) {
        *(s8 *)((u8 *)pl->x3B0 + 0x3D0) = 0;
        if (pl->sw.trg & 0xC00) {
            u8 *n = *(u8 **)((u8 *)pl->x3B0 + 0x3B0);
            if (n == 0) {
                pl->x3B0 = list;
            } else {
                pl->x3B0 = n;
            }
        } else if (*(s16 *)((u8 *)pl->x3B0 + 0x302) == -1) {
            pl->x3B0 = list;
        }
        if (tgt0 != pl->x3B0) {
            cnWrap_SoundRequest(0xA);
        }
    } else {
        pl->x3B0 = list;
        cnWrap_SoundRequest(0xA);
    }
    *(s8 *)((u8 *)pl->x3B0 + 0x3D0) = 1;
}
