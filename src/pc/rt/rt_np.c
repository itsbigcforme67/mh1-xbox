/* rt_np.c - direct-connect co-op glue (ONLINE=1 builds only; docs/network.md 1a and 3.4).
 *
 * The game's own in-quest code sends and receives (src/main/net/netsyn01.c net_send_pl,
 * netsyn02_nm.c net_receive_pl, netsyn03.c the network-delay slots). On the PS2 they sit on
 * the AQ / Cng / mcsls stack and Capcom's relaying game server; here the cut is at
 * AQ_data_put (send) and self_data_ctrl (receive), and src/pc/net/net_peer.c carries the
 * packets between the players directly, the host relaying.
 *
 * Slots are the game's own: the host is slot 0, joiners 1-3 in joining order;
 * game_w.master = this machine's slot (the PS2 sets it from CngNetAQConnectIdGet),
 * game_w.pl_num = the player count, game_w.pl_state[slot] = 1.
 *
 * Online_ck() is this file's `rt_online`: 0 until a co-op quest starts, so a single-player
 * run of mhview_online behaves as mhview.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <time.h>
#endif
#include "rt.h"
#include "types.h"
#include "game.h"
#include "pl.h"
#include "../net/net_peer.h"

#define PF(pl, T, o) (*(T *)((u8 *)(pl) + (o)))
#define GW8(o) (((u8 *)&game_w)[o])

extern u8 Ken_data[][0x18];
extern u8 Battle_type[];

static int rt_online;
static int want_role;               /* 1 host, 2 join (from the command line) */
static const char *want_addr;
static int want_port = NP_DEFAULT_PORT, want_players = 2;
static unsigned long st_tx, st_rx, st_drop;
static int trace = -1;

/* Online_ck (0x162D60): system_w+0x10 != 0 on the PS2 */
int Online_ck(void) { return rt_online; }
int rt_np_active(void) { return rt_online; }

static double now_s(void)
{
#ifdef _WIN32
    return GetTickCount() / 1000.0;
#else
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec + t.tv_nsec / 1e9;
#endif
}

static void sleep_ms(int ms)
{
#ifdef _WIN32
    Sleep(ms);
#else
    struct timespec t = { ms / 1000, (ms % 1000) * 1000000L };
    nanosleep(&t, NULL);
#endif
}

/* command line: --host [BIND_IP] / --join HOST_IP, --port N, --players N (host: wait for N) */
int rt_np_arg(int argc, char **argv, int *i)
{
    const char *a = argv[*i];
    if (!strcmp(a, "--host")) {
        want_role = 1;
        if (*i + 1 < argc && argv[*i + 1][0] >= '0' && argv[*i + 1][0] <= '9' && strchr(argv[*i + 1], '.'))
            want_addr = argv[++*i];
        return 1;
    }
    if (!strcmp(a, "--join") && *i + 1 < argc) {
        want_role = 2;
        want_addr = argv[++*i];
        return 1;
    }
    if (!strcmp(a, "--port") && *i + 1 < argc) {
        want_port = atoi(argv[++*i]);
        return 1;
    }
    if (!strcmp(a, "--players") && *i + 1 < argc) {
        want_players = atoi(argv[++*i]);
        if (want_players < 1) want_players = 1;
        if (want_players > NP_MAX) want_players = NP_MAX;
        return 1;
    }
    return 0;
}

int rt_np_wanted(void) { return want_role; }


/* The Elder's offline quests, as the port has played them (docs/pc.md "All offline quests
 * played"; goals as the port's tests read them from the mission files, monster kinds by
 * their em number). Shown when --host is given without --quest. */
static const struct { int no, stars; const char *goal; } quests[] = {
    { 131, 1, "deliver items (the first gathering quest)" }, { 132, 1, "deliver items" }, { 133, 1, "deliver items" },
    { 134, 1, "deliver items" }, { 135, 1, "deliver items" },
    { 136, 2, "hunt 3 Velociprey (urgent)" }, { 141, 2, "deliver items" }, { 142, 2, "deliver items" }, { 143, 2, "deliver items" },
    { 138, 2, "deliver an egg" },
    { 137, 3, "Velocidrome (urgent)" }, { 152, 3, "Velocidrome" }, { 144, 3, "monster kind 6" }, { 148, 3, "monster kind 6" },
    { 145, 3, "10 Velociprey" }, { 146, 3, "deliver an egg" }, { 147, 3, "deliver items" }, { 149, 3, "deliver items" },
    { 150, 4, "item + monster kind 6" }, { 151, 4, "15 Velociprey + Rathalos" }, { 153, 4, "2 eggs" }, { 154, 4, "monster kind 8" },
    { 155, 4, "15 monsters kind 13" }, { 156, 4, "Gendrome" }, { 157, 4, "3 eggs" }, { 158, 4, "15 Vespoid" },
    { 159, 4, "deliver items" }, { 160, 4, "Iodrome" },
    { 139, 5, "Rathalos" }, { 140, 5, "item + Rathalos" }, { 161, 5, "20 Velociprey + Rathalos" }, { 162, 5, "Velocidrome" },
    { 163, 5, "3 eggs" }, { 165, 5, "20 monsters kind 13 + Plesioth" }, { 166, 5, "Gendrome" }, { 167, 5, "Iodrome" },
    { 168, 5, "Vespoid + Plesioth" }, { 171, 5, "Monoblos" },
};

