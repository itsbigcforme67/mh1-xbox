/* Differences between the MH1 sources and PS2 Monster Hunter G that the shared C can express with a macro
 * (include first; empty for MH1, the MH1 PC build and the G PC build). Values come from check.py diffs against G.
 * MHG_PS2: the PS2 compiler building G (the G PC build keeps the MH1 values for things its runtime owns). */
#ifndef G_ALIAS_H
#define G_ALIAS_H
#if defined(VERSION_MHG) && defined(__MWERKS__)
#define MHG_PS2 1
/* G renumbered the texture slots: MH1 value, G value (menu slots move by 0x27, the 0x15x group by 0x29) */
#define GSLOT(mh1, g) (g)
/* any other constant that differs in G (sound effect numbers, counts, ...) */
#define GV(mh1, g) (g)
/* G reads these strings through a pointer cell in its data (the MH1 sources use the array itself); the symbols are
 * PROVIDEd at the G addresses in config/mhg/g_names.txt, so only the PS2 G build uses them */
extern void *gptr_002EC620[];
extern void *gptr_002EC62C[];
extern void *gptr_00527ED4[];
extern void *gptr_00528410[];
extern void *gptr_00528414[];
extern void *gptr_00528418[];
extern void *gptr_0052841C[];
extern void *gptr_00528420[];
extern void *gptr_00528424[];
extern void *gptr_005262BC[];
extern void *gptr_00526324[];
extern void *gptr_00527BB0[];
extern void *gptr_00527BF4[];
#define GSTR(mh1, cell) (cell[0])
#else
#define GSTR(mh1, cell) (mh1)
#define GSLOT(mh1, g) (mh1)
#if defined(VERSION_MHG) && defined(MHG_NET)
/* the G PC online build's lobby-server client (lbnet.h, cnlbs01/03): G's command indices and work sizes, as on the
 * PS2 (the PC imports G's command tables and CnetSys_w from G's lobby.bin; docs/network.md "G online") */
#define GV(mh1, g) (g)
#else
#define GV(mh1, g) (mh1)
#endif
#endif
#endif
