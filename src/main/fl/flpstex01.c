/* fl library: flPS2SendTextureRegister (SLPM_654.95 0x00179570-0x001795DC) fills the texture register packet psTexture_data (a file-static 0x80 byte
 * buffer in the original) with flPS2SetTextureRegister and queues it; returns 0 when the register set-up fails. The first argument is passed through. */
#include "types.h"

extern u8 psTexture_data_976[];
extern int flSystemRenderOperation;

int flPS2SetTextureRegister();
void flPS2psAddQueue();

int flPS2SendTextureRegister(int a) {
    if (flPS2SetTextureRegister(a, psTexture_data_976 + 0x20, psTexture_data_976 + 0x30, psTexture_data_976 + 0x40, psTexture_data_976 + 0x50,
                                psTexture_data_976 + 0x60, psTexture_data_976 + 0x70, flSystemRenderOperation) == 0) {
        return 0;
    }
    flPS2psAddQueue(psTexture_data_976);
    return 1;
}
