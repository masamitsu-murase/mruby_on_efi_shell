
#include <Library/UefiBootServicesTableLib.h>

#include "uefi.h"

void init_uefi_mrblib(mrb_state *mrb);

void
mrb_init_uefi(mrb_state *mrb)
{
    struct RClass *mrb_uefi;

    mrb_pack_gem_init(mrb);
    mrb_numeric_ext_init(mrb);

    mrb_uefi = mrb_define_module(mrb, "UEFI");

    mrb_define_const(mrb, mrb_uefi, "SPECIFICATION_VERSION", mrb_fixnum_value(gST->Hdr.Revision));
    mrb_define_const(mrb, mrb_uefi, "VERSION", mrb_str_new_cstr(mrb, UEFI_LIB_VERSION));

    mrb_init_uefi_boot_service(mrb, mrb_uefi);
    mrb_init_uefi_runtime_service(mrb, mrb_uefi);
    mrb_init_uefi_gop(mrb, mrb_uefi);
    mrb_init_uefi_low_level(mrb, mrb_uefi);
    mrb_init_uefi_status(mrb, mrb_uefi);
    mrb_init_uefi_handle(mrb, mrb_uefi);
    mrb_init_uefi_pointer(mrb, mrb_uefi);
    mrb_init_uefi_protocol(mrb, mrb_uefi);

    init_uefi_mrblib(mrb);
}

