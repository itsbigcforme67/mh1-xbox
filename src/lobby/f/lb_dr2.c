/* Lobby browser: font size selection, http state dispatch (0x5D9740..0x602900 small functions, hand written). */
#include "lobby_f.h"
extern u8 *bsw;
extern u8 BFontSizeN[];
extern u8 BFontSizeH[];

/* normal / halftone font size of the current page line for size class `a` (x180, x181 of the browser work) */
void DispFontSize(int a) {
    u8 *w;
    w = bsw;
    w[0x180] = BFontSizeN[(a & 0xFF) * 8 + w[*(s16 *)(w + 0x124) + 0x168]];
    w = bsw;
    w[0x181] = BFontSizeH[(a & 0xFF) * 8 + w[*(s16 *)(w + 0x124) + 0x168]];
}

extern u8 HttpWork[];
void http_test_proc(u8 *p);
extern void (*http_test_proc_jmp_178[])(u8 *);
extern BSSYS *bsSys;
extern u8 *bsCur;
extern s16 BsTimer1;

/* http task state machine: dispatch on the task's state byte */
void http_test_proc(u8 *p) {
    http_test_proc_jmp_178[p[0x40]](p);
}

/* claim the first free http task slot (only slot 0 is ever looked at: the loop ends when the s8 counter leaves 0) */
u8 *HttpTaskPull(void) {
    int i;
    u8 *w;
    i = 0;
    w = HttpWork;
loop_1:
    if (*(s8 *)(w + 0x68) == 0) {
        memset(w, 0, 0xC0);
        *(s8 *)(w + 0x68) = 1;
        *(void **)(w + 0x64) = http_test_proc;
        *(s8 *)(w + 0x46) = i;
        return w;
    }
    i += 1;
    w += 0xC0;
    if (i != 0) {
        return 0;
    }
    goto loop_1;
}

/* does the buffer hold a Shift-JIS lead byte in its first n bytes? (flag at +0x3F) */
int is_sjis(u8 *obj, u8 *s, int n) {
    if (n > 0) {
        do {
            u8 c = *s;
            if ((c >= 0x81 && c <= 0x8D) || (c >= 0x8F && c <= 0xA0)) {
                obj[0x3F] = 1;
                return 1;
            }
            n -= 1;
            s += 1;
        } while (n > 0);
    }
    obj[0x3F] = 0;
    return 0;
}

/* scroll request from a button: mode 1-4 move the page, 5-6 move the cursor line */
void BtnScrollXY(int dx, int dy, u8 mode) {
    switch (mode) {
    case 1:
    case 2:
    case 3:
    case 4:
        *(s32 *)((u8 *)bsSys + 4) += dx;
        *(s32 *)((u8 *)bsSys + 8) += dy;
        break;
    case 5:
    case 6:
        bsCur[5] += dy;
        break;
    }
    BsTimer1 = 3;
    bsCur[3] = mode;
}

extern s32 ContentType_encoded;
extern s32 ContentLength_0;
extern s32 CacheControl_nocache;
extern s32 Pragma_nocache;
extern char verisign_root_ca[];
typedef struct CAINFO { char *data; int size; } CAINFO;
void CpInetHttpInitialize();
void BsMemAllocInitialize();
int BsMemAlloc();
int BsMemRealloc();
int BsMemFree();
void sceHTTPSetMallocFunction();
void sceHTTPSetReallocFunction();
void sceHTTPSetFreeFunction();
int sceHTTPInit();
int get_CA_size();
int sceHTTPSInitCertFromMemory();

/* set up the http library: clear the task table and header strings, hook the allocator, load the root certificate */
int HttpTaskInitialize(void) {
    CAINFO ca;
    Pragma_nocache = 0;
    CacheControl_nocache = 0;
    ContentType_encoded = 0;
    ContentLength_0 = 0;
    memset(HttpWork, 0, 0xC0);
    CpInetHttpInitialize();
    BsMemAllocInitialize();
    sceHTTPSetMallocFunction(BsMemAlloc);
    sceHTTPSetReallocFunction(BsMemRealloc);
    sceHTTPSetFreeFunction(BsMemFree);
    if (sceHTTPInit() < 0) {
        return -1;
    }
    ca.data = verisign_root_ca;
    ca.size = get_CA_size();
    return -(sceHTTPSInitCertFromMemory(0, 1, &ca, 0, 0) < 0);
}

