/* xemu test aid (tools/xemu_loader_test.sh, tools/xemu_relaunch_test.sh): boots from the test DVD and
 * starts the XBE named in the first line of E:\launch.txt (e.g. E:\Games\MHG\default.xbe), so the
 * network loader runs from its folder on the hard disk as on the real Xbox (Evolution X starts it from
 * there). It takes that line out of the file first (the file goes when it is empty). Installed as the
 * scratch disk's C:\xboxdash.xbe it is also the "dashboard" that a program going back to the dashboard
 * (XLaunchXBE(NULL)) reaches: it then starts the next line, as the owner picking the game again would;
 * with no line left it only says so and waits, so a test run ends there instead of looping. (Without a
 * dashboard on C: xemu stops at the kernel's error 21 screen.) Never installed on hardware. */
#include <hal/debug.h>
#include <hal/video.h>
#include <hal/xbox.h>
#include <nxdk/mount.h>
#include <windows.h>
#include <stdio.h>
#include <string.h>

/* "copy SRC DST" lines before the XBE line: a file copied (written through) before that start, e.g. the next start's
 * pad script and env.txt into the game's data folder (tools/xemu_relaunch_test.sh --kit: no loader serves them) */
static void copy_file(const char *src, const char *dst)
{
    static char buf[4 << 20];       /* (a game log of a long session: a few hundred KB) */
    FILE *f = fopen(src, "rb");
    HANDLE h;
    DWORD done;
    size_t n;
    if (!f) {
        debugPrint("testlaunch: no %s\n", src);
        return;
    }
    n = fread(buf, 1, sizeof buf, f);
    fclose(f);
    h = CreateFileA(dst, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH, NULL);
    if (h == INVALID_HANDLE_VALUE) {
        debugPrint("testlaunch: cannot write %s\n", dst);
        return;
    }
    WriteFile(h, buf, (DWORD)n, &done, NULL);
    CloseHandle(h);
    debugPrint("testlaunch: copied %s to %s\n", src, dst);
}

int main(void)
{
    static char rest[2048];
    char path[200] = "";
    size_t n = 0;
    FILE *f;

    XVideoSetMode(640, 480, 32, REFRESH_DEFAULT);
    nxMountDrive('E', "\\Device\\Harddisk0\\Partition1\\");
    if ((f = fopen("E:\\launch.txt", "r")) != NULL) {
        while (fgets(path, sizeof path, f)) {
            char a[96], b[96];
            path[strcspn(path, "\r\n")] = 0;
            if (sscanf(path, "copy %95s %95s", a, b) == 2) {
                copy_file(a, b);
                path[0] = 0;
                continue;
            }
            if (path[0])
                break;
        }
        n = fread(rest, 1, sizeof rest - 1, f);
        fclose(f);
        remove("E:\\launch.txt");
        if (n) {                        /* the lines after it: the next boots (written through: a reboot follows) */
            HANDLE h = CreateFileA("E:\\launch.txt", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
                                   FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH, NULL);
            DWORD done;
            if (h != INVALID_HANDLE_VALUE) {
                WriteFile(h, rest, (DWORD)n, &done, NULL);
                CloseHandle(h);
            }
        }
    }
    if (!path[0]) {
        debugPrint("testlaunch: no E:\\launch.txt (any more): the test run is over.\n");
        for (;;)
            Sleep(1000);
    }
    debugPrint("testlaunch: starting %s\n", path);
    Sleep(500);
    XLaunchXBE(path);
    debugPrint("testlaunch: could not start it.\n");
    for (;;)
        Sleep(1000);
}
