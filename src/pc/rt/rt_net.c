/* rt_net.c - online play glue for the PC build (ONLINE=1 only, docs/network.md).
 *
 * rt_net_test() is the headless online test: it drives the game's own lobby-server client
 * (Capcom's cnet code, src/lobby/cnet: cnLBS_*) against a server, in the order the lobby UI
 * (src/lobby/f/lb_cli.c, lbc_login_*, lbc_top_menu_*) calls it, with the UI replaced by a
 * small state machine. The connection goes through the game's own tcp_init (lb_tcp01.c: host
 * name and port from the server table the portal page normally delivers, DNS, TCP) and the
 * port's network backend (src/pc/net/net_cpinet.c).
 *
 * Target: RT_NET_HOST (default 127.0.0.1) and RT_NET_PORT (default 10200). There is no default
 * that points at a public server; net_cpinet.c refuses non-private addresses and the known
 * MH Oldschool addresses.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>
#ifdef _WIN32
#include <windows.h>
#define SLEEP_MS(ms) Sleep(ms)
#else
#include <time.h>
static void SLEEP_MS(int ms)
{
    struct timespec t = { 0, ms * 1000000L };
    nanosleep(&t, NULL);
}
#endif
#include "types.h"
#include "lbnet.h"
#include "rt_log.h"
#include "../net/net_cpinet.h"

/* game data (lobby.bin, filled by rt_gen / rt_import_lobby) */
extern char bsCsvWork[];            /* server table of the portal page; first field = login password */
extern char ConnectLbsId[];         /* id of the chosen lobby server in that table */
extern char D_3A3B71[];             /* the login key (the MMBB id), 10 characters */
extern u8 ConnWork[];               /* +4 = socket */
extern s8 MediaVersion[];
extern char patch_buff[];

/* the game's functions */
s32 tcp_init(void);
int cnLBS_RecvData(int sock);
void cnLBS_Init_LoginLobbyServer(void);
int cnLBS_Set_LoginFirstData(void *fd);
int cnLBS_Set_CallBackNoticeEvent(int idx, void (*fn)());
int cnLBS_LoginLobbyServer(CNET_LOGIN cfg, int cb);
int cnLBS_Answer_LoginWarningMessage(int ok);
int cnLBS_Send_LoginUserAccount(char *id, char *handle, void *mini);
int cnLBS_Send_UserMiniData(void *mini, int len, void (*cb)());
int cnLBS_Send_LoginFinish(void);
int cnLBS_Read_TopInformation(void (*cb)());
int cnLBS_Get_TopInformation(void *d);
int cnLBS_Read_CurrentPlace(void (*cb)());
int cnLBS_Get_CurrentPlace(s16 *d);
int cnLBS_Read_PlazaAllocation(int unused, int what, void (*cb)());
int cnLBS_Get_PlazaCount(u16 *n);
int cnLBS_Get_PlazaName(int idx, char *d);
int cnLBS_Get_PlazaStatus(int idx, u8 *d);
int cnLBS_Get_mhPlazaJoinUser(int idx, u16 *a, u16 *b);
int cnLBS_PlazaEntry(int id, void (*cb)());
int cnLBS_Read_LobbyAllocation(int unused, int what, void (*cb)());
int cnLBS_Get_LobbyCount(u16 *n);
int cnLBS_Get_LobbyName(int idx, char *d);
int cnLBS_LobbyEntry(int id, void (*cb)());
int cnLBS_LogoutLobbyServer(void (*cb)());
int cnLBS_Get_ServerMessage(char *d);

static char t_server_msg[0x300];
static int t_phase;                 /* where the scripted client is */
static int t_wait;                  /* a request is in flight */
static int t_fail;
static int t_last_val, t_last_id;
static unsigned char t_mini[0x40];

enum {
    P_CONNECT, P_LOGIN, P_LOGIN_WAIT, P_MINI, P_FINISH, P_TOPINFO, P_PLACE, P_PLAZAS, P_PLAZA_ENTRY,
    P_LOBBIES, P_LOBBY_ENTRY, P_LOGOUT, P_DONE
};

static const char *phase_name[] = {
    "connect", "login", "login (waiting for the server)", "mini data", "login finish", "top information",
    "current place", "plaza list", "plaza entry", "lobby list", "lobby entry", "logout", "done"
};

static void note(const char *fmt, ...)
{
    char b[300];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(b, sizeof b, fmt, ap);
    va_end(ap);
    printf("nettest: %s\n", b);
    fflush(stdout);
    rt_log("nettest: %s", b);
}

/* completion callbacks: the game calls cb(res, &res), res = {val, id} by value */
static void cb_done(CNET_RES res, void *p)
{
    (void)p;
    t_last_val = res.val;
    t_last_id = res.id;
    t_wait = 0;
    if (res.val != 0 && res.val != 2) {
        t_fail = 1;
        cnLBS_Get_ServerMessage(t_server_msg);
    }
}

