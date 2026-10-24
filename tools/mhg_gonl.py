#!/usr/bin/env python3
"""mhg_gonl.py LIST|NAME... [--out DIR] : PC C for Monster Hunter G lobby functions the G online port needs and the PC
does not have (docs/network.md 3.12), one file per function in src/lobby/gonl/gen/ (the G online PC build links them;
they win over MH1's copies of the same names).

Each file is m2c's draft of the G function (tools/draft.py mhg/lobby) passed through tools/mhg_c.py: G gp-relative
globals and literal addresses become names (config/mhg/symbols, config/mhg/pc_names.txt); an address without a name gets
one, g_<module>_<ADDR> (added to config/mhg/pc_names.txt, so the PC links it into its copy of that module), never a
literal address in the C. m2c's per-function declarations stay in the function's file (one function per file, so two
functions' guesses about one symbol's type cannot clash); unknown types become s32 (functions) / u8 (data, so m2c's
byte offsets stay byte offsets).

These are drafts: not compared with anything (the PS2 G build does not compile them), reviewed where a run showed a
problem. A file that needs a hand fix gets "HAND-FIXED" in its header and is never overwritten (--force overwrites)."""
import os
import re
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, 'tools'))
import mhg_c  # noqa: E402

MODS = [('main', 0x100000, 0x4A0C80), ('sub_main', 0x4A0C80, 0x542600), ('lobby', 0x542600, 0x762600)]   # (G vram: config/mhg/*.yaml)


def module_of(a):
    for m, lo, hi in MODS:
        if lo <= a < hi:
            return m
    return None


NEW_NAMES = {}


def gen_name(a):
    m = module_of(a)
    if m is None:
        return None
    n = 'g_%s_%06X' % ({'main': 'main', 'sub_main': 'sub', 'lobby': 'lb'}[m], a)
    NEW_NAMES[n] = (m, a)
    return n


CTX_PATH = os.path.join(ROOT, 'build/mhg/gonl_ctx.h')
CTX = {}     # function name -> return type for the declarations m2c then leaves out


def ctx_type(t):
    t = t.strip()
    if '*' in t:
        return 'void *'
    if t in ('f32', 'float', 'f64', 'double'):
        return None
    if t in ('s64', 'u64', 'CNET_RES', 'GRES', 'CBRES'):     # (the 8-byte result structs go by value as one s64)
        return 's64'
    return 's32'


