
#include <string.h>

#include "mruby.h"
#include "mruby/array.h"
#include "mruby/string.h"
#include "mruby/variable.h"

#include "uefi.h"

void mrb_uefi_guid_get_guid(mrb_state *mrb, mrb_value guid, EFI_GUID *pguid)
{
    mrb_value data;

    data = mrb_funcall(mrb, guid, "data", 0);
    memcpy(pguid, RSTRING_PTR(data), sizeof(*pguid));
}

mrb_value mrb_uefi_guid_set_guid(mrb_state *mrb, EFI_GUID *guid)
{
    mrb_value data = mrb_str_buf_new(mrb, sizeof(EFI_GUID));
    mrb_value uefi_ns, guid_class;

    mrb_str_resize(mrb, data, sizeof(EFI_GUID));
    memcpy(RSTRING_PTR(data), guid, sizeof(*guid));

    uefi_ns = mrb_vm_const_get(mrb, mrb_intern(mrb, "UEFI"));
    guid_class = mrb_const_get(mrb, uefi_ns, mrb_intern(mrb, "Guid"));

    return mrb_funcall(mrb, guid_class, "new", 1, data);
}

