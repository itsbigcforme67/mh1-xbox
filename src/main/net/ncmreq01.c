/* SLPM_654.95 0x0026C770-0x0026CB58: Ncm_mssage_disp_req .. Net_disp_net_name_req. See ncmreq_nm.c. */
#include "types.h"

typedef struct NCQ {
    u8 pad00[0x32];
    s8 ia;
    s8 ib;
    u8 pad34[0xC];
    u8 qa[8];
    u8 ka[8];
    u8 qb[8];
    u8 kb[8];
} NCQ;
extern NCQ net_common_w;






void Ncm_mssage_disp_req(u8 id) {
    switch (id) {
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
    case 14:
    case 15:
    case 16:
    case 17:
    case 18:
    case 19:
    case 20:
    case 21:
    case 22:
    case 23:
    case 24:
    case 25:
    case 26:
    case 27:
    case 32:
    case 33:
    case 34:
    case 35:
    case 36:
    case 37:
    case 38:
    case 39:
    case 40:
    case 41:
    case 42:
    case 43:
    case 44:
    case 45:
    case 46:
    case 47:
    case 48:
    case 49:
    case 50:
    case 51:
    case 52:
    case 53:
    case 54:
    case 55:
    case 56:
    case 57:
    case 58:
    case 59:
    case 60:
    case 61:
    case 62:
    case 63:
    case 64:
    case 65:
    case 66:
    case 67:
    case 68:
    case 69:
    case 70:
    case 71:
    case 72:
    case 73:
    case 74:
    case 75:
    case 76:
    case 77:
    case 78:
    case 79:
    case 80:
    case 81:
    case 82:
    case 83:
    case 84:
    case 85:
    case 86:
    case 87:
    case 88:
    case 89:
    case 90:
    case 91:
    case 92:
    case 93:
    case 94:
    case 95:
    case 96:
    case 97:
    case 98:
    case 99:
    case 100:
    case 101:
    case 102:
    case 103:
    case 104:
    case 105:
    case 106:
    case 107:
    case 108:
    case 109:
    case 110:
    case 111:
    case 112:
    case 113:
    case 114:
    case 115:
    case 116:
    case 117:
    case 118:
    case 119:
    case 120:
    case 121:
        if (net_common_w.ib < 8) {
            net_common_w.qb[net_common_w.ib] = id;
            net_common_w.kb[net_common_w.ib] = 0;
            net_common_w.ib++;
        }
        break;
    case 28:
    case 29:
    case 30:
    case 31:
        if (net_common_w.ia < 8) {
            net_common_w.qa[net_common_w.ia] = id;
            net_common_w.ka[net_common_w.ia] = 0;
            net_common_w.ia++;
        }
        break;
    }
}

void Ncm_mssage_disp_option_req(u8 id) {
    switch (id) {
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
    case 14:
    case 15:
    case 16:
    case 17:
    case 18:
    case 19:
    case 20:
    case 21:
    case 22:
    case 23:
    case 24:
    case 25:
    case 26:
    case 27:
    case 32:
    case 33:
    case 34:
    case 35:
    case 36:
    case 37:
    case 38:
    case 39:
    case 40:
    case 41:
    case 42:
    case 43:
    case 44:
    case 45:
    case 46:
    case 47:
    case 48:
    case 49:
    case 50:
    case 51:
    case 52:
    case 53:
    case 54:
    case 55:
    case 63:
    case 64:
    case 65:
    case 66:
    case 67:
    case 68:
    case 69:
    case 70:
    case 71:
    case 72:
    case 73:
    case 74:
    case 75:
    case 76:
    case 77:
    case 78:
    case 79:
    case 80:
    case 81:
    case 82:
    case 83:
    case 84:
    case 85:
    case 86:
    case 87:
    case 88:
    case 89:
    case 90:
    case 91:
    case 92:
    case 93:
    case 94:
    case 95:
    case 96:
    case 97:
    case 98:
    case 99:
    case 100:
    case 101:
    case 102:
    case 103:
    case 104:
    case 105:
    case 106:
    case 107:
    case 108:
    case 109:
    case 110:
    case 111:
    case 112:
    case 113:
    case 114:
    case 115:
    case 116:
    case 117:
    case 118:
    case 119:
    case 120:
        if (net_common_w.ib < 8) {
            net_common_w.qb[net_common_w.ib] = id;
            net_common_w.kb[net_common_w.ib] = 4;
            net_common_w.ib++;
        }
        break;
    case 28:
    case 29:
    case 30:
    case 31:
        if (net_common_w.ia < 8) {
            net_common_w.qa[net_common_w.ia] = id;
            net_common_w.ka[net_common_w.ia] = 4;
            net_common_w.ia++;
        }
        break;
    }
}

void Ncm_err_mssage_disp_req(u8 id) {
    switch (id) {
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
    case 14:
        if (net_common_w.ib < 8) {
            net_common_w.qb[net_common_w.ib] = id;
            net_common_w.kb[net_common_w.ib] = 3;
            net_common_w.ib++;
        }
        break;
    }
}

void Ncm_menu_disp_req(u8 id) {
    switch (id) {
    case 0:
    case 1:
    case 2:
        if (net_common_w.ia < 8) {
            net_common_w.qa[net_common_w.ia] = id;
            net_common_w.ka[net_common_w.ia] = 1;
            net_common_w.ia++;
        }
        break;
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
        if (net_common_w.ib < 8) {
            net_common_w.qb[net_common_w.ib] = id;
            net_common_w.kb[net_common_w.ib] = 1;
            net_common_w.ib++;
        }
        break;
    }
}

void Net_disp_net_name_req(u8 id) {
    switch (id) {
    case 0:
    case 1:
    case 2:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
        if (net_common_w.ib < 8) {
            net_common_w.qb[net_common_w.ib] = id;
            net_common_w.kb[net_common_w.ib] = 2;
            net_common_w.ib++;
        }
        break;
    case 3:
    case 4:
        if (net_common_w.ia < 8) {
            net_common_w.qa[net_common_w.ia] = id;
            net_common_w.ka[net_common_w.ia] = 2;
            net_common_w.ia++;
        }
        break;
    }
}
