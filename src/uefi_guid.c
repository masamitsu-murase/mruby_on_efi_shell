
#include "mruby.h"
#include "mruby/array.h"

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

