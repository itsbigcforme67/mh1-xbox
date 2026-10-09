/* rt_online.c - the online town on screen (ONLINE=1 builds only; docs/network.md "The online town").
 *
 * On the PS2, choosing the network mode on the title menu sets system_w+0x10 (mode_sel_end) and Game_task
 * (src/main/game/f_game_nm.c) takes its online path in step 2:
 *   game_w.step 0  ms_network_sub    network settings, device, DNAS            -> the PC has none of it
 *   game_w.step 1  server_connect    internet_browser (KDDI portal pages: server table, MMBB id / password),
 *                                    internet_server_select, internet_connect_10 (tcp_init)
 *                                    -> replaced here: a host-side config (rt_online_config) gives the server
 *                                       table the portal would have delivered, then the game's own tcp_init
 *                  net_ToNetworkLobby the lobby client's work (cw = client_work, pNet = network_work)
 *   game_w.step 2  internet_lobby_act every frame until it returns 0 (a match was made): the game's own lobby
 *                                    client and its screens (src/lobby/f/lb_cli.c: login, top menu, plaza,
 *                                    lobby = the town stage through Visual_main / vs_square, rooms, game ready)
 *   game_w.step 3  internet_to_modem -> the in-quest session (here: net_peer / rt_np, docs/network.md 3.4)
 * The PC runs this file in game mode 6 (where the offline game runs the village, rt_village.c): the host draws
 * the town stage, the hunters and the game's 2D as for the village.
 *
 * Config (the portal replacement): mh1online.ini next to mh1pc.ini (written with defaults on first use):
 *   server = 127.0.0.1:10200      lobby server (loopback / private addresses only; MH Oldschool is refused)
 *   id = 0000000001               the MMBB id the portal gave (10 characters, sent in the login)
 *   password = LOCALTEST0000000   the password the portal's server table carried (16 characters)
 * RT_NET_HOST / RT_NET_PORT / RT_NET_ID / RT_NET_PASS override the file (tests).
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "rt.h"
#include "rt_log.h"
#include "types.h"
#ifdef _WIN32
#include <windows.h>
#else
#include <time.h>
#endif

extern char bsCsvWork[];            /* the portal's server table: first 16 bytes = login password */
extern char ConnectLbsId[];         /* id of the chosen lobby server in that table */
extern char D_3A3B71[];             /* the MMBB id (login key), 10 characters */
extern u8 ConnWork[];               /* +4 = socket */
extern u8 *cw, *pNet;
extern u8 client_work[], network_work[];
extern u8 system_w[], game_w[], lb_sys[];
s32 tcp_init(void);
void net_ToNetworkLobby(void);
s32 internet_lobby_act(void);
int CpInetInitialize(void);
void rt_np_set_online(int on);
void rt_game_move(void);
void rt_font_tick_begin(void);
void rt_prims_reset(void);
const char *rt_mc_root(void);
void rt_cam_init(int stage);

static int wanted, active, phase, ticks, trace = -1;
static char host[128] = "127.0.0.1", key[16] = "0000000001", pass[24] = "LOCALTEST0000000";
static int port = 10200;
static char sname[64] = "Local test server";

enum { O_CONNECT, O_LOBBY, O_MATCHED, O_FAILED };

int rt_online_wanted(void) { return wanted; }
int rt_online_active(void) { return active; }
void rt_online_request(void) { wanted = 1; }

static void say(const char *fmt, const char *a, int b)
{
    char m[300];
    snprintf(m, sizeof m, fmt, a, b);
    fprintf(stderr, "online: %s\n", m);
    rt_log("online: %s", m);
}

