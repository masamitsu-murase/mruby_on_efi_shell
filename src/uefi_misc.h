#ifndef UEFI_MISC_H__
#define UEFI_MISC_H__

#include "uefi.h"

mrb_value uefi_ascii_to_utf16(mrb_state *mrb, mrb_value ascii);
mrb_value uefi_utf16_to_ascii(mrb_state *mrb, CHAR16 *utf16);
mrb_value uefi_const_get_under_uefi(mrb_state *mrb, const char *name);

#endif

