/* AC97 output for the Xbox: our replacement for nxdk's lib/hal/audio.c (same API,
 * hal/audio.h; nxdk's SDL2 audio driver calls it). Defining XAudioInit/Play/Pause/
 * ProvideSamples here keeps the linker from pulling nxdk's audio.obj out of its library.
 *
 * Why (docs/xbox.md "Audio after a quit and relaunch"): nxdk's driver calls back (SDL's
 * "a buffer is free") only when the PCM-out and the S/PDIF-out DMA have both finished a
 * buffer, and after a "last valid buffer" interrupt it ignores completions until the next
 * interrupt. If the S/PDIF channel does not run (xemu does not play it; on the real Xbox
 * after a quit + relaunch without a power cycle it was left in an unknown state) or the
 * PCM DMA ran dry once, no callback ever comes: the SDL audio thread waits forever and the
 * game is silent ("0 frames played"). Here:
 * - the analog PCM-out channel alone drives the callbacks, one per finished buffer
 *   (counted, so two completions before the DPC runs are two callbacks);
 * - S/PDIF gets the same buffers but is never waited on;
 * - a channel that halted (ran dry) is restarted when new samples come;
 * - init stops both DMA channels and the interrupt from a previous init (SDL's close
 *   calls XAudioInit again), cold-resets the AC-link, resets both bus masters and clears
 *   their status; xbox_ac97_stop() stops the DMA before a quit to the dashboard. */
#include "xbox_watch.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <hal/audio.h>
#include <xboxkrnl/xboxkrnl.h>
#include "../rt/rt_log.h"

#define AUDIO_IRQ 6
/* nForce ACI bus master registers (byte offsets from 0xFEC00000) */
#define PI_SR    0x106          /* PCM in and mic in: not used, reset so no status of theirs raises the interrupt */
#define PI_CR    0x10B
#define MC_SR    0x126
#define MC_CR    0x12B
#define PO_BDBAR 0x110          /* PCM out: descriptor list base */
#define PO_LVI   0x115          /* last valid index */
#define PO_SR    0x116          /* status: 0 DCH halted, 2 LVBCI, 3 BCIS, 4 FIFOE */
#define PO_CR    0x11B          /* control: 0 run, 1 reset, 2 LVBIE, 3 IOCE, 4 FEIE */
#define SP_BDBAR 0x170          /* S/PDIF out, the same layout */
#define SP_LVI   0x175
#define SP_SR    0x176
#define SP_CR    0x17B
#define GLOB_CNT 0x12C          /* bit 1: AC-link cold reset (0 = in reset) */
#define GLOB_STA 0x130          /* bit 8: primary codec ready */
#define CR_RUN   0x1D           /* run + interrupts on completion, last buffer, FIFO error */
#define CR_PAUSE 0x1C

AC97_DEVICE ac97Device;
static KINTERRUPT irq_obj;
static KDPC dpc_obj;
static bool irq_connected, playing, dpc_ready;
/* codec register access: only when GLOB_CNT kept the cold-reset bit we set (real hardware).
 * xemu reads GLOB_CNT as 0 and hangs on a codec register access (13 Oct 2026), so there
 * the codec is left alone. RT_AC97_NOCODEC=1 skips it everywhere (test aid). */
static bool codec_access;
/* no codec answered (GLOB_STA bit 8 never set, even after a second cold reset): no codec register is touched
 * (a read of an absent codec stalled the CPU: the owner's black screen at "ac97 open" after a Back + Start relaunch,
 * 21 Oct 2026, GLOB_STA 00308000) and no DMA runs; the game goes on silent */
static bool ac97_dead;
#define STEP(...) (rt_log("ac97: " __VA_ARGS__), rt_log_commit())     /* to the disk at once: a hang leaves the last step */

/* a time-out for the waits on the controller: KeTickCount is in ms */
static bool past(DWORD t0, DWORD ms) { return KeTickCount - t0 >= ms; }
static volatile LONG done_pending;      /* finished PCM-out buffers not yet called back */
static volatile unsigned long n_isr, n_bcis, n_cb;      /* for xbox_ac97_dump */

static volatile unsigned char *mmio8(void) { return (volatile unsigned char *)0xFEC00000; }
static volatile unsigned int *mmio32(int off) { return (volatile unsigned int *)(0xFEC00000 + off); }

static void __stdcall dpc(PKDPC d, PVOID ctx, PVOID a1, PVOID a2)
{
    (void)d; (void)ctx; (void)a1; (void)a2;
    while (done_pending > 0) {
        InterlockedDecrement(&done_pending);
        n_cb++;
        if (ac97Device.callback)
            ac97Device.callback(&ac97Device, ac97Device.callbackData);
    }
}