def write_context():
    """m2c guesses a callee's arguments from the registers set before the call, and shows them positionally (a1 alone
    becomes the first argument). A context of the signatures known so far (the generated files, then the PC's own
    G-side C) gives the right count: build/mhg/gonl_ctx.h, from the files of the previous run (run twice after a big
    change). Functions with float arguments are left out (their m2c prototype keeps them as floats)."""
    sigs = {}
    srcs = [os.path.join(ROOT, 'src/lobby/gonl/gen', f) for f in sorted(os.listdir(os.path.join(ROOT, 'src/lobby/gonl/gen')))]
    srcs += [os.path.join(ROOT, f) for f in ('src/lobby/gonl/gonl_g.c', 'src/lobby/f/lb_g.c', 'src/lobby/f/lb_guild_g.c')]
    for f in srcs:
        if not os.path.exists(f) or not f.endswith('.c'):
            continue
        for m in re.finditer(r'^(?:static )?(\w[\w ]*?\**)\s*\b(\w+)\(([^()]*)\)\s*\{', open(f).read(), re.M):
            ret, name, params = m.group(1).strip(), m.group(2), m.group(3).strip()
            if name in sigs or ret == 'static':
                continue
            if params in ('', 'void'):
                ps = []
            else:
                ps = []
                for p in params.split(','):     # by m2c's argN name: a function that ignores $a0 starts at arg1
                    k = re.search(r'\barg(\d+)$', p.strip())
                    k = int(k.group(1)) if k else len(ps)
                    while len(ps) < k:
                        ps.append('s32')
                    ps.append(ctx_type(re.sub(r'\b\w+$', '', p.strip()) or 's32'))
            if None in ps or '...' in params:
                continue
            r = 's32' if ret == 'void' else (ctx_type(ret) or None)     # (void as s32: G code reads a v0 its callee left)
            if r is None:
                continue
            # a G function can hand $a1-$a3 on to its own callee without naming them (cnLBS_LobbyEntry passes the
            # callback in $a2 to __cnetSub_Set_BgProcess): its callers pass all four argument registers
            while '/gonl/gen/' in f and len(ps) < 4:
                ps.append('s32')
            sigs[name] = (r, ps)
    # then the PC's own C of the library functions G code calls (main's C, the cnet packet helpers): ANSI and K&R
    # definitions. Not MH1's lobby / cnet request C: G changed some of those signatures (cnLBS_Send_UserMiniData takes a
    # callback in G), and a call must keep what the G code passes.
    import glob
    pcsrc = [os.path.join(ROOT, 'src/lobby/cnet/cnlbsh.c'), os.path.join(ROOT, 'src/lobby/cnet/cnlbsg.c')]   # (+ the bg process core)
    for d in ('src/main', 'src/pc/rt'):
        pcsrc += sorted(glob.glob(os.path.join(ROOT, d, '**', '*.c'), recursive=True))
    for f in pcsrc:
        if f.endswith('_nm.c') and os.path.exists(f[:-5] + '.c'):
            pass
        try:
            text = open(f, errors='replace').read()
        except OSError:
            continue
        for m in re.finditer(r'^(\w[\w ]*?\**)\s*\b(\w+)\(([^()]*)\)\s*\{', text, re.M):
            ret, name, params = m.group(1).strip(), m.group(2), m.group(3).strip()
            if name in sigs or ret.startswith(('static', 'if', 'else', 'return', 'while', 'switch')):
                continue
            if params in ('', 'void'):
                continue        # (matching C passes registers on unnamed: `void f(void) { g(); }` takes g's arguments)
            ps = [ctx_type(re.sub(r'\b\w+(\[\w*\])?$', '', p.strip()) or 's32') for p in params.split(',')]
            if None in ps or '...' in params or ret in ('', 'else'):
                continue
            r = 's32' if ret == 'void' else ctx_type(ret)
            if r:
                sigs[name] = (r, ps)
        for m in re.finditer(r'^(\w[\w ]*?\**)\s*\b(\w+)\(([\w, ]+)\)\n((?:[\w ]+?[\s*]+\w+(?:\[\w*\])?;\n)+)\{', text, re.M):
            ret, name, plist, decls = m.group(1).strip(), m.group(2), m.group(3), m.group(4)
            if name in sigs or ret.startswith('static'):
                continue
            types = dict((d.group(2), d.group(1)) for d in re.finditer(r'^\s*([\w ]+?[\s*]+)(\w+)(?:\[\w*\])?;', decls, re.M))
            ps = [ctx_type(types.get(a.strip(), 'int')) for a in plist.split(',')]
            r = 's32' if ret == 'void' else ctx_type(ret)
            if None not in ps and r:
                sigs[name] = (r, ps)
    os.makedirs(os.path.dirname(CTX_PATH), exist_ok=True)
    with open(CTX_PATH, 'w') as f:
        f.write('typedef int s32; typedef unsigned int u32; typedef long long s64;\n')
        # the C library G calls (a length set in a tail jump's delay slot is kept only when m2c knows the count)
        LIBC = {'memcpy': ('void *', ['void *', 'void *', 's32']), 'memset': ('void *', ['void *', 's32', 's32']),
                'memcmp': ('s32', ['void *', 'void *', 's32']), 'strncpy': ('void *', ['void *', 'void *', 's32']),
                'strcpy': ('void *', ['void *', 'void *']), 'strcmp': ('s32', ['void *', 'void *']),
                'strncmp': ('s32', ['void *', 'void *', 's32']), 'strlen': ('s32', ['void *']),
                'strcat': ('void *', ['void *', 'void *']), 'strchr': ('void *', ['void *', 's32'])}
        for n, (r, ps) in LIBC.items():
            sigs[n] = (r, ps)
        sigs['cnLBS_Send_ChatBinary'] = ('s32', ['void *', 's32'])     # (its matching C is a pass-through `(void)`)
        for n, (r, ps) in sorted(sigs.items()):
            f.write('%s %s(%s);\n' % (r, n, ', '.join(ps) or 'void'))
    CTX.clear()
    CTX.update((n, r) for n, (r, ps) in sigs.items())


def declare_ctx_calls(src):
    """the functions m2c knew from the context get no declaration from it: declare them (no parameters, as the rest)"""
    head = ''
    for n in sorted(set(re.findall(r'\b([A-Za-z_]\w*)\b', src)) & set(CTX)):    # (calls, and function names as values)
        if n in CTX and not re.search(r'^\w[\w ]*\**\s*\b%s\(' % n, src, re.M):
            head += '%s %s();\n' % ('void *' if CTX[n] == 'void *' else 's64' if CTX[n] == 's64' else 's32', n)
    return head + src


BLOCKS = {}


def asm_of(name):
    """the function's asm block (tools/draft.py's view of asm/mhg/<module>)"""
    import draft as draft_py
    mod = 'mhg/' + (module_of(gaddr(name)) or 'lobby')
    if mod not in BLOCKS:
        BLOCKS[mod] = draft_py.blocks(mod)
    b = BLOCKS[mod].get(name)
    return b[1] if b else ''


def draft(name):
    mhg_c.WIDTHS = mhg_c.asm_widths(asm_of(name))
    # m2c's EE target: G passes arguments 5-8 in $8-$11 (mips-mwcc reads them from the stack and loses them)
    env = dict(os.environ, DRAFT_T=os.environ.get('DRAFT_T', 'mipsee-gcc-c'))
    if CTX:     # (without the function itself: m2c would take its parameter types from there, void * for an s32 *)
        own = CTX_PATH + '.one'
        with open(own, 'w') as f:
            f.writelines(l for l in open(CTX_PATH) if not re.search(r'\b%s\(' % name, l))
        env['DRAFT_CTX'] = own
    p = subprocess.run([sys.executable, os.path.join(ROOT, 'tools/draft.py'), 'mhg/' + (module_of(gaddr(name)) or 'lobby'), name],
                       capture_output=True, text=True, env=env)
    return p.stdout


