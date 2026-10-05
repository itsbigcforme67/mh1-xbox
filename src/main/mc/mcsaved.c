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

int mc_copy_opt_only(save)
int save;
{
    u8 *d = data_load_ptr;

    if (save == 0) {
        memcpy(option_w, d + 0x10250, 0x10);
        memcpy(option_w + 0xDB0, d + 0x11000, 0x21C);
        *(s16 *)(option_w + 0xFCC) |= *(s16 *)(d + 0x1121C);
        option_w[0xFCE] = d[0x1121E];
        Init_reibun();
        system_w_set();
    } else {
        memcpy(d + 0x10250, option_w, 0x10);
        memcpy(d + 0x11000, option_w + 0xDB0, 0x21C);
        *(s16 *)(d + 0x1121C) = *(s16 *)(option_w + 0xFCC);
        d[0x1121E] = option_w[0xFCE];
    }
    return 1;
}

int mc_copy_patch(save)
int save;
{
    u8 *d = data_load_ptr;

    if (save == 0) {
        memcpy(patch_buff, d + 0x208, 0x10000);
        memcpy(patch_buff + 0x20000, d + 0x10208, 0x40);
    } else {
        memcpy(d + 0x208, patch_buff, 0x10000);
        memcpy(d + 0x10208, patch_buff + 0x20000, 0x40);
    }
    return 1;
}
