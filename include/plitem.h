#ifndef PLITEM_H
#define PLITEM_H
/* Item_data row (0x10 bytes) as a struct, for the few player functions that read 16-bit members
 * (the original folds the member offset into the symbol, which only a struct array does).
 * Do not include together with plf.h (it declares Item_data as bytes). */
#include "types.h"
typedef struct ITEM_DATA {
    u8 type;       /* 0x00 4 = ammo, 5 = shared stack */
    u8 use;        /* 0x01 1 = usable item, 2 = ammo (Pl_shell_set) */
    u8 _02;
    u8 max;        /* 0x03 stack limit, 0xFF = unlimited */
    u8 _04[4];
    s16 ammo;      /* 0x08 shot type of an ammo item (Shell_type_set) */
    u16 se;        /* 0x0A pick-up sound kind (Pl_item_get_se) */
    u8 _0C[4];
} ITEM_DATA;
extern ITEM_DATA Item_data[];
typedef struct SHELL_ROW { u8 ammo_max; u8 se_kind; u8 pow; u8 _03; } SHELL_ROW;
extern SHELL_ROW Shell_data[];
#endif
