/* Network loader, one per game (docs/xbox.md "Network loader").
 *
 * The same program is installed as E:\Games\MH1\default.xbe (Evolution X lists it as "Monster Hunter")
 * and as E:\Games\MHG\default.xbe ("Monster Hunter G"; tools/xbox_upload.sh --loader builds both titles
 * and installs both). It serves the game of the folder it was started from: a "game=mh1" / "game=mhg"
 * line in that folder's loader.txt, else the folder's name (MHG: G, anything else: MH1). The game is
 * <folder>\mh1.xbe or mhg.xbe. On each start it:
 *   1. sends the last game log (<folder>\mh1log.txt) and the bug reports not sent yet (<folder>\reports\, the
 *      game's Back + Black reporter, xbox_report.c) to the laptop,
 *   2. downloads the newest game build (/mh1.xbe or /mhg.xbe) when the laptop has a new one,
 *   3. starts the game.
 * Every request names the game (?game=mhg); tools/xbox_serve.py serves both games at once and still
 * answers the old MH1 loader's requests without it. When the game exits it starts this loader again
 * (launch data "MH1X", xbox_log.c); then the loader only sends the log and goes back to the dashboard.
 * (A first version polled the laptop every 3 s for a new build; the Xbox's network froze after 5
 * connections, ping included, cause unknown. So each start makes at most 4 connections, and the next
 * start comes from the pad.)
 * The laptop is found at the address in the first line of <folder>\loader.txt ("192.168.1.166:8765"
 * when the file is missing). Only our own build files travel this way; there is no login.
 * When the laptop runs tools/xbox_serve.py --data FOLDER for this game, /version ends in a "DATA" line
 * and the loader first installs that game's data into its own folder (install.c), resuming where it
 * stopped; never over the loader itself.
 */
#include <hal/debug.h>
#include <hal/video.h>
#include <hal/xbox.h>
#include <nxdk/mount.h>
#include <nxdk/net.h>
#include <lwip/sockets.h>
#include <lwip/netif.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../xnet_http.h"
#include "../xbox_report.h"
#include "install.h"
#include "../xbox_path.h"

extern struct netif *g_pnetif;

/* this loader's folder (with a trailing backslash) and game, set at the start of main */
static char DIR[200] = "E:\\Games\\MH1\\";
static char GAMEID[8] = "mh1";              /* "mh1" / "mhg": the ?game= of every request */
static char GAME[240], GAME_VER[240], GAME_LOG[240], HOSTFILE[240], TEST_INPUT[240], TEST_ENV[240], REPORTS[240];

static void set_paths(void)
{
    snprintf(GAME, sizeof GAME, "%s%s.xbe", DIR, GAMEID);
    snprintf(GAME_VER, sizeof GAME_VER, "%s%s.ver", DIR, GAMEID);
    snprintf(GAME_LOG, sizeof GAME_LOG, "%smh1log.txt", DIR);
    snprintf(REPORTS, sizeof REPORTS, "%sreports", DIR);
    snprintf(TEST_INPUT, sizeof TEST_INPUT, "%sdata\\input.txt", DIR);
    snprintf(TEST_ENV, sizeof TEST_ENV, "%sdata\\env.txt", DIR);
}

/* which folder and game: the folder of this XBE (on E:), the game from a "game=" line in its
 * loader.txt, else from the folder's name */
static void find_game(void)
{
    char dir[200], line[96];
    const char *base;
    FILE *f;
    int found = 0;
    if (xbox_image_dir(dir, sizeof dir, NULL, 0))
        snprintf(DIR, sizeof DIR, "%s\\", dir);
    else
        snprintf(dir, sizeof dir, "%.*s", (int)strlen(DIR) - 1, DIR);   /* not on E: (xemu's DVD): MH1's folder */
    base = strrchr(dir, '\\') ? strrchr(dir, '\\') + 1 : dir;
    snprintf(HOSTFILE, sizeof HOSTFILE, "%sloader.txt", DIR);
    if ((f = fopen(HOSTFILE, "r")) != NULL) {
        while (fgets(line, sizeof line, f))
            if (!strncmp(line, "game=", 5)) {
                line[5 + strcspn(line + 5, "\r\n ")] = 0;
                if (!strcmp(line + 5, "mh1") || !strcmp(line + 5, "mhg")) {
                    snprintf(GAMEID, sizeof GAMEID, "%s", line + 5);
                    found = 1;
                }
            }
        fclose(f);
    }
    if (!found)
        snprintf(GAMEID, sizeof GAMEID, "%s", ((base[0] | 0x20) == 'm' && (base[1] | 0x20) == 'h' &&
                                               (base[2] | 0x20) == 'g' && !base[3]) ? "mhg" : "mh1");
    set_paths();
}

