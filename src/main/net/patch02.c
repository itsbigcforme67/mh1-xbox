/* patch02 - SLPM_654.95 0x0028CAC0-0x0028CC40 (PatchLoadinDNAS_Init, PatchLoadinDNAS_Main): see patch01.c. Main steps
 * DR_NO_0: copy the received half, request the overlay data, wait for the load, then run the patch (func_A2FD30 applies
 * one step, > 0 done, < 0 failed). Returns DP_end (0 running, 1 nothing to do, 2 patched, -1 failed). */
#include "types.h"

extern u8 patch_buff[];
extern s8 DP_cnt;
extern s8 DP_end;
extern s8 DP_r_no;
extern s8 DR_NO_0;

void *memcpy();
int NetLoadWait();
int Net_put_overlay_data();
int PatchExecCS();
int PatchInitCS();
int func_A2FD30();

void PatchLoadinDNAS_Init()
{
    DR_NO_0 = 0;
    DP_r_no = 0;
    DP_cnt = 0;
    DP_end = 0;
}

int PatchLoadinDNAS_Main()
{
    int r;
    u8 *t = patch_buff + 0x20000;

    if (DP_end != 0) {
        return (s8)(int)DP_end;
    }
    switch (DR_NO_0) {
    case 0:
        if (*(s32 *)(t + 0x14) == 0) {
            DP_end = 1;
        } else {
            DR_NO_0++;
            DP_r_no = 0;
            DP_cnt = 0;
            memcpy(patch_buff + 0x10000, patch_buff, 0x10000);
        case 1:
            DR_NO_0++;
            Net_put_overlay_data(2);
        }
        break;
    case 2:
        if (NetLoadWait(DR_NO_0) == 0) {
            DR_NO_0++;
        case 3:
            r = func_A2FD30(&DP_r_no, &DP_cnt, patch_buff + 0x10000, *(s32 *)(t + 0x14), 0x10000);
            if (r > 0) {
                PatchExecCS(0, 0);
                DR_NO_0 = 0;
                DP_end = 2;
                DP_r_no = 0;
                DP_cnt = 0;
            } else if (r < 0) {
                PatchInitCS();
                DR_NO_0 = 0;
                DP_end = -1;
                DP_r_no = 0;
                DP_cnt = 0;
            }
        }
        break;
    }
    return DP_end;
}
