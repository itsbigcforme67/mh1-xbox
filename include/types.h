#ifndef TYPES_H
#define TYPES_H
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;
typedef int s32;
typedef float f32;
/* Version macros. The shared C is written with MH1's numbers; these swap in another game's number at compile time.
 * - MH2V(mh1, mh2): the first value for MH1 *and G*, the second for MH2. Nested forms like MH2V(GV(a, g), b) are G inside MH1's slot.
 * - MH2_OFS(p, o) (mh2_ofs.h, generated): an MH1 byte offset o into *p, as MH2 has it. It picks the map by sizeof(*p):
 *   0x970 = MH2's PLW (MH2_OFS_PLW), 0x9D0 = MH2's EMW (MH2_OFS_EMW); any other size keeps o. MH2_EM(o) = MH2_OFS_EMW(o)
 *   where there is no pointer.
 * - One macro per global (mh2_lay_ofs.h, generated): QWO quest_w, NCO net_common_w, UDMO User_data, LBM lb_sys, PMO PitMenu.
 *   CNO is CnetSys_w (lbnet.h for G, cnet_mh2.h for MH2); GWX / MH2_GW game_w (gwx.h, G and MH2).
 * - G-only maps of the same kind: SKO soft keyboard (sk_g.h), YNO yn work (yn.h), IBO lobby item box (lbsys_g.h), GV/GSLOT (g_alias.h).
 * - Outside MH2 (the #else branch) all the MH2 macros are the plain value, for MH1, G and every PC build without VERSION_MH2.
 * - Rule (docs/mh2.md 5.3): inside any of these, write the MH1 offset once. CNO(MH2V(a, b)) moves it twice. */
#ifdef VERSION_MH2
#include "mh2_ofs.h"
#define MH2V(mh1, mh2) (mh2)    /* a constant MH2 changed */
#define MH2_EM(o) MH2_OFS_EMW(o)  /* MH2 offset of an EMW field where there is no pointer (pads of local partial EMW copies, emw01.c) */
typedef u16 stg_t;              /* stage number: 16 bits in MH2 */
typedef u16 qno_t;              /* quest number in copies of quest_w's head: MH2 reads it as unsigned 16-bit */
#include "mh2_lay_ofs.h"
#else
#define MH2_OFS(p, o) (o)
#define MH2V(mh1, mh2) (mh1)
#define MH2_EM(o) (o)
typedef u8 stg_t;
typedef s16 qno_t;
#define QWO(o) (o)
#define UDMO(o) (o)
#define NCO(o) (o)
#define LBM(o) (o)
#define PMO(o) (o)
#endif
#endif
