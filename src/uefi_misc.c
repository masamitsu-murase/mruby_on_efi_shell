
#include "string.h"
#include "wchar.h"

#include "mruby.h"
#include "mruby/string.h"
#include "mruby/variable.h"

#include "uefi.h"

mrb_value
uefi_ascii_to_utf16(mrb_state *mrb, mrb_value ascii)
{
    /* Include terminator */
    /* utf16 is not a normal string. */
    mrb_value utf16 = mrb_str_new(mrb, NULL, (RSTRING_LEN(ascii) + 1) * 2);
    int i;

    for (i=0; i<RSTRING_LEN(ascii); i++){
        RSTRING_PTR(utf16)[i*2] = RSTRING_PTR(ascii)[i];
        RSTRING_PTR(utf16)[i*2 + 1] = 0;
    }
    RSTRING_PTR(utf16)[RSTRING_LEN(utf16) - 2] = 0;
    RSTRING_PTR(utf16)[RSTRING_LEN(utf16) - 1] = 0;
    return utf16;
}

mrb_value
uefi_utf16_to_ascii(mrb_state *mrb, CHAR16 *utf16)
{
    UINTN len = wcslen(utf16);
    mrb_value ascii = mrb_str_new(mrb, NULL, len);
    UINTN i;

    for (i=0; i<len; i++){
        RSTRING_PTR(ascii)[i] = (char)utf16[i];
    }
    RSTRING_PTR(ascii)[len] = '\0';

    return ascii;
}

mrb_value
uefi_const_get_under_uefi(mrb_state *mrb, const char *name)
{
    mrb_value uefi_ns;
    uefi_ns = mrb_vm_const_get(mrb, mrb_intern(mrb, "UEFI"));
    return mrb_const_get(mrb, uefi_ns, mrb_intern(mrb, name));
}