/* an interrupt on the AC97's line that none of the known status bits explains (IRQ 6 is the ACI's alone): with a level
 * interrupt, "not mine" (FALSE) brings it straight back for ever and the console hangs with a black screen. So every
 * ACI interrupt enable is switched off instead (the bus masters' CR, GIE): the game goes on silent rather than hanging,
 * and the log says so (xbox_ac97_dump). A guard for the owner's black screen on a warm relaunch (Oct 2026), whose
 * cause is not known. */
static volatile unsigned long n_unknown;
static volatile unsigned unknown_sta;
static BOOLEAN __stdcall isr(PKINTERRUPT intr, PVOID ctx)
{
    volatile unsigned char *pb = mmio8();
    unsigned char a = pb[PO_SR] & 0x1C, s = pb[SP_SR] & 0x1C;
    (void)intr; (void)ctx;
    if (!a && !s) {
        unsigned g = *mmio32(GLOB_STA) & 0xC01;   /* GPI change / codec resume interrupts (write 1 to clear): not ours, but a
                                                   * level interrupt nobody clears would come back for ever */
        unsigned char pi = pb[PI_SR] & 0x1C, mc = pb[MC_SR] & 0x1C;
        if (!g && !pi && !mc) {
            if (n_unknown++ == 0)
                unknown_sta = *mmio32(GLOB_STA);
            pb[PO_CR] = 0;
            pb[SP_CR] = 0;
            pb[PI_CR] = 0;
            pb[MC_CR] = 0;
            *mmio32(GLOB_CNT) &= ~1u;
            if (n_unknown > 100)
                ac97_dead = true;       /* it keeps coming (XAudioPlay switches them on again): audio off for this run */
            return TRUE;
        }
        *mmio32(GLOB_STA) = g;
        pb[PI_SR] = pi;
        pb[MC_SR] = mc;
        return TRUE;
    }
    if (a)
        pb[PO_SR] = a;                  /* write-1-to-clear */
    if (s)
        pb[SP_SR] = s;
    n_isr++;
    if (a & 0x08) {                     /* a PCM-out buffer finished */
        n_bcis++;
        InterlockedIncrement(&done_pending);
        KeInsertQueueDpc(&dpc_obj, NULL, NULL);
    }
    return TRUE;
}


/* the codec's registers (primary codec, mapped at 0x000-0x07F); an access waits for the
 * codec access semaphore (GLOB 0x134 bit 0: reading it takes the semaphore) */
static int cas_wait(void)
{
    DWORD t0 = KeTickCount;
    while (*mmio32(0x134) & 1)
        if (past(t0, 20))
            return 0;
    return 1;
}
static unsigned codec_rd(int reg)
{
    cas_wait();
    return *(volatile unsigned short *)(0xFEC00000 + reg);
}
static void codec_wr(int reg, unsigned v)
{
    cas_wait();
    *(volatile unsigned short *)(0xFEC00000 + reg) = (unsigned short)v;
}

/* the controller and codec state, into the game log (on open, and when the movie finds the
 * device not playing: rt_movie.c) */
void xbox_ac97_dump(const char *when)
{
    volatile unsigned char *pb = mmio8();
    rt_log("ac97 %s: GLOB_CNT %08X GLOB_STA %08X | PO civ %u lvi %u sr %04X picb %u cr %02X | "
           "SP civ %u lvi %u sr %04X picb %u cr %02X | isr %lu bcis %lu cb %lu",
           when, *mmio32(GLOB_CNT), *mmio32(GLOB_STA),
           pb[0x114], pb[PO_LVI], *(volatile unsigned short *)(pb + PO_SR), *(volatile unsigned short *)(pb + 0x118), pb[PO_CR],
           pb[0x174], pb[SP_LVI], *(volatile unsigned short *)(pb + SP_SR), *(volatile unsigned short *)(pb + 0x178), pb[SP_CR],
           n_isr, n_bcis, n_cb);
    if (n_unknown)
        rt_warn("ac97 %s: %lu interrupt(s) with no known cause (first with GLOB_STA %08X): the AC97's interrupts were switched "
                "off (silent rather than a hang)", when, n_unknown, unknown_sta);
    if (!codec_access)
        rt_log("ac97 %s: codec registers not read (GLOB_CNT without the cold-reset bit: xemu)", when);
    else
    rt_log("ac97 %s: codec 00 %04X master 02 %04X pcm 18 %04X pwr 26 %04X ext 2A %04X dac 2C %04X",
           when, codec_rd(0x00), codec_rd(0x02), codec_rd(0x18), codec_rd(0x26), codec_rd(0x2A), codec_rd(0x2C));
}

