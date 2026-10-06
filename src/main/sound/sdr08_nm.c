/* sdr08 - SLPM_654.95 0x002145D0-0x00214860: sdr_dmaadr_set (queues three "set DMA address" commands, one per address / size pair, size rounded
 * up to 16 and split into a 24-bit count of 16-byte units and the top bits in the second word) and SdrDmaLoadReq (checks the arguments
 * of a sample data load, needs four free queue slots, queues the load command 0x4D and then the three address commands). The
 * command word is (value & 0xFFFFFF) | (opcode << 24). */
#include "types.h"

typedef struct SNDQUE {
    int cmd;        /* 0x00 command word, < 0 when the slot is free */
    int w1;         /* 0x04 */
    int w2;         /* 0x08 */
} SNDQUE;

extern SNDQUE sndque_tbl[];
extern volatile int sque_w_idx[];   /* unknown size on purpose: lui/addiu access, not gp */

void sdr_dmaadr_set(int addr0, int size0, int addr1, int size1, int addr2, int size2);

void sdr_dmaadr_set(int addr0, int size0, int addr1, int size1, int addr2, int size2)
{
    int idx = sque_w_idx[0];
    SNDQUE *q;

    size0 = (size0 + 0xF) & ~0xF;
    q = &sndque_tbl[idx];
    q->cmd = ((size0 >> 4) & 0xFFFFFF) | 0x4F000000;
    q->w1 = addr0 | (size0 >> 20);
    idx = (idx + 1) % 32;
    size1 = (size1 + 0xF) & ~0xF;
    q = &sndque_tbl[idx];
    q->cmd = ((size1 >> 4) & 0xFFFFFF) | 0x4F000000;
    q->w1 = addr1 | (size1 >> 20);
    idx = (idx + 1) % 32;
    size2 = (size2 + 0xF) & ~0xF;
    q = &sndque_tbl[idx];
    q->cmd = ((size2 >> 4) & 0xFFFFFF) | 0x4F000000;
    q->w1 = addr2 | (size2 >> 20);
    idx = (idx + 1) % 32;
    sque_w_idx[0] = idx;
}

int SdrDmaLoadReq(u32 id, int addr0, int size0, int addr1, int size1, int addr2, int size2)
{
    int idx;
    int n;

    if (id >= 0x1000) {
        return -2;
    }
    if (((addr0 | addr1) | addr2) & 0xF) {
        return -3;
    }
    if (size0 >= 0x1FFFF1 || size1 >= 0x1FFFF1 || size2 >= 0x1FFFF1) {
        return -4;
    }
    idx = sque_w_idx[0];
    n = 4;
    do {
        if (sndque_tbl[idx].cmd >= 0) {
            return -1;
        }
        idx = (idx + 1) % 32;
        n--;
    } while (n > 0);
    sndque_tbl[sque_w_idx[0]].cmd = (((id << 8) & 0xFFFFFF)) | 0x4D000000;
    sque_w_idx[0] = (sque_w_idx[0] + 1) % 32;
    sdr_dmaadr_set(addr0, size0, addr1, size1, addr2, size2);
    return 0;
}
