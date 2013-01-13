
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
bs_pointer(mrb_state *mrb, mrb_value self)
{
    return mrb_uefi_pointer_make(mrb, gBS);
}

static mrb_value
bs_protocols_per_handle(mrb_state *mrb, mrb_value self)
{
    mrb_value handle;
    mrb_value ret;
    EFI_HANDLE raw_handle;
    EFI_GUID **ppguids;
    UINTN size;
    EFI_STATUS status;

    mrb_get_args(mrb, "o", &handle);
    raw_handle = mrb_uefi_handle_raw_value(mrb, handle);

    status = gBS->ProtocolsPerHandle(raw_handle, &ppguids, &size);
    if (EFI_ERROR(status)){
        return mrb_nil_value();
    }

    ret = mrb_ary_new_capa(mrb, (int)size);
    {
        UINTN i;
        for (i=0; i<size; i++){
            int arena = mrb_gc_arena_save(mrb);
            mrb_ary_push(mrb, ret, mrb_uefi_guid_set_guid(mrb, ppguids[i]));
            mrb_gc_arena_restore(mrb, arena);
        }
    }
    gBS->FreePool(ppguids);

    return ret;
}

static mrb_value
bs_locate_handle_buffer(mrb_state *mrb, mrb_value self)
{
    mrb_value guid;
    mrb_value ret;
    UINTN size;
    EFI_HANDLE *phandles;

    mrb_get_args(mrb, "o", &guid);
    if (mrb_nil_p(guid)){
        /* AllHandles */
        EFI_STATUS status;
        status = gBS->LocateHandleBuffer(AllHandles, NULL, NULL, &size, &phandles);
        if (EFI_ERROR(status)){
            return mrb_nil_value();
        }
    }else{
        /* ByProtocol */
        EFI_STATUS status;
        EFI_GUID efi_guid;

        mrb_uefi_guid_get_guid(mrb, guid, &efi_guid);
        status = gBS->LocateHandleBuffer(ByProtocol, &efi_guid, NULL, &size, &phandles);
        if (EFI_ERROR(status)){
            return mrb_nil_value();
        }
    }

    ret = mrb_ary_new_capa(mrb, (int)size);
    {
        UINTN i;
        for (i=0; i<size; i++){
            int arena = mrb_gc_arena_save(mrb);
            mrb_ary_push(mrb, ret, mrb_uefi_handle_make(mrb, phandles[i]));
            mrb_gc_arena_restore(mrb, arena);
        }
    }
    gBS->FreePool(phandles);

    return ret;
}

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

    mrb_define_module_function(mrb, bs, "pointer", bs_pointer, ARGS_NONE());

    mrb_define_module_function(mrb, bs, "protocols_per_handle", bs_protocols_per_handle, ARGS_REQ(1));
    mrb_define_module_function(mrb, bs, "locate_handle_buffer", bs_locate_handle_buffer, ARGS_REQ(1));
    mrb_define_module_function(mrb, bs, "locate_protocol", bs_locate_protocol, ARGS_REQ(1));
}

