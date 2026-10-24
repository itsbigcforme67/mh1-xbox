/* src/ps2patch/launch_hook.c - the in-game half of the memory-card launcher (docs/network.md 3.8, docs/PLAYING.md).
 * Our own code. Built with the ps2sdk EE gcc by tools/ps2launch/build.sh together with online.c (our replacement of
 * internet_browser, renamed mh1l_internet_browser) into one blob that the launcher (launcher.c) copies into the dead
 * body of ms_net_patch_set (main 0x2687E0, 6920 bytes; never run against our server, which sends no patches; its
 * first two instructions become "return 1" so it stays harmless). The blob is linked at 0x268800 with the game's
 * addresses (tools/ps2launch/syms.ld); the game's code is MWCC EABI, the blob gcc n32: the same registers for up to
 * four integer arguments, s0-s7 / sp / ra saved the same way, no small data (-G0).
 *
 * mh1l_hook replaces the call of PatchExecCS in Load_overlay (main 0x23E5EC: every overlay load). It runs PatchExecCS
 * as before, then, if the overlay just loaded is the lobby (lobby.bin: CRC32 of the original internet_browser,
 * 720 bytes at 0x5B5980, as on the owner's disc), turns its first instruction into a jump to mh1l_internet_browser.
 * So the lobby is patched at every load (also after a hunt), the disc and the card stay untouched.
 * Monster Hunter G gets its own copy of this blob (the same C, linked with G's addresses: tools/ps2launch/syms_mhg.ld,
 * at 0x2130D0 inside G's ms_net_patch_set, 0x2130B0-0x214754). */
#include "types.h"
#include "ps2patch.h"

#ifndef IB_ADDR                    /* MH1; tools/ps2launch/build.sh passes G's for the G blob (-DIB_ADDR=...) */
#define IB_ADDR 0x5B5980u           /* internet_browser (lobby.bin, loaded at 0x533980) */
#define IB_SIZE 720u
#define IB_CRC 0xDD22F920u          /* CRC32 of those 720 bytes in SLPM-65495's lobby.bin (a checksum, not the bytes) */
#endif
/* G (SLPM-65869): internet_browser at 0x5A2910 (lobby.bin loaded at 0x542600), 768 bytes, CRC32 0x9482CFD7; the
 * same steps as MH1's (one more MMBB_LOGIN case that our fill never takes), the same bsCsvWork layout (tcp_init and
 * create_server_table are identical in G) */

void PatchExecCS(int slot, int ov);
s32 mh1l_internet_browser(void);

static void flush_caches(void)
{
    /* FlushCache(0) = write back the data cache, FlushCache(2) = invalidate the instruction cache (syscall 0x64) */
    __asm__ volatile("li $3, 0x64\n\tmove $4, $0\n\tsyscall\n\tnop\n\t"
                     "li $3, 0x64\n\tli $4, 2\n\tsyscall\n\tnop" ::: "$3", "$4", "memory");
}

static u32 crc32_mem(const u8 *p, u32 n)
{
    u32 c = 0xFFFFFFFFu;
    int k;
    while (n--) {
        c ^= *p++;
        for (k = 0; k < 8; k++)
            c = (c >> 1) ^ (0xEDB88320u & -(c & 1));
    }
    return ~c;
}

/* counts lobby patches (read by tests through PCSX2's memory, if needed) */
u32 mh1l_lobby_patched __attribute__((section(".data"))) = 0;

void mh1l_hook(int slot, int ov)
{
    volatile u32 *ib = (volatile u32 *)IB_ADDR;
    PatchExecCS(slot, ov);
    if (crc32_mem((const u8 *)IB_ADDR, IB_SIZE) == IB_CRC) {
        ib[0] = 0x08000000u | (((u32)mh1l_internet_browser >> 2) & 0x03FFFFFFu);    /* j mh1l_internet_browser */
        ib[1] = 0;                                                                    /* nop */
        mh1l_lobby_patched++;
        flush_caches();
    }
}
