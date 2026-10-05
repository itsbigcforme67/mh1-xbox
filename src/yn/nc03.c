/* nc03 - yn.bin network config 0x00534AF0-0x00534B14: yn_netcnf_get_filename. Whole file in netcnf_nm.c. */
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






void yn_netcnf_get_filename(u8 *arg0, int arg1) {
    sprintf(arg0 + 0x19AF4, lit_1846_0053E3A0, arg1, lit_1847_0053E3B0);
}