def convert(src):
    # names for addresses mhg_c could not name
    src = mhg_c.fix(src)

    def todo(m):
        a = int(m[2], 16)
        n = gen_name(a)
        return '(*(%s)&%s)' % (m[1], n) if n else m[0]
    src = re.sub(r'\(\*\(([^()]*)\)G_TODO\(0x([0-9A-Fa-f]+)\)\)', todo, src)

    def dn(m):
        n = gen_name(int(m[1], 16))
        return n or m[0]
    src = re.sub(r'/\*G:0x[0-9A-Fa-f]+\*/(?:/\*G:0x[0-9A-Fa-f]+\*/)*D_0*([0-9A-F]{6})\b', dn, src)
    src = src.replace('(*(void *)(', '(*(u8 *)(').replace('(*(void *)&', '(*(u8 *)&')
    # m2c's void * temporaries: byte pointers (GNU C's void * arithmetic is by bytes too, but they get dereferenced)
    src = re.sub(r'^    void \*(temp_|var_)', r'    u8 *\1', src, flags=re.M)     # a field of unknown type: G reads bytes there (lbu) in every case seen
    src = by_value_args(src)
    src = byte_pointers(src)
    src = byte_globals(src)
    src = pointer_globals(src)
    src = sign_extends(src)
    src = struct_args(src)
    src = arg_positions(src)
    src = tail_jumps(src)
    src = narrow_s64(src)
    src = deref_ints(src)
    src = function_tables(src)
    src = tex_slots(src)
    src = unprototype(src)
    # m2c's declarations of names that became NAME+offset are no declarations any more
    src = re.sub(r'^extern [^;\n]*\(\*\([^;\n]*;\n', '', src, flags=re.M)
    # (data m2c typed by its loads and stores keeps that type: `dialogData = arg0` is a word store; byte_globals keeps
    # m2c's byte offsets on it)
    # m2c declares the D_ names it saw; after renaming, declare the new names once
    used = set(re.findall(r'\bg_(?:main|sub|lb)_[0-9A-F]{6}\b', src))
    decl = ''.join('extern u8 %s;\n' % n for n in sorted(used) if not re.search(r'extern [^;]*\b%s\b' % n, src))
    return declare_ctx_calls(decl + undeclared(src))


def g_tex_slot(g):
    # G renumbered mem_tex (docs/mhg.md): the PC keeps MH1's numbers (its own loads and MH1's C agree), so G's code
    # must use MH1's slot: G 0x179..0x188 = MH1 0x150..0x15F (the 0x15x group, +0x29: the lobby UI sprites G 0x180 =
    # MH1 0x157), G 0x13F..0x178 = MH1 0x118.. (menu / HUD slots, +0x27). With G's numbers the lobby drew empty or
    # white squares (the town name tags' icon, the rule sheet's buttons)
    if 0x179 <= g <= 0x188:
        return g - 0x29
    if 0x13F <= g < 0x179:
        return g - 0x27
    return g


def tex_slots(src):
    src = re.sub(r'\breload_tex\((\d+), 0x([0-9A-Fa-f]+)\)',
                 lambda m: 'reload_tex(%s, 0x%X)' % (m[1], g_tex_slot(int(m[2], 16))), src)
    return re.sub(r'\bSetTextureStage\(0x([0-9A-Fa-f]+)\)', lambda m: 'SetTextureStage(0x%X)' % g_tex_slot(int(m[1], 16)), src)


