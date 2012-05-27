#ifndef UEFI_LIB__
#define UEFI_LIB__

#include "mruby.h"

#define UEFI_LIB_VERSION "0.0.1"

void mrb_init_uefi(mrb_state *mrb);
void mrb_init_uefi_gop(mrb_state *mrb, struct RClass *mrb_uefi);
void mrb_init_uefi_low_level(mrb_state *mrb, struct RClass *mrb_uefi);

#endif
