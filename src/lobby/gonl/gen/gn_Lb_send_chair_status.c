/* G online (PC): Lb_send_chair_status, written from Monster Hunter G's code at lobby 0x59ADC0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Lb_send_data_to_myself();
s32 lb_send_data();
void * memset();
void * strcpy();
void Lb_send_chair_status(void *arg0, s8 arg1, s8 arg2) {
    u8 g_frame[0x50] __attribute__((aligned(16)));   /* G's stack frame */
#define sp40 (*(s8 *)(g_frame + 0x40))
#define sp41 (*(s8 *)(g_frame + 0x41))
#define sp42 (*(s32 *)(g_frame + 0x42))

    memset(&sp40, 0, 0xB);
    sp40 = arg1;
    sp41 = arg2;
    strcpy(&sp42, arg0);
    lb_send_data(0xB, 5, &sp40, M2C_ERROR(/* Read from unset register $a3 */));
    Lb_send_data_to_myself(0xB, 5, &sp40, M2C_ERROR(/* Read from unset register $a3 */));
}