# G functions that carry an MH1 name (the split matched them by place) but are other code: the PC's cnet C keeps the
# MH1 function of that name, so G's gets a name of its own in every generated file
G_RENAME = {
    '_cnet_CallBack_Result_PersonalDataChange': 'gonl_cb_FileDownload',    # G 0x5BFD00: the file download burst's result
    'check_sender0': 'gonl_send_pl_status0',                   # G 0x59A820: Lb_send_pl_status(a0, 0) (MH1's is a compare)
    'cnLBS_Get_PlazaStatus': 'gonl_Get_LobbyStatus',           # G 0x5AC320: a lobby (town) record's status
    'cnLBS_Get_mhPlazaJoinUser': 'gonl_Get_LobbyJoinUser',     # G 0x5AC1F0: a lobby record's two user counts
    'cnLBS_Get_RoomStatus': 'gonl_Get_PlazaStatus',            # G 0x5E37C0: a plaza record's status (MH1: a room's)
    'cnLBS_Get_mhRoomJoinUser': 'gonl_Get_PlazaJoinUser',      # G 0x5E3660: a plaza record's two user counts
    '_cnet_RecvFromLbs_BothAnnexJoinUser': 'gnet_6205_both',    # G 0x5E3700: plaza / lobby join users (G's records)
    '_cnet_RecvFromLbs_AnswerLobbyExplain': 'gnet_620A_ans',    # G 0x5E3940: plaza / lobby explanation (G's records)
    # G 0x5B0690: G's whisper send (u8, id, text, len, callback; row 0xFD, the u8 last), MH1's ChatMessageTU with one more
    # argument; the split's cnLBS_Send_ChatMessageTU (G 0x5B08B0) is the real ChatBinaryTU (row 0x102, MH1's shape)
    'cnLBS_Send_ChatBinaryTU': 'gonl_Send_ChatMessageTU',
    '__cnet_SendSet_ChatMessageTU': 'gonl_SendSet_PlazaEntry',  # G 0x5ACC30: row 46, three u16 (cnLBS_PlazaEntry's send)
    # G's room getters (src/lobby/cnet/cnet_g.c): the split names them after MH1's lobby getters, or the field moved
    'cnLBS_Get_LobbyStatus': 'gnet_get_room_status',            # G 0x5BB650: room status (room table +0x1E)
    'cnLBS_Get_mhLobbyJoinUser': 'gnet_get_room_joinuser',      # G 0x5BB550: a room's two user counts (+0xA / +0xC)
    'cnLBS_Get_RoomPasswordInfo': 'gnet_get_room_password',     # G 0x5BE0E0: +0x1F (MH1's struct: +0x1D)
    'cnLBS_Get_MatchEntryJoinUser': 'gnet_get_room_entryusers', # G 0x5BF210: +0x1A / +0x1C (MH1's struct: +0x18 / +0x1A)
    # G's town-search string sends under MH1's rule-read names: as those names they replaced the PC's rule reads
    '__cnet_SendReq_RuleListHeadWord': 'gonl_SendReq_6316',     # G 0x5AEC60: row 95 (6316), an obfuscated string
    '__cnet_SendReq_RuleNumOfChoice': 'gonl_SendReq_6317',      # G 0x5AED70: row 99 (6317), an obfuscated string
    'cnLBS_Read_RoomRuleCaption': 'gnet_read_6316',             # G 0x5AEBE0..: its read wrappers of those (src/lobby/cnet/cnet_g.c)
    'cnLBS_Read_RoomRuleChoiceCount': 'gnet_read_6317',
    'cnLBS_RoomEntry': 'gnet_room_entry',                       # G 0x5BBED0: 6406 with the callback (cnet_g.c)
    'cnLBS_Read_LobbyName': 'gnet_read_6403',                   # G 0x5BB4D0: a room's join users (PieceJoinUser(2), 6403)
    # G burst callbacks under MH1 callback names the PC's MH1 bursts use (room rules read / set)
    '_cnet_CallBack_Result_RoomRuleCaption': 'gonl_cb_BurstFail_5ABA90',   # G 0x5ABA90: a burst ends with -1
    '_cnet_CallBack_Result_RoomSetFinish': 'gonl_cb_PlazaCount_5ABE90',    # G 0x5ABE90: the plaza count read's result
}


def renamed(src):
    for a, b in G_RENAME.items():
        src = re.sub(r'\b%s\b' % re.escape(a), b, src)
    return src


# one-line corrections applied to a function's draft every time it is generated (instead of a HAND-FIXED copy)
FIXUPS = {
    'Lb_check_receipt': [   # G passes its own a0 (the sender's id) on to Lb_get_plID unchanged
        (r'= Lb_get_plID\(\)', '= Lb_get_plID(arg0)')],
    'Lb_get_comment': [     # the same (G 0x56C2B0)
        (r'= Lb_get_plID\(\)', '= Lb_get_plID(arg0)')],
    'lbc_login_init': [   # the PC's cnLBS_LoginLobbyServer takes CNET_LOGIN by value (EE GCC passes a pointer to a copy)
        (r'cnLBS_LoginLobbyServer\(&sp40, ', 'cnLBS_LoginLobbyServer(*(struct { u8 b[0x40]; } *)&sp40, ')],
    'Lb_move_common': [   # em_work keeps MH1's 0xA10 stride on the PC (EMW_STRIDE, include/version.h): G's 0xA00 moved
                          # only slot 0 (the town / guild hall NPCs 1-8 never ran their init and stood at 0, 0, 0)
        (r'var_s3 \+= 0xA00;', 'var_s3 += EMW_STRIDE;')],
    'lb_rule_seet_set': [   # m2c puts the inner switch's (empty) default label into the outer switch: two defaults
        (r'\n    default:                                        /\* switch 2 \*/\n', '\n'),
        (r'        break;\n    \}\n\}\n', '        break;\n    }\n    return 0;\n}\n')],
    'lb_guild_make_room': [   # Lbc_SetPropaty's other results (a break out of the switch): G returns 2 (still busy)
        (r'\n    \}\n\}\n$', '\n    }\n    return 2;\n}\n')],
    'lobby_func_00570320': [   # a byte of LobbyInfo+0x16 at a byte offset m2c holds in a pointer
        (r'\*\(&\(\*\(u8 \*\)\(\(u8 \*\)&LobbyInfo \+ 0x16\)\) \+ var_a0\)', '*((u8 *)&LobbyInfo + 0x16 + (s32) var_a0)')],
}


def fixups(name, src):
    for a, b in FIXUPS.get(name, []):
        src, n = re.subn(a, b, src)
        if n == 0:
            print('fixup for %s did not apply: %s' % (name, a))
    return src


KEEP_MH1 = {'nwDispStr_Html', 'Display_StringData', 'Analysis_StringData', 'DispDialogData', 'Draw_menu_square'}


LABELS = None


def labels():
    """G lobby address -> its asm label (the names m2c's drafts call functions by)"""
    global LABELS
    if LABELS is None:
        import glob
        LABELS = {}
        for f in glob.glob(os.path.join(ROOT, 'asm/mhg/lobby/text/*.s')):
            cur = None
            for ln in open(f):
                m = re.match(r'glabel (\S+)', ln)
                if m:
                    cur = m[1]
                    continue
                if cur:
                    m = re.match(r'\s*/\* [0-9A-F]+ ([0-9A-F]{8}) ', ln)
                    if m:
                        LABELS[int(m[1], 16)] = cur
                        cur = None
    return LABELS