extern u8 Hn_Size[8];
extern char lit_155_006676E0[];
char *strstr(const char *, const char *);
void font_data_clear();
void tagoutprintf4();
void font_data_set();

/* tag handler: remember the heading size digit before the cursor, skip to the end of the tag */
int tagAct_042(u8 **pp, char *s) {
    font_data_clear();
    bsw[0x2D3] = Hn_Size[(u8)((*pp)[-1] - 0x30)] + 0x30;
    bsw[0x2D4] = 0;
    *pp = (u8 *)strstr((char *)*pp, lit_155_006676E0) + 1;
    tagoutprintf4(s);
    font_data_set();
    return 0;
}

/* table image stack (page style images, 0x100 bytes each, depth counter at +0xE96A) */
u8 *pullTableImage(void) {
    u8 *p;
    p = bsw + 0xE96A;
    if (*p != 0) {
        *p = *p - 1;
    }
    p = bsw;
    return p + (p[0xE96A] << 8) + 0xD96A;
}

void pushTableImage(u8 *src) {
    u8 *w;
    u8 *p;
    w = bsw;
    if (*(s8 *)(w + 0x186) == 0) {
        memcpy(w + (w[0xE96A] << 8) + 0xD96A, src, 0x100);
        p = bsw + 0xE96A;
        if (*p < 0xF) {
            *p = *p + 1;
        }
    }
}

extern int strlen();
void tagoutprintf3();

/* tag handler: strip trailing blanks of the tag text, then print it */
int tagAct_035(int arg0, u8 *s) {
    int n;
    n = strlen(s);
    if (n != 0 && s[n - 1] == 0x20) {
        do {
            u8 *t = s + n;
            n -= 1;
            t[-1] = 0;
        } while (n != 0 && s[n - 1] == 0x20);
    }
    tagoutprintf3(s);
    bsw[0x17F] = 0;
    return 0;
}

void font_data_clear();
void font_data_set();

int tagAct_602(int arg0, char *s) {
    u8 *w;
    int v;
    font_data_clear();
    tagoutprintf3(s);
    w = bsw;
    v = w[*(s16 *)(w + 0x124) + 0x168];
    if (v >= 2) {
        v = (v - 1) & 0xFF;
    }
    w[0x2D3] = (v & 0xFF) + 0x30;
    bsw[0x2D4] = 0;
    font_data_set();
    return 0;
}

void font_data_off(void) {
    u8 *w;
    s16 *p;
    u8 a;
    w = bsw;
    if (w[0xD892] != 0) {
        int k = *(u16 *)(w + 0xD894) * 0x5C;
        a = w[k + (int)w + 0x252F];
    } else {
        a = w[0x17C];
    }
    p = (s16 *)(w + 0x124);
    if (*p > 0) {
        *p = *p - 1;
    } else {
        *p = 0;
    }
    DispFontSize(a);
}

void stockTableOutline(int x, int y, int x2, int y2, int t0, int t1, int t2, int t3, u8 *p);

/* draw the outline of a table cell box (not while a page style image is being recorded) */
void Disp_TABLE_Line(u8 *o) {
    u8 *w = bsw;
    if (*(s8 *)(w + 0x186) == 0 && *(w + 0xE96B) == 0) {
        u8 *img = pullTableImage();
        u16 y = *(u16 *)(o + 0x2A);
        u16 x = *(u16 *)(o + 0x28);
        stockTableOutline(x, y, (x + *(u16 *)(o + 0x1C)) & 0xFFFF, (y + *(u16 *)(o + 0x1E)) & 0xFFFF, o[0x1A], *(int *)(o + 0x54), o[0x45], *(int *)(o + 0x58), img);
    }
}

