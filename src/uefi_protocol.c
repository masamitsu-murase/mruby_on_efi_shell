
#include <Uefi/UefiBaseType.h>
#include <stdio.h>
#include <string.h>

#include "mruby.h"
#include "mruby/array.h"
#include "mruby/class.h"
#include "mruby/string.h"
#include "mruby/variable.h"
#include "mruby/data.h"
#include "mruby/value.h"

#include "uefi.h"

/* Currently only 64bit-based (X64) platform is supported. */
typedef UINT64 base_type;
typedef base_type EFIAPI func_type(base_type arg0, base_type arg1, base_type arg2,
                                   base_type arg3, base_type arg4, base_type arg5,
                                   base_type arg6, base_type arg7, base_type arg8,
                                   base_type arg9, base_type arg10, base_type arg11,
                                   base_type arg12, base_type arg13, base_type arg14,
                                   base_type arg15);

union supported_arg_type
{
    base_type value;
    EFI_STATUS status;
    VOID *ptr;
    EFI_HANDLE handle;
    INT8   i8;
    UINT8  u8;
    INT16  i16;
    UINT16 u16;
    INT32  i32;
    UINT32 u32;
    INT64  i64;
    UINT64 u64;
};

#define ASSERT_EQ(a, b)        typedef int assert_base_type[((a)==(b))?1:-1]
#define ASSERT_BASE_TYPE(type) ASSERT_EQ(sizeof(base_type), sizeof(type))
ASSERT_BASE_TYPE(union supported_arg_type);


#define INTERN(mrb, str) mrb_intern2(mrb, str, sizeof(str) - 1)

static base_type
cast_to_base_type(mrb_state *mrb, mrb_value value, mrb_value type)
{
    union supported_arg_type ret = { 0 };

    if (mrb_symbol(type) == INTERN(mrb, "e")){
        ret.status = mrb_uefi_status_raw_value(mrb, value);
    }else if (mrb_symbol(type) == INTERN(mrb, "p")){
        if (mrb_string_p(value)){
            ret.ptr = (VOID *)RSTRING_PTR(value);
        }else{
            /* Pointer class instance */
            ret.ptr = mrb_uefi_pointer_raw_value(mrb, value);
        }
    }else if (mrb_symbol(type) == INTERN(mrb, "h")){
        ret.handle = mrb_uefi_handle_raw_value(mrb, value);
    }else if (mrb_symbol(type) == INTERN(mrb, "u64")){
        ret.u64 = (UINT64)mrb_fixnum(value);
    }else if (mrb_symbol(type) == INTERN(mrb, "i64")){
        ret.i64 = (INT64)mrb_fixnum(value);
    }else if (mrb_symbol(type) == INTERN(mrb, "u32")){
        ret.u32 = (UINT32)mrb_fixnum(value);
    }else if (mrb_symbol(type) == INTERN(mrb, "i32")){
        ret.i32 = (INT32)mrb_fixnum(value);
    }else if (mrb_symbol(type) == INTERN(mrb, "u16")){
        ret.u16 = (UINT16)mrb_fixnum(value);
    }else if (mrb_symbol(type) == INTERN(mrb, "i16")){
        ret.i16 = (INT16)mrb_fixnum(value);
    }else if (mrb_symbol(type) == INTERN(mrb, "u8")){
        ret.u8 = (UINT8)mrb_fixnum(value);
    }else if (mrb_symbol(type) == INTERN(mrb, "i8")){
        ret.i8 = (INT8)mrb_fixnum(value);
    }else{
        mrb_raise(mrb, E_ARGUMENT_ERROR, "Bug Unknown type");
    }

    return ret.value;
}

static mrb_value
cast_from_base_type(mrb_state *mrb, base_type value, mrb_sym type)
{
    union supported_arg_type val;
    val.value = value;

    if (type == INTERN(mrb, "e")){
        return mrb_uefi_status_make(mrb, val.status);
    }else if (type == INTERN(mrb, "p")){
        /* Pointer class instance */
        return mrb_uefi_pointer_make(mrb, val.ptr);
    }else if (type == INTERN(mrb, "h")){
        return mrb_uefi_handle_make(mrb, val.handle);
    }else if (type == INTERN(mrb, "u64")){
        return mrb_fixnum_value((mrb_int)val.u64);
    }else if (type == INTERN(mrb, "i64")){
        return mrb_fixnum_value((mrb_int)val.i64);
    }else if (type == INTERN(mrb, "u32")){
        return mrb_fixnum_value(val.u32);
    }else if (type == INTERN(mrb, "i32")){
        return mrb_fixnum_value(val.i32);
    }else if (type == INTERN(mrb, "u16")){
        return mrb_fixnum_value(val.u16);
    }else if (type == INTERN(mrb, "i16")){
        return mrb_fixnum_value(val.i16);
    }else if (type == INTERN(mrb, "u8")){
        return mrb_fixnum_value(val.u8);
    }else if (type == INTERN(mrb, "i8")){
        return mrb_fixnum_value(val.i8);
    }else{
        mrb_raise(mrb, E_ARGUMENT_ERROR, "Bug Unknown type");
    }

    return mrb_nil_value();
}

static mrb_value
mrb_uefi_protocol_raw_function(mrb_state *mrb, mrb_value self)
{
    mrb_value args, arg_type;
    mrb_value ret;
    mrb_sym ret_type;
    mrb_int offset;
    func_type *func_ptr;

    mrb_get_args(mrb, "AAni", &args, &arg_type, &ret_type, &offset);

    {
        mrb_value pointer;
        VOID *raw_ptr;

        pointer = mrb_iv_get(mrb, self, INTERN(mrb, "@pointer"));
        raw_ptr = mrb_uefi_pointer_raw_value(mrb, pointer);
        func_ptr = *(func_type **)((UINTN)raw_ptr + offset);
    }

    {
        base_type val[16] = { 0 };
        base_type ret_val;
        int i, len;
        for (i = 0, len = RARRAY_LEN(args); i < len; i++){
            val[i] = cast_to_base_type(mrb, RARRAY_PTR(args)[i], RARRAY_PTR(arg_type)[i]);
        }

        ret_val = (*func_ptr)(val[0], val[1], val[2], val[3], val[4], val[5],
                              val[6], val[7], val[8], val[9], val[10], val[11],
                              val[12], val[13], val[14], val[15]);
        ret = cast_from_base_type(mrb, ret_val, ret_type);
    }

    return ret;
}

void
mrb_init_uefi_protocol(mrb_state *mrb, struct RClass *mrb_uefi)
{
    struct RClass *cls = mrb_define_class_under(mrb, mrb_uefi, "Protocol", mrb->object_class);

    mrb_const_set(mrb, mrb_obj_value(cls),
                  mrb_intern(mrb, "FUNCTION_POINTER_SIZE"),
                  mrb_fixnum_value(sizeof(VOID *)));
    mrb_define_method(mrb, cls, "call_raw_function", mrb_uefi_protocol_raw_function, ARGS_REQ(4));
}

