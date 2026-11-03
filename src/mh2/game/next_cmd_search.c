/* Monster Hunter 2: next_cmd_search, the length table of the monster command interpreter. Written from our own
   disassembly of the owner's disc. Built alone for the byte match; em_cmd_r23.c includes it for the PC. */
#include "em_cmd.h"

/* game 0x7262D0, called with what is in the registers. The real one stays in the asm (config/mh2/hand_parts.txt); it has
   to be here, static, because the original keeps p in a1 across this call and MWCC only does that for a same-file static. */
static void game_func_007262D0(EMW *em, u8 *p, int v) {
    ((u8 *)em)[0x95E] = v;
}

u8 *next_cmd_search(EMW *em, u8 *p) {
    switch (*p) {
    case 0x1:
        p += 2;
        break;
    case 0x2:
        p += 2;
        break;
    case 0x3:
        p += 2;
        break;
    case 0x4:
        p += 1;
        break;
    case 0x5:
        p += 4;
        break;
    case 0x6:
        if (p[1] == 3) {
            p += 5;
        } else {
            p += 4;
        }
        break;
    case 0x7:
        p += 2;
        break;
    case 0x8:
        p += 2;
        break;
    case 0x1F:
        p += 2;
        break;
    case 0x9:
        p += 2;
        break;
    case 0xA:
        p += 2;
        break;
    case 0xB:
        p += 1;
        switch (*p) {
        case 0:
            p += 2;
            break;
        case 1:
        case 2:
            p += 1;
            break;
        }
        break;
    case 0xC:
        p += 3;
        break;
    case 0xD:
        p += 2;
        break;
    case 0xE:
        p += 1;
        switch (*p) {
        case 0:
            p += 3;
            break;
        case 1:
        case 2:
            p += 1;
            break;
        }
        break;
    case 0xF:
        p += 6;
        break;
    case 0x10:
        p += 1;
        break;
    case 0x11:
        p += 1;
        break;
    case 0x12:
        p += 1;
        break;
    case 0x13:
        p += 1;
        break;
    case 0x14:
        p += 1;
        switch (*p) {
        case 0:
            p += 2;
            break;
        case 1:
        case 2:
            p += 1;
            break;
        }
        break;
    case 0x15:
        p += 1;
        switch (*p++) {
        case 0:
            p += 3;
        case 1:
            p += 2;
            break;
        case 2:
        case 3:
            break;
        }
        break;
    case 0x16:
        p += 2;
        break;
    case 0x17:
        p += 5;
        break;
    case 0x18:
        p += 1;
        break;
    case 0x19:
        p += 1;
        break;
    case 0x1A:
        p += 3;
        break;
    case 0x1B:
        p += 1;
        switch (*p) {
        case 0:
            p += 2;
            break;
        case 1:
        case 2:
            p += 1;
            break;
        }
        break;
    case 0x1C:
        p += 1;
        switch (*p++) {
        case 0:
            p += 3;
        case 1:
            p += 1;
            break;
        case 2:
        case 3:
            break;
        }
        break;
    case 0x1D:
        p += 1;
        switch (*p++) {
        case 0:
            p += 3;
        case 1:
            p += 1;
            break;
        case 2:
        case 3:
            break;
        }
        break;
    case 0x1E:
        p += 1;
        break;
    case 0x20:
        p += 1;
        switch (*p++) {
        case 0:
            p += 3;
        case 1:
            p += 1;
            break;
        case 2:
        case 3:
            break;
        }
        break;
    case 0x21:
        p += 1;
        switch (*p) {
        case 0:
            p += 1;
            break;
        case 1:
        case 2:
            p += 1;
            break;
        }
        break;
    case 0x22:
        p += 1;
        switch (*p) {
        case 0:
            p += 2;
            break;
        case 1:
        case 2:
            p += 1;
            break;
        }
        break;
    case 0x36:
        p += 1;
        switch (*p) {
        case 0:
            p += 2;
            break;
        case 1:
        case 2:
            p += 1;
            break;
        }
        break;
    case 0x23:
        p += 1;
        switch (*p++) {
        case 0:
            p += 3;
        case 1:
            p += 1;
            break;
        case 2:
        case 3:
            break;
        }
        break;
    case 0x24:
        p += 1;
        switch (*p) {
        case 0:
            p += 2;
            break;
        case 1:
            p += 1;
            break;
        }
        break;
    case 0x25:
        p += 1;
        break;
    case 0x26:
        p += 2;
        break;
    case 0x27:
        p += 1;
        switch (*p++) {
        case 0:
            p += 3;
        case 1:
            p += 1;
            break;
        case 2:
        case 3:
            break;
        }
        break;
    case 0x28:
        p += 2;
        break;
    case 0x29:
        p += 2;
        break;
    case 0x2A:
        p += 2;
        break;
    case 0x2B:
        p += 1;
        switch (*p) {
        case 0:
            p += 3;
            break;
        case 1:
        case 2:
            p += 1;
            break;
        }
        break;
    case 0x2C:
    case 0x70:
        p += 1;
        switch (*p++) {
        case 0:
            p += 3;
        case 1:
            p += 1;
            break;
        case 2:
        case 3:
            break;
        }
        break;
    case 0x2D:
        p += 1;
        break;
    case 0x2E:
        p += 2;
        break;
    case 0x2F:
        p += 2;
        break;
    case 0x30:
        p += 1;
        break;
    case 0x31:
        p += 1;
        break;
    case 0x32:
        p += 1;
        switch (*p) {
        case 0:
            p += 2;
            break;
        case 1:
        case 2:
            p += 1;
            break;
        }
        break;
    case 0x33:
        p += 1;
        switch (*p++) {
        case 0:
            p += 3;
        case 1:
            p += 1;
            break;
        case 2:
        case 3:
            break;
        }
        break;
    case 0x34:
        p += 1;
        switch (*p) {
        case 0:
            p += 3;
            break;
        case 1:
        case 2:
            p += 1;
            break;
        }
        break;
    case 0x35:
        p += 2;
        break;
    case 0x37:
        p += 2;
        break;
    case 0x38:
        p += 2;
        break;
    case 0x39:
        p += 2;
        break;
    case 0x3A:
        p += 2;
        break;
    case 0x3B:
        p += 2;
        break;
    case 0x3C:
        p += 2;
        break;
    case 0x3D:
        p += 1;
        switch (*p) {
        case 0:
            p += 3;
            break;
        case 1:
        case 2:
            p += 1;
            break;
        }
        break;
    case 0x3E:
        p += 1;
        switch (*p) {
        case 0:
            p += 3;
        case 1:
            p += 3;
            break;
        case 2:
        case 3:
            p += 1;
            break;
        }
        break;
    case 0x3F:
        p += 4;
        break;
    case 0x40:
        p += 2;
        break;
    case 0x41:
        p += 2;
        break;
    case 0x42:
        p += 1;
        switch (*p) {
        case 0:
            p += 2;
            break;
        case 1:
        case 2:
            p += 1;
            break;
        }
        break;
    case 0x69:
        p += 1;
        switch (*p) {
        case 0:
            p += 2;
            break;
        case 1:
        case 2:
            p += 1;
            break;
        }
        break;
    case 0x44:
        p += 2;
        break;
    case 0x45:
        p += 2;
        break;
    case 0x46:
        p += 1;
        switch (*p) {
        case 0:
            p += 3;
            break;
        case 1:
        case 2:
            p += 1;
            break;
        }
        break;
    case 0x47:
        p += 2;
        break;
    case 0x48:
        p += 2;
        break;
    case 0x49:
        p += 2;
        break;
    case 0x4A:
        p += 2;
        break;
    case 0x4B:
        p += 1;
        break;
    case 0x4C:
        p += 1;
        break;
    case 0x4D:
        p += 1;
        break;
    case 0x4E:
    case 0x4F:
    case 0x50:
        p += 2;
        break;
    case 0x51:
        p += 2;
        break;
    case 0x52:
        p += 1;
        break;
    case 0x53:
        p += 1;
        break;
    case 0x54:
        p += 2;
        break;
    case 0x55:
        p += 2;
        break;
    case 0x56:
        p += 1;
        switch (*p) {
        case 0:
            p += 3;
            break;
        case 1:
        case 2:
            p += 1;
            break;
        }
        break;
    case 0x57:
        p += 1;
        switch (*p) {
        case 0:
            p += 3;
        case 1:
            p += 3;
            break;
        case 2:
        case 3:
            p += 1;
            break;
        }
        break;
    case 0x58:
        p += 1;
        break;
    case 0x59:
        p += 2;
        break;
    case 0x5A:
        p += 1;
        switch (*p) {
        case 0:
            p += 2;
            break;
        case 1:
        case 2:
            p += 1;
            break;
        }
        break;
    case 0x5B:
        p += 2;
        break;
    case 0x5C:
        p += 2;
        break;
    case 0x5D:
        p += 2;
        break;
    case 0x5E:
        p += 1;
        switch (*p) {
        case 0:
            p += 2;
            break;
        case 1:
        case 2:
            p += 1;
            break;
        }
        break;
    case 0x5F:
        p += 1;
        break;
    case 0x60:
        p += 2;
        break;
    case 0x61:
        p += 1;
        break;
    case 0x62:
        p += 2;
        break;
    case 0x63:
        p += 1;
        switch (*p) {
        case 0:
            p += 2;
            break;
        case 1:
        case 2:
            p += 1;
            break;
        }
        break;
    case 0x64:
        p += 1;
        switch (*p) {
        case 0:
            p += 2;
            break;
        case 1:
        case 2:
            p += 1;
            break;
        }
        break;
    case 0x65:
        p += 1;
        break;
    case 0x66:
        p += 2;
        break;
    case 0x67:
        p += 2;
        break;
    case 0x68:
        p += 1;
        break;
    case 0x80:
        p += 1;
        switch (*p) {
        case 0:
            p += 3;
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
        case 13:
            p += 2;
            break;
        case 255:
            p += 1;
            break;
        }
        break;
    case 0x81:
        p += 2;
        break;
    case 0x82:
        p += 3;
        break;
    case 0x83:
        p += 1;
        switch (*p) {
        case 0:
            p += 4;
            break;
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 255:
            p += 1;
            break;
        }
        break;
    case 0x84:
    case 0x7B:
        p += 1;
        break;
    case 0xFF:
        p += 2;
        break;
    case 0x90:
        p += 2;
        break;
    case 0x91:
    case 0x99:
        p += 2;
        break;
    case 0x92:
    case 0x93:
        p += 1;
        break;
    case 0x94:
        p += 1;
        switch (*p++) {
        case 0:
            p += 2;
        case 1:
            p += 1;
            break;
        case 2:
        case 3:
            break;
        }
        break;
    case 0x71:
        p += 1;
        switch (*p) {
        case 0:
            p += 2;
            break;
        case 1:
        case 2:
            p += 1;
            break;
        }
        break;
    case 0x72:
        p += 2;
        break;
    case 0x73:
        p += 1;
        switch (*p++) {
        case 0:
            p += 3;
        case 1:
            p += 1;
            break;
        case 2:
        case 3:
            break;
        }
        break;
    case 0x74:
        p += 1;
        switch (*p) {
        case 0:
            p += 2;
            break;
        case 1:
        case 2:
            p += 1;
            break;
        }
        break;
    case 0x75:
        p += 1;
        switch (*p++) {
        case 0:
            p += 3;
        case 1:
            p += 1;
            break;
        case 2:
        case 3:
            break;
        }
        break;
    case 0x76:
        p += 1;
        switch (*p++) {
        case 0:
            p += 3;
        case 1:
            p += 1;
            break;
        case 2:
        case 3:
            break;
        }
        break;
    case 0x77:
        p += 2;
        break;
    case 0x78:
        p += 1;
        switch (*p) {
        case 0:
            p += 3;
            break;
        case 1:
        case 2:
            p += 1;
            break;
        }
        break;
    case 0x79:
        p += 1;
        switch (*p++) {
        case 0:
            p += 4;
        case 1:
            p += 1;
            break;
        case 2:
        case 3:
            break;
        }
        break;
    case 0x7A:
        p += 1;
        switch (*p++) {
        case 0:
            p += 3;
        case 1:
            p += 1;
            break;
        case 2:
        case 3:
            break;
        }
        break;
    case 0x7C:
        p += 2;
        break;
    default:
        game_func_007262D0(em, p, 4);
        break;
    }
    return p;
}