extern u8 pal[];
void BsGameSideFontPalInit();
void flfntSetPalData(int, int, int, int, int);

/* fixed browser font palettes 0x10 (white on black) and 0x11 (grey, dark outline) */
void BsFixPalInit(void) {
    BsGameSideFontPalInit();
    pal[1] = 0;
    pal[0] = 1;
    *(s32 *)(pal + 4) = -1;
    flfntSetPalData(0x10, 0, 0xFF000000, -1, -1);
    pal[9] = 0;
    pal[8] = 1;
    *(s32 *)(pal + 0xC) = 0xFF000001;
    flfntSetPalData(0x11, 0x808080, 0xFF222222, 0xFF111111, 0xFF010101);
}

extern char *BadHeaderList[2];
extern char BsCacheTmpUrlstr[];
void BsUrlCopy_SS();
void _my_tolower();
int strncmp(const char *, const char *, int);

/* index of the first "bad header" prefix the url starts with, -1 if none */
int BsUrlBadHeaderGet(char *url) {
    int i;
    char **p;
    char *cur;
    i = 0;
    p = BadHeaderList;
    BsUrlCopy_SS(BsCacheTmpUrlstr, url);
    _my_tolower(BsCacheTmpUrlstr);
    cur = BadHeaderList[0];
    if (cur != 0) {
        do {
            if (strncmp(BsCacheTmpUrlstr, *p, strlen(cur)) == 0) {
                break;
            }
            p += 1;
            cur = *p;
            i += 1;
        } while (cur != 0);
    }
    if (*p != 0) {
        return i;
    }
    i = -1;
    return i;
}

extern BSNODE BcRequest_head;

/* mark every queued browser request as cancelled (html requests: flag in the task, image requests: state 6) */
void BsRequestCancelAll(void) {
    BSNODE *n;
    n = &BcRequest_head;
    for (;;) {
        n = n->next;
        if (n == 0) {
            return;
        }
        if (n->used == 0) {
            return;
        }
        switch (n->used) {
        case 6:
            break;
        case 5:
        case 4:
            n->used = 6;
            n->x124 = 7;
            n->x125 = 0;
            break;
        case 3:
            *(u8 *)(*(int *)((u8 *)n + 0x118) + 0x35) = 1;
            break;
        }
    }
}

extern char *bsCsv;
extern char *bsUrl;
extern s8 bsIsOnRequesting;
extern char lit_429_00667370[];
extern char lit_430_00667378[];
int BsRequestPostAdd();
void BsRequestHtmlPost();
int strcmp(const char *, const char *);

/* post the lobby info (user id and the csv work) to the server and start the request */
int PostLbsInfoGetOrGameEnd(void) {
    char *id;
    int r;
    id = bsSys->id1;
    if (*(u8 *)id != 0 && strcmp(id, bsCsv) != 0) {
        r = BsRequestPostAdd(lit_429_00667370, ((BSSYS *)bsSys)->id1);
    } else {
        r = BsRequestPostAdd(lit_429_00667370, bsCsv);
    }
    if (r < 0) {
        return -1;
    }
    if (BsRequestPostAdd(lit_430_00667378, bsCsv + 0x11) < 0) {
        return -1;
    }
    BsRequestHtmlPost(&bsUrl);
    bsIsOnRequesting = 1;
    ((BSSYS *)bsSys)->x34 = 1;
    return 1;
}

extern u8 BsLbsErrNum;
void To_ReqCancelWait();
void To_QuitMain_Init();

/* react to a lobby server error flagged by the network side (state 1: leave the browser) */
void BsCheckLbsError(void) {
    u8 e;
    e = BsLbsErrNum;
    switch (e) {
    case 0:
        break;
    case 1:
        switch (bsSys->x2E) {
        case 1:
            To_ReqCancelWait(0xF, e);
            break;
        case 8:
            To_ReqCancelWait(0xC, e);
            break;
        default:
            To_QuitMain_Init(0, e);
            bsSys->x01 = 2;
            bsSys->x02 = 0;
            break;
        }
        BsLbsErrNum = 2;
        break;
    case 2:
    case 3:
        break;
    }
}

