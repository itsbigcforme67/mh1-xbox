#ifndef CLAY_H
#define CLAY_H
/* "Clay": Capcom's model instances (0x8C bytes). handle -1 = empty.
 * Offsets from matched code (set06.c, get_clay_ptr.c). */
#include "types.h"

typedef struct CLAY {
    s32 handle;             /* 0x00 passed to flExecuteClay; -1 = none */
    u8 _pad04[0x84];
    s32 attr;               /* 0x88 passed to clay_attr_set */
} CLAY;

void clay_attr_set(s32);
void clay_attr_reset(void);
void flExecuteClay(s32, int);

#endif
