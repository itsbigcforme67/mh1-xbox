/* imece: reset_temp (SLPM_654.95 0x00243360-0x002433F8): clears the first two bytes of each of the 8 temporary dictionary pages and resets the allocation window. IME engine (name entry), see imead.c for the context. */
#include "types.h"

extern u8 temp_pages[8][0x400];
extern u8 *temp_top, *temp_end;
extern int temp_page;

void reset_temp(void)
{
    int i;
    u8 *p;

    for (i = 0; i < 8; i++) {
        p = temp_pages[i];
        p[1] = 0;
        p[0] = 0;
    }
    temp_top = temp_pages[0];
    temp_end = temp_pages[1];
    temp_page = 0;
}