extern char lit_444_00667798[];
extern char *special_character_tbl[5];
extern char change_character_tbl[5];
char *strstr(const char *, const char *);

/* replace html character references (&lt; ...) in place by the single characters from the two tables */
void check_special_character(char *s) {
    char **var_s2;
    char *temp_s4;
    int var_s3;
    char *temp_s1;
    char *temp_v0;
    char *temp_v0_2;
    char *var_s0;
    char *var_v1;
    temp_v0 = strstr(s, lit_444_00667798);
    var_s0 = temp_v0;
    if (temp_v0 != 0) {
        do {
            temp_s1 = var_s0 + 1;
            var_s3 = 0;
            var_s2 = special_character_tbl;
        loop_2:
            temp_s4 = *var_s2;
            if (strncmp(var_s0 + 1, temp_s4, strlen(temp_s4)) == 0) {
                *var_s0 = change_character_tbl[var_s3];
                var_v1 = var_s0 + strlen(temp_s4);
                do {
                    var_v1 += 1;
                    var_s0 += 1;
                    *var_s0 = *var_v1;
                } while (*var_v1 != 0);
            } else {
                var_s3 += 1;
                var_s2 += 1;
                if (var_s3 < 5) {
                    goto loop_2;
                }
            }
            temp_v0_2 = strstr(temp_s1, lit_444_00667798);
            var_s0 = temp_v0_2;
        } while (temp_v0_2 != 0);
    }
}

extern u8 INTCTYPE_MAP_006535E0[];

/* print a tag text that may end in a blank-like character (kept back for the next call) */
void tagoutprintf2(u8 *arg0) {
    int temp_a0;
    int temp_a1;
    int var_a3;
    u8 temp_v1;
    u8 var_s0;
    u8 *temp_a0_2;
    u8 *temp_v0;
    s32 *np;

    var_a3 = 0;
    temp_v0 = bsw;
    var_s0 = 0;
    temp_a0 = *(s32 *)(temp_v0 + 4);
    temp_a1 = temp_a0 - 1;
    np = (s32 *)(temp_v0 + 4);
    if (0 < temp_a1) {
        do {
            if (INTCTYPE_MAP_006535E0[arg0[var_a3 & 0xFF]] & 1) {
                var_a3 = (var_a3 + 1) & 0xFF;
            }
            var_a3 = (var_a3 + 1) & 0xFF;
        } while (var_a3 < (*(volatile s32 *)np - 1));
    }
    if ((var_a3 & 0xFF) == temp_a1) {
        temp_a0_2 = (u8 *)(temp_a0 + (int)arg0);
        temp_v1 = temp_a0_2[-1];
        if (INTCTYPE_MAP_006535E0[temp_v1] & 1) {
            temp_a0_2[-1] = 0;
            var_s0 = temp_v1;
        }
    }
    tagoutprintf3(arg0, temp_a1, np, var_a3);
    if (!(var_s0 & 0xFF)) {
        *(s32 *)(bsw + 4) = 0;
        arg0[0] = 0;
        return;
    }
    *(s32 *)(bsw + 4) = 1;
    arg0[0] = var_s0;
    arg0[1] = 0;
}

extern u8 *bsOW[500];

/* form reset helper: find the form start object before object idx, then trim the text pointer of the following inputs */
void ResetFormParam(int idx) {
    int i;
    int found;
    int k;
    u8 **p;
    u8 *o;
    u8 *t;
    i = 0;
    p = bsOW;
    do {
        o = *p;
        if (o == 0 || o[0] == 0) {
            break;
        }
        switch (o[2]) {
        case 0x13:
            found = i;
            break;
        case 4:
            if (i == idx) {
                goto second;
            }
            break;
        }
        i += 1;
        p += 1;
    } while (i < 500);
second:
    k = found + 1;
    if (k < 500) {
        p = &bsOW[k];
        do {
            o = *p;
            if (o == 0 || o[0] == 0) {
                break;
            }
            switch (o[2]) {
            case 6:
            case 7:
                t = *(u8 **)(o + 0x64);
                if (*t != 0) {
                    do {
                        t -= 1;
                    } while (*t != 0);
                    t += 1;
                    *t = 0;
                    *(u8 **)(*p + 0x64) = t;
                }
                break;
            case 0x13:
                return;
            }
            k += 1;
            p += 1;
        } while (k < 500);
    }
}