/* "/version" -> "/version?game=mhg" */
static const char *url(const char *path)
{
    static char u[2][160];
    static int k;
    k ^= 1;
    snprintf(u[k], sizeof u[k], "%s?game=%s", path, GAMEID);
    return u[k];
}

static char *read_file(const char *path, int *n)
{
    FILE *f = fopen(path, "rb");
    char *b;
    if (!f)
        return NULL;
    fseek(f, 0, SEEK_END);
    *n = ftell(f);
    fseek(f, 0, SEEK_SET);
    if ((b = malloc(*n + 1)) != NULL) {
        *n = fread(b, 1, *n, f);
        b[*n] = 0;
    }
    fclose(f);
    return b;
}

/* one WriteFile for the whole file: pdclib's fwrite hands the kernel 1 KB at a time, which
 * took ~10 s for the 3.5 MB game in xemu (and the screen sat at "Downloading" all along;
 * on 12/13 Oct the owner saw the box "hang" right there, docs/xbox.md "Loader download froze") */
static int write_file(const char *path, const void *p, int n)
{
    HANDLE h = CreateFileA(path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    DWORD done = 0;
    BOOL ok;
    if (h == INVALID_HANDLE_VALUE)
        return -1;
    ok = WriteFile(h, p, (DWORD)n, &done, NULL);
    return CloseHandle(h) && ok && done == (DWORD)n ? 0 : -1;
}

/* the NIC must be stopped before XLaunchXBE: a program started right after
 * one that left it running hangs in nxNetInit (seen 11 Oct 2026) */
static int net_up;

static void launch(const char *xbe)
{
    if (net_up)
        nxNetShutdown();
    XLaunchXBE(xbe);
}

static void launch_game(void)
{
    FILE *f = fopen(GAME, "rb");
    if (!f) {
        debugPrint("\nNo game build at %s yet, and the laptop did not send one.\n", GAME);
        debugPrint("Back to the dashboard in 30 s.\n");
        Sleep(30000);
        launch(NULL);
    }
    fclose(f);
    debugPrint("Starting the game.\n");
    launch(GAME);
}

/* a test run's scripted input and RT_* switches (tools/xbox_serve.py --input/--env):
 * "MH1TEST <input bytes> <env bytes>" then both; a part of length 0 removes the file,
 * so no test script lingers on the disk for a normal start */
static void fetch_test(void)
{
    int n, ni, ne;
    char *b = xh_http("GET", url("/test"), NULL, 0, &n), *p;
    if (!b)
        return;
    if (sscanf(b, "MH1TEST %d %d", &ni, &ne) == 2 && (p = strchr(b, '\n')) != NULL &&
        ni >= 0 && ne >= 0 && p + 1 + ni + ne == b + n) {
        p++;
        if (ni)
            write_file(TEST_INPUT, p, ni);
        else
            remove(TEST_INPUT);
        if (ne)
            write_file(TEST_ENV, p + ni, ne);
        else
            remove(TEST_ENV);
        if (ni || ne)
            debugPrint("Test run: input %d bytes, env %d bytes.\n", ni, ne);
    }
    free(b);
}

/* The game's save, sent with its log (tools/xbox_serve.py POST /save?game=G -> build/xbox/saves/<time>-<game>/), so the
 * owner's exact save can be loaded on the PC (tools/xbox_save_to_pc.py). The save directory the game writes (mc_xbox.c):
 * E:\UDATA\<title id>\<save id>\ (the PS2 save files byte for byte, plus SaveMeta.xbx). Body: "MH1SAVE <dir> <n>", per
 * file "F <size> <name>" + bytes, "E". Returns 1 sent, 0 no save, -1 failed. Agent E, 21 Oct 2026. */
static int send_save(void)
{
    const char *tid = strcmp(GAMEID, "mhg") ? "4D480001" : "4D480002";      /* xbox_title.h */
    char dir[96], pat[112], path[200], head[160], *body = NULL, *r;
    int len = 0, cap = 0, nf = 0, i, n;
    WIN32_FIND_DATAA fd;
    HANDLE h;
    struct { char name[64]; char *data; int n; } f[8];

    snprintf(dir, sizeof dir, "E:\\UDATA\\%s\\%s00000001", tid, tid);
    snprintf(pat, sizeof pat, "%s\\*", dir);
    if ((h = FindFirstFileA(pat, &fd)) == INVALID_HANDLE_VALUE)
        return 0;
    do {
        if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) || nf >= 8 || strlen(fd.cFileName) >= sizeof f[0].name)
            continue;
        snprintf(path, sizeof path, "%s\\%s", dir, fd.cFileName);
        if ((f[nf].data = read_file(path, &f[nf].n)) == NULL)
            continue;
        snprintf(f[nf].name, sizeof f[nf].name, "%s", fd.cFileName);
        cap += f[nf].n + 96;
        nf++;
    } while (FindNextFileA(h, &fd));
    FindClose(h);
    if (nf == 0)
        return 0;
    cap += 64;
    if ((body = malloc((size_t)cap)) != NULL) {
        len = snprintf(body, (size_t)cap, "MH1SAVE %s00000001 %d\n", tid, nf);
        for (i = 0; i < nf; i++) {
            n = snprintf(head, sizeof head, "F %d %s\n", f[i].n, f[i].name);
            memcpy(body + len, head, (size_t)n);
            len += n;
            memcpy(body + len, f[i].data, (size_t)f[i].n);
            len += f[i].n;
        }
        memcpy(body + len, "E\n", 2);
        len += 2;
    }
    for (i = 0; i < nf; i++)
        free(f[i].data);
    if (!body)
        return -1;
    r = xh_http("POST", url("/save"), body, len, &n);
    free(body);
    free(r);
    return r ? 1 : -1;
}

