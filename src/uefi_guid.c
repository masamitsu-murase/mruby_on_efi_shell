
#include "uefi.h"

void mrb_uefi_guid_get_guid(mrb_state *mrb, mrb_value guid, EFI_GUID *pguid)
{
    UINT8 *buf = (UINT8 *)pguid;
    int i;
    mrb_value data;

    data = mrb_funcall(mrb, guid, "data", 0);
    for (i=0; i<sizeof(EFI_GUID); i++){
        mrb_value num = mrb_funcall(mrb, data, "[]", 1, mrb_fixnum_value(i));
        buf[i] = (UINT8)(mrb_fixnum(num));
    }
}