def symbol_addr(name):
    for f in ('lobby', 'main', 'sub_main'):
        for ln in open(os.path.join(ROOT, 'config/mhg/symbols/%s.txt' % f)):
            m = re.match(r'(\S+) = 0x([0-9A-Fa-f]+);', ln)
            if m and m[1] == name:
                return int(m[2], 16)
    return None


def calls_through_bytes(src):
    out, i = '', 0
    for m in re.finditer(r'(?<![\w)])\*\(&(\w+) \+ \(', src):
        if m.start() < i:
            continue
        k, depth = m.end(), 1          # the index expression, balanced
        while k < len(src) and depth:
            depth += (src[k] == '(') - (src[k] == ')')
            k += 1
        inner = src[m.end():k - 1]
        if not inner.endswith(' * 4') or not src.startswith(')(', k):
            continue
        out += src[i:m.start()] + '(*(s32 *)((u8 *)&%s + (%s)))' % (m.group(1), inner)
        i = k + 1
    return out + src[i:]


def function_tables(src):
    """m2c's call through a table of functions, (*(s32 *)((u8 *)&TBL + (I * 4)))(...), becomes a call through a C
    array of the same functions by name (the PC's copy of G's table holds addresses the PC cannot call)"""
    import struct
    d = open(os.path.join(ROOT, 'build/mhg/split/lobby.bin'), 'rb').read()
    # (a table of bytes-typed data called through: `*(&TBL + (I * 4))(` is the same call)
    src = calls_through_bytes(src)
    key = '(*(s32 *)((u8 *)&'
    out, i, decls = '', 0, ''
    while True:
        j = src.find(key, i)
        if j < 0:
            return decls + out + src[i:] if decls else src
        k, depth = j + 1, 1
        while depth:
            depth += (src[k] == '(') - (src[k] == ')')
            k += 1
        m = re.match(r'^\(\*\(s32 \*\)\(\(u8 \*\)&(\w+) \+ \((.*) \* 4\)\)\)$', src[j:k], re.S)
        if not m or src[k] != '(':
            out += src[i:k]
            i = k
            continue
        tbl, idx = m[1], m[2]
        a = symbol_addr(tbl)
        names = []
        while a and 0x542600 <= a < 0x542600 + len(d):
            w = struct.unpack_from('<I', d, a - 0x542600)[0]
            if w not in labels():
                break
            names.append(labels()[w])
            a += 4
        if not names:
            out += src[i:k]
            i = k
            continue
        decls += ''.join('s32 %s();\n' % n for n in sorted(set(names)))
        decls += 'static s32 (*const %s_g[])() = { %s };   /* G\'s table of functions, by name */\n' % (tbl, ', '.join(names))
        out += src[i:j] + '%s_g[%s]' % (tbl, idx)
        i = k
        # m2c writes `()` when the caller's own a0.. reach the table's function untouched (the G code does not set
        # them): pass the caller's arguments on
        if src.startswith('()', k):
            heads = re.findall(r'^\w[\w *]*?\b\w+\(([^()]*)\) \{$', src[:j], re.M)
            args = re.findall(r'\b(arg\d)\b', heads[-1]) if heads else []
            if args:
                out += '(%s)' % ', '.join(args)
                i = k + 2
    return src


def frame_size(addr):
    """the G function's stack frame (its first `addiu sp, sp, -N`), 0 for a leaf without one"""
    import struct
    mod = module_of(addr) or 'lobby'
    base = dict((m, lo) for m, lo, hi in MODS)[mod]
    d = open(os.path.join(ROOT, 'build/mhg/split/%s.bin' % mod), 'rb').read()
    for k in range(4):
        w = struct.unpack_from('<I', d, addr - base + 4 * k)[0]
        if w >> 16 == 0x27BD and w & 0x8000:
            return 0x10000 - (w & 0xFFFF)
    return 0


def stack_buffers(src, addr):
    """G's stack slots are one frame: m2c's locals spXX (one variable each, of the type it saw) become views into one
    byte array of G's frame size, so a record copied to sp70 is read back through sp71 / sp72 as on the PS2, and a
    slot used as a buffer has the room G gave it"""
    n = frame_size(addr)
    if not n:
        return src
    decls = [m for m in re.finditer(r'^    ([\w ]+?\**)\s*\b(sp([0-9A-F]+));( +/\*.*?\*/)?\n', src, re.M)
             if m.group(1).split()[0] not in ('return', 'goto', 'case')]
    used = set(re.findall(r'\b(sp([0-9A-F]+))\b', src))
    if not used:
        return src
    typed = {}
    for m in decls:
        if int(m.group(3), 16) < n:
            typed[m.group(2)] = m.group(1).strip()
            src = src.replace(m.group(0), '', 1)
    lines = ['    u8 g_frame[0x%X] __attribute__((aligned(16)));   /* G\'s stack frame */\n' % n]
    for var, off in sorted(used, key=lambda v: int(v[1], 16)):
        if int(off, 16) >= n:
            continue
        lines.append('#define %s (*(%s *)(g_frame + 0x%s))\n' % (var, typed.get(var, 'u8'), off))
    # after the function's opening brace
    k = src.index('{\n', src.index(') {')) + 2
    return src[:k] + ''.join(lines) + src[k:]


