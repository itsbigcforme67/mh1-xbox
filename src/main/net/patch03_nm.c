/* NEAR-MATCH, not built: about 130 of 185 instructions differ (register allocation: original key s0, n s1, e s2). */
/* patch03 - SLPM_654.95 0x0028C7D0-0x0028CAB4 (PatchExecCS): applies a downloaded patch image held in patch_buff
 * (second half, +0x10000): 8 byte magic, u32 entry count at +8, then entries from +0xC of
 * { u32 key (arg0 << 4 | arg1), u8 *dest, u32 size, data }. Entries whose key matches are copied to dest: size with
 * bit 31 set means "copy size & 0x7FFFFFFF bytes from the pointer stored after the size", otherwise the data follows
 * inline. Entries are 4 byte aligned. FlushCache(0) at the end. Layout guessed from the code. */
#include "types.h"

extern u8 patch_buff[];
extern u8 MediaVersion[];
extern char lit_200_00385090[];
extern char lit_201_003850A0[];

int strncmp();
char *strncpy();
int printf();
int FlushCache();

void PatchExecCS(arg0, arg1)
int arg0;
int arg1;
{
    u8 *t = patch_buff + 0x20000;
    s32 key = ((arg0 & 0xFF) * 0x10) | (arg1 & 0xFF);
    s32 n;
    s32 j;
    s32 size;
    s32 i;
    u8 *e;
    s32 cnt;
    u8 *src;
    u8 *d;

    if (*(s32 *)(t + 0x14) != 0) {
        n = *(s32 *)(patch_buff + 0x10008);
        e = patch_buff + 0x1000C;
        if (strncmp(patch_buff + 0x10000, lit_200_00385090, 8) == 0) {
            if (n != 0) {
                if (strncmp(MediaVersion, t, 0xA) < 0) {
                    strncpy(MediaVersion, t, 0xA);
                } else {
                    printf(lit_201_003850A0);
                }
            }
            for (i = 0; i < n; i++) {
                s32 k = *(s32 *)e;
                d = *(u8 **)(e + 4);
                e += 8;
                size = *(s32 *)e;
                e += 4;
                if (key == k) {
                    if (size & 0x80000000) {
                        src = *(u8 **)e;
                        cnt = size & 0x7FFFFFFF;
                        e += 4;
                        for (j = 0; j < cnt; j++) {
                            *d++ = *src++;
                        }
                    } else {
                        for (j = 0; j < size; j++) {
                            *d++ = *e++;
                        }
                    }
                } else if (size & 0x80000000) {
                    e += 4;
                } else {
                    e += size;
                }
                e = (u8 *)(((s32)(e + 3) >> 2) * 4);
            }
            FlushCache(0);
        }
    }
}
