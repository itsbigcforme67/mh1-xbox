/* xemu test aid (tools/xemu_loader_test.sh, tools/xemu_relaunch_test.sh): boots from the test DVD and
 * starts the XBE named in the first line of E:\launch.txt (e.g. E:\Games\MHG\default.xbe), so the
 * network loader runs from its folder on the hard disk as on the real Xbox (Evolution X starts it from
 * there). It takes that line out of the file first (the file goes when it is empty): a program that goes
 * "back to the dashboard" makes xemu boot this DVD again, which then starts the next line, as the owner
 * picking the game again in the dashboard would; with no line left it only says so and waits, so a test
 * run ends there instead of looping. (The launch data type can't tell the boots apart: xemu's cold boot
 * already reports 2.) Never installed on hardware. */
#include <hal/debug.h>
#include <hal/video.h>
#include <hal/xbox.h>
#include <nxdk/mount.h>
#include <windows.h>
#include <stdio.h>
#include <string.h>

int main(void)
{
    static char rest[2048];
    char path[200] = "";
    size_t n = 0;
    FILE *f;

    XVideoSetMode(640, 480, 32, REFRESH_DEFAULT);
    nxMountDrive('E', "\\Device\\Harddisk0\\Partition1\\");
    if ((f = fopen("E:\\launch.txt", "r")) != NULL) {
        if (fgets(path, sizeof path, f))
            path[strcspn(path, "\r\n")] = 0;
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