def byte_pointers(src):
    """m2c's pointer arithmetic is in bytes (a `s32 *` walked by `var += 0x11`, a `u16 *` by `+= 2`): pointer locals become
    u8 * and each dereference `*var` reads its type through a cast"""
    for m in list(re.finditer(r'^    ((?:s8|u8|s16|u16|s32|u32|f32|s64|u64|s128) \*+)(\w+);$', src, re.M)):
        typ, var = m.group(1).strip(), m.group(2)
        if typ in ('s8 *', 'u8 *'):
            continue
        src = src.replace(m.group(0), '    u8 @%s;   /* (%s) */' % (var, typ), 1)
        src = re.sub(r'\*%s\b' % var, '(*(%s)%s)' % (typ, var), src)
        src = src.replace('    u8 @%s;' % var, '    u8 *%s;' % var)
    return src


def byte_globals(src):
    """the same for globals: m2c's `*(&NAME + k)` / `&NAME + k` count k in bytes whatever NAME's type"""
    types = dict((m.group(2), m.group(1).strip()) for m in re.finditer(r'^extern ([\w ]+?\**)\s*(\w+);', src, re.M))
    for name, typ in types.items():
        if typ in ('u8', 's8'):
            continue
        # *(&NAME + E) with E balanced
        out, i = '', 0
        key = '*(&%s + ' % name
        while True:
            j = src.find(key, i)
            if j < 0:
                out += src[i:]
                break
            k, depth = j + 2, 1
            while depth:
                c = src[k]
                depth += (c == '(') - (c == ')')
                k += 1
            expr = src[j + len(key):k - 1]
            out += src[i:j] + '(*(%s *)((u8 *)&%s + %s))' % (typ, name, expr)
            i = k
        src = re.sub(r'(?<!\(u8 \*\))&%s \+ ' % name, '(u8 *)&%s + ' % name, out)
    return src


def arg_positions(src):
    """m2c names a parameter by its register (arg1 = $a1): a function that ignores $a0 is `f(s32 arg1, s32 arg2)`,
    which C would take as the first two. Fill the gaps with unused parameters."""
    def one(m):
        ps = [p.strip() for p in m.group(2).split(',')] if m.group(2).strip() not in ('', 'void') else []
        out = []
        for p in ps:
            k = re.search(r'\barg(\d+)$', p)
            while k and len(out) < int(k.group(1)):
                out.append('s32 unused_a%d' % len(out))
            out.append(p)
        return '%s(%s) {' % (m.group(1), ', '.join(out) if out else 'void')
    return re.sub(r'^(\w[\w *]*?\b\w+)\(([^()]*)\) \{$', one, src, flags=re.M)


def tail_jumps(src):
    """a G function that is only a jump to another (`j f`, arguments untouched) comes out of m2c as `void g(void) { f(); }`:
    pass eight argument words on (extra ones are harmless in the PC's calling convention) and the result back"""
    A = ', '.join('a%d' % i for i in range(8))
    return re.sub(r'^\w[\w *]*?\b(\w+)\(void\) \{\n    (\w+)\(\);\n\}$',
                  lambda m: 's32 %s(%s) {   /* a jump to %s */\n    return (s32) %s(%s);\n}' % (
                      m.group(1), ', '.join('s32 a%d' % i for i in range(8)), m.group(2), m.group(2), A),
                  src, flags=re.M)


CALLBACKS = set()      # functions G calls with an 8-byte result by value (set in main from the generated files)


def find_callbacks():
    """names used as values (a callback handed to the network layer) or named CallBack_*: their first parameter is the
    8-byte result passed by value"""
    gen = os.path.join(ROOT, 'src/lobby/gonl/gen')
    for f in os.listdir(gen):
        t = open(os.path.join(gen, f)).read()
        CALLBACKS.update(re.findall(r'\(s32\) (\w+)\b(?!\s*\()', t))
        CALLBACKS.update(re.findall(r'[(,]\s*&?((?:CallBack|lobby_func|gonl_cb|_cnet_CallBack)\w*)\s*[,)]', t))


def narrow_s64(src):
    """m2c's EE target types a register the code moves with 64-bit instructions as s64; on the PC an s64 argument takes
    two words. Parameters, locals and casts become s32, except a callback's first parameter (the 8-byte result)"""
    def sig(m):
        head, params = m.group(1), m.group(2)
        name = re.split(r'[\s*]', head)[-1]
        ps = [p.strip() for p in params.split(',')] if params.strip() not in ('', 'void') else []
        out = []
        for i, p in enumerate(ps):
            if re.match(r'[su]64 arg\d+$', p) and not (i == 0 and (name in CALLBACKS or name.startswith(('CallBack', '_cnet_CallBack', 'gonl_cb')))):
                p = 's32' + p[3:]
            out.append(p)
        return '%s(%s) {' % (head, ', '.join(out) if out else 'void')
    src = re.sub(r'^(\w[\w *]*?\b\w+)\(([^()]*)\) \{$', sig, src, flags=re.M)
    src = re.sub(r'^    [su]64 ((?:var|temp)_\w+);', r'    s32 \1;', src, flags=re.M)
    # (not an 8-byte result being passed on: `(s64) *(s64 *)&sp18`)
    return re.sub(r'(?<![*\w])\([su]64\) (?!\*\(s64 \*\)&|sp[0-9A-F]+\b)', '(s32) ', src)


