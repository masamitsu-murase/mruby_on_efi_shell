#ifndef UEFI_LIB__
#define UEFI_LIB__

#include "mruby.h"
#include "uefi_misc.h"

#define UEFI_LIB_VERSION "0.0.1"

void mrb_init_uefi(mrb_state *mrb);
void mrb_init_uefi_gop(mrb_state *mrb, struct RClass *mrb_uefi);
void mrb_init_uefi_low_level(mrb_state *mrb, struct RClass *mrb_uefi);
void mrb_init_uefi_runtime_service(mrb_state *mrb, struct RClass *mrb_uefi);
void mrb_pack_gem_init(mrb_state *mrb);

void mrb_uefi_guid_get_guid(mrb_state *mrb, mrb_value guid, EFI_GUID *pguid);
mrb_value mrb_uefi_guid_set_guid(mrb_state *mrb, EFI_GUID *guid);

#endif
