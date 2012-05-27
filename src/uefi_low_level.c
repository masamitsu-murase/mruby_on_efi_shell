
#include <Library/IoLib.h>

#include "uefi.h"

static mrb_value
low_level_io_read8(mrb_state *mrb, mrb_value obj)
{
    mrb_value y;
    UINT16 addr;

    mrb_get_args(mrb, "o", &y);
    addr = (UINT16)mrb_fixnum(y);

    return mrb_fixnum_value(IoRead8(addr));
}

static mrb_value
low_level_io_read16(mrb_state *mrb, mrb_value obj)
{
    mrb_value y;
    UINT16 addr;

    mrb_get_args(mrb, "o", &y);
    addr = (UINT16)mrb_fixnum(y);

    return mrb_fixnum_value(IoRead16(addr));
}

static mrb_value
low_level_io_write8(mrb_state *mrb, mrb_value obj)
{
    mrb_value y, z;
    UINT16 addr;
    UINT8 value;

    mrb_get_args(mrb, "oo", &y, &z);
    addr = (UINT16)mrb_fixnum(y);
    value = (UINT8)mrb_fixnum(z);

    IoWrite8(addr, value);

    return mrb_nil_value();
}

static mrb_value
low_level_io_write16(mrb_state *mrb, mrb_value obj)
{
    mrb_value y, z;
    UINT16 addr;
    UINT16 value;

    mrb_get_args(mrb, "oo", &y, &z);
    addr = (UINT16)mrb_fixnum(y);
    value = (UINT16)mrb_fixnum(z);

    IoWrite16(addr, value);

    return mrb_nil_value();
}

void
mrb_init_uefi_low_level(mrb_state *mrb, struct RClass *mrb_uefi)
{
    struct RClass *mrb_ns;

    mrb_ns = mrb_define_module_under(mrb, mrb_uefi, "LowLevel");
    mrb_define_module_function(mrb, mrb_ns, "io_read8", low_level_io_read8, 1);
    mrb_define_module_function(mrb, mrb_ns, "io_read16", low_level_io_read16, 1);
    mrb_define_module_function(mrb, mrb_ns, "io_write8", low_level_io_write8, 2);
    mrb_define_module_function(mrb, mrb_ns, "io_write16", low_level_io_write16, 2);
}

