
#include <Library/UefiRuntimeServicesTableLib.h>

#include "mruby.h"
#include "mruby/string.h"

#include "uefi.h"

static mrb_value
rs_get_variable(mrb_state *mrb, mrb_value obj)
{
    mrb_value name, guid;
    mrb_value name_u16;
    EFI_STATUS status;
    EFI_GUID var_guid;
    UINTN size;

    mrb_get_args(mrb, "oo", &name, &guid);

    name_u16 = uefi_ascii_to_utf16(mrb, name);
    mrb_uefi_guid_get_guid(mrb, guid, &var_guid);
    status = gRT->GetVariable((CHAR16 *)(RSTRING_PTR(name_u16)), &var_guid,
                              NULL, &size, NULL);
    if (status != EFI_BUFFER_TOO_SMALL){
        return mrb_nil_value();
    }

    return mrb_nil_value();
}

void
mrb_init_uefi_runtime_service(mrb_state *mrb, struct RClass *mrb_uefi)
{
    struct RClass *mrb_ns;

    mrb_ns = mrb_define_module_under(mrb, mrb_uefi, "RuntimeService");
    mrb_define_module_function(mrb, mrb_ns, "get_variable", rs_get_variable, 2);
}

