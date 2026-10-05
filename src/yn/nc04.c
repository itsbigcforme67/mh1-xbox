/* nc04 - yn.bin network config 0x00534B90-0x00534DF4: yn_netcnf_num_to_ip, yn_hard_init, yn_hard_status_check, yn_hard_type_check, yn_hard_select_set. Whole file in netcnf_nm.c. */
#include "yn.h"

int DeviceLoadDriver();
int DeviceLoadDriver2();
int DeviceRollbackDriver();
int DeviceSelectInitialize();
int DeviceUpdateStatus();
int FlushCache();
int sceNetcnfifAllocWorkarea();
int sceNetcnfifCheck();
int sceNetcnfifDataInit();
int sceNetcnfifFreeWorkarea();
int sceNetcnfifGetCount();
int sceNetcnfifGetList();
int sceNetcnfifGetResult();
int sceNetcnfifInit();
int sceNetcnfifLoadEntry();
int sceNetcnfifSetFNoDecode();
int sceNetcnfifSetup();
int sceSifLoadModule();
int sceSifLoadStartModule();
int sceSifSearchModuleByName();
int sceSifStopModule();
int sceSifUnloadModule();
extern s32 CurDevice;
extern u8 DeviceWork[];
extern u8 lit_1349_0053E398[];
extern u8 lit_135_0053E300[];
extern u8 lit_136_0053E330[];
extern u8 lit_1846_0053E3A0[];
extern u8 lit_1847_0053E3B0[];
extern u8 lit_1903[];
extern u8 lit_203_0053E360[];
extern u8 lit_204_0053E380[];
extern u8 netcnf_arg[];
extern u8 netcnfif_arg[];

s32 yn_netcnf_init1();
s32 yn_netcnf_init2();
s32 yn_netcnf_exit();
u8 *yn_netcnf_search_usr_name();
void yn_netcnf_work_to_ifc();
void yn_netcnf_ifc_to_work();
void yn_netcnf_work_to_dev();
void yn_netcnf_dev_to_work();
void yn_netcnf_setup_devwork();
s32 yn_netcnf_pastdata_check();
s32 yn_netcnf_pastproxy_check();
s32 yn_netcnf_ip_check();
void yn_netcnf_set_current();
s32 yn_netcnf_get_num();
s32 yn_netcnf_get_list();
void yn_netcnf_net_allload();
s32 yn_netcnf_magicno_check();
s32 yn_netcnf_magicno_check_sub();
void yn_netcnf_get_filename();
void yn_netcnf_ip_to_num();
void yn_netcnf_num_to_ip();
void yn_hard_init();
s32 yn_hard_status_check();
s32 yn_hard_type_check();
s32 yn_hard_select_set();
void yn_utf8_to_sjis();
void yn_sjis_to_utf8();
void module_unload_00535090(s32 id);
void module_load_00535140();























typedef struct DEVW {
    s32 state;          /* 0x00 -1 = unused slot, 1 = ... (guess) */
    u8 _pad04[0x10];
    char *name;         /* 0x14 */
    char *vendor;       /* 0x18 */
} DEVW;                 






void yn_netcnf_num_to_ip(char *dst, u8 *ip) {
    sprintf(dst, lit_1903, ip[0], ip[1], ip[2], ip[3]);
}

void yn_hard_init(void) {
    if (CurDevice != 0) {
        DeviceRollbackDriver();
    }
    DeviceSelectInitialize();
    DeviceUpdateStatus();
}

/* 0x1C */

s32 yn_hard_status_check(char *name, char *vendor) {
    s32 i;
    DEVW *d;

    i = 0;
    d = (DEVW *)DeviceWork;
    do {
        if (d->state != -1 && strcmp(d->name, name) == 0 && strcmp(d->vendor, vendor) == 0) {
            return i;
        }
        i++;
        d++;
    } while (i < 0x23);
    return -1;
}

s32 yn_hard_type_check(char *name, char *vendor) {
    s32 i;
    DEVW *d;

    i = 0;
    d = (DEVW *)DeviceWork;
    do {
        if (d->state != -1 && strcmp(d->name, name) == 0 && strcmp(d->vendor, vendor) == 0) {
            if (((DEVW *)DeviceWork)[i].state == 1) {
                return 1;
            }
            return 2;
        }
        i++;
        d++;
    } while (i < 0x23);
    return 0;
}

s32 yn_hard_select_set(s32 n) {
    s32 cur;
    DEVW *d;

    d = &((DEVW *)DeviceWork)[n];
    cur = CurDevice;
    if (cur != 0) {
        return -(cur != (s32)d);
    }
    if (d->state != 1) {
        return -2;
    }
    CurDevice = (s32)d;
    DeviceLoadDriver(cur, d);
    DeviceLoadDriver2();
    return 1;
}