/* the portal replacement: mh1online.ini (written with the defaults when missing), then the environment */
static void rt_online_config(void)
{
    char path[1100], line[300], *s;
    FILE *f;
    const char *e;
    snprintf(path, sizeof path, "%s", rt_mc_root());
    s = strrchr(path, '/');
    if (!s)
        s = strrchr(path, '\\');
    if (s)
        snprintf(s, sizeof path - (size_t)(s - path), "/mh1online.ini");
    else
        snprintf(path, sizeof path, "mh1online.ini");
    f = fopen(path, "r");
    if (!f) {
        f = fopen(path, "w");
        if (f) {
            fprintf(f, "# MH1 PC online settings (what the KDDI portal page gave the PS2).\n"
                       "# Only loopback / private addresses are accepted: run tools/mh1_testserver.py.\n"
                       "server = %s:%d\nname = %s\nid = %s\npassword = %s\n", host, port, sname, key, pass);
            fclose(f);
            say("wrote %s with the defaults (local test server)", path, 0);
        }
    } else {
        while (fgets(line, sizeof line, f)) {
            char *k = line, *v = strchr(line, '='), *t;
            if (!v || *k == '#' || *k == ';')
                continue;
            *v++ = 0;
            while (isspace((unsigned char)*k)) k++;
            for (t = k + strlen(k); t > k && isspace((unsigned char)t[-1]); ) *--t = 0;
            while (isspace((unsigned char)*v)) v++;
            for (t = v + strlen(v); t > v && isspace((unsigned char)t[-1]); ) *--t = 0;
            if (!strcmp(k, "server")) {
                char *c = strrchr(v, ':');
                if (c) { *c = 0; port = atoi(c + 1); }
                snprintf(host, sizeof host, "%s", v);
            } else if (!strcmp(k, "id")) {
                snprintf(key, sizeof key, "%.10s", v);
            } else if (!strcmp(k, "name")) {
                snprintf(sname, sizeof sname, "%s", v);
            } else if (!strcmp(k, "password")) {
                snprintf(pass, sizeof pass, "%.16s", v);
            }
        }
        fclose(f);
    }
    if ((e = getenv("RT_NET_HOST")) && *e) snprintf(host, sizeof host, "%s", e);
    if ((e = getenv("RT_NET_PORT")) && *e) port = atoi(e);
    if ((e = getenv("RT_NET_ID")) && *e) snprintf(key, sizeof key, "%.10s", e);
    if ((e = getenv("RT_NET_PASS")) && *e) snprintf(pass, sizeof pass, "%.16s", e);
    /* the server table internet_browser leaves in bsCsvWork (lb_tcp01.c reads it): password, one server */
    memset(bsCsvWork, 0, 0x1000);
    strncpy(bsCsvWork, pass, 0x10);
    strncpy(ConnectLbsId, "LOCALTESTLBS", 12);
    strncpy(bsCsvWork + 0x603, "LOCALTESTLBS", 12);
    snprintf(bsCsvWork + 0x22F, 0x40, "%s:%d", host, port);
    strcpy(bsCsvWork + 0x2522, "0");            /* users now / at most (create_server_table) */
    strcpy(bsCsvWork + 0x266C, "100");
    {   /* the server list the portal leaves (Get_ServerName: id at +0, name at +0xD, 0x102 a server) */
        extern char BsLbsInfo[];
        memset(BsLbsInfo, 0, 0xA14);
        memcpy(BsLbsInfo, "LOCALTESTLBS", 12);
        snprintf(BsLbsInfo + 0xD, 0x40, "%s", sname);
    }
    {   /* the number of lobby servers in the table (create_server_table counts them): one. With one server the
         * first login goes straight on to the lobby (lbc_login_finish); with several the client logs out and shows
         * the server list (server_select) */
        extern s8 BsLbsCount;
        BsLbsCount = 1;
    }
    memset(D_3A3B71, ' ', 0xA);
    memcpy(D_3A3B71, key, strlen(key) < 0xA ? strlen(key) : 0xA);
}