/* table cell above (row span 2 search): walk the siblings of the parent's first child until one in the same column is found */
u8 *check_upTD_rowspan2(u8 *o) {
    u8 *a;
    u8 *v;
    u8 c;
    a = *(u8 **)(o + 4);
    if (a == 0) {
        return 0;
    }
    v = *(u8 **)(a + 0xC);
    if (v == 0) {
        return 0;
    }
    c = o[0x4C];
    do {
        if (v[0x4C] == c) {
            return (v[0x48] > 1) ? v : 0;
        }
        v = *(u8 **)(v + 8);
    } while (v != 0);
    return 0;
}

/* table cell above (row span search): in a page table walk the cells of the parent row, otherwise take the cell record at +0x36 */
u8 *check_upTD_rowspan(u8 *o) {
    u8 *w;
    u8 *a;
    u8 *v;
    u8 c;
    u16 t;
    w = bsw;
    if (*(s8 *)(w + 0x186) == -0xA) {
        a = *(u8 **)o;
        if (a == 0) {
            return 0;
        }
        if (a[0x4D] == 0) {
            return 0;
        }
        a = *(u8 **)(a + 4);
        if (a == 0) {
            return 0;
        }
        v = *(u8 **)(a + 0xC);
        if (v == 0) {
            return 0;
        }
        c = o[0x4C];
        do {
            if (v[0x4C] == c) {
                return (v[0x48] > 1) ? v : 0;
            }
            v = *(u8 **)(v + 8);
        } while (v != 0);
        goto ret0;
    }
    t = *(u16 *)(o + 0x36);
    if (t != 0) {
        return w + (t & 0xFFFF) * 0x5C + 0x24E0;
    }
ret0:
    return 0;
}

void init_td_data();
int SetTableData();
void set_TH_TD_data_1st();
int check_rowspan_sub();

/* resolve the row spans of a table row: set the cell, check it against the previous one, continue with the cell above */
int check_rowspan2(u8 *prev, u8 *o) {
    s32 temp_s3;
    u8 *cell;
    u8 *w;
    u8 *c;
    u8 *r;
    for (;;) {
        init_td_data();
        if (SetTableData(4) < 0) {
            return -1;
        }
        w = bsw;
        c = (u8 *)((*(u16 *)(w + 0xD894) * 0x5C) + (int)w);
        temp_s3 = *(s32 *)(c + 0x24E0);
        cell = c + 0x24E0;
        if (temp_s3 == 0) {
            return -1;
        }
        w[0x18D] = 1;
        set_TH_TD_data_1st(cell, temp_s3);
        if (check_rowspan_sub(cell, temp_s3, prev) < 0) {
            return -1;
        }
        r = check_upTD_rowspan2(o);
        prev = r;
        if (r == 0) {
            return 0;
        }
    }
}

/* resolve the row spans of the current table cell (see check_rowspan2) */
int check_rowspan(void) {
    u8 *var_v0;
    u8 *temp_a0;
    u8 *temp_v0;
    u8 *var_s1;
    s32 var_s0;

    temp_a0 = bsw;
    temp_v0 = (u8 *)(temp_a0 + (*(u16 *)(temp_a0 + 0xD894) * 0x5C));
    var_s0 = *(s32 *)(temp_v0 + 0x24E0);
    var_s1 = temp_v0 + 0x24E0;
    if (var_s0 == 0) {
        return -1;
    }
    var_v0 = check_upTD_rowspan(var_s1);
    if (var_v0 == 0) {
        goto block_13;
    }
loop_5:
    if (check_rowspan_sub(var_s1, var_s0, var_v0) < 0) {
        return -1;
    }
    if (SetTableData(4) < 0) {
        return -1;
    }
    temp_a0 = bsw;
    temp_v0 = (u8 *)(temp_a0 + (*(u16 *)(temp_a0 + 0xD894) * 0x5C));
    var_s0 = *(s32 *)(temp_v0 + 0x24E0);
    var_s1 = temp_v0 + 0x24E0;
    if (var_s0 == 0) {
        return -1;
    }
    set_TH_TD_data_1st(var_s1, var_s0);
    var_v0 = check_upTD_rowspan(var_s1);
    if (var_v0 == 0) {
block_13:
        return 0;
    }
    goto loop_5;
}

