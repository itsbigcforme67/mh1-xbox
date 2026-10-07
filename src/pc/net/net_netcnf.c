/* net_netcnf.c - Sony's libnetcnfif (network connection settings, kept on the memory card) for
 * the PC / Xbox port.
 *
 * On the PS2 the yn overlay (src/yn, Capcom's "network settings" screens) talks to the IOP
 * netcnf service to list, load and save connection settings (PPP / Ethernet entries). The
 * host already has a network connection, so the port keeps an empty settings store: no
 * entries, every request completes at once and succeeds. The calling convention (read from
 * yn_netcnf_init1 / init2 / get_num / get_list in src/yn/netcnf_nm.c): a request function
 * returns >= 0 when accepted, the caller polls sceNetcnfifCheck() until it returns 0, then
 * reads the result with sceNetcnfifGetResult(&res) (which returns >= 0 on success).
 *
 * Only linked into ONLINE=1 builds, and only needed once the yn overlay is ported.
 */
#include <stdint.h>

static int32_t last_result;

int sceNetcnfifInit(void) { last_result = 0; return 0; }
int sceNetcnfifSetup(void) { return 0; }
int sceNetcnfifTerm(void) { return 0; }
int sceNetcnfifSync(int mode) { (void)mode; return 0; }
int sceNetcnfifCheck(void) { return 0; }                         /* 0 = the last request is done */
int sceNetcnfifGetResult(int32_t *res) { if (res) *res = last_result; return 0; }
int sceNetcnfifSetFNoDecode(int on) { (void)on; return 0; }
int sceNetcnfifAllocWorkarea(int size) { (void)size; last_result = 0; return 0; }
int sceNetcnfifFreeWorkarea(void) { return 0; }
int sceNetcnfifSetEnv(void *env) { (void)env; return 0; }
int sceNetcnfifGetCount(const char *dir, int type) { (void)dir; (void)type; last_result = 0; return 0; }   /* no stored entries */
int sceNetcnfifGetList(const char *dir, int type, void *list, int n) { (void)dir; (void)type; (void)list; (void)n; last_result = 0; return 0; }
int sceNetcnfifLoadEntry(const char *dir, int type, const char *name, void *out) { (void)dir; (void)type; (void)name; (void)out; last_result = -1; return 0; }
int sceNetcnfifLoadEntryAuto(void *a, void *b) { (void)a; (void)b; last_result = -1; return 0; }
int sceNetcnfifAddEntry(const char *dir, int type, const char *name, void *e) { (void)dir; (void)type; (void)name; (void)e; last_result = 0; return 0; }
int sceNetcnfifEditEntry(const char *dir, int type, const char *name, void *e) { (void)dir; (void)type; (void)name; (void)e; last_result = 0; return 0; }
int sceNetcnfifDeleteEntry(const char *dir, int type, const char *name) { (void)dir; (void)type; (void)name; last_result = 0; return 0; }
int sceNetcnfifDeleteAll(const char *dir) { (void)dir; last_result = 0; return 0; }
int sceNetcnfifSetLatestEntry(const char *dir, int type, const char *name) { (void)dir; (void)type; (void)name; last_result = 0; return 0; }
int sceNetcnfifCheckCapacity(const char *dir) { (void)dir; last_result = 0; return 0; }
int sceNetcnfifDataInit(void *d) { (void)d; return 0; }