/* Game_task mode 6 with the network mode chosen: connect first */
void rt_online_enter(void)
{
    if (trace < 0)
        trace = getenv("RT_ONLINE_TRACE") != NULL;
    rt_online_config();
    {   /* started with --online (no title / CONTINUE): the card's hunter (--hunter N), else a test name (RT_NAME) */
        extern u8 User_data[];
        int rt_np_load_hunter(void);
        if (!User_data[8] && rt_np_load_hunter() != 0 && getenv("RT_NAME"))
            snprintf((char *)User_data + 8, 0x10, "%s", getenv("RT_NAME"));
    }
    say("connecting to %s port %d", host, port);
    CpInetInitialize();
    {   /* the network device ms_network_sub would have chosen (DeviceGetOptionalStatus reads it for the login's first
         * data): kind 1, nothing else known. Guess: 1 = the network adaptor (broadband); 2 / 3 are the modems (PPP) */
        static s32 dev[4] = { 1, 0, 0, 0 };
        extern s32 *CurDevice;
        CurDevice = dev;
    }
    {   /* what Game_task / all_reset clear before the lobby overlay runs (as rt_village_enter): the quest's monsters,
         * set objects and prims of whatever ran before (a --quest start, or a co-op quest) */
        extern u8 em_work[];
        void clr_set_work(void);
        void prim_init(void);
        memset(em_work, 0, 0xA10 * 20);
        clr_set_work();
        prim_init();
    }
    game_w[0x1DC] = 1;              /* the lobby overlay is loaded (Game_task step 0 online) */
    active = 1;
    phase = O_CONNECT;
    ticks = 0;
}

/* online, every machine runs at the PS2's 30 ticks a second, also headless (screenshot) runs, which otherwise run
 * thousands of ticks while one answer is on its way (as rt_np.c does during a co-op quest) */
static void pace(void)
{
    static double t0;
    static long n;
    double t;
#ifdef _WIN32
    t = GetTickCount() / 1000.0;
#else
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    t = ts.tv_sec + ts.tv_nsec / 1e9;
#endif
    if (n == 0 || t - (t0 + n / 30.0) > 0.5)
        t0 = t - n / 30.0;              /* start, or far behind: no catching up */
    else if (t0 + n / 30.0 > t) {
        double w = t0 + n / 30.0 - t;
#ifdef _WIN32
        Sleep((DWORD)(w * 1000.0));
#else
        struct timespec d = { 0, (long)(w * 1e9) };
        nanosleep(&d, NULL);
#endif
    }
    n++;
}

/* One tick. Returns the quest number once a match was made, -1 when the online mode ended, else 0. */
int rt_online_tick(void)
{
    static int last31 = -1, last33 = -1, last34 = -1, last3 = -1;
    s32 r;
    ticks++;
    pace();
    rt_font_tick_begin();
    rt_prims_reset();
    switch (phase) {
    case O_CONNECT:
        r = tcp_init();
        if (r == 0) {
            say("connected%s (socket %d)", "", *(s32 *)(ConnWork + 4));
            net_ToNetworkLobby();       /* cw = client_work, pNet = network_work, the client's state */
            rt_np_set_online(1);        /* Online_ck() = 1 (system_w+0x10) */
            system_w[0x10] = 1;
            phase = O_LOBBY;
        } else if (r == -1 || ticks > 30 * 30) {
            say("could not connect to %s port %d", host, port);
            phase = O_FAILED;
        }
        return 0;
    case O_LOBBY:
        if (lb_sys[3] == 4 && cw[0x2C31] == 3)
            rt_game_move();             /* in the town: set objects and effects (as rt_village_tick) */
        r = internet_lobby_act();
        if (trace && (cw[0x2C31] != last31 || cw[0x2C33] != last33 || cw[0x2C34] != last34 || lb_sys[3] != last3)) {
            fprintf(stderr, "online: tick %d client %d/%d/%d/%d town step %d stage %d ret %d\n", ticks, cw[0x2C31],
                    cw[0x2C32], cw[0x2C33], cw[0x2C34], (s8)lb_sys[3], game_w[0x14], r);
            last31 = cw[0x2C31]; last33 = cw[0x2C33]; last34 = cw[0x2C34]; last3 = lb_sys[3];
        }
        if (trace && ticks % 60 == 0 && cw[0x2C31] == 3) {     /* every hunter in the town: slot, name, stage, position */
            extern u8 player_work[];
            int i;
            for (i = 0; i < 8; i++) {
                u8 *p = player_work + 0xA00 * i;
                if (p[0])
                    fprintf(stderr, "online: tick %d slot %d%s \"%.16s\" stage %d pos %.0f %.0f %.0f\n", ticks, i,
                            i == game_w[0xD1] ? " (me)" : "", (char *)p + 0x8D4, p[0x736], *(float *)(p + 0xAC),
                            *(float *)(p + 0xB0), *(float *)(p + 0xB4));
            }
        }
        if (r == 0) {
            say("match made%s (internet_lobby_act returned %d)", "", 0);
            phase = O_MATCHED;
        }
        return 0;
    case O_FAILED:
        active = 0;
        wanted = 0;
        rt_np_set_online(0);
        system_w[0x10] = 0;
        say("back to the village%s (offline, %d)", "", 0);
        return 0;                       /* the next tick runs the village */
    }
    return 0;
}

