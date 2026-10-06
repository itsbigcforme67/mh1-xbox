/* Network play sync, session start (SLPM_654.95 0x001BCD80-0x001BD254): net_start_ck, the start handshake of an online session:
 * counts dropped players, then steps game_w+0xD6 through 0 (announce, reset the net timers), 1 (wait for all players, 3600 frame timeout),
 * 2 (derive the net delay in frames from the slowest ping). Returns 1 when the session may start. */
#include "types.h"
#include "netsyn.h"

extern NGW game_w;
extern u8 player_work[];
extern u8 dropout_flag;
extern u8 dropout_time_cnt[4];
extern u8 old_join_status[4];
extern u8 net_time_cnt;
extern s16 no_send_timer;
extern s32 net_time[4][10];
f32 flFloor(f32);
int mcsls_get_ping_ave(int);
void net_game_w_clear(void);
void net_send_sys(u8, u8);
void Quest_error_set2();
void all_reset(void);

int net_start_ck(void) {
    int i;
    int j;
    int r;
    int cnt;
    u8 n;
    u32 ping;
    f32 f;
    NPLV *pl;
    s16 t;

    n = game_w.pl_num;
    r = 0;
    if (Online_ck() == 0) {
        return 0;
    }
    t = no_send_timer + 1;
    no_send_timer = t;
    if (t >= 0x3C) {
        no_send_timer = 0;
        net_send_sys(8, game_w.master);
    }
    dropout_flag = 0;
    for (i = 0; i < game_w.pl_num; i++) {
        if (game_w.pl_state[i] != 0xFF) {
            goto next;
        }
        if (old_join_status[i] != game_w.pl_state[i]) {
            dropout_flag |= (1 << i);
            dropout_time_cnt[i] = net_time_cnt;
        }
        n--;
        if (game_w.master == i) {
            game_w.step = 0;
            game_w.mode = 5;
            Quest_error_set2();
            all_reset();
            game_w.x124 = 0;
            net_game_w_clear();
            return 0;
        }
      next:
        old_join_status[i] = game_w.pl_state[i];
    }
    switch (game_w.xD6) {
    case 0:
        game_w.xD6++;
        game_w.x124 = 0;
        net_time_cnt = 0;
        pl = (NPLV *)(player_work + game_w.master * 0xA00);
        pl->slot[0].timer = 0;
        pl->slot[0].x = 0;
        pl->slot[0].y = 0;
        pl->slot[0].z = 0;
        game_w.x108[0] = 0;
        pl = (NPLV *)(player_work + game_w.master * 0xA00);
        pl->slot[1].timer = 0;
        pl->slot[1].x = 0;
        pl->slot[1].y = 0;
        pl->slot[1].z = 0;
        game_w.x108[1] = 0;
        for (i = 0; i < 4; i++) {
            for (j = 0; j < 10; j++) {
                net_time[i][j] = 0;
            }
        }
        net_send_sys(3, game_w.master);
        if (game_w.pl_num == 1) {
            r = 1;
            game_w.x1B0 = 1;
        }
        break;
    case 1:
        game_w.x124++;
        if (game_w.x124 >= 0xE10) {
            game_w.step = 0;
            game_w.mode = 5;
            Quest_error_set2();
            all_reset();
        } else {
            cnt = 0;
            for (i = 0; i < game_w.pl_num; i++) {
                if (game_w.xD8[i] == game_w.xD6 + 1) {
                    cnt++;
                }
            }
            if (cnt == n - 1 && cnt != 0 || n < 2) {
                game_w.xD8[0] = 0;
                game_w.xE0[0] = 0;
                game_w.xD8[1] = 0;
                game_w.xE0[1] = 0;
                game_w.xD8[2] = 0;
                game_w.xE0[2] = 0;
                game_w.xD8[3] = 0;
                game_w.xE0[3] = 0;
                game_w.xD6++;
            }
        }
        break;
    case 2:
        for (i = 0; i < game_w.pl_num; i++) {
            if (i != game_w.master && game_w.pl_state[i] != 0xFF) {
                ping = mcsls_get_ping_ave(i);
                f = 1.0f + flFloor((f32)ping / 33.33f);
                if (game_w.x1B0 < (s16)f) {
                    game_w.x1B0 = (s16)f;
                }
            }
        }
        if (game_w.x1B0 == 0) {
            game_w.x1B0 = 1;
        }
        if (game_w.x1B0 >= 0x3D) {
            game_w.x1B0 = 0x3C;
        }
        game_w.x124 = 0;
        r = 1;
        net_game_w_clear();
        break;
    }
    return r == 1;
}