/* after a cold reset: everything powered, master and PCM out unmuted at 0 dB, the DAC at
 * 48 kHz (variable rate off: the fixed 48 kHz) */
static void codec_setup(void)
{
    DWORD t0 = KeTickCount;
    codec_wr(0x26, 0x0000);             /* power-down bits PR0-PR6 cleared */
    while ((codec_rd(0x26) & 0x000F) != 0x000F && !past(t0, 200)) {     /* ADC/DAC/analog/ref ready */
        LARGE_INTEGER t;
        t.QuadPart = -10 * 1000;
        KeDelayExecutionThread(KernelMode, FALSE, &t);
    }
    codec_wr(0x2A, codec_rd(0x2A) & ~1u);       /* VRA off */
    codec_wr(0x02, 0x0000);             /* master */
    codec_wr(0x04, 0x0000);             /* headphone / aux out */
    codec_wr(0x06, 0x0000);             /* mono out */
    codec_wr(0x18, 0x0808);             /* PCM out */
}

static bool bm_reset(int cr, int sr)
{
    volatile unsigned char *pb = mmio8();
    DWORD t0 = KeTickCount;
    bool ok = true;
    pb[cr] = 0;                         /* stop (and no interrupts) */
    while (!(pb[sr] & 0x01) && !past(t0, 50))  /* DCH: the DMA engine halted */
        ;
    pb[cr] = 0x02;                      /* reset the bus master */
    t0 = KeTickCount;
    while (pb[cr] & 0x02)
        if (past(t0, 50)) {
            ok = false;
            break;
        }
    pb[cr] = 0;
    pb[sr] = 0x1E;                      /* clear any status left */
    return ok;
}

/* stop both DMA channels (before a quit to the dashboard: nothing left running into the
 * next program), interrupts off */
void xbox_ac97_stop(void)
{
    /* run bits off, wait for the halt, reset both bus masters, interrupts masked (CR 0) and status cleared:
     * the next program (the loader, then the game again without a power cycle) finds the controller idle */
    bm_reset(PO_CR, PO_SR);
    bm_reset(SP_CR, SP_SR);
    if (irq_connected) {
        KeDisconnectInterrupt(&irq_obj);
        irq_connected = false;
    }
    playing = false;
}

