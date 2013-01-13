
#include <Uefi/UefiBaseType.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>

#include "mruby.h"
#include "mruby/array.h"
#include "mruby/class.h"
#include "mruby/hash.h"
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
get_raw_value(mrb_state *mrb, mrb_sym type, VOID *ptr)
{
    if (type == INTERN(mrb, "e")){
        return mrb_uefi_status_make(mrb, *(EFI_STATUS *)ptr);
    }else if (type == INTERN(mrb, "p")){
        /* Pointer class instance */
        return mrb_uefi_pointer_make(mrb, *(VOID **)ptr);
    }else if (type == INTERN(mrb, "h")){
        return mrb_uefi_handle_make(mrb, *(EFI_HANDLE *)ptr);
    }else if (type == INTERN(mrb, "u64")){
        return mrb_fixnum_value((mrb_int)(*(UINT64 *)ptr));
    }else if (type == INTERN(mrb, "i64")){
        return mrb_fixnum_value((mrb_int)(*(INT64 *)ptr));
    }else if (type == INTERN(mrb, "u32")){
        return mrb_fixnum_value(*(UINT32 *)ptr);
    }else if (type == INTERN(mrb, "i32")){
        return mrb_fixnum_value(*(INT32 *)ptr);
    }else if (type == INTERN(mrb, "u16")){
        return mrb_fixnum_value(*(UINT16 *)ptr);
    }else if (type == INTERN(mrb, "i16")){
        return mrb_fixnum_value(*(INT16 *)ptr);
    }else if (type == INTERN(mrb, "u8")){
        return mrb_fixnum_value(*(UINT8 *)ptr);
    }else if (type == INTERN(mrb, "i8")){
        return mrb_fixnum_value(*(INT8 *)ptr);
    }else{
        mrb_raise(mrb, E_ARGUMENT_ERROR, "Bug Unknown type");
    }

    return mrb_nil_value();
}

