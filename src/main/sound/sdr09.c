/* sdr09 - SLPM_654.95 0x00215320-0x0021572C: sending_req, serialises one queued sound driver command (a 12-byte sndque_tbl entry) into
 * the byte buffer: the top byte of the command word (opcode) decides how many bytes of the word and which extra fields are copied
 * (makebuff / makebuff8 / makebuff_ext / makebuff_tq / makebuff_vmix, see sdr05.c). Returns 0 when done (also for the empty / end
 * markers 0x7F and 0xFF), -1 for an unknown opcode group or a full buffer. The meaning of the opcodes is not worked out. */
#include "types.h"

typedef struct SNDQUE {
    int cmd;        /* 0x00 */
    u8 b4;          /* 0x04 */
    u8 b5;          /* 0x05 */
    u16 h6;         /* 0x06 */
    u8 b8;          /* 0x08 */
    u8 pad[3];
} SNDQUE;

int makebuff(u32, int);
int makebuff8(u32, int, int, int, int, int);
int makebuff_ext(u32, int, int);
int makebuff_tq(u32, int, int, int, int);
int makebuff_vmix(SNDQUE *);

int sending_req(SNDQUE *q)
{
    int op;
    int g;
    int w;

    w = q->cmd;
    op = w >> 24;
    if (op == 0x7F) {
        return 0;
    }
    if (op == -1) {
        return 0;
    }
    g = op & 0xF0;
    switch (g) {
    case 0:
        return makebuff_tq(w, q->b4, q->b5, q->h6, q->b8);
    case 0x20:
        if (op & 8) {
            if (op == 0x29) {
                return makebuff(w, 3);
            }
            return makebuff(w, 1);
        }
        if (op == 0x22 || op == 0x20 || op == 0x24 || op == 0x23) {
            return makebuff(w, 3);
        }
        return makebuff(w, 2);
    case 0x30:
    case 0x70:
        if (op == 0x38 || op == 0x30 || op == 0x71 || op == 0x73) {
            return makebuff(w, 2);
        }
        if (op == 0x31 || op == 0x37 || (u32)(op - 0x39) <= 2 || op == 0x72 || op == 0x75) {
            return makebuff(w, 3);
        }
        if (op == 0x3D || op == 0x36 || op == 0x74) {
            return makebuff(w, 4);
        }
        if (op == 0x77 || op == 0x3C || op == 0x3E) {
            return makebuff8(w, 5, q->b4, 0, 0, 0);
        }
        if (op == 0x3F) {
            return makebuff8(w, 7, q->b4, q->b5, q->h6 & 0xFF, 0);
        }
        if (op == 0x35) {
            return makebuff_vmix(q);
        }
        return makebuff(w, 1);
    case 0x40:
        if ((u32)(op - 0x47) <= 3 || op == 0x41 || op == 0x42) {
            return makebuff(w, 2);
        }
        if (op == 0x4B) {
            return makebuff(w, 3);
        }
        if (op == 0x45 || op == 0x4C) {
            return makebuff(w, 4);
        }
        if (op == 0x44) {
            return makebuff8(w, 6, q->b4, q->b5, 0, 0);
        }
        if (op == 0x4D || op == 0x4E) {
            return makebuff_ext(w, 3, 0x15);
        }
        if (op == 0x4F) {
            return makebuff8(*(int *)&q->b4, 6, (w >> 8) & 0xFF, w & 0xFF, 0, 0);
        }
        return makebuff(w, 1);
    case 0x60:
    case 0x50:
        return makebuff(w, 2);
    default:
        return -1;
    }
}