static int ask_quest(void)
{
    char line[64];
    int k, q;
    fprintf(stderr, "\nCo-op host: pick the quest everyone plays (quest number, Enter = 131):\n");
    for (k = 0; k < (int)(sizeof quests / sizeof quests[0]); k++)
        fprintf(stderr, "  %3d  %d*  %s\n", quests[k].no, quests[k].stars, quests[k].goal);
    fprintf(stderr, "quest> ");
    if (!fgets(line, sizeof line, stdin))
        return 0;
    q = line[0] == '\n' ? 131 : (int)strtol(line, NULL, 0);
    if (q < 1 || q > 0xB1) {
        fprintf(stderr, "co-op: no quest %s", line);
        return 0;
    }
    return q;
}

/* Before the quest is set up: host waits for the joiners and announces the quest; a joiner
 * connects and waits for it. Returns the quest number, 0 = no co-op, -1 = failed. */
int rt_np_setup(int quest_no)
{
    int wid = getenv("RT_WEAPON") ? atoi(getenv("RT_WEAPON")) : 156;
    int wait_s = getenv("RT_NP_WAIT") ? atoi(getenv("RT_NP_WAIT")) : 300, t;
    if (!want_role)
        return 0;
    if (want_role == 1) {
        if (!quest_no)
            quest_no = ask_quest();     /* the list on the console */
        if (!quest_no) {
            fprintf(stderr, "co-op: --host needs a quest (--quest N, or pick one from the list)\n");
            return -1;
        }
        if (np_host(want_addr, want_port, wid) != 0)
            return -1;
        fprintf(stderr, "co-op: quest %d, waiting for %d more player(s) on port %d\n", quest_no, want_players - 1, want_port);
        for (t = 0; np_players() < want_players && t < wait_s * 100; t++) {
            np_poll();
            sleep_ms(10);
        }
        np_poll();
        if (np_players() < want_players)
            fprintf(stderr, "co-op: only %d player(s) after %d s, starting anyway\n", np_players(), wait_s);
        sleep_ms(100);          /* the joiners' HELLO (their weapons) */
        np_poll();
        np_host_start(quest_no);
    } else {
        if (np_join(want_addr, want_port, wid) != 0)
            return -1;
        for (t = 0; !np_started() && np_connected(0) && t < wait_s * 100; t++) {
            np_poll();
            sleep_ms(10);
        }
        if (!np_started()) {
            fprintf(stderr, "co-op: the host did not start a quest\n");
            return -1;
        }
        quest_no = np_quest();
    }
    fprintf(stderr, "co-op: quest %d, %d player(s), this is slot %d\n", quest_no, np_players(), np_slot());
    return quest_no;
}

int rt_np_slot(void) { return want_role ? np_slot() : 0; }
int rt_np_players(void) { return want_role ? np_players() : 1; }
int rt_np_weapon(int slot) { return np_weapon(slot); }

/* rt_player_game_init, before pl_init: the session's slots, as Game_task step 3 and
 * init_pl_work do online (the other slots' equipment from the host's table, which stands in
 * for the mini data). */
void rt_np_init_slots(void)
{
    int n = np_players(), me = np_slot(), s;
    if (!want_role || !np_started())
        return;
    game_w.master = (u8)me;
    game_w.pl_num = (u8)n;
    GW8(0x21B) = 0;                 /* host slot (host_change; guess) */
    GW8(0x1B0) = 2;                 /* frames of network delay (net_start_ck computes it from the ping) */
    for (s = 0; s < 8; s++)
        game_w.pl_state[s] = s < n ? 1 : 0;
    for (s = 0; s < n; s++) {
        PLW *pl = &player_work[s];
        int wid = np_weapon(s);
        if (s == me)
            continue;
        if (wid <= 0 || wid >= 234)
            wid = 156;
        pl->be_flag = 1;
        pl->id = (u16)s;
        pl->stg = game_w.stage;
        PF(pl, u8, 0x10) = 0;
        PF(pl, u8, 0x35E) = 0;
        PF(pl, u8, 0x35F) = 6;
        pl->wpn_kind = (u16)wid;
        pl->work34C = Ken_data[wid][0];
        pl->kind = Battle_type[pl->work34C];
    }
    rt_online = 1;
    if (trace < 0)
        trace = getenv("RT_NP_TRACE") != NULL;
}

