/* mkm_nm - SLPM_654.95 0x00121F10-0x00122120 Sethierarchy, 0x00122D50-0x00122E68 mkMaterial: model work setup
   steps (material table, bone hierarchy). Working file. */
#include "types.h"
#include "mdlw.h"

typedef struct MKW {            /* MDLW with the material pointer at 0x10 */
    u8 _pad00[0xA];
    s16 tex_start;              /* 0x0A */
    s16 mat_n;                  /* 0x0C */
    s16 mat_no;                 /* 0x0E */
    u8 *mat;                    /* 0x10 material records (0x4C bytes each) */
} MKW;

extern s32 mem_tex[];
extern char lit_451_00358460[];

s32 plAMOGetMaterialNum(int);
void plAMOSetMaterialData(int, int, void *);
void system_error(char *, int, int, int);
int get_start_material(int);
u8 *get_material_ptr(int);
void set_used_material(int, int);

/* Reserves the model's material records, fills them from the AMO and relocates each texture index through mem_tex. */
int mkMaterial(MKW *w, int amo) {
    int h;
    int n;
    int i;
    u8 *m;

    n = (s16)plAMOGetMaterialNum(amo);
    if (n == 0) {
        system_error(lit_451_00358460, 0, 0, 0);
    }
    h = get_start_material(n);
    if (h < 0) {
        return -1;
    }
    w->mat_no = h;
    w->mat_n = n;
    w->mat = get_material_ptr(h);
    set_used_material(h, n);
    m = w->mat;
    i = 0;
    if (0 < w->mat_n) {
        do {
            plAMOSetMaterialData(amo, i, m);
            i++;
            *(s32 *)(m + 0x44) = mem_tex[w->tex_start + *(s32 *)(m + 0x44)];
            m += 0x4C;
        } while (i < w->mat_n);
    }
    return 0;
}

extern s32 data_load_ptr;

int plAHIGetModelNum(int);
int plAHIGetTreeNum(int);
int plAHIGetTreeModelNum(int, int);
void plCreateInitMotionSetFromAHI(void *, int, int, int);
int flCreateInitMotionSetHandle(void *);
void flGetHierarchySI(void *, int);
int get_start_hierarchy(int);
u8 *get_hierarchy_ptr(int);
void set_used_hierarchy(int, int);

/* Allocates the bone hierarchy for a model (type 0 none, 1 one set, 2 two sets for the two halves) and builds one
   skeleton per tree of the AHI file at amo + 0x128000. */
void Sethierarchy(int amo, MDLW *w, int type) {
    int ahi;
    int n;
    int h;
    int i;
    int k;
    u8 *b0;
    u8 *b1;
    u8 *q;
    MDLW *p;
    u8 tmp[16];

    w->type = type;
    ahi = amo + 0x128000;
    switch (type) {
    case 0:
        w->hier_no = 0;
        w->hier_n = 0;
        w->hier0 = 0;
        w->hier1 = 0;
        w->num = 0;
        return;
    case 1:
        n = plAHIGetModelNum(ahi);
        break;
    case 2:
        n = plAHIGetModelNum(ahi) * 2;
        break;
    }
    h = get_start_hierarchy(n);
    if (h >= 0) {
        w->hier_no = h;
        w->hier_n = n;
        w->hier0 = get_hierarchy_ptr(h);
        if (type == 2) {
            w->hier1 = get_hierarchy_ptr(h + n / 2);
        } else {
            w->hier1 = 0;
        }
        set_used_hierarchy(h, n);
        w->num = plAHIGetTreeNum(ahi);
        k = 0;
        b0 = w->hier0;
        b1 = w->hier1;
        i = 0;
        if (w->num > 0) {
            p = w;
            do {
                plCreateInitMotionSetFromAHI(tmp, data_load_ptr, ahi, i);
                p->si[0] = flCreateInitMotionSetHandle(tmp);
                q = b0 + k * 0x190;
                flGetHierarchySI(q, p->si[0]);
                p->a[0] = q;
                if (type == 2) {
                    q = b1 + k * 0x190;
                    flGetHierarchySI(q, p->si[0]);
                    p->b[0] = q;
                }
                k += plAHIGetTreeModelNum(ahi, i);
                i++;
                p = (MDLW *)((u8 *)p + 4);
            } while (i < w->num);
        }
    }
}
