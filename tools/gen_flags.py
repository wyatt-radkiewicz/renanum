from sys import stdout

# list of sections, each section containing list of seperate
# mutually exclusive flags
flags = [
    [ ('DOT', 'r_fw__') ],
    [ ('SEP', 'rifw__') ],
    [ ('BIN', 'ri_wi_'), ('OCT', 'ri_wi_'), ('DEC', 'rifwif'),
      ('HEX', 'rifwif') ],
]

flag_comments = [
    ['// Enable a trailing/leading dot when reading a floating point number\n'],
    ['// Enable seperator \'_\' characters in the middle of the number\n'],
    ['// Read or write ONLY binary (base 2, no prefix) numbers\n',
     '// Read or write ONLY octal (base 8, no prefix) numbers\n',
     '// Read or write ONLY decimal (base 10, no prefix) numbers\n',
     '// Read or write ONLY hexadecimal (base 16, no prefix) numbers\n']
]

max_flags_section = max(map(lambda x: len(x), flags))

# type, name, integer == 0, float == 1
types = [
    ('uint64_t', 'u64', 0),
    ('int64_t', 'i64', 0),
    ('double', 'f64', 1),
]

def combine_flags(a: str, b: str) -> str:
    return ''.join(map(lambda c: c[0] if c[0] == c[1] else '_', zip(a, b)))

def get_tabs(i: int) -> str:
    return '\t' * i

#dbglist = set()

def write_type_generic_selection(prefix: str, tabs: int, allowed: str, read_mode: str):
    # Get array of allowed types [int, float]
    allowed = allowed[1:3] if read_mode else allowed[4:]
    allowed = list(map(lambda f: f[1] == 'if'[f[0]], enumerate(allowed)))
    stdout.write('_Generic(*(out) \\\n')
    for ty in types:
        stdout.write(get_tabs(tabs + 1) + f', {ty[0]} : ')
        if not allowed[ty[2]]:
            stdout.write(f'_rena_rd_err_type_flags \\\n')
            continue
        stdout.write(f'{prefix}_{ty[1]} \\\n')
        #dbglist.add((f'{prefix}_{ty[1]}', ty[0]))
    stdout.write(get_tabs(tabs + 1) + ', default : _rena_rd_err_type \\\n')
    stdout.write(get_tabs(tabs) + ')')

def sort_flags(x: tuple[int, int]) -> int:
    return x[0] * max_flags_section + x[1]

def write_flags_selection(stack: list[tuple[int, int]], max_flags: int):
    if len(stack) == max_flags:
        prefix = '_rena_rd'
        allowed = 'rifwif'

        fnty = stack.copy()
        fnty.sort(key=sort_flags)

        for i in fnty:
            flag = flags[i[0]][i[1]]
            allowed = combine_flags(allowed, flag[1])
            prefix += '_' + flag[0].lower()
        write_type_generic_selection(prefix, len(fnty), allowed, 'r')
        stdout.write(',' + ' \\\n')
        pass
    else:
        # Pick one
        stdout.write(f'_Generic((flag{len(stack)}){{0}}, \\\n')
        for i in range(0, len(flags)):
            valid_combo = not list(filter(lambda t: t[0] == i, stack))
            for j in range(0, len(flags[i])):
                stdout.write(get_tabs(len(stack) + 1)
                             + f'RENA_{flags[i][j][0]} : ')
                if valid_combo:
                    write_flags_selection(stack + [(i, j)], max_flags)
                else:
                    stdout.write('_rena_rd_err_flag_type, \\\n')
        stdout.write(get_tabs(len(stack) + 1) + 'default : _rena_rd_err_flag \\\n')
        stdout.write(get_tabs(len(stack)) + ')')
        stdout.write((',' if len(stack) > 0 else '') + ' \\\n')




stdout.write('// THIS IS AUTOGENERATED BY gen_flags.py\n')
stdout.write('// start --gen_flags.py--\n')
stdout.write('#include <stddef.h>\n\n')

stdout.write('// rena flags\n')
for (i, exclusive_flags) in enumerate(flags):
    for (j, flag) in enumerate(exclusive_flags):
        stdout.write(flag_comments[i][j])
        stdout.write(f'typedef struct _rena_{flag[0].lower()} '
            + f'{{ int _; }} RENA_{flag[0]};\n')

stdout.write('\n\n')

for i in range(0, len(flags) + 1):
    stdout.write(f'#define _RENA_RD{i}(str, out')
    for j in range(0, i):
        stdout.write(f', flag{j}')
    stdout.write(') (')
    if i == 0:
        write_type_generic_selection('_rena_rd', 0, 'rifwif', 'r')
    else:
        write_flags_selection([], i)
    stdout.write(')(str, out) \n')

stdout.write('\n\n')
stdout.write('#define _RENA_NPARAM(_IGNORE, ')
for i in range(0, len(flags)):
    stdout.write(f'_{i}, ')
stdout.write('MACRO_NAME, ...) MACRO_NAME\n')

stdout.write('#define rena_rd(str, out, ...) _RENA_NPARAM(_IGNORE, '
             + '##__VA_ARGS__ \\\n')
for i in range(len(flags), -1, -1):
    stdout.write(' ' * 44 + ', _RENA_RD' + str(i) + ' \\\n')
stdout.write(' ' * 44 + ', ...)(str, out, ##__VA_ARGS__)\n')

stdout.write('\n\n')
#dbglist = list(dbglist)
#dbglist.sort()

stdout.write(f'static __attribute__((error(\n"\\n=======================================\\n"\n"RENA_ERR - UNSUPPORTED TYPE"\n"\\n=======================================")))\nconst char *_rena_rd_err_type(const char *str, void *out) {{\n\treturn NULL;\n}}\n\n')
stdout.write(f'static __attribute__((error(\n"\\n=======================================\\n"\n"RENA_ERR - NOT A RENA FLAG"\n"\\n=======================================")))\nconst char *_rena_rd_err_flag(const char *str, void *out) {{\n\treturn NULL;\n}}\n\n')
stdout.write(f'static __attribute__((error(\n"\\n=======================================\\n"\n"RENA_ERR - INVALID TYPE OR FLAG COMBO"\n"\\n=======================================")))\nconst char *_rena_rd_err_type_flags(const char *str, void *out) {{\n\treturn NULL;\n}}\n')

stdout.write('// end --gen_flags.py--\n\n')

#for func in dbglist:
    #stdout.write(f'static __attribute__((always_inline)) const char *{func[0]}(const char *str, {func[1]} *out) {{ return "{func[0]}"; }}\n')



#stdout.write('int main(int argc, char **argv) {\n\t\n')
#stdout.write('\tuint64_t d;\n')
#stdout.write('\tprintf("%s\\n", rena_rd("4.5", &d));\n')
#stdout.write('\treturn 0;\n}\n')


