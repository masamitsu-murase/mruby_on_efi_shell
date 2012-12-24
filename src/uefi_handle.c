
#include <Uefi/UefiBaseType.h>
#include <stdio.h>
#include <string.h>

#include "mruby.h"
#include "mruby/class.h"
#include "mruby/string.h"
#include "mruby/variable.h"
#include "mruby/data.h"
#include "mruby/value.h"

#include "uefi.h"


struct MRB_UEFI_HANDLE_DATA
{
    EFI_HANDLE handle;
};

static struct MRB_UEFI_HANDLE_DATA*
mrb_uefi_handle_alloc(mrb_state *mrb, EFI_HANDLE handle)
{
    struct MRB_UEFI_HANDLE_DATA *hd;
    hd = (struct MRB_UEFI_HANDLE_DATA *)mrb_malloc(mrb, sizeof(*hd));
    hd->handle = handle;
    return hd;
}

static void
mrb_uefi_handle_free(mrb_state *mrb, void *ptr)
{
    mrb_free(mrb, ptr);
}

static struct mrb_data_type mrb_uefi_handle_type = {
    "UEFI::Handle", mrb_uefi_handle_free
};

static mrb_value
mrb_uefi_handle_wrap(mrb_state *mrb, struct RClass *h_cls, struct MRB_UEFI_HANDLE_DATA *hd)
{
    return mrb_obj_value(Data_Wrap_Struct(mrb, h_cls, &mrb_uefi_handle_type, hd));
}

static mrb_value
mrb_uefi_handle_make_helper(mrb_state *mrb, struct RClass *h_cls, EFI_HANDLE handle)
{
    struct MRB_UEFI_HANDLE_DATA *hd = mrb_uefi_handle_alloc(mrb, handle);
    return mrb_uefi_handle_wrap(mrb, h_cls, hd);
}

mrb_value
mrb_uefi_handle_make(mrb_state *mrb, EFI_HANDLE handle)
{
    mrb_value h_cls = uefi_const_get_under_uefi(mrb, "Handle");
    return mrb_uefi_handle_make_helper(mrb, mrb_class_ptr(h_cls), handle);
}

EFI_HANDLE
mrb_uefi_handle_raw_value(mrb_state *mrb, mrb_value handle)
{
    struct MRB_UEFI_HANDLE_DATA *hd;
    hd = (struct MRB_UEFI_HANDLE_DATA *)mrb_get_datatype(mrb, handle, &mrb_uefi_handle_type);
    return hd->handle;
}

static mrb_value
mrb_uefi_handle_eq(mrb_state *mrb, mrb_value self)
{
    struct MRB_UEFI_HANDLE_DATA *hd1, *hd2;
    mrb_value other;
    mrb_get_args(mrb, "o", &other);
    hd1 = (struct MRB_UEFI_HANDLE_DATA *)mrb_get_datatype(mrb, self, &mrb_uefi_handle_type);
    hd2 = (struct MRB_UEFI_HANDLE_DATA *)mrb_get_datatype(mrb, other, &mrb_uefi_handle_type);
    if (!hd1 || !hd2){
        return mrb_false_value();
    }

    if (hd1->handle != hd2->handle){
        return mrb_false_value();
    }

    return mrb_true_value();
}

static mrb_value
mrb_uefi_handle_to_s(mrb_state *mrb, mrb_value self)
{
    struct MRB_UEFI_HANDLE_DATA *hd;
    char buf[32];

    hd = (struct MRB_UEFI_HANDLE_DATA *)mrb_get_datatype(mrb, self, &mrb_uefi_handle_type);
#if defined(MDE_CPU_X64)
    snprintf(buf, sizeof(buf), "0x%016llX", (UINTN)(hd->handle));
#elif defined(MDE_CPU_IA32)
    snprintf(buf, sizeof(buf), "0x%08lX", (UINTN)(hd->handle));
#else
#error Not Supported
#endif
    return mrb_str_new_cstr(mrb, buf);
}

static mrb_value
mrb_uefi_handle_inspect(mrb_state *mrb, mrb_value self)
{
    struct MRB_UEFI_HANDLE_DATA *hd;
    char buf[32];

    hd = (struct MRB_UEFI_HANDLE_DATA *)mrb_get_datatype(mrb, self, &mrb_uefi_handle_type);
#if defined(MDE_CPU_X64)
    snprintf(buf, sizeof(buf), "<Handle 0x%016llX>", (UINTN)(hd->handle));
#elif defined(MDE_CPU_IA32)
    snprintf(buf, sizeof(buf), "<Handle 0x%08lX>", (UINTN)(hd->handle));
#else
#error Not Supported
#endif
    return mrb_str_new_cstr(mrb, buf);
}

static mrb_value
mrb_uefi_handle_value(mrb_state *mrb, mrb_value self)
{
    struct MRB_UEFI_HANDLE_DATA *hd;
    mrb_value value = mrb_str_new(mrb, NULL, sizeof(EFI_HANDLE));

    hd = (struct MRB_UEFI_HANDLE_DATA *)mrb_get_datatype(mrb, self, &mrb_uefi_handle_type);
    memcpy(RSTRING_PTR(value), &hd->handle, sizeof(hd->handle));
    return value;
}

void
mrb_init_uefi_handle(mrb_state *mrb, struct RClass *mrb_uefi)
{
    struct RClass *h_cls;

    h_cls = mrb_define_class_under(mrb, mrb_uefi, "Handle", mrb->object_class);
    MRB_SET_INSTANCE_TT(h_cls, MRB_TT_DATA);

    mrb_define_method(mrb, h_cls, "==", mrb_uefi_handle_eq, ARGS_REQ(1));
    mrb_define_method(mrb, h_cls, "to_s", mrb_uefi_handle_to_s, ARGS_NONE());
    mrb_define_method(mrb, h_cls, "inspect", mrb_uefi_handle_inspect, ARGS_NONE());
    mrb_define_method(mrb, h_cls, "value", mrb_uefi_handle_value, ARGS_NONE());

    mrb_const_set(mrb, mrb_obj_value(h_cls), mrb_intern(mrb, "NULL"),
                  mrb_uefi_handle_make_helper(mrb, h_cls, NULL));
}

