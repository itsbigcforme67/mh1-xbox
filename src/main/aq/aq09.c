/* SLPM_654.95 0x0022D740-0x0022D7B8: set_other_data (quest network: copies a received player block (up to 0x20 bytes) into a
 * local record and hands it to sync_host_sub unless flag is set). See aq_nm.c. */
#include "types.h"

typedef struct OTHBUF {
    s32 a;
    s32 b;
    u8 data[0x28];
} OTHBUF;

void *memcpy(void *, const void *, int);
void sync_host_sub(u8 *d, int idx);

void set_other_data(u8 *d, int flag) {
    OTHBUF buf;
    u8 *src = d + 8;
    OTHBUF *b = &buf;
    int pl = *(u16 *)(d + 4);
    int n;

    {
        s32 t = *(s32 *)d;
        b->a = 1;
        b->b = t;
    }
    n = d[9];
    if (n > 0x20) {
        n = 0x20;
    }
    memcpy(b->data, src, n);
    if (flag == 0) {
        sync_host_sub((u8 *)b, pl - 1);
    }
}