def deref_ints(src):
    """`*(temp + k)` where temp is an int that holds an address (m2c read a pointer as s32): a byte read through it"""
    ints = set(re.findall(r'^    s32 (\w+);', src, re.M))
    for v in ints:
        src = re.sub(r'(?<![\w)\]])\*\(%s \+ ' % v, '*(u8 *)(%s + ' % v, src)
    return src


def struct_args(src):
    """a callback called with its 8-byte result by value and a pointer to it: `f((s64) sp18, &sp18)` passes the bytes
    at sp18, not sp18's value widened"""
    return re.sub(r'\(s64\) (sp[0-9A-F]+)\b', r'*(s64 *)&\1', src)     # (an 8-byte result in a frame slot)


def sign_extends(src):
    """m2c's 64-bit sign / zero extension `(s64) (X << 0x38) >> 0x38` shifts a 32-bit int by 56 (undefined in C: GCC drops
    the loop that counts with it): a cast to the narrow type"""
    while True:
        new = sign_extends_once(src)
        if new == src:
            return src
        src = new


def sign_extends_once(src):
    W = {'0x38': '8', '0x30': '16', '0x20': '32'}
    out, i = '', 0
    while True:
        m = re.compile(r'\(([su])64\) \(').search(src, i)
        if not m:
            return out + src[i:]
        q, depth = m.end() - 1, 0
        k = q
        while True:
            depth += (src[k] == '(') - (src[k] == ')')
            k += 1
            if depth == 0:
                break
        inner = src[q + 1:k - 1]
        t = re.match(r'^(.*) << (0x38|0x30|0x20)$', inner, re.S)
        if t and src.startswith(' >> %s' % t[2], k) and 'M2C_ERROR' not in inner:
            out += src[i:m.start()] + '((%s%s) (%s))' % (m[1], W[t[2]], t[1])
            i = k + len(' >> %s' % t[2])
        else:
            out += src[i:m.end()]
            i = m.end()


def pointer_globals(src):
    """and for pointers kept in globals: `(*(s32 **)&cw) + 0xFA84` is m2c's cw plus 0xFA84 bytes: the pointer is read
    as a u8 *; a dereference `*(*(s32 **)&X)` keeps its width"""
    T = r'(s16|u16|s32|u32|f32|s64|u64)'
    X = r'(&\w+|\(\(u8 \*\)&\w+ \+ 0x[0-9A-Fa-f]+\))'
    src = re.sub(r'(?<![\w)\]])\*\(\*\(%s \*\*\)%s\)' % (T, X), r'(*(\1 *)(*(u8 **)\2))', src)
    return re.sub(r'\(\*\(%s \*\*\)%s\)' % (T, X), r'(*(u8 **)\2)', src)


def by_value_args(src):
    """an 8-byte struct passed by value (a callback's CNET_RES) arrives as `s64 arg0`, which the function copies to a
    stack slot spXX; m2c names its bytes unkspYY: read them from the copy"""
    m = re.search(r'\bsp([0-9A-F]+) = arg0;', src)
    if not m or 'unksp' not in src:
        return src
    base = int(m[1], 16)
    src = re.sub(r'^extern u8 unksp\w+;\n', '', src, flags=re.M)
    return re.sub(r'\bunksp([0-9A-F]+)\b', lambda k: '(((s8 *)&sp%X)[%d])' % (base, int(k[1], 16) - base), src)


def unprototype(src):
    """m2c's prototypes of called functions guess the argument count from one call each: declare them without
    parameters (K&R), and a void one as returning s32 (its result is sometimes used); a float argument keeps the
    prototype (it would be passed as a double otherwise)"""
    def one(m):
        ret, name, params = m.group(1), m.group(2), m.group(3)
        if re.search(r'\bf32\b|\bf64\b', params):
            return m.group(0)
        if ret.strip() == 'void':
            ret = 's32 '
        return '%s%s();%s' % (ret, name, m.group(4))
    return re.sub(r'^([\w ]+?\**\s*)(\w+)\(([^;\n]*)\);(\s*/\* extern \*/)', one, src, flags=re.M)


def undeclared(src):
    """names the conversion introduced (&NAME) without a declaration: bytes"""
    out = []
    for n in sorted(set(re.findall(r'&(\w+)\b', src))):
        if n in CTX or re.search(r'^(?:extern |static )?\w[\w ]*\**\s*\b%s\b\s*(?:\(|;|\[)' % n, src, re.M):
            continue
        if re.search(r'^\s+(?:s8|u8|s16|u16|s32|u32|f32|s64|u64|s128|void \*)\s+\**%s\b' % n, src, re.M):
            continue
        out.append('extern u8 %s;\n' % n)
    return ''.join(out) + src


def next_symbol_gap(a):
    """bytes from a G address to the next named G object (a size for a name the generator made), 4..0x100"""
    import bisect
    keys = []
    for f in ('main', 'sub_main'):
        for ln in open(os.path.join(ROOT, 'config/mhg/symbols/%s.txt' % f)):
            m = re.match(r'\S+ = 0x([0-9A-Fa-f]+);', ln)
            if m:
                keys.append(int(m[1], 16))
    keys.sort()
    i = bisect.bisect_right(keys, a)
    gap = (keys[i] - a) if i < len(keys) else 0x100
    return max(4, min(gap, 0x100))


