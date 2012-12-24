
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

struct EFI_STATUS_INFO
{
    const EFI_STATUS status;
    const char *name;
    const char *full_name;
};

#define INFO(a) { EFI_##a, #a, ("EFI_" #a) }
static const struct EFI_STATUS_INFO efi_status_info_list[] = {
    INFO(SUCCESS),

    INFO(LOAD_ERROR),
    INFO(INVALID_PARAMETER),
    INFO(UNSUPPORTED),
    INFO(BAD_BUFFER_SIZE),
    INFO(BUFFER_TOO_SMALL),
    INFO(NOT_READY),
    INFO(DEVICE_ERROR),
    INFO(WRITE_PROTECTED),
    INFO(OUT_OF_RESOURCES),
    INFO(VOLUME_CORRUPTED),
    INFO(VOLUME_FULL),
    INFO(NO_MEDIA),
    INFO(MEDIA_CHANGED),
    INFO(NOT_FOUND),
    INFO(ACCESS_DENIED),
    INFO(NO_RESPONSE),
    INFO(NO_MAPPING),
    INFO(TIMEOUT),
    INFO(NOT_STARTED),
    INFO(ALREADY_STARTED),
    INFO(ABORTED),
    INFO(ICMP_ERROR),
    INFO(TFTP_ERROR),
    INFO(PROTOCOL_ERROR),
    INFO(INCOMPATIBLE_VERSION),
    INFO(SECURITY_VIOLATION),
    INFO(CRC_ERROR),
    INFO(END_OF_MEDIA),
    INFO(END_OF_FILE),

    INFO(WARN_UNKNOWN_GLYPH),
    INFO(WARN_DELETE_FAILURE),
    INFO(WARN_WRITE_FAILURE)
};
#undef INFO


struct MRB_UEFI_STATUS_DATA
{
    EFI_STATUS status;
};

static struct MRB_UEFI_STATUS_DATA*
mrb_uefi_status_alloc(mrb_state *mrb, EFI_STATUS status)
{
    struct MRB_UEFI_STATUS_DATA *sd;
    sd = (struct MRB_UEFI_STATUS_DATA *)mrb_malloc(mrb, sizeof(*sd));
    sd->status = status;
    return sd;
}

static void
mrb_uefi_status_free(mrb_state *mrb, void *ptr)
{
    mrb_free(mrb, ptr);
}

static struct mrb_data_type mrb_uefi_status_type = {
    "UEFI::Status", mrb_uefi_status_free
};

static mrb_value
mrb_uefi_status_wrap(mrb_state *mrb, struct RClass *st_cls, struct MRB_UEFI_STATUS_DATA *sd)
{
    return mrb_obj_value(Data_Wrap_Struct(mrb, st_cls, &mrb_uefi_status_type, sd));
}

static mrb_value
mrb_uefi_status_make_helper(mrb_state *mrb, struct RClass *st_cls, EFI_STATUS status)
{
    struct MRB_UEFI_STATUS_DATA *sd = mrb_uefi_status_alloc(mrb, status);
    return mrb_uefi_status_wrap(mrb, st_cls, sd);
}

mrb_value
mrb_uefi_status_make(mrb_state *mrb, EFI_STATUS status)
{
    mrb_value st_cls = uefi_const_get_under_uefi(mrb, "Status");
    return mrb_uefi_status_make_helper(mrb, mrb_class_ptr(st_cls), status);
}

EFI_STATUS
mrb_uefi_status_raw_value(mrb_state *mrb, mrb_value status)
{
    struct MRB_UEFI_STATUS_DATA *sd;
    sd = (struct MRB_UEFI_STATUS_DATA *)mrb_get_datatype(mrb, status, &mrb_uefi_status_type);
    return sd->status;
}

static mrb_value
mrb_uefi_status_eq(mrb_state *mrb, mrb_value self)
{
    struct MRB_UEFI_STATUS_DATA *sd1, *sd2;
    mrb_value other;
    mrb_get_args(mrb, "o", &other);
    sd1 = (struct MRB_UEFI_STATUS_DATA *)mrb_get_datatype(mrb, self, &mrb_uefi_status_type);
    sd2 = (struct MRB_UEFI_STATUS_DATA *)mrb_get_datatype(mrb, other, &mrb_uefi_status_type);
    if (!sd1 || !sd2){
        return mrb_false_value();
    }

    if (sd1->status != sd2->status){
        return mrb_false_value();
    }

    return mrb_true_value();
}

