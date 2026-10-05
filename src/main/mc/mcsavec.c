/* Save data packing for the memory card, SLPM_654.95 main 0x2814E0-0x281C00:
 * encode_data / decode_data scramble the save image (header: u16 version 0x100,
 * u16 key seed, u16 checksum, u16 0x5963, then 0x8A20 u16 words XORed with a
 * key stream key = key * 0xB0 % 65363), check_sum_*, copy of the option block,
 * the patch buffer and the three character slots (0x480 bytes each) to or from
 * data_load_ptr. Layout of the image (offsets into data_load_ptr): 0x208 patch
 * (0x10000), 0x10208 patch tail (0x40), 0x10248 save time (McReadClock), 0x10250
 * options (0x10), 0x10260 slot 0..2 (0x480 each), 0x11000 options part 2
 * (0x21C), 0x1121C/0x1121E option flags. Meanings are guesses. */
#include "types.h"

extern u8 *data_load_ptr;
extern u8 option_w[];
extern u8 patch_buff[];

int ran_suu();
void *memcpy();
void *memset();
void flMemset();
void McReadClock();
void Init_reibun();
void system_w_set();
void McActInit();
void load_file_mdl();
int McActAvailSet();
int mc_copy_opt_only();
int mc_copy_patch();
int user_data_copy2();
int user_data_clr();

int user_data_clr(slot)
int slot;
{
    flMemset(option_w + (slot & 0xFF) * 0x480 + 0x10, 0, 0x480);
    return 1;
}

void User_data_init(void)
{
    user_data_clr(0);
    user_data_clr(1);
    user_data_clr(2);
}

int save_data_sub(save, mask)
int save;
int mask;
{
    int r;
    u8 *d = data_load_ptr;

    r = 0;
    if (save == 1) {
        McReadClock(d + 0x10248);
    }
    if (mask & 1) {
        mc_copy_opt_only(save);
    } else if (save == 0) {
        *(s16 *)(option_w + 0xFCC) |= *(s16 *)(d + 0x1121C);
    }
    if (mask & 2) {
        r |= user_data_copy2(0, save);
    }
    if (mask & 4) {
        r |= user_data_copy2(1, save);
    }
    if (mask & 8) {
        r |= user_data_copy2(2, save);
    }
    if (mask & 0x10) {
        mc_copy_patch(save);
    }
    return r;
}

void card_data_init(w)
u8 *w;
{
    u8 *d = data_load_ptr;

    memset(d, 0, 0x11450);
    McActInit(0);
    load_file_mdl(d + 0x12000, 0x6D3);
    *(s32 *)(w + 0x48) = McActAvailSet(d + 0x12000);
}