static void
set_raw_value(mrb_state *mrb, mrb_sym type, VOID *ptr, mrb_value value)
{
    if (type == INTERN(mrb, "e")){
        *(EFI_STATUS *)ptr = mrb_uefi_status_raw_value(mrb, value);
    }else if (type == INTERN(mrb, "p")){
        *(VOID **)ptr = mrb_uefi_pointer_raw_value(mrb, value);
    }else if (type == INTERN(mrb, "h")){
        *(EFI_HANDLE *)ptr = mrb_uefi_handle_raw_value(mrb, value);
    }else if (type == INTERN(mrb, "u64")){
        *(UINT64 *)ptr = (UINT64)mrb_fixnum(value);
    }else if (type == INTERN(mrb, "i64")){
        *(INT64 *)ptr = (INT64)mrb_fixnum(value);
    }else if (type == INTERN(mrb, "u32")){
        *(UINT32 *)ptr = (UINT32)mrb_fixnum(value);
    }else if (type == INTERN(mrb, "i32")){
        *(INT32 *)ptr = (INT32)mrb_fixnum(value);
    }else if (type == INTERN(mrb, "u16")){
        *(UINT16 *)ptr = (UINT16)mrb_fixnum(value);
    }else if (type == INTERN(mrb, "i16")){
        *(INT16 *)ptr = (INT16)mrb_fixnum(value);
    }else if (type == INTERN(mrb, "u8")){
        *(UINT8 *)ptr = (UINT8)mrb_fixnum(value);
    }else if (type == INTERN(mrb, "i8")){
        *(INT8 *)ptr = (INT8)mrb_fixnum(value);
    }else{
        mrb_raise(mrb, E_ARGUMENT_ERROR, "Bug Unknown type");
    }
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

static VOID *
mrb_uefi_protocol_pointer(mrb_state *mrb, mrb_value self)
{
    mrb_value pointer = mrb_iv_get(mrb, self, INTERN(mrb, "@pointer"));
    return mrb_uefi_pointer_raw_value(mrb, pointer);
}

static mrb_value
mrb_uefi_protocol_get_raw_value(mrb_state *mrb, mrb_value self)
{
    mrb_sym type;
    mrb_int offset;
    UINTN addr;

    mrb_get_args(mrb, "ni", &type, &offset);
    addr = (UINTN)mrb_uefi_protocol_pointer(mrb, self);
    return get_raw_value(mrb, type, (VOID *)(addr + offset));
}

static mrb_value
mrb_uefi_protocol_set_raw_value(mrb_state *mrb, mrb_value self)
{
    mrb_sym type;
    mrb_int offset;
    mrb_value value;
    UINTN addr;

    mrb_get_args(mrb, "nio", &type, &offset, &value);
    addr = (UINTN)mrb_uefi_protocol_pointer(mrb, self);
    set_raw_value(mrb, type, (VOID *)(addr + offset), value);

    return value;
}

static void
mrb_uefi_protocol_init_type_info(mrb_state *mrb, struct RClass *cls)
{
#define SET_TYPE_INFO(name, type)                        \
    do{                                                  \
        struct struct_##name                             \
        {                                                \
            CHAR8 dummy_buf;                             \
            type value;                                  \
        };                                               \
        int size = sizeof(type);                                        \
        int alignment = offsetof(struct struct_##name, value);          \
        int arena = mrb_gc_arena_save(mrb);                             \
        mrb_value h = mrb_hash_new_capa(mrb, 2);                        \
        mrb_hash_set(mrb, h, sym_size, mrb_fixnum_value(size));         \
        mrb_hash_set(mrb, h, sym_alignment, mrb_fixnum_value(alignment)); \
        mrb_hash_set(mrb, info, mrb_symbol_value(INTERN(mrb, #name)), h); \
        mrb_gc_arena_restore(mrb, arena);                               \
    }while(0)

    mrb_value info = mrb_hash_new_capa(mrb, 12);
    mrb_value sym_size = mrb_symbol_value(INTERN(mrb, "size"));
    mrb_value sym_alignment = mrb_symbol_value(INTERN(mrb, "alignment"));

    /* :p, :u8, :u16, :u32, :u64, :i8, :i16, :i32, :i64, :h, :e, :b */
    SET_TYPE_INFO(p, VOID *);
    SET_TYPE_INFO(u8, UINT8);
    SET_TYPE_INFO(u16, UINT16);
    SET_TYPE_INFO(u32, UINT32);
    SET_TYPE_INFO(u64, UINT64);
    SET_TYPE_INFO(i8, INT8);
    SET_TYPE_INFO(i16, INT16);
    SET_TYPE_INFO(i32, INT32);
    SET_TYPE_INFO(i64, INT64);
    SET_TYPE_INFO(h, EFI_HANDLE);
    SET_TYPE_INFO(e, EFI_STATUS);
    SET_TYPE_INFO(b, BOOLEAN);

    mrb_const_set(mrb, mrb_obj_value(cls), INTERN(mrb, "TYPE_INFO"), info);
#undef SET_TYPE_INFO
}

static mrb_value
mrb_str_get_pointer(mrb_state *mrb, mrb_value self)
{
    mrb_value pointer;

    /* Use mrb_str_resize instead of static str_modify */
    mrb_str_resize(mrb, self, RSTRING_LEN(self));
    pointer = mrb_uefi_pointer_make(mrb, RSTRING_PTR(self));
    mrb_iv_set(mrb, pointer, INTERN(mrb, "@origin"), self);

    return pointer;
}

static void
mrb_extend_string(mrb_state *mrb)
{
    mrb_define_method(mrb, mrb->string_class, "pointer", mrb_str_get_pointer, ARGS_NONE());
}

void
mrb_init_uefi_protocol(mrb_state *mrb, struct RClass *mrb_uefi)
{
    struct RClass *cls = mrb_define_class_under(mrb, mrb_uefi, "Protocol", mrb->object_class);

    mrb_uefi_protocol_init_type_info(mrb, cls);
    mrb_define_method(mrb, cls, "call_raw_function", mrb_uefi_protocol_raw_function, ARGS_REQ(4));
    mrb_define_method(mrb, cls, "get_raw_value", mrb_uefi_protocol_get_raw_value, ARGS_REQ(2));
    mrb_define_method(mrb, cls, "set_raw_value", mrb_uefi_protocol_set_raw_value, ARGS_REQ(3));

    mrb_extend_string(mrb);
}

