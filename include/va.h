/* Variable-argument macros as the original PS2 MWCC (3.0b52) expands them: the argument pointer is the end of the
   register save area minus the unused register slots. Recovered from font_print_sp / str_gattai (n >= 8 ? 0 : (8 - n) * 8,
   n = __builtin_args_info(2) = number of named argument registers). Use together with <typedef char *va_list>. */
#ifndef MH_VA_H
#define MH_VA_H
typedef char *va_list;
#define va_start(ap, parm) ((ap) = (va_list)((char *)__builtin_next_arg(parm) - (__builtin_args_info(2) >= 8 ? 0 : (8 - __builtin_args_info(2)) * 8)))
#endif