/* ------------------------------------------------------------ the in-game browser
 * The PS2 opens KDDI portal pages in its own browser (MainBrowser, lb_browser: HTTPS, HTML) for the personal data
 * of a new account, the top page and other links; the owner paused the browser (docs/DECISIONS.md). Here a page
 * "opens and is closed at once": MainBrowser returns 1 (closed) on its first call, the personal data stays as it
 * was (lbc_browser_04 then registers nothing), and the lobby client goes on as after the page. */
extern char FirstURL[];
int MainBsInitialize(int a) { (void)a; return 0; }
int MainBrowser(void)
{
    say("browser page %s skipped (no in-game browser on the PC)%.0d", FirstURL, 0);
    return 1;
}

/* ------------------------------------------------------------ quest download
 * Entering a lobby, the plaza first asks the server for its downloadable (event) quest files
 * (Lbc_DownloadQuest -> cnLBS_Read_FileDownload, a background job). The job itself,
 * __cnet_bgProg_ReadFileDownloadAllocation (main 0x27D4A0, 0x240 bytes), is not decompiled; here it ends at once
 * with "no files" (count 0 at 0x6AFF7C, which cnLBS_Get_FileDownloadInfo reports), as against a server without
 * event quests. The burst slot it runs in: run 0x677324, callback 0x677328, state 0x677344, step 0x677345. */
extern unsigned char rt_lb_mem[];
#define LBA(a) (rt_lb_mem + ((a) - 0x533980u))
typedef struct { s8 val; s8 id; u8 pad[6]; } ON_RES;
void __cnet_bgProg_ReadFileDownloadAllocation(void)
{
    void (*cb)(ON_RES, ON_RES *);
    ON_RES r;
    if (*LBA(0x677344) == 0)
        return;
    memset(&r, 0, sizeof r);
    *LBA(0x6AFF7C) = 0;
    *LBA(0x677344) = 0;
    *LBA(0x677345) = 0;
    memcpy(&cb, LBA(0x677328), sizeof cb);
    say("quest download%s: none (event quests are not supported, %d files)", "", 0);
    if (cb)
        cb(r, &r);
}

/* the other hunters the host draws in the town: in use (Lb_set_player), not this machine's, on the same stage */
int rt_online_visible(int slot)
{
    extern u8 player_work[];
    u8 *p = player_work + 0xA00 * (slot & 7), *me = player_work + 0xA00 * (game_w[0xD1] & 7);
    return active && phase == O_LOBBY && slot != game_w[0xD1] && p[0] && p[0x736] == me[0x736];
}