int main(void)
{
    unsigned long ldt;
    const unsigned char *ld;
    int after_game, n;
    char *log, *ver, *have, *xbe, *data, path[32];
    const char *title;

    XVideoSetMode(640, 480, 32, REFRESH_DEFAULT);
    nxMountDrive('E', "\\Device\\Harddisk0\\Partition1\\");
    after_game = XGetLaunchInfo(&ldt, &ld) == 0 && ldt == LDT_TITLE && ld && !memcmp(ld, "MH1X", 4);
    find_game();
    xh_read_host(HOSTFILE);
    title = strcmp(GAMEID, "mhg") ? "Monster Hunter" : "Monster Hunter G";
    debugPrint("%s network loader (%s)\n", title, DIR);
    if (nxNetInit(NULL) != 0) {
        debugPrint("No network.\n");
        if (after_game)             /* the game just exited: not straight back into it */
            launch(NULL);
        launch_game();
    }
    net_up = 1;
    debugPrint("Xbox %s, laptop %s:%d\n", ip4addr_ntoa(netif_ip4_addr(g_pnetif)), xh_host, xh_port);

    if ((log = read_file(GAME_LOG, &n)) != NULL) {
        char *r = xh_http("POST", url("/log"), log, n, &n);
        debugPrint(r ? "Sent the last game log.\n" : "Could not send the last game log.\n");
        if (r)
            remove(GAME_LOG);
        free(r);
        free(log);
        if ((n = send_save()) != 0)
            debugPrint(n > 0 ? "Sent the save.\n" : "Could not send the save.\n");
    }
    if ((n = xbox_report_send_pending(REPORTS, GAMEID)) != 0)       /* one POST for all of them */
        debugPrint(n > 0 ? "Sent %d bug report(s).\n" : "Could not send the bug reports (kept for the next start).\n", n);

    if (after_game) {
        debugPrint("Game exited. Back to the dashboard.\n");
        Sleep(3000);
        launch(NULL);
    }
    if (!(ver = xh_http("GET", url("/version"), NULL, 0, &n))) {
        debugPrint("The laptop does not answer.\n");
        launch_game();
    }
    data = strstr(ver, "\nDATA");
    ver[strcspn(ver, "\n")] = 0;
    if (data && install_data(DIR, GAMEID) < 0) {
        debugPrint("Back to the dashboard in 60 s.\n");
        Sleep(60000);
        launch(NULL);
    }
    have = read_file(GAME_VER, &n);
    if (strcmp(ver, "none") && (!have || strcmp(have, ver))) {
        DWORD t0 = GetTickCount(), t1;
        debugPrint("Downloading build %s ...\n", ver);
        snprintf(path, sizeof path, "/%s.xbe", GAMEID);
        if ((xbe = xh_http("GET", url(path), NULL, 0, &n)) != NULL) {
            t1 = GetTickCount();
            debugPrint("Got %d bytes in %lu ms. Writing it ...\n", n, (unsigned long)(t1 - t0));
            if (write_file(GAME, xbe, n) == 0 && write_file(GAME_VER, ver, strlen(ver)) == 0)
                debugPrint("Written in %lu ms.\n", (unsigned long)(GetTickCount() - t1));
            else
                debugPrint("Could not write %s; starting the copy on the disk.\n", GAME);
            free(xbe);
        } else
            debugPrint("Download failed; starting the copy on the disk.\n");
    }
    fetch_test();
    launch_game();
}