/* AQ_data_put (aq_nm.c): queue a game packet for everyone. Byte 1 is the packet's length
 * (header included), byte 2 the sender's slot. */
int AQ_data_put(int ch, u8 *d, int mode)
{
    int n = d[1];
    (void)mode;
    if (!rt_online)
        return -1;
    if (ch <= 0 || ch >= 0xB)
        return -4;
    d[2] = game_w.master;
    if (n < 4)
        n = 4;
    if (n > NP_PKT_MAX)
        return -6;
    np_send(ch, d, n);
    st_tx++;
    if (trace)
        fprintf(stderr, "np: tx ch %d kind %d len %d\n", ch, d[0], n);
    return 0;
}

/* net_start_ck (netsyn07_nm.c, game13): the quest start was agreed before the stage loaded */
int net_start_ck(void) { return 1; }

void net_receive_pl(int no, u8 *buf, int force);

/* every game tick, before the players move: the received packets, as AQ_recv /
 * self_data_ctrl dispatch them */
void rt_np_tick(void)
{
    static u8 buf[NP_PKT_MAX];
    int from, type, n;
    if (!want_role)
        return;
    if (rt_online) {
        /* System_timer: the PS2's Scheduler counts it every frame; the PC's rt_sys_tick
         * does not, and pl_move_sub sends the position packet (kind 2) when
         * System_timer % 20 == this slot */
        extern u16 System_timer;
        static double t0;
        static long n_ticks;
        double t;
        System_timer++;
        /* all players run at the PS2's 30 ticks a second, also headless runs (which
         * otherwise run as fast as they can and leave the others behind) */
        t = now_s();
        if (n_ticks == 0 || t - (t0 + n_ticks / 30.0) > 0.5)
            t0 = t - n_ticks / 30.0;            /* start, or far behind: no catching up */
        else if (t0 + n_ticks / 30.0 > t)
            sleep_ms((int)((t0 + n_ticks / 30.0 - t) * 1000.0));
        n_ticks++;
    }
    np_poll();
    if (rt_online) {        /* a player who left: as mcsls_force_drop marks it (pl_state 0xFF), and no longer shown */
        int s;
        for (s = 0; s < game_w.pl_num; s++)
            if (s != game_w.master && np_gone(s) && game_w.pl_state[s] != 0xFF) {
                game_w.pl_state[s] = 0xFF;
                player_work[s].x01 = 0;
                fprintf(stderr, "co-op: player %d left the quest\n", s);
            }
    }
    while ((n = np_recv(&from, &type, buf, sizeof buf)) >= 0) {
        st_rx++;
        if (n < 4 || buf[2] == game_w.master) {
            st_drop++;
            continue;
        }
        if (trace)
            fprintf(stderr, "np: rx from %d ch %d kind %d len %d\n", from, type, buf[0], n);
        if (!rt_online)
            continue;
        switch (type) {
        case 1: case 2: case 3: case 4:
            if (type - 1 != buf[2] || type - 1 >= game_w.pl_num) {
                st_drop++;
                break;
            }
            net_receive_pl(type, buf, 0);
            break;
        default:            /* 6 chat, 7 host, 8 monsters, 10 sys: not wired yet (M3) */
            st_drop++;
            break;
        }
    }
    {
        static int k;
        int s;
        ++k;
        if (trace && k % 300 == 0)
            fprintf(stderr, "np: %lu sent, %lu received, %lu not used\n", st_tx, st_rx, st_drop);
        if (getenv("RT_NP_POS") && rt_online && k % atoi(getenv("RT_NP_POS")) == 0)   /* test aid: every player's position as this machine sees it */
            for (s = 0; s < game_w.pl_num; s++) {
                PLW *p = &player_work[s];
                fprintf(stderr, "np-pos: tick %d me %d slot %d stg %d pos %.0f %.0f %.0f ang %04X/%04X act %d/%d\n", k, game_w.master, s,
                        p->stg, p->pos[0], p->pos[1], p->pos[2], p->ang[1] & 0xFFFF, p->ang_y & 0xFFFF, p->flag14, p->flag15);
            }
    }
}

void rt_np_close(void)
{
    if (want_role)
        np_close();
}

/* the other player in `slot` is in the quest and on this player's area */
int rt_np_visible(int slot)
{
    PLW *p = &player_work[slot & 7], *me = &player_work[game_w.master];
    return rt_online && slot != game_w.master && slot < game_w.pl_num && p->be_flag && p->x01 && p->stg == me->stg;
}
