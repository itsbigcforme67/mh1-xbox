/* rt_mcs.c - online hunts on the game's own in-hunt protocol (ONLINE=1 builds; docs/network.md 3.10).
 *
 * After a lobby match whose game rule (6914) is "mh1-mcs" (mh1-server --lobby-mcs), the PC does what a PS2 does
 * (docs/network.md 1a, 3.7), with the game's own code wherever it is decompiled:
 *
 *   1. internet_to_modem (lobby, Game_task step 2/3): cmcs_00 .. cmcs_06 close the lobby connection, connect to the
 *      game server of 6916, answer its 0x1031 with the 0x82 login (mmbbc_encode of the MMBB login), LobbyToMcsInit
 *      (player count, own player number), AQ_init and AQSession_init_online -> CngSessionStart_online -> mcsls_init;
 *   2. Game_task step 3: AQ_exec until AQSession_wait, then pl_num = AQ_join_num_get(), pl_state[];
 *   3. game0 / Plsel_task (player_sel, player_wait): Copy_user_id, sel_default_set, the ready flag and net_send_sys 2
 *      (character select, the hunter id, slot 0's quest) until every player's flag came, then once more and 30 frames;
 *   4. the stage loads (the PC's own flow, rt_player_game_init), the other hunters from the lobby's mini data
 *      (Set_mini_data_to_pl by the hunter id each sent in sys 2, as init_pl_work does);
 *   5. game13: net_start_ck (netsyn07_nm.c, sys 3 rounds and the network delay from mcsls' ping) until it says 1;
 *   6. every tick: AQ_exec (mcsls_move: receive, relay records, ping, keep-alives; then the AQ packets to
 *      self_data_ctrl -> net_receive_pl / _em / _sys / _host) and AQ_send (the tick's packets as one Cng message).
 *
 * So the bytes on the wire are the PS2's: a patched PS2 (src/ps2patch), a PC and an Xbox hunt together through
 * mh1-server's mcs relay (tools/server/mcs.py). The direct-connect co-op (net_peer.c, rt_np.c) stays as it was.
 * The game files run here: src/main/aq/aq_nm.c, src/main/net/{aqcmd_nm,cng_nm,cngmsg_nm,cnmsg_nm,mcsls_*}.c,
 * netsyn04.c / netsyn07_nm.c, the lobby's cmcs_* (tools/build_pc.sh, ONLINE=1). */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gwx.h"
#include "swx.h"
#include "rt.h"
#include "types.h"
#include "game.h"
#include "pl.h"

#define GW8(o) (((u8 *)&game_w)[GWX(o)])

extern u8 select_w[];
#define SW_QUEST SWX(0xAC)  /* the quest (G 0xD0, set by G's lobby code; include/swx.h) */
#define SW_READY SWX(0x8C)  /* the ready flags (G 0xAC); sys 2 carries G's layout in the G build (netsyn08.c) */
#ifdef VERSION_MHG
/* MH1 data the PC's in-hunt session C (aq_nm.c, cng_nm.c: MH1's code) uses and G's main lacks (G's AQ_init calls
 * CngNetAQInit instead of the TCP config set): without these the G build had code stand-ins (rt_gen.c) for them and
 * AQ_init's write to ave_cnf crashed (24 Oct 2026). Host objects of MH1's sizes. */
u8 ave_cnf[0x2C] __attribute__((aligned(16)));
u8 netmod_load_flag;
#endif
extern u8 system_w[];
extern u8 COM_R_No_1;
extern u8 cng_netAQ[];
extern u8 mcsls_w[];
extern u8 ConnWork[];
extern u8 InetGame[];
extern u8 room_member_handle[][0x11];

s32 internet_to_modem(void);
void AQ_exec(void);
void AQ_send(void);
int AQSession_wait(void);
int AQ_join_num_get(void);
void AQ_recv_flag_set(int v);
void AQ_session_exit_online(void);
void CngNetAQInit(void *q);
void Copy_user_id(u8 no);
void init_select_work(int k);
void sel_default_set(void);
void net_send_sys(u8 kind, u8 slot);
int mcs_net_start_ck(void);
int mcs_AQ_data_put(int ch, u8 *d, int mode);
void mcs_force_drop(u32 n);
void Set_mini_data_to_pl(s8 *key, u8 *pl);
void Set_userdata(PLW *pl);
int CpInetTcpAbort(int h);
int CpInetTcpDelete(int *h);
double rt_np_now_s(void);
void rt_np_sleep_ms(int ms);

/* the AQ channels' receive pools (recv_buf_adrs, init_AQWork) are only reached through that pointer table: named here so
 * the build defines them and rt_data turns the table's words into these host buffers */
extern u8 u1_recv_buff[], u2_recv_buff[], u3_recv_buff[], u4_recv_buff[], u6_recv_buff[], u7_recv_buff[], u8_recv_buff[],
    u9_recv_buff[], u10_recv_buff[];
