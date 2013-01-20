#ifndef UEFI_LIB__
#define UEFI_LIB__

#include "mruby.h"
#include "uefi_misc.h"

#define UEFI_LIB_VERSION "1.0.0"

void mrb_init_uefi(mrb_state *mrb);
void mrb_init_uefi_gop(mrb_state *mrb, struct RClass *mrb_uefi);
void mrb_init_uefi_low_level(mrb_state *mrb, struct RClass *mrb_uefi);
void mrb_init_uefi_runtime_service(mrb_state *mrb, struct RClass *mrb_uefi);
void mrb_init_uefi_boot_service(mrb_state *mrb, struct RClass *mrb_uefi);

void mrb_uefi_guid_get_guid(mrb_state *mrb, mrb_value guid, EFI_GUID *pguid);
mrb_value mrb_uefi_guid_set_guid(mrb_state *mrb, EFI_GUID *guid);

void mrb_init_uefi_status(mrb_state *mrb, struct RClass *mrb_uefi);
mrb_value mrb_uefi_status_make(mrb_state *mrb, EFI_STATUS status);
EFI_STATUS mrb_uefi_status_raw_value(mrb_state *mrb, mrb_value status);
void mrb_init_uefi_handle(mrb_state *mrb, struct RClass *mrb_uefi);
mrb_value mrb_uefi_handle_make(mrb_state *mrb, EFI_HANDLE handle);
EFI_HANDLE mrb_uefi_handle_raw_value(mrb_state *mrb, mrb_value handle);
void mrb_init_uefi_pointer(mrb_state *mrb, struct RClass *mrb_uefi);
mrb_value mrb_uefi_pointer_make(mrb_state *mrb, VOID *pointer);
VOID *mrb_uefi_pointer_raw_value(mrb_state *mrb, mrb_value pointer);
void mrb_init_uefi_protocol(mrb_state *mrb, struct RClass *mrb_uefi);


/* under ext */
void mrb_pack_gem_init(mrb_state *mrb);
void mrb_numeric_ext_init(mrb_state *mrb);

#endif
