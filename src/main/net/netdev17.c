/* netdev17 - InetDnsGetIPAddress (SLPM_654.95 0x00237BA0-0x00237DB4): resolves a host name in two steps (a = step). Step 0: a dotted address string is
   converted directly; otherwise the 8-entry name cache (0x104 bytes each: name, address) is searched, and on a miss a DNS ticket is taken. Step 1
   polls CpInetDnsLookUp, stores a new answer in the cache (ring of 8) and releases the ticket. Returns 1 when done, 0 while waiting, -1 on error.
   Written new in this pass from an m2c draft. */
#include "types.h"
extern u8 Resolv_cache_004FAB70[];
extern s32 Resolv_cache_cnt_0038A5F4;
extern s16 Ticket_id;
s16 CpInetDnsGetTicket();
int CpInetDnsLookUp();
void CpInetDnsReleaseTicket();
u32 InetIPAddrFromString();
int strncmp();
char *strncpy();

int InetDnsGetIPAddress(s8 *a, s16 *b, u32 *c, char *name) {
    u32 ip;
    int i;
    u8 *e;
    int r;

    switch (*a) {
    case 0:
        *c = InetIPAddrFromString(name);
        if (*c != 0) {
            *a = 0;
            *b = 0;
            return 1;
        }
        i = 0;
        e = Resolv_cache_004FAB70;
        for (;;) {
            if (strncmp(e, name, 0x100) == 0) {
                *a = 0;
                *b = 0;
                *c = *(u32 *)(e + 0x100);
                return 1;
            }
            i++;
            e += 0x104;
            if (i >= 8) {
                break;
            }
        }
        Ticket_id = CpInetDnsGetTicket(name);
        if (Ticket_id == -1) {
            return -1;
        }
        (*a)++;
        *b = 0;
        break;
    case 1:
        r = CpInetDnsLookUp(Ticket_id, &ip);
        switch (r) {
        case -3:
            return 0;
        default:
        case -2:
        case -1:
            CpInetDnsReleaseTicket(Ticket_id);
            *a = 0;
            *b = 0;
            return -1;
        case 1:
            CpInetDnsReleaseTicket(Ticket_id);
            *a = 0;
            *b = 0;
            *c = ip;
            e = Resolv_cache_004FAB70 + Resolv_cache_cnt_0038A5F4 * 0x104;
            strncpy(e, name, 0x100);
            *(u32 *)(e + 0x100) = ip;
            Resolv_cache_cnt_0038A5F4++;
            Resolv_cache_cnt_0038A5F4 = Resolv_cache_cnt_0038A5F4 % 8;
            return 1;
        }
    }
    return 0;
}
