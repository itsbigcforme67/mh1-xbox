/* G online (PC): lobby_func_0059AB90, written from Monster Hunter G's code at lobby 0x59AB90 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 lobby_func_00595530();
extern u8 lb_sys;
s32 lb_send_dataTU(); /* extern */

void lobby_func_0059AB90(void *arg0, s32 arg1) {
    u8 g_frame[0x40] __attribute__((aligned(16)));   /* G's stack frame */
#define sp30 (*(s16 *)(g_frame + 0x30))
#define sp32 (*(s16 *)(g_frame + 0x32))
#define sp34 (*(s8 *)(g_frame + 0x34))
#define sp35 (*(u8 *)(g_frame + 0x35))
#define sp36 (*(u8 *)(g_frame + 0x36))
#define sp37 (*(u8 *)(g_frame + 0x37))
#define sp38 (*(u8 *)(g_frame + 0x38))

    if (((*(f32 *)((u8 *)(arg0) + 0xAC)) != 0.0f) || ((*(f32 *)((u8 *)(arg0) + 0xB0)) != 0.0f) || ((*(f32 *)((u8 *)(arg0) + 0xB4)) != 0.0f)) {
        sp30 = M2C_ERROR(/* Read from unset register $f0 */);
        sp32 = M2C_BITWISE(s16, (*(f32 *)((u8 *)(arg0) + 0xB4)));
        sp36 = (*(u8 *)((u8 *)(arg0) + 0x14));
        sp37 = (*(u8 *)((u8 *)(arg0) + 0x15));
        sp34 = ((s32) (*(u16 *)((u8 *)(arg0) + 0xE)) >> 8) & 0xFFFF;
        sp38 = (*(u8 *)((u8 *)(arg0) + 0x8EC));
        if (lobby_func_00595530((s32) (*(u8 *)((u8 *)(arg0) + 0x14)), (s32) (*(u8 *)((u8 *)(arg0) + 0x15))) == 1) {
            sp35 = (*(u8 *)((u8 *)&lb_sys + 0x7A));
        } else if (((*(u8 *)((u8 *)(arg0) + 0x14)) == 0) && ((*(u8 *)((u8 *)(arg0) + 0x15)) == 0x4D)) {
            sp35 = (u8) *(*(u16 **)((u8 *)(arg0) + 0x878));
            (*(u16 *)((u8 *)&lb_sys + 0x14)) &= ~(1 << sp35);
        }
        lb_send_dataTU(0xA, 0xA, &sp30, arg1);
    }
}

