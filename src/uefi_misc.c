
#include "mruby.h"
#include "mruby/string.h"
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

