
/*
 * Extend Integer
 * This is a too easy implementation,
 * so it might be quite different from the original one.
 */

#include "mruby.h"
#include "error.h"
#include "mruby/array.h"
#include "mruby/class.h"
#include "mruby/numeric.h"
#include "mruby/string.h"
#include "mruby/variable.h"

static mrb_value
mrb_numeric_ext_chr(mrb_state *mrb, mrb_value self)
{
    UINT8 value;

    if (mrb_fixnum(self) < 0 || mrb_fixnum(self) > 0xFF){
        mrb_raise(mrb, E_RANGE_ERROR, "Out of ASCII-8bit char range.");
    }

    value = (UINT8)mrb_fixnum(self);
    return mrb_str_new(mrb, (char *)&value, 1);
}

void mrb_numeric_ext_init(mrb_state *mrb)
{
    /* Currently, only Fixnum has chr because Bignum is not implemented yet. */
    mrb_define_method(mrb, mrb->fixnum_class, "chr", mrb_numeric_ext_chr, ARGS_NONE());
}

