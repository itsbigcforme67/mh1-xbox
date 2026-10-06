/* patch01 - SLPM_654.95 0x0028C750-0x0028C7C8 (PatchInitCS) and 0x0028CAC0-0x0028CC40 (PatchLoadinDNAS_Init,
 * PatchLoadinDNAS_Main): the network patch download state (patch_buff: two 0x10000 byte halves, then the media
 * version string and a 4 character tag at +0x20000). Main steps DR_NO_0: copy the received half, request the
 * overlay data, wait for the load, then run the patch (func_A2FD30 applies one step, > 0 done, < 0 failed).
 * Returns DP_end (0 running, 1 nothing to do, 2 patched, -1 failed). Names are from the symbol table. */
#include "types.h"

extern u8 patch_buff[];
extern u8 MediaVersion[];
extern char lit_98_00385088[];
extern s8 DP_cnt;
extern s8 DP_end;
extern s8 DP_r_no;
extern s8 DR_NO_0;

void *memset();
char *strncpy();
void *memcpy();
int NetLoadWait();
int Net_put_overlay_data();
int PatchExecCS();
int func_A2FD30();

void PatchInitCS()
{
    u8 *t = patch_buff + 0x20000;

    memset(patch_buff, 0, 0x10000);
    memset(patch_buff + 0x10000, 0, 0x10000);
    strncpy(t, MediaVersion, 0xA);
    strncpy(t + 0xA, lit_98_00385088, 4);
    *(s32 *)(t + 0x14) = 0;
}