/* copy the cell settings of a table cell record (TH/TD) to the new cell and count the use of the source */
void set_TH_TD_data_1st(u8 *d, u8 *s) {
    u8 v;
    u8 v2;
    if (*(s8 *)(bsw + 0x186) == -0xA) {
        d[0x4C] = s[0x4C];
        d[0x4D] = s[0x4D];
        d[0x45] = s[0x45];
        *(u16 *)(d + 0x32) = *(u16 *)(s + 0x32);
        *(u16 *)(d + 0x30) = *(u16 *)(s + 0x30);
        d[0x50] = d[0x50] | bsw[0x18A];
        *(s32 *)(d + 0x54) = *(s32 *)(bsw + 0xF18);
        v = bsw[0xF16];
        if (v != 0) {
            d[0x4A] = v;
        } else {
            d[0x4A] = s[0x4A];
        }
        v2 = bsw[0xF17];
        if (v2 != 0) {
            d[0x4B] = v2;
        } else {
            d[0x4B] = s[0x4B];
        }
        d[0x44] = 0;
        *(u16 *)(d + 0x20) = *(u16 *)(bsw + 0xF10);
    }
    s[0x4C] = s[0x4C] + 1;
}

/* horizontal alignment offset (+0x3A) of a table cell: centre or right align inside the free width */
void set_align_data(u8 *arg0) {
    s32 temp_a1;
    s32 temp_t0;
    s32 var_v1;
    u16 temp_a3;
    u8 temp_a2_2;
    u8 *temp_a2;
    u16 *tp;

    if (arg0[0x50] & 1) {
        *(s16 *)(arg0 + 0x3A) = 0;
        return;
    }
    temp_a2 = bsw;
    tp = (u16 *)(temp_a2 + (*(u16 *)(temp_a2 + 0x188) * 4) + 0x1540);
    if (*(s8 *)(temp_a2 + 0x186) == 0) {
        *(s16 *)(arg0 + 0x3A) = 0;
        temp_a3 = *tp;
        temp_t0 = *(u16 *)(bsw + 0xD8DC) - *(u16 *)(arg0 + 0x3E);
        if (temp_a3 < temp_t0) {
            temp_a2_2 = arg0[0x4A];
            if ((temp_a2_2 != 2) && (arg0[0x51] == 0)) {
                if (arg0[0x1B] == 3) {
                    goto block_8;
                }
                if (temp_a2_2 == 3) {
                    var_v1 = temp_t0 - temp_a3;
                    goto block_14;
                }
            } else {
block_8:
                temp_a1 = temp_t0 - temp_a3;
                var_v1 = temp_a1 >> 1;
                if (temp_a1 < 0) {
                    var_v1 = (temp_a1 + 1) >> 1;
                }
block_14:
                *(s16 *)(arg0 + 0x3A) = (s16)var_v1;
            }
        }
    }
}

extern u8 INTCTYPE_MAP_006535E0[];

/* skip leading blanks of a text unless the page asks to keep them */
u8 *cut_spacer_string(u8 *s) {
    u8 *w;
    w = bsw;
    if (w[0xD8CC] == 0 && w[0x18B] == 0) {
        while (*s != 0 && (INTCTYPE_MAP_006535E0[*s] & 2)) {
            s += 1;
        }
    }
    return s;
}