static mrb_value
mrb_uefi_status_error_p(mrb_state *mrb, mrb_value self)
{
    struct MRB_UEFI_STATUS_DATA *sd;

    sd = (struct MRB_UEFI_STATUS_DATA *)mrb_get_datatype(mrb, self, &mrb_uefi_status_type);
    if (!sd){
        return mrb_nil_value();
    }

    if (!EFI_ERROR(sd->status)){
        return mrb_false_value();
    }

    return mrb_true_value();
}

static mrb_value
mrb_uefi_status_success_p(mrb_state *mrb, mrb_value self)
{
    struct MRB_UEFI_STATUS_DATA *sd;

    sd = (struct MRB_UEFI_STATUS_DATA *)mrb_get_datatype(mrb, self, &mrb_uefi_status_type);
    if (!sd){
        return mrb_nil_value();
    }

    if (sd->status != EFI_SUCCESS){
        return mrb_false_value();
    }

    return mrb_true_value();
}

static void
mrb_uefi_status_init_efi_status_code(mrb_state *mrb, struct RClass *st_cls)
{
    int i;
    mrb_value st_cls_value = mrb_obj_value(st_cls);
    for (i = 0; i < sizeof(efi_status_info_list)/sizeof(efi_status_info_list[0]); i++){
        const struct EFI_STATUS_INFO *info = &efi_status_info_list[i];
        mrb_const_set(mrb, st_cls_value, mrb_intern(mrb, info->name),
                      mrb_uefi_status_make_helper(mrb, st_cls, info->status));
    }
}

static const char *
mrb_uefi_status_to_s_helper(mrb_state *mrb, mrb_value self)
{
    int i;
    struct MRB_UEFI_STATUS_DATA *sd;
    sd = (struct MRB_UEFI_STATUS_DATA *)mrb_get_datatype(mrb, self, &mrb_uefi_status_type);

    for (i = 0; i < sizeof(efi_status_info_list)/sizeof(efi_status_info_list[0]); i++){
        const struct EFI_STATUS_INFO *info = &efi_status_info_list[i];
        if (info->status == sd->status){
            /* found! */
            return info->full_name;
        }
    }
    return "Unknown status";
}

static mrb_value
mrb_uefi_status_to_s(mrb_state *mrb, mrb_value self)
{
    return mrb_str_new_cstr(mrb, mrb_uefi_status_to_s_helper(mrb, self));
}

static mrb_value
mrb_uefi_status_inspect(mrb_state *mrb, mrb_value self)
{
    struct MRB_UEFI_STATUS_DATA *sd;
    const char *str;
    char buf[64];

    str = mrb_uefi_status_to_s_helper(mrb, self);
    sd = (struct MRB_UEFI_STATUS_DATA *)mrb_get_datatype(mrb, self, &mrb_uefi_status_type);
#if defined(MDE_CPU_X64)
    snprintf(buf, sizeof(buf), "<%s (0x%016llX)>", str, sd->status);
#elif defined(MDE_CPU_IA32)
    snprintf(buf, sizeof(buf), "<%s (0x%016lX)>", str, sd->status);
#else
#error Not Supported.
#endif
    return mrb_str_new_cstr(mrb, buf);
}

static mrb_value
mrb_uefi_status_value(mrb_state *mrb, mrb_value self)
{
    struct MRB_UEFI_STATUS_DATA *sd;
    mrb_value value = mrb_str_new(mrb, NULL, sizeof(EFI_STATUS));

    sd = (struct MRB_UEFI_STATUS_DATA *)mrb_get_datatype(mrb, self, &mrb_uefi_status_type);
    memcpy(RSTRING_PTR(value), &sd->status, sizeof(sd->status));
    return value;
}

void
mrb_init_uefi_status(mrb_state *mrb, struct RClass *mrb_uefi)
{
    struct RClass *st_cls;

    st_cls = mrb_define_class_under(mrb, mrb_uefi, "Status", mrb->object_class);
    MRB_SET_INSTANCE_TT(st_cls, MRB_TT_DATA);

    mrb_define_method(mrb, st_cls, "==", mrb_uefi_status_eq, ARGS_REQ(1));
    mrb_define_method(mrb, st_cls, "error?", mrb_uefi_status_error_p, ARGS_NONE());
    mrb_define_method(mrb, st_cls, "success?", mrb_uefi_status_success_p, ARGS_NONE());
    mrb_define_method(mrb, st_cls, "to_s", mrb_uefi_status_to_s, ARGS_NONE());
    mrb_define_method(mrb, st_cls, "inspect", mrb_uefi_status_inspect, ARGS_NONE());
    mrb_define_method(mrb, st_cls, "value", mrb_uefi_status_value, ARGS_NONE());

    mrb_uefi_status_init_efi_status_code(mrb, st_cls);
}

