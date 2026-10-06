/* netdev05 - set_device_no (SLPM_654.95 0x00233180-0x00233274): looks a (device id, sub id) pair up in the Modem then the Ether device tables
   (0x1C-byte records, end at id 0) and copies the record into out with x10 replaced by the 5th argument; fills out with 0xFF and returns -1 if
   unknown. Written new in this pass; field names are guesses. */
#include "types.h"
typedef struct DEVREC { s32 x00; s32 x04; s32 id; s32 sub; s32 x10; s32 x14; s32 x18; } DEVREC;
extern DEVREC Modem_device_list[];
extern DEVREC Ether_device_list[];
void *memset();
int set_device_no(DEVREC *out, int id, int sub, int unused, int x10) {
    DEVREC *p;

    p = Modem_device_list;
    for (;;) {
        if (p->id == 0) {
            break;
        }
        if (p->id == id && p->sub == sub) {
            out->x00 = p->x00;
            out->x04 = p->x04;
            out->id = id;
            out->sub = sub;
            out->x10 = x10;
            out->x14 = p->x14;
            out->x18 = p->x18;
            return 0;
        }
        p++;
    }
    p = Ether_device_list;
    for (;;) {
        if (p->id == 0) {
            break;
        }
        if (p->id == id && p->sub == sub) {
            out->x00 = p->x00;
            out->x04 = p->x04;
            out->id = id;
            out->sub = sub;
            out->x10 = x10;
            out->x14 = p->x14;
            out->x18 = p->x18;
            return 0;
        }
        p++;
    }
    memset(out, 0xFF, 0x1C);
    return -1;
}