def write_names(names):
    """lobby names: config/mhg/pc_names.txt (the PC aliases them into its copy of lobby.bin); main / sub_main names:
    config/mhg/pc_data.txt (the PC imports them as host objects with the ELF's bytes and pointers)"""
    p = os.path.join(ROOT, 'config/mhg/pc_names.txt')
    have = open(p).read()
    with open(p, 'a') as f:
        for n, (m, a) in sorted(names.items()):
            if m == 'lobby' and ' %s ' % n not in have:
                f.write('lobby %s 0x%06X  # tools/mhg_gonl.py: a G address the G online code uses, no name yet\n' % (n, a))
    p = os.path.join(ROOT, 'config/mhg/pc_data.txt')
    have = open(p).read()
    with open(p, 'a') as f:
        for n, (m, a) in sorted(names.items()):
            if m != 'lobby' and '\n%s = ' % n not in have:
                f.write('[%s]\n%s = 0x%08X; // size:0x%X   # tools/mhg_gonl.py: G data the G online code uses (size: up to the '
                        'next name)\n' % (m, n, a, next_symbol_gap(a)))


HEADER = '''/* G online (PC): %s, written from Monster Hunter G's code at lobby 0x%06X by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

'''


GADDR = {}


def gaddr(name):
    """a G function's address: the lobby's names first, then main's and sub_main's (G code the lobby calls)"""
    if not GADDR:
        for f in ('sub_main', 'main', 'lobby'):
            for ln in open(os.path.join(ROOT, 'config/mhg/symbols/%s.txt' % f)):
                m = re.match(r'(\S+) = 0x([0-9A-Fa-f]+);', ln)
                if m:
                    GADDR[m[1]] = int(m[2], 16)
    return GADDR.get(name, 0)


def main():
    args = [a for a in sys.argv[1:] if not a.startswith('--')]
    out = os.path.join(ROOT, 'src/lobby/gonl/gen')
    if '--out' in sys.argv:
        out = sys.argv[sys.argv.index('--out') + 1]
        args.remove(out)
    names = []
    for a in args:
        names += [l.split()[-1] for l in open(a)] if os.path.isfile(a) else [a]
    os.makedirs(out, exist_ok=True)
    if '--no-ctx' not in sys.argv:
        write_context()
    find_callbacks()
    bad = []
    names = [n for n in names if not (0x5C1800 <= gaddr(n) < 0x5E1000)]    # the in-game browser: not on the PC
    # the lobby-server receive handlers are src/lobby/cnet/cnet_g.c's (gnet_*, named per address in pc_names.txt);
    # the text / dialog drawing keeps MH1's C (it works in G's village): see KEEP_MH1
    gnet = set(int(l.split()[2], 16) for l in open(os.path.join(ROOT, 'config/mhg/pc_names.txt'))
               if len(l.split()) > 2 and l.split()[1].startswith('gnet_'))
    names = [n for n in names if (gaddr(n) not in gnet or n in G_RENAME) and n not in KEEP_MH1 and not re.match(r'(Bs|bs|Draw|draw)[A-Z]', n)]
    # G functions the PC already has as reviewed C (the G village: src/lobby/f/lb_g.c, lb_guild_g.c; cnet_g.c) stay theirs
    have = set()
    for f in ('src/lobby/f/lb_g.c', 'src/lobby/f/lb_guild_g.c', 'src/lobby/cnet/cnet_g.c'):
        have |= set(re.findall(r'^[A-Za-z_][\w \*]*?\b(\w+)\([^;]*$', open(os.path.join(ROOT, f)).read(), re.M))
    names = [n for n in names if n not in have]
    for n in names:
        path = os.path.join(out, 'gn_' + G_RENAME.get(n, n) + '.c')     # (gn_: an object name of its own; src/lobby/b/nm has files named after functions too)
        if os.path.exists(path) and 'HAND-FIXED' in open(path).read() and '--force' not in sys.argv:
            for a in re.findall(r'\bg_(?:main|sub|lb)_([0-9A-F]{6})\b', open(path).read()):
                gen_name(int(a, 16))      # (its names stay declared)
            continue
        d = draft(n)
        if 'Decompilation failure' in d or 'not found' in d:
            bad.append(n)
            d = '#error "m2c could not decompile %s: write it by hand"\n/*\n%s*/\n' % (n, d.replace('*/', '* /'))
        else:
            d = fixups(n, renamed(stack_buffers(convert(d), gaddr(n))))
        open(path, 'w').write(HEADER % (G_RENAME.get(n, n), gaddr(n)) + d)
    if NEW_NAMES:
        write_names(NEW_NAMES)
    # m2c declares every unknown callee s32: give a char / short / float one its real return type (the Xbox clang reads
    # the whole return register; tools/check_kr_returns.py, which full_check runs)
    subprocess.run([sys.executable, os.path.join(ROOT, 'tools/check_kr_returns.py'), '--fix'], cwd=ROOT)
    print('%d functions, %d by hand (%s), %d new names' % (len(names), len(bad), ' '.join(bad), len(NEW_NAMES)))


if __name__ == '__main__':
    main()
