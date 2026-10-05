/* select.bin 0x00533A00-0x00533BDC: overlay entry task (loads textures and sound packs,
   starts the card, fade and demo tasks). */
#include "select.h"

void Demo_task();

void Init_task(STASK *t) {
    void *p = data_load_ptr;
    switch (t->step) {
    case 0:
        load_texlist(PIT_TEX[0], 2, 0);
        FlushCache(0);
        load_bin(0x10002, p);
        flSndPackLoad(p, 1);
        FlushCache(0);
        load_bin(0x10005, p);
        flSndPackLoad(p, 6);
        FlushCache(0);
        load_bin(0x10006, p);
        flSndPackLoad(p, 7);
        SoftKeyboard_init();
        init_std_rate();
        sprite_work_init();
        model_work_init();
        init_card_w();
        MemcardInit();
        select_w.xAC = 0;
        Quest_init();
        PitWork_init();
        Tsk_Execute(Card_task, 13);
        Tsk_Execute(Fade_task, 9);
        t->step++;
        break;
    case 1:
        t->step++;
        if (system_w.x0B == 0) {
            system_w.x0B = 1;
            McOperationSet(0, 2);
            break;
        }
        t->step++;
        break;
    case 2:
        if (McCardOperation() & 0xFF) {
            t->step++;
        }
        trans();
        break;
    case 3:
        system_w_set();
        Tsk_Execute(Demo_task, 3);
        Tsk_Exit(t);
        break;
    }
}
