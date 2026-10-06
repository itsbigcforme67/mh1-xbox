/* power01 - SLPM_654.95 0x00290C60-0x00290E7C: PS2 HDD/console power-off handling (ps2HddPowerOffSet starts a thread
 * that waits for the power-off interrupt, then shuts the HDD down and powers the console off) and Quest_price_return
 * (give back the quest fee that was held). Names of the sce* calls are from the symbol table; the semaphore/thread
 * parameter blocks are guesses from the stores. */
#include "types.h"

extern char lit_232_00386410[];
extern char lit_233_00386418[];
extern char lit_234_00386420[];
extern char lit_235_00386428[];
extern u8 stack[];
extern s32 quest_price;
extern char _gp;

int WaitSema();
int sceSifSearchModuleByName();
int sceDevctl();
int sceCdPowerOff();
int ChangeThreadPriority();
int CreateSema();
int CreateThread();
int GetThreadId();
int StartThread();
int sceCdPOffCallback();
int iSignalSema();
void Gold_add();
void PowerOffHandler();
void PowerOffThread();

typedef struct SEMA_PARAM {
    s32 x00;
    s32 x04;
    s32 x08;
    s32 x0C;
    s32 x10;
    s32 x14;
    s32 pad[2];
} SEMA_PARAM;

typedef struct THREAD_PARAM {
    s32 x00;
    void *func;     /* 0x04 */
    void *stack;    /* 0x08 */
    s32 stack_size; /* 0x0C */
    void *gp;       /* 0x10 */
    s32 prio;       /* 0x14 */
    s32 pad[6];
} THREAD_PARAM;

int ps2HddPowerOffSet()
{
    int sema;
    THREAD_PARAM tp;
    SEMA_PARAM sp;

    sp.x04 = 1;
    sp.x08 = 0;
    sp.x14 = 0;
    sema = CreateSema(&sp);
    ChangeThreadPriority(GetThreadId(), 2);
    tp.stack_size = 0x2000;
    tp.gp = &_gp;
    tp.func = PowerOffThread;
    tp.stack = stack;
    tp.prio = 1;
    StartThread(CreateThread(&tp), sema);
    sceCdPOffCallback(PowerOffHandler, sema);
    return 1;
}

void PowerOffThread()
{
    int sp1C;

    WaitSema();
    if (sceSifSearchModuleByName(lit_232_00386410) >= 0) {
        sp1C = sceDevctl(lit_233_00386418, 0x4807, 0, 0, 0, 0);
        switch (sp1C) {
        case 3:
            do {
            } while (sceDevctl(lit_234_00386420, 0x4402, 0, 0, 0, 0) < 0);
            break;
        default:
            sceDevctl(lit_235_00386428, 0x5003, 0, 0, 0, 0);
            do {
            } while (sceDevctl(lit_233_00386418, 0x4806, 0, 0, 0, 0) < 0);
            break;
        }
    } else {
        do {
        } while (sceDevctl(lit_234_00386420, 0x4402, 0, 0, 0, 0) < 0);
    }
    do {
    } while (!sceCdPowerOff(&sp1C) || sp1C != 0);
}

void PowerOffHandler()
{
    iSignalSema();
}

void Quest_price_return()
{
    if (quest_price != 0) {
        Gold_add(quest_price);
        quest_price = 0;
    }
}
