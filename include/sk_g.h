#ifndef SK_G_H
#define SK_G_H
/* G's soft keyboard state (lpSKey) is 12 bytes bigger than MH1's: the 0x44 string moved +8, everything from 0x140 on +12
 * (offsets read off tools/check.py diffs of hk_*, cmd_delete, kbd_free_set, sk_conv_init). SKO(mh1_offset) = G offset. */
#ifdef VERSION_MH2
/* MH2: the sound request stays 12 up as in G, everything after it is 16 up (kbdExecServer, sk_henkan_sub, yn_kigou_inbuf_set) */
#define SKO(o) ((o) < 0x44 ? (o) : (o) < 0x140 ? (o) + 8 : (o) == 0x140 ? (o) + 12 : (o) + 16)
#elif defined(VERSION_MHG)
#define SKO(o) ((o) < 0x44 ? (o) : (o) < 0x140 ? (o) + 8 : (o) + 12)
#else
#define SKO(o) (o)
#endif
#include "g_alias.h"
#ifdef VERSION_MHG
#define SK_ON(v) ((v) == 1)   /* G tests the conversion flag with == 1 in the cmd / cursor code */
#else
#define SK_ON(v) ((v) != 0)
#endif
#ifdef MHG_PS2
#define kouho_work kouho_work_g   /* G's candidate buffers sit at 0x41E020 */
/* G: the soft keyboard queues its sound (a request word in lpSKey, MH1 offset 0x140) instead of calling se_req */
#define SK_SE(snd) (*(s32 *)(lpSKey + SKO(0x140)) = (snd))
#elif defined(VERSION_MH2)
#define SK_SE(snd) (*(s32 *)(lpSKey + SKO(0x140)) = (snd))
#else
#define SK_SE(snd) se_req(7, (snd), 0)
#endif
#endif
