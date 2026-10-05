/* lbui, run 11: Lb_get_cursor_col .. Lb_get_cursor_col (lobby.bin 0x0059A6C0-0x0059A754): the matching functions of lbui_nm.c. */
#pragma readonly_strings on
#include "lbui_proto.h"

int Lb_get_cursor_col(void) {
    f32 a = 0.0000958738f * (f32)(u32)(u16)((System_timer & 0x3F) << 10);

    return (((s8)(int)(80.0f * flSin(a)) + 0x9F) << 24) | 0xFF00;
}