/* the login callback: res.id tells what happened (CallBack_Result_LoginLobbyServer in lb_cli.c) */
static int ev_userlist, ev_userid, ev_loginok, ev_warning, ev_patch, ev_regulation;

static void cb_login(CNET_RES res, void *p)
{
    (void)p;
    if (res.val == -1) {
        t_fail = 1;
        cnLBS_Get_ServerMessage(t_server_msg);
        note("login callback: error id %d: \"%s\"", res.id, t_server_msg);
        return;
    }
    switch (res.id) {
    case 0: ev_loginok = 1; note("login ok (server notice 6104)"); break;
    case 1: ev_userlist = 1; note("server sent the account list"); break;
    case 2: ev_userid = 1; note("server accepted the account"); break;
    case 3: ev_patch = 1; note("server asks for a patch"); break;
    case 4: ev_warning = 1; note("server sent a warning message"); break;
    case 5: ev_regulation = 1; note("server asks to agree to the regulations"); break;
    default: note("login callback: id %d val %d", res.id, res.val); break;
    }
}

static void cb_notice(CNET_RES res, void *p)
{
    (void)p;
    note("notice event %d", res.id);
}

static int send_wait(int rc)
{
    if (rc < 0) {
        note("request refused (%d)", rc);
        t_fail = 1;
        return -1;
    }
    t_wait = 1;
    return 0;
}

/* ---- the scripted client ---- */

static int issued;                  /* the current phase's request has been sent */

/* sends the request of a phase (0 = sent, waiting; 1 = nothing to wait for) */
static int issue(int ph)
{
    switch (ph) {
    case P_MINI:
        note("send mini data");
        return send_wait(cnLBS_Send_UserMiniData(t_mini, 0x40, (void (*)())cb_done));
    case P_FINISH:
        note("send login finish");
        cnLBS_Send_LoginFinish();
        return 1;
    case P_TOPINFO:
        note("read top information");
        return send_wait(cnLBS_Read_TopInformation((void (*)())cb_done));
    case P_PLACE:
        note("read current place");
        return send_wait(cnLBS_Read_CurrentPlace((void (*)())cb_done));
    case P_PLAZAS:
        note("read plaza allocation");
        return send_wait(cnLBS_Read_PlazaAllocation(0, 7, (void (*)())cb_done));
    case P_PLAZA_ENTRY:
        note("enter plaza 1");
        return send_wait(cnLBS_PlazaEntry(1, (void (*)())cb_done));
    case P_LOBBIES:
        note("read lobby allocation");
        return send_wait(cnLBS_Read_LobbyAllocation(0, 7, (void (*)())cb_done));
    case P_LOBBY_ENTRY:
        note("enter lobby 1");
        return send_wait(cnLBS_LobbyEntry(1, (void (*)())cb_done));
    case P_LOGOUT:
        note("log out");
        return send_wait(cnLBS_LogoutLobbyServer((void (*)())cb_done));
    }
    return 1;
}

/* reports what a finished phase delivered */
static void finished(int ph)
{
    char name[0x50];
    u16 n = 0, a = 0, b = 0;
    s16 pl[3] = { 0, 0, 0 };
    int i;
    switch (ph) {
    case P_TOPINFO: {
        static u8 top[0x1004];
        cnLBS_Get_TopInformation(top);
        top[0x1000] = 0;
        note("top information: level %d, text \"%.60s\"", top[0], (char *)top + 4);
        break;
    }
    case P_PLACE:
        cnLBS_Get_CurrentPlace(pl);
        note("current place: %d %d %d", pl[0], pl[1], pl[2]);
        break;
    case P_PLAZAS:
        cnLBS_Get_PlazaCount(&n);
        note("plazas: %u", n);
        for (i = 1; i <= (int)n && i <= 10; i++) {
            u8 st = 0;
            memset(name, 0, sizeof name);
            cnLBS_Get_PlazaName(i, name);
            cnLBS_Get_PlazaStatus(i, &st);
            cnLBS_Get_mhPlazaJoinUser(i, &a, &b);
            note("  plaza %d \"%s\" status %d users %u/%u", i, name, st, a, b);
        }
        break;
    case P_LOBBIES:
        cnLBS_Get_LobbyCount(&n);
        note("lobbies: %u", n);
        for (i = 1; i <= (int)n && i <= 14; i++) {
            memset(name, 0, sizeof name);
            cnLBS_Get_LobbyName(i, name);
            note("  lobby %d \"%s\"", i, name);
        }
        break;
    }
}