void *rt_mcs_pools[] = { u1_recv_buff, u2_recv_buff, u3_recv_buff, u4_recv_buff, u6_recv_buff, u7_recv_buff, u8_recv_buff,
                         u9_recv_buff, u10_recv_buff };
extern u8 *recv_buf_adrs[11];

static int mode;            /* 1: this session runs on mcsls */
static int my_slot, n_players;      /* the session's (the quest set-up clears game_w; rt_mcs_init_slots puts them back) */
static u8 ids[4][8], states[8];     /* each slot's hunter id (sys 2) and state */

int rt_mcs_slot(void) { return my_slot; }
int rt_mcs_players(void) { return n_players; }
static int trace;
static double t_start;

int rt_mcs_on(void) { return mode; }
void rt_mcs_configure(int on) { mode = on; }

/* CngNetTimeGet (cnmsg_nm.c reads the EE's timer 0): seconds since the first call, from the host clock */
f32 CngNetTimeGet(void)
{
    if (t_start == 0)
        t_start = rt_np_now_s() - 1.0;
    return (f32)(rt_np_now_s() - t_start);
}

/* CnInetNetworkAveTcpPoll (cng00.c, the IOP connection state machine): the host's sockets are up, state 2 */
int CnInetNetworkAveTcpPoll(void) { return 2; }

static int mcsls_state(void) { return mcsls_w[6]; }

/* one session frame as Game_task step 4 frames it: AQ_exec before the game, AQ_send after */
static void frame(void (*between)(void))
{
    double t0 = rt_np_now_s(), d;
    AQ_exec();
    if (between)
        between();
    AQ_send();
    d = 1.0 / 30 - (rt_np_now_s() - t0);
    rt_np_sleep_ms(d > 0 ? (int)(d * 1000) : 0);    /* (also when late: on the Xbox each wait counts as progress) */
}

static int ready_all(void)
{
    int i;
    for (i = 0; i < 4; i++)
        if (select_w[SW_READY + i] == 0 && game_w.pl_state[i] != 0xFF)
            return 0;
    return 1;
}

/* After the lobby's match (rt_online.c matched, which leaves the lobby socket to cmcs_00): steps 1-3 above.
 * Returns the quest (select_w+SWX(0xAC), the room's, re-sent by slot 0 in sys 2), -1 on failure. */
int rt_mcs_setup(void)
{
    int t, r, i, me;
    trace = getenv("RT_NP_TRACE") != NULL;
    for (i = 1; i < 11; i++)
        if (i != 5 && (u8 *)recv_buf_adrs[i] != (u8 *)rt_mcs_pools[i < 5 ? i - 1 : i - 2]) {
            fprintf(stderr, "mcs: recv_buf_adrs[%d] is not the host buffer\n", i);
            return -1;
        }
    /* 1: internet_to_modem until it says connected (1) or failed (-1); cmcs_02 waits up to 10 frames per try, cmcs_04
     * 0x258 frames for the 0x1031 */
    CngNetAQInit(cng_netAQ);
    system_w[0x35] = 0;                 /* "loading" (AQ_exec skips the session while set): the title's edit screen
                                         * leaves it 1, the PS2's game11 / game12 clear it after their loads */
    GW8(0xD2) = 1;                      /* game_w.x0D2 (Game_task step 0) = AQ_init's mode */
    COM_R_No_1 = 0;
    for (t = 0; (r = internet_to_modem()) == 0 && t < 30 * 60; t++)
        rt_np_sleep_ms(33);
    if (r != 1) {
        fprintf(stderr, "mcs: no game-server session (internet_to_modem %d, step %d)\n", r, COM_R_No_1);
        return -1;
    }
    fprintf(stderr, "mcs: logged in to the game server: %d players, this is player %d\n", InetGame[0], InetGame[1]);
    /* 2: Game_task step 3 */
    for (t = 0; t < 0xE10; t++) {
        AQ_exec();
        if (AQSession_wait())
            break;
        rt_np_sleep_ms(33);
    }
    if (t >= 0xE10)
        return -1;
    game_w.pl_num = (u8)AQ_join_num_get();
    GW8(0x21B) = 0;
    for (i = 0; i < 8; i++)
        game_w.pl_state[i] = i < game_w.pl_num ? 1 : 0;
    AQ_recv_flag_set(1);
    me = game_w.master;
    frame(NULL);                        /* Game_task step 4's first AQ_exec: the packet timer starts (AQ_data_put
                                         * refuses packets while it is 0) */
    /* 3: game0 (Copy_user_id) and Plsel_task: player_sel, then player_wait */
    for (i = 0; i < 4; i++)
        memset(game_w.x1E8[i], 0, 8);
    Copy_user_id(me);
    init_select_work(0);
    sel_default_set();
    select_w[SW_READY + me] = 1;
    net_send_sys(2, me);
    for (t = 0; !ready_all(); t++) {
        frame(NULL);
        if (game_w.pl_state[me] == 0xFF || t > 30 * 120) {
            fprintf(stderr, "mcs: the players' select did not complete (mcsls state %d, error %d)\n", mcsls_state(),
                    *(s32 *)(mcsls_w + 0x160));
            return -1;
        }
        if (t % 60 == 59)
            net_send_sys(8, me);        /* Plsel_task's keep-alive every 60 frames */
        if (trace && t % 30 == 0)
            fprintf(stderr, "mcs: select wait %d: mcsls state %d, ready %d %d %d %d\n", t, mcsls_state(),
                    (s8)select_w[SW_READY], (s8)select_w[SW_READY + 1], (s8)select_w[SW_READY + 2],
                    (s8)select_w[SW_READY + 3]);
    }
    net_send_sys(2, me);                /* player_wait */
    for (t = 0; t < 0x1E; t++)
        frame(NULL);
    my_slot = me;
    n_players = game_w.pl_num;
    memcpy(ids, game_w.x1E8, sizeof ids);
    memcpy(states, game_w.pl_state, sizeof states);
    fprintf(stderr, "mcs: every player is in (mcsls state %d): quest %d, %d players, this is slot %d\n", mcsls_state(),
            *(u16 *)(select_w + SW_QUEST), game_w.pl_num, me);
    return *(u16 *)(select_w + SW_QUEST);
}

