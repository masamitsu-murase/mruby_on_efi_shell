
#include <Library/UefiBootServicesTableLib.h>

#include "mruby.h"
#include "mruby/string.h"
#include "mruby/class.h"
#include "mruby/array.h"
#include "mruby/hash.h"
#include "mruby/variable.h"
#include "mruby/data.h"
#include "mruby/value.h"

#include "uefi.h"

#include <stdio.h>


static mrb_value
bs_locate_protocol(mrb_state *mrb, mrb_value self)
{
    mrb_value guid;
    EFI_STATUS status;
    EFI_GUID raw_guid;
    VOID *protocol;

    mrb_get_args(mrb, "o", &guid);
    mrb_uefi_guid_get_guid(mrb, guid, &raw_guid);
    status = gBS->LocateProtocol(&raw_guid, NULL, &protocol);
    if (EFI_ERROR(status)){
        return mrb_nil_value();
    }

    return mrb_uefi_pointer_make(mrb, protocol);
}

void
mrb_init_uefi_boot_service(mrb_state *mrb, struct RClass *mrb_uefi)
{
    struct RClass *bs;

    bs = mrb_define_module_under(mrb, mrb_uefi, "BootService");

    mrb_define_module_function(mrb, bs, "locate_protocol", bs_locate_protocol, ARGS_REQ(1));
}

