
#include <Library/UefiRuntimeServicesTableLib.h>

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
rs_pointer(mrb_state *mrb, mrb_value obj)
{
    return mrb_uefi_pointer_make(mrb, gRT);
}

static mrb_value
rs_get_variable(mrb_state *mrb, mrb_value obj)
{
    mrb_value name, guid;
    mrb_value name_u16;
    mrb_value ret_val;
    EFI_STATUS status;
    EFI_GUID var_guid;
    UINTN size;
    UINT8 *data;

    mrb_get_args(mrb, "oo", &name, &guid);

    name_u16 = uefi_ascii_to_utf16(mrb, name);
    mrb_uefi_guid_get_guid(mrb, guid, &var_guid);
    size = 0;
    status = gRT->GetVariable((CHAR16 *)(RSTRING_PTR(name_u16)), &var_guid,
                              NULL, &size, NULL);
    if (status != EFI_BUFFER_TOO_SMALL){
        return mrb_nil_value();
    }

    data = (UINT8 *)malloc(size);
    if (!data){
        /* Should exception be raied? */
        return mrb_nil_value();
    }
    status = gRT->GetVariable((CHAR16 *)(RSTRING_PTR(name_u16)), &var_guid,
                              NULL, &size, data);
    ret_val = mrb_ary_new_capa(mrb, size);
    {
        UINTN i;
        for (i=0; i<size; i++){
            mrb_ary_push(mrb, ret_val, mrb_fixnum_value(data[i]));
        }
    }
    free(data);

    return ret_val;
}

static mrb_value
rs_set_variable(mrb_state *mrb, mrb_value obj)
{
    mrb_value name, guid, data;
    mrb_int attr;
    mrb_value name_u16;
    EFI_STATUS status;
    EFI_GUID var_guid;
    int i;
    UINT8 *buf;

    mrb_get_args(mrb, "ooiA", &name, &guid, &attr, &data);

    name_u16 = uefi_ascii_to_utf16(mrb, name);
    mrb_uefi_guid_get_guid(mrb, guid, &var_guid);
    buf = (UINT8 *)malloc(RARRAY_LEN(data) * sizeof(UINT8));
    if (!buf){
        return mrb_nil_value();
    }
    for (i=0; i<RARRAY_LEN(data); i++){
        buf[i] = (UINT8)mrb_fixnum(RARRAY_PTR(data)[i]);
    }

    status = gRT->SetVariable((CHAR16 *)(RSTRING_PTR(name_u16)), &var_guid,
                              (UINT32)attr, (UINTN)RARRAY_LEN(data), buf);
    free(buf);
    if (EFI_ERROR(status)){
        return mrb_nil_value();
    }

    return mrb_true_value();
}

static mrb_value
rs_get_all_variable_names(mrb_state *mrb, mrb_value obj)
{
    mrb_value names;
    UINTN size, buf_size;
    CHAR16 *buf;
    EFI_GUID guid;
    EFI_STATUS status;

    names = mrb_ary_new(mrb);

    size = buf_size = 64; /* initial size */
    buf = (CHAR16 *)malloc(buf_size);
    if (!buf){
        return mrb_nil_value();
    }

    buf[0] = 0;
    status = gRT->GetNextVariableName(&size, buf, &guid);
    while(status == EFI_SUCCESS || status == EFI_BUFFER_TOO_SMALL){
        if (status == EFI_BUFFER_TOO_SMALL){
            CHAR16 *new_buf = (CHAR16 *)realloc(buf, size);
            if (!new_buf){
                free(buf);
                return mrb_nil_value();
            }

            buf = new_buf;
            buf_size = size;
        }else if (status == EFI_SUCCESS){
            mrb_value hash;

            int arena = mrb_gc_arena_save(mrb);
            hash = mrb_hash_new_capa(mrb, 2);
            mrb_hash_set(mrb, hash, mrb_symbol_value(mrb_intern(mrb, "guid")),
                         mrb_uefi_guid_set_guid(mrb, &guid));
            mrb_hash_set(mrb, hash, mrb_symbol_value(mrb_intern(mrb, "name")),
                         uefi_utf16_to_ascii(mrb, buf));
            mrb_ary_push(mrb, names, hash);
            mrb_gc_arena_restore(mrb, arena);
        }

        size = buf_size;
        status = gRT->GetNextVariableName(&size, buf, &guid);
    }
    free(buf);
    if (status != EFI_NOT_FOUND){
        return mrb_nil_value();
    }

    return names;
}

static mrb_value
rs_reset_system(mrb_state *mrb, mrb_value self)
{
    mrb_int type;
    mrb_value status, data;
    mrb_value st_cls;

    data = mrb_nil_value();
    mrb_get_args(mrb, "io|o", &type, &status, &data);
    st_cls = uefi_const_get_under_uefi(mrb, "Status");

    switch(type){
      case EfiResetCold:
      case EfiResetWarm:
      case EfiResetShutdown:
        break;
      default:
        mrb_raise(mrb, E_TYPE_ERROR, "wrong argument: type");
        break;
    }
    if (mrb_obj_class(mrb, status) != mrb_class_ptr(st_cls)){
        mrb_raise(mrb, E_TYPE_ERROR, "wrong argument: status");
    }
    if (!mrb_nil_p(data) && mrb_obj_class(mrb, data) != mrb->string_class){
        mrb_raise(mrb, E_TYPE_ERROR, "wrong argument: data");
    }

    if (mrb_nil_p(data)){
        gRT->ResetSystem((EFI_RESET_TYPE)type, mrb_uefi_status_raw_value(mrb, status),
                         0, NULL);
    }else{
        gRT->ResetSystem((EFI_RESET_TYPE)type, mrb_uefi_status_raw_value(mrb, status),
                         (UINTN)RSTRING_LEN(data),
                         RSTRING_PTR(data));
    }

    return mrb_nil_value();
}

static void
mrb_init_reset_type(mrb_state *mrb, struct RClass *mrb_ns)
{
    struct RESET_TYPE_INFO
    {
        EFI_RESET_TYPE type;
        const char *name;
    };

#define INFO(a) { Efi##a, #a }
    const struct RESET_TYPE_INFO info_list[] = {
        INFO(ResetCold),
        INFO(ResetWarm),
        INFO(ResetShutdown)
    };
#undef INFO

    int i;

    for (i = 0; i < sizeof(info_list)/sizeof(info_list[0]); i++){
        const struct RESET_TYPE_INFO *info = &info_list[i];
        mrb_const_set(mrb, mrb_obj_value(mrb_ns), mrb_intern(mrb, info->name),
                      mrb_fixnum_value(info->type));
    }
}

void
mrb_init_uefi_runtime_service(mrb_state *mrb, struct RClass *mrb_uefi)
{
    struct RClass *mrb_ns;

    mrb_ns = mrb_define_module_under(mrb, mrb_uefi, "RuntimeService");

    mrb_define_module_function(mrb, mrb_ns, "pointer", rs_pointer, ARGS_NONE());
    mrb_define_module_function(mrb, mrb_ns, "get_variable", rs_get_variable, ARGS_REQ(4));
    mrb_define_module_function(mrb, mrb_ns, "set_variable", rs_set_variable, ARGS_REQ(2));
    mrb_define_module_function(mrb, mrb_ns, "get_all_variable_names", rs_get_all_variable_names, ARGS_NONE());
    mrb_define_module_function(mrb, mrb_ns, "reset_system", rs_reset_system, ARGS_ANY());

    mrb_init_reset_type(mrb, mrb_ns);
}