int rt_net_test(const char *scenario)
{
    const char *host = getenv("RT_NET_HOST");
    const char *port = getenv("RT_NET_PORT");
    int ticks, limit = 20000 / 5, upto = P_DONE;

    if (!host || !*host)
        host = "127.0.0.1";
    if (!port || !*port)
        port = "10200";
    if (scenario && !strcmp(scenario, "connect"))
        upto = P_LOGIN;
    else if (scenario && !strcmp(scenario, "login"))
        upto = P_MINI;
    else if (scenario && !strcmp(scenario, "top"))
        upto = P_PLACE;
    else if (scenario && !strcmp(scenario, "plaza"))
        upto = P_LOBBIES;
    else if (scenario && !strcmp(scenario, "lobby"))
        upto = P_LOGOUT;
    note("scenario %s against %s:%s", scenario ? scenario : "full", host, port);
    CpInetInitialize();

    /* the server table the portal page delivers: first field = password, one entry
     * (id at +0x603, "host:port" at +0x22F) */
    memset(bsCsvWork, 0, 0x1000);
    strncpy(bsCsvWork, "LOCALTEST0000000", 0x10);
    strncpy(ConnectLbsId, "LOCALTESTLBS", 12);
    strncpy(bsCsvWork + 0x603, "LOCALTESTLBS", 12);
    snprintf(bsCsvWork + 0x22F, 0x40, "%s:%s", host, port);
    strncpy(D_3A3B71, "TESTKEY001", 0xA);
    memset(t_mini, 0, sizeof t_mini);
    strncpy((char *)t_mini + 4, "nettest", 8);

    t_phase = P_CONNECT;
    for (ticks = 0; ticks < limit && t_phase < upto && !t_fail; ticks++) {
        if (t_phase == P_CONNECT) {
            s32 r = tcp_init();
            if (r == 0) {
                note("tcp connected (socket %d)", *(s32 *)(ConnWork + 4));
                t_phase = P_LOGIN;
            } else if (r == -1) {
                note("connect failed");
                t_fail = 1;
            }
            SLEEP_MS(5);
            continue;
        }
        if (t_phase > P_LOGIN)
            cnLBS_RecvData(*(s32 *)(ConnWork + 4));
        if (t_phase == P_LOGIN) {
            /* lbc_login_init (lb_cli.c / b/nm/lbc_login_init.c) */
            struct { u8 b10, b11, b12, b13; char ver[0x10]; s16 h[8]; } fd;
            CNET_LOGIN lg;
            memset(&fd, 0, 0x24);
            cnLBS_Init_LoginLobbyServer();
            cnLBS_Set_CallBackNoticeEvent(1, (void (*)())cb_notice);
            cnLBS_Set_CallBackNoticeEvent(5, (void (*)())cb_notice);    /* chat */
            cnLBS_Set_CallBackNoticeEvent(0xF, (void (*)())cb_notice);  /* plaza join user */
            cnLBS_Set_CallBackNoticeEvent(0x15, (void (*)())cb_notice);
            fd.b11 = 1;
            fd.b12 = 4;
            strncpy(fd.ver, "1.00", 0xA);
            cnLBS_Set_LoginFirstData(&fd);
            memset(&lg, 0, sizeof lg);
            lg.x00[0] = 1;
            strncpy(lg.key, D_3A3B71, 0xA);
            strncpy(lg.pass, bsCsvWork, 0x10);
            lg.patch_buf = (s32)(intptr_t)patch_buff;
            if (cnLBS_LoginLobbyServer(lg, (int)(intptr_t)cb_login) != 0) {
                note("cnLBS_LoginLobbyServer refused");
                t_fail = 1;
                break;
            }
            note("login armed, waiting for the server's first packet");
            t_phase = P_LOGIN_WAIT;
            issued = 0;
        } else if (t_phase == P_LOGIN_WAIT) {
            if (ev_warning) {
                ev_warning = 0;
                cnLBS_Answer_LoginWarningMessage(1);
            }
            if (ev_regulation) {      /* the UI would show a web page; the test client cannot */
                note("regulation page requested: not supported by the test client");
                t_fail = 1;
            }
            if (ev_userlist) {
                ev_userlist = 0;
                cnLBS_Send_LoginUserAccount(NULL, "NETTEST", t_mini);
                note("sent the account request (new hunter)");
            }
            if (ev_loginok) {
                t_phase = P_MINI;
                issued = 0;
            }
        } else if (!t_wait) {
            if (issued) {
                finished(t_phase);
                t_phase++;
                issued = 0;
            }
            if (t_phase < upto && !issued) {
                int r = issue(t_phase);
                issued = 1;
                if (r == 1) {           /* nothing to wait for */
                    finished(t_phase);
                    t_phase++;
                    issued = 0;
                }
            }
        }
        SLEEP_MS(5);
    }
    if (t_fail && t_server_msg[0])
        note("server message: \"%s\"", t_server_msg);
    note("stopped in phase \"%s\"%s", phase_name[t_phase], t_fail ? " (failed)" : (ticks >= limit ? " (timeout)" : ""));
    printf("NETTEST_PHASE=%d\n", t_phase);
    return (t_fail || ticks >= limit) ? 1 : 0;
}
