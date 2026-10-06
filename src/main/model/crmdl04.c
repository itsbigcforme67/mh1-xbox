/* crmdl04 - SLPM_654.95 0x00124F80-0x00125060: edit_create_model (the two character-edit models: loads each, reserves a model work slot and calls
 * model_work_set; the screen y of the second is 10 + 0x32). */
#include "types.h"
#include "mdlw.h"

extern s32 pl_area_top;
extern s32 EDIT_TEX[];
extern s32 edit_mdlw[2];
extern s32 edit_top[2];

void load_edit_model(int);
int get_start_mdlw(int);
MDLW *get_mdlw_ptr(int);
void set_used_mdlw(int, int);
MDLW *model_work_set(int, int, int, int, int, int);

void edit_create_model(void) {
    int a;
    int h;
    int i;

    for (i = 0; i < 2; i++) {
        load_edit_model(i);
        a = pl_area_top;
        h = get_start_mdlw(1);
        if (h < 0) {
            break;
        }
        edit_top[i] = h;
        edit_mdlw[i] = (s32)get_mdlw_ptr(h);
        set_used_mdlw(h, 1);
        model_work_set((s16)h, a, (s16)(10 + i * 0x32), EDIT_TEX[i], 0x900, 2);
    }
}