/* rt_np_init_slots for an mcs session: the slots are the session's (game_w.master / pl_num / pl_state set above);
 * every other hunter's equipment and look from the lobby's mini data of the hunter id it sent (init_pl_work) */
void rt_mcs_init_slots(void)
{
    int n = n_players, me = my_slot, s;
    game_w.master = (u8)me;
    game_w.pl_num = (u8)n;
    memcpy(game_w.x1E8, ids, sizeof ids);
    memcpy(game_w.pl_state, states, sizeof states);
    for (s = 0; s < n; s++) {
        PLW *pl = &player_work[s];
        if (s == me) {
            Set_userdata(pl);
            continue;
        }
        pl->be_flag = 1;
        pl->id = (u16)s;
        pl->stg = game_w.stage;
        *((u8 *)pl + 0x10) = 0;
        Set_mini_data_to_pl((s8 *)game_w.x1E8[s], (u8 *)pl);
        memcpy(pl->name, (u8 *)pl + 0x8D4, sizeof pl->name);
    }
}

static int start_r, start_echo;
static void start_step(void) { start_r = mcs_net_start_ck(); }

/* game13: net_start_ck until every player has loaded (sys 3) and the network delay is known */
void rt_mcs_start(void)
{
    int t;
    GW8(0xD6) = 0;
    system_w[0x35] = 0;                 /* (game12's end) */
    *(s32 *)((u8 *)&game_w + GWX(0x124)) = 0;
    for (t = 0; t < 0xE10 + 30; t++) {
        start_r = 0;
        frame(start_step);
        if (start_r)
            break;
    }
    fprintf(stderr, "mcs: start %s after %d frames, network delay %d frames\n", start_r ? "agreed" : "NOT agreed", t,
            *(s16 *)((u8 *)&game_w + GWX(0x1B0)));
    start_echo = start_r ? 6 : 0;
}

/* net_start_ck sends its "loaded" (sys 3, round 2) once, in step 0. A PS2 clears what it got of it at the end of its
 * stage load (game11 clears game_w+0xD8..0xDB), so a PC that loaded first and sent it while the PS2 was still in
 * game10 would leave the PS2 waiting at "クエストを開始します" for ever (seen in tools/test_ps2_hunt.sh). The PC agrees
 * only once every other player's sys 3 came, i.e. once they are past their loads: it says "loaded" a few more times
 * then (every second, 6 times). A player already in the hunt only stores the value (net_receive_sys kind 3). */
static void start_echo_send(void)
{
    u8 d6 = GW8(0xD6);
    GW8(0xD6) = 1;                      /* sys 3 carries xD6 + 1 = 2: round 2 done */
    net_send_sys(3, game_w.master);
    GW8(0xD6) = d6;
}

/* every game tick (rt_np_tick): last tick's packets out, this tick's in */
void rt_mcs_tick(void)
{
    static int k, last_state = -1;
    AQ_send();
    AQ_exec();
    k++;
    if (start_echo && k % 30 == 0) {
        start_echo_send();
        start_echo--;
    }
    if (trace && (k % 300 == 0 || mcsls_state() != last_state))
        fprintf(stderr, "mcs: tick %d mcsls state %d error %d pl_state %d %d %d %d\n", k, mcsls_state(),
                *(s32 *)(mcsls_w + 0x160), game_w.pl_state[0], game_w.pl_state[1], game_w.pl_state[2],
                game_w.pl_state[3]);
    last_state = mcsls_state();
}

/* after the quest (game mode 6 step 0): AQ_session_exit_online; the game-server connection closed */
void rt_mcs_end(void)
{
    if (!mode)
        return;
    AQ_session_exit_online();
    CpInetTcpAbort(*(s32 *)(ConnWork + 4));
    CpInetTcpDelete((int *)(ConnWork + 4));
    mode = 0;
}