void XAudioInit(int sampleSizeInBits, int numChannels, XAudioCallback callback, void *data)
{
    xbox_watch_mark("audio: AC97 start (XAudioInit)");
    AC97_DEVICE *d = &ac97Device;
    LARGE_INTEGER t;
    KIRQL irql;
    ULONG vector;
    int n = 0;

    STEP("init: GLOB_CNT %08X GLOB_STA %08X PO cr %02X sr %04X SP cr %02X sr %04X", *mmio32(GLOB_CNT), *mmio32(GLOB_STA),
         mmio8()[PO_CR], *(volatile unsigned short *)(mmio8() + PO_SR), mmio8()[SP_CR], *(volatile unsigned short *)(mmio8() + SP_SR));
    xbox_ac97_stop();
    bm_reset(PI_CR, PI_SR);
    bm_reset(MC_CR, MC_SR);
    *mmio32(GLOB_CNT) &= ~1u;           /* GIE off: no GPI / resume interrupts (a previous program may have left it on) */
    *mmio32(GLOB_STA) = 0xC01;          /* their status cleared */
    STEP("init: previous DMA stopped");
    if (dpc_ready)
        KeRemoveQueueDpc(&dpc_obj);
    done_pending = 0;
    MmLockUnlockBufferPages(d, sizeof *d, FALSE);
    d->mmio = (unsigned int *)0xFEC00000;
    d->nextDescriptor = 0;
    d->callback = callback;
    d->callbackData = data;
    d->sampleSizeInBits = sampleSizeInBits;
    d->numChannels = numChannels;
    memset((void *)d->pcmOutDescriptor, 0, sizeof d->pcmOutDescriptor);
    memset((void *)d->pcmSpdifDescriptor, 0, sizeof d->pcmSpdifDescriptor);

    STEP("init: AC-link cold reset");
    /* AC-link cold reset, then up to 250 ms for the codec; once more with a longer reset if it did not answer */
    for (n = 0; n < 2; n++) {
        DWORD t0;
        *mmio32(GLOB_CNT) &= ~2u;
        t.QuadPart = -10 * 1000 * (n ? 20 : 1);     /* 1 ms, the second time 20 ms */
        KeDelayExecutionThread(KernelMode, FALSE, &t);
        *mmio32(GLOB_CNT) |= 2u;
        t0 = KeTickCount;
        while (!(*mmio32(GLOB_STA) & 0x100) && !past(t0, 250)) {
            t.QuadPart = -10 * 1000;
            KeDelayExecutionThread(KernelMode, FALSE, &t);
        }
        if ((*mmio32(GLOB_STA) & 0x100) || !(*mmio32(GLOB_CNT) & 2))   /* ready, or xemu (GLOB_CNT reads 0) */
            break;
        rt_warn("ac97: no codec ready after a cold reset (GLOB_STA %08X)%s", *mmio32(GLOB_STA),
                n ? ": audio off for this run" : ", resetting again");
    }
    ac97_dead = (*mmio32(GLOB_CNT) & 2) && !(*mmio32(GLOB_STA) & 0x100);

    *mmio32(GLOB_STA) = 0xC01;          /* the reset's resume interrupt status */
    codec_access = !ac97_dead && (*mmio32(GLOB_CNT) & 2) && !getenv("RT_AC97_NOCODEC");
    STEP("init: codec %s (GLOB_STA %08X)", ac97_dead ? "absent" : codec_access ? "ready, setting up" : "not accessed", *mmio32(GLOB_STA));
    if (codec_access)
        codec_setup();
    STEP("init: bus masters");
    if (!bm_reset(PO_CR, PO_SR) || !bm_reset(SP_CR, SP_SR)) {
        rt_warn("ac97: a bus master did not leave reset: audio off for this run");
        ac97_dead = true;
    }
    *mmio32(0x100) = 0;                 /* no PCM in */
    *mmio32(PO_BDBAR) = MmGetPhysicalAddress((PVOID)d->pcmOutDescriptor);
    *mmio32(SP_BDBAR) = MmGetPhysicalAddress((PVOID)d->pcmSpdifDescriptor);

    vector = HalGetInterruptVector(AUDIO_IRQ, &irql);
    KeInitializeDpc(&dpc_obj, dpc, NULL);
    dpc_ready = true;
    KeInitializeInterrupt(&irq_obj, isr, NULL, vector, irql, LevelSensitive, FALSE);
    if (callback && !ac97_dead) {       /* (SDL's close calls this with none: stay stopped) */
        STEP("init: interrupt %lu connect", (unsigned long)vector);
        KeConnectInterrupt(&irq_obj);
        irq_connected = true;
        XAudioPause();
        STEP("init: done (interrupts so far: %lu, unexplained %lu)", n_isr, n_unknown);
    }
}

void XAudioPlay(void)
{
    if (ac97_dead)
        return;
    mmio8()[PO_CR] = CR_RUN;
    mmio8()[SP_CR] = CR_RUN;
    playing = true;
}

void XAudioPause(void)
{
    if (ac97_dead)
        return;
    mmio8()[PO_CR] = CR_PAUSE;
    mmio8()[SP_CR] = CR_PAUSE;
    playing = false;
}

void XAudioProvideSamples(unsigned char *buffer, unsigned short bufferLength, int isFinal)
{
    AC97_DEVICE *d = &ac97Device;
    volatile unsigned char *pb = mmio8();
    unsigned short ctl = 0x8000 | (isFinal ? 0x4000 : 0);
    unsigned int addr = MmGetPhysicalAddress((PVOID)buffer);
    int i = d->nextDescriptor;

    d->pcmOutDescriptor[i].bufferStartAddress = addr;
    d->pcmOutDescriptor[i].bufferLengthInSamples = bufferLength / 2;
    d->pcmOutDescriptor[i].bufferControl = ctl;
    d->pcmSpdifDescriptor[i].bufferStartAddress = addr;
    d->pcmSpdifDescriptor[i].bufferLengthInSamples = bufferLength / 2;
    d->pcmSpdifDescriptor[i].bufferControl = ctl;
    pb[PO_LVI] = (unsigned char)i;
    pb[SP_LVI] = (unsigned char)i;
    d->nextDescriptor = (unsigned char)((i + 1) % 32);
    if (playing && !ac97_dead) {        /* a channel that ran dry halted at its last buffer: go on */
        if (pb[PO_SR] & 0x01)
            pb[PO_CR] = CR_RUN;
        if (pb[SP_SR] & 0x01)
            pb[SP_CR] = CR_RUN;
    }
}
