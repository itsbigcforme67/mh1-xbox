/* netdev11 - InetConnectAllCore (SLPM_654.95 0x00238110-0x002381F0): logs the connection arguments (InetDbgPrint) and dispatches to the step
   handler prot_jmp_tbl_362[*a] (prot_00 ... with the same eight arguments). Parameter types are guesses from use. Written new in this pass. */
#include "types.h"
extern char lit_365_0036CF10[];
extern void (*prot_jmp_tbl_362[])();
int InetDbgPrint();
void InetConnectAllCore(s8 *a, s8 *b, s8 *c, s16 *d, void *e, void *f, void *g, s8 *h) {
    InetDbgPrint(1, 0x1B, 1, lit_365_0036CF10, *d, *a, *b, *c, *h);
    prot_jmp_tbl_362[*a](a, b, c, d, e, f, g, h);
}
