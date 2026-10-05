/* em_cmd_r23 - monster command interpreter 0x00564480-0x00565518: em_cmd_top, cmd_end_search, next_cmd_search. Whole file in em_cmd_nm.c. */
#include "em_cmd.h"











































































































































u8 *em_cmd_top(EMW *em) {
    EM_FIELD(em, s8 *, 0x826) = 0;
    return em->cmd_top = (*em->cmd_tbl)[em->cmd_idx];
}

u8 *cmd_end_search(EMW *em, u8 *p, int a2, int a3) {
    u8 c;
    u8 m;

    c = a2;
    m = a3;
    for (;;) {
        while (p[0] != c) {
            p = next_cmd_search(em, p);
        }
        if (!(p[0] == c && p[1] == 0)) {
            break;
        }
        p = next_cmd_search(em, p);
        for (;;) {
            if (p[0] == c && p[1] == m) {
                p = next_cmd_search(em, p);
                break;
            }
            if (p[0] == c) {
                p = next_cmd_search(em, p);
            }
            p = cmd_end_search(em, p, a2, a3);
        }
    }
    return p;
}

u8 *next_cmd_search(EMW *em, u8 *p) {
    u8 *temp_v1_10;
    u8 *temp_v1_16;
    u8 *temp_v1_19;
    u8 *temp_v1_22;
    u8 *temp_v1_36;
    u8 *temp_v1_4;
    u8 *temp_v1_8;
    u8 *var_a1;
    u8 temp_v0;
    u8 temp_v1;
    u8 temp_v1_11;
    u8 temp_v1_12;
    u8 temp_v1_13;
    u8 temp_v1_14;
    u8 temp_v1_15;
    u8 temp_v1_17;
    u8 temp_v1_18;
    u8 temp_v1_20;
    u8 temp_v1_21;
    u8 temp_v1_23;
    u8 temp_v1_24;
    u8 temp_v1_25;
    u8 temp_v1_26;
    u8 temp_v1_27;
    u8 temp_v1_28;
    u8 temp_v1_29;
    u8 temp_v1_2;
    u8 temp_v1_30;
    u8 temp_v1_31;
    u8 temp_v1_32;
    u8 temp_v1_33;
    u8 temp_v1_34;
    u8 temp_v1_35;
    u8 temp_v1_37;
    u8 temp_v1_3;
    u8 temp_v1_5;
    u8 temp_v1_6;
    u8 temp_v1_7;
    u8 temp_v1_9;

    var_a1 = p;
    temp_v0 = *var_a1;
    switch (temp_v0) {
    case 0x1:
        var_a1 += 2;
        break;
    case 0x2:
        var_a1 += 2;
        break;
    case 0x3:
        var_a1 += 2;
        break;
    case 0x4:
        var_a1 += 1;
        break;
    case 0x5:
        var_a1 += 4;
        break;
    case 0x6:
        var_a1 += 4;
        break;
    case 0x7:
        var_a1 += 2;
        break;
    case 0x8:
        var_a1 += 2;
        break;
    case 0x9:
        var_a1 += 2;
        break;
    case 0xA:
        var_a1 += 2;
        break;
    case 0xB:
        var_a1 += 1;
        temp_v1 = *var_a1;
        switch (temp_v1) {
        case 0:
            var_a1 += 2;
            break;
        case 1:
        case 2:
            var_a1 += 1;
            break;
        }
        break;
    case 0xC:
        var_a1 += 3;
        break;
    case 0xD:
        var_a1 += 2;
        break;
    case 0xE:
        var_a1 += 1;
        temp_v1_2 = *var_a1;
        switch (temp_v1_2) {
        case 0:
            var_a1 += 2;
            break;
        case 1:
        case 2:
            var_a1 += 1;
            break;
        }
        break;
    case 0xF:
        var_a1 += 6;
        break;
    case 0x10:
        var_a1 += 1;
        break;
    case 0x11:
        var_a1 += 1;
        break;
    case 0x12:
        var_a1 += 1;
        break;
    case 0x13:
        var_a1 += 1;
        break;
    case 0x14:
        var_a1 += 1;
        temp_v1_3 = *var_a1;
        switch (temp_v1_3) {
        case 0:
            var_a1 += 2;
            break;
        case 1:
        case 2:
            var_a1 += 1;
            break;
        }
        break;
    case 0x15:
        var_a1 += 1;
        switch (*var_a1++) {
        case 0:
            var_a1 += 3;
        case 1:
            var_a1 += 1;
            break;
        case 2:
        case 3:
            break;
        }
        break;
    case 0x16:
        var_a1 += 2;
        break;
    case 0x17:
        var_a1 += 5;
        break;
    case 0x18:
        var_a1 += 1;
        break;
    case 0x19:
        var_a1 += 1;
        break;
    case 0x1A:
        var_a1 += 2;
        break;
    case 0x1B:
        var_a1 += 1;
        temp_v1_6 = *var_a1;
        switch (temp_v1_6) {
        case 0:
            var_a1 += 2;
            break;
        case 1:
        case 2:
            var_a1 += 1;
            break;
        }
        break;
    case 0x1C:
        var_a1 += 1;
        temp_v1_7 = *var_a1;
        switch (temp_v1_7) {
        case 0:
            var_a1 += 2;
            break;
        case 1:
        case 2:
            var_a1 += 1;
            break;
        }
        break;
    case 0x1D:
        var_a1 += 1;
        switch (*var_a1++) {
        case 0:
            var_a1 += 3;
        case 1:
            var_a1 += 1;
            break;
        case 2:
        case 3:
            break;
        }
        break;
    case 0x1E:
        var_a1 += 1;
        break;
    case 0x20:
        var_a1 += 1;
        switch (*var_a1++) {
        case 0:
            var_a1 += 3;
        case 1:
            var_a1 += 1;
            break;
        case 2:
        case 3:
            break;
        }
        break;
    case 0x21:
        var_a1 += 1;
        temp_v1_12 = *var_a1;
        switch (temp_v1_12) {
        case 0:
            var_a1 += 1;
            break;
        case 1:
        case 2:
            var_a1 += 1;
            break;
        }
        break;
    case 0x22:
        var_a1 += 1;
        temp_v1_13 = *var_a1;
        switch (temp_v1_13) {
        case 0:
            var_a1 += 2;
            break;
        case 1:
        case 2:
            var_a1 += 1;
            break;
        }
        break;
    case 0x23:
        var_a1 += 1;
        temp_v1_14 = *var_a1;
        switch (temp_v1_14) {
        case 0:
            var_a1 += 3;
            break;
        case 1:
        case 2:
            var_a1 += 1;
            break;
        }
        break;
    case 0x24:
        var_a1 += 1;
        temp_v1_15 = *var_a1;
        switch (temp_v1_15) {
        case 0:
            var_a1 += 2;
            break;
        case 1:
            var_a1 += 1;
            break;
        }
        break;
    case 0x25:
        var_a1 += 1;
        break;
    case 0x26:
        var_a1 += 2;
        break;
    case 0x27:
        var_a1 += 1;
        switch (*var_a1++) {
        case 0:
            var_a1 += 3;
        case 1:
            var_a1 += 1;
            break;
        case 2:
        case 3:
            break;
        }
        break;
    case 0x28:
        var_a1 += 2;
        break;
    case 0x29:
        var_a1 += 2;
        break;
    case 0x2A:
        var_a1 += 2;
        break;
    case 0x2B:
        var_a1 += 1;
        temp_v1_18 = *var_a1;
        switch (temp_v1_18) {
        case 0:
            var_a1 += 3;
            break;
        case 1:
        case 2:
            var_a1 += 1;
            break;
        }
        break;
    case 0x2C:
        var_a1 += 1;
        switch (*var_a1++) {
        case 0:
            var_a1 += 3;
        case 1:
            var_a1 += 1;
            break;
        case 2:
        case 3:
            break;
        }
        break;
    case 0x2D:
        var_a1 += 1;
        break;
    case 0x2E:
        var_a1 += 2;
        break;
    case 0x2F:
        var_a1 += 2;
        break;
    case 0x30:
        var_a1 += 1;
        break;
    case 0x31:
        var_a1 += 1;
        break;
    case 0x32:
        var_a1 += 1;
        temp_v1_21 = *var_a1;
        switch (temp_v1_21) {
        case 0:
            var_a1 += 2;
            break;
        case 1:
        case 2:
            var_a1 += 1;
            break;
        }
        break;
    case 0x33:
        var_a1 += 1;
        switch (*var_a1++) {
        case 0:
            var_a1 += 3;
        case 1:
            var_a1 += 1;
            break;
        case 2:
        case 3:
            break;
        }
        break;
    case 0x34:
        var_a1 += 1;
        temp_v1_24 = *var_a1;
        switch (temp_v1_24) {
        case 0:
            var_a1 += 3;
            break;
        case 1:
        case 2:
            var_a1 += 1;
            break;
        }
        break;
    case 0x35:
        var_a1 += 2;
        break;
    case 0x38:
        var_a1 += 2;
        break;
    case 0x39:
        var_a1 += 2;
        break;
    case 0x3A:
        var_a1 += 2;
        break;
    case 0x3B:
        var_a1 += 2;
        break;
    case 0x3C:
        var_a1 += 2;
        break;
    case 0x3D:
        var_a1 += 1;
        temp_v1_25 = *var_a1;
        switch (temp_v1_25) {
        case 0:
            var_a1 += 2;
            break;
        case 1:
        case 2:
            var_a1 += 1;
            break;
        }
        break;
    case 0x3E:
        var_a1 += 1;
        temp_v1_26 = *var_a1;
        switch (temp_v1_26) {
        case 0:
            var_a1 += 3;
        case 1:
            var_a1 += 2;
            break;
        case 2:
        case 3:
            var_a1 += 1;
            break;
        }
        break;
    case 0x3F:
        var_a1 += 3;
        break;
    case 0x40:
        var_a1 += 2;
        break;
    case 0x41:
        var_a1 += 2;
        break;
    case 0x42:
        var_a1 += 1;
        temp_v1_27 = *var_a1;
        switch (temp_v1_27) {
        case 0:
            var_a1 += 2;
            break;
        case 1:
        case 2:
            var_a1 += 1;
            break;
        }
        break;
    case 0x44:
        var_a1 += 2;
        break;
    case 0x45:
        var_a1 += 2;
        break;
    case 0x46:
        var_a1 += 1;
        temp_v1_28 = *var_a1;
        switch (temp_v1_28) {
        case 0:
            var_a1 += 3;
            break;
        case 1:
        case 2:
            var_a1 += 1;
            break;
        }
        break;
    case 0x47:
        var_a1 += 2;
        break;
    case 0x48:
        var_a1 += 2;
        break;
    case 0x49:
        var_a1 += 2;
        break;
    case 0x4A:
        var_a1 += 2;
        break;
    case 0x4B:
        var_a1 += 1;
        break;
    case 0x4C:
        var_a1 += 1;
        break;
    case 0x4D:
        var_a1 += 1;
        break;
    case 0x4E:
    case 0x4F:
    case 0x50:
        var_a1 += 2;
        break;
    case 0x51:
        var_a1 += 2;
        break;
    case 0x52:
        var_a1 += 1;
        break;
    case 0x53:
        var_a1 += 1;
        break;
    case 0x54:
        var_a1 += 2;
        break;
    case 0x55:
        var_a1 += 2;
        break;
    case 0x56:
        var_a1 += 1;
        temp_v1_29 = *var_a1;
        switch (temp_v1_29) {
        case 0:
            var_a1 += 2;
            break;
        case 1:
        case 2:
            var_a1 += 1;
            break;
        }
        break;
    case 0x57:
        var_a1 += 1;
        temp_v1_30 = *var_a1;
        switch (temp_v1_30) {
        case 0:
            var_a1 += 3;
        case 1:
            var_a1 += 2;
            break;
        case 2:
        case 3:
            var_a1 += 1;
            break;
        }
        break;
    case 0x58:
        var_a1 += 1;
        break;
    case 0x59:
        var_a1 += 2;
        break;
    case 0x5A:
        var_a1 += 1;
        temp_v1_31 = *var_a1;
        switch (temp_v1_31) {
        case 0:
            var_a1 += 2;
            break;
        case 1:
        case 2:
            var_a1 += 1;
            break;
        }
        break;
    case 0x5B:
        var_a1 += 2;
        break;
    case 0x5C:
        var_a1 += 2;
        break;
    case 0x5D:
        var_a1 += 2;
        break;
    case 0x5E:
        var_a1 += 1;
        temp_v1_32 = *var_a1;
        switch (temp_v1_32) {
        case 0:
            var_a1 += 2;
            break;
        case 1:
        case 2:
            var_a1 += 1;
            break;
        }
        break;
    case 0x5F:
        var_a1 += 1;
        break;
    case 0x60:
        var_a1 += 2;
        break;
    case 0x61:
        var_a1 += 1;
        break;
    case 0x62:
        var_a1 += 2;
        break;
    case 0x63:
        var_a1 += 2;
        break;
    case 0x64:
        var_a1 += 1;
        temp_v1_33 = *var_a1;
        switch (temp_v1_33) {
        case 0:
            var_a1 += 2;
            break;
        case 1:
        case 2:
            var_a1 += 1;
            break;
        }
        break;
    case 0x65:
        var_a1 += 1;
        break;
    case 0x66:
        var_a1 += 2;
        break;
    case 0x67:
        var_a1 += 2;
        break;
    case 0x68:
        var_a1 += 1;
        break;
    case 0x80:
        var_a1 += 1;
        temp_v1_34 = *var_a1;
        switch (temp_v1_34) {
        case 0x0:
            var_a1 += 3;
        case 0x1:
        case 0x2:
        case 0x3:
        case 0x4:
        case 0x5:
        case 0x6:
        case 0x7:
        case 0x8:
        case 0x9:
        case 0xA:
            var_a1 += 2;
            break;
        case 0xFF:
            var_a1 += 1;
            break;
        }
        break;
    case 0x81:
        var_a1 += 2;
        break;
    case 0x82:
        var_a1 += 3;
        break;
    case 0x83:
        var_a1 += 1;
        temp_v1_35 = *var_a1;
        switch (temp_v1_35) {
        case 0x0:
            var_a1 += 4;
            break;
        case 0x1:
        case 0x2:
        case 0x3:
        case 0x4:
        case 0x5:
        case 0xFF:
            var_a1 += 1;
            break;
        }
        break;
    case 0x84:
        var_a1 += 1;
        break;
    case 0xFF:
        var_a1 += 2;
        break;
    case 0x90:
        var_a1 += 2;
        break;
    case 0x91:
        var_a1 += 2;
        break;
    case 0x92:
    case 0x93:
        var_a1 += 1;
        break;
    case 0x94:
        var_a1 += 1;
        switch (*var_a1++) {
        case 0:
            var_a1 += 2;
        case 1:
            var_a1 += 1;
            break;
        case 2:
        case 3:
            break;
        }
        break;
    default:
        EM_FIELD(em, s8 *, 0x9DA) = 4;
        break;
    }
    return var_a1;
}
