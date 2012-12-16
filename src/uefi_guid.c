
#include "mruby.h"
#include "mruby/array.h"
#include "mruby/variable.h"

#include "uefi.h"

void mrb_uefi_guid_get_guid(mrb_state *mrb, mrb_value guid, EFI_GUID *pguid)
{
    UINT8 *buf = (UINT8 *)pguid;
    int i;
    mrb_value data;

    data = mrb_funcall(mrb, guid, "data", 0);
    for (i=0; i<sizeof(EFI_GUID); i++){
        mrb_value num = RARRAY_PTR(data)[i];
        buf[i] = (UINT8)(mrb_fixnum(num));
    }
}

mrb_value mrb_uefi_guid_set_guid(mrb_state *mrb, EFI_GUID *guid)
{
    mrb_value array = mrb_ary_new_capa(mrb, sizeof(EFI_GUID));
    mrb_value uefi_ns, guid_class;
    int i;
    UINT8 *buf = (UINT8 *)guid;

    for (i=0; i<sizeof(EFI_GUID); i++){
        mrb_ary_push(mrb, array, mrb_fixnum_value(buf[i]));
    }

    uefi_ns = mrb_vm_const_get(mrb, mrb_intern(mrb, "UEFI"));
    guid_class = mrb_const_get(mrb, uefi_ns, mrb_intern(mrb, "Guid"));

    return mrb_funcall(mrb, guid_class, "new", 1, array);
}

