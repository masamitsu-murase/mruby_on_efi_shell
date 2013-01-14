
#include <Library/UefiBootServicesTableLib.h>

#include "mruby.h"

#include "uefi.h"

#define ARENA_SAVE()    arena=mrb_gc_arena_save(mrb)
#define ARENA_RESTORE() mrb_gc_arena_restore(mrb, arena)

void init_uefi_mrblib(mrb_state *mrb);

void
mrb_init_uefi(mrb_state *mrb)
{
    int arena;
    struct RClass *mrb_uefi;

    mrb_pack_gem_init(mrb);
    mrb_numeric_ext_init(mrb);

    mrb_uefi = mrb_define_module(mrb, "UEFI");

    mrb_define_const(mrb, mrb_uefi, "SPECIFICATION_VERSION", mrb_fixnum_value(gST->Hdr.Revision));
    mrb_define_const(mrb, mrb_uefi, "VERSION", mrb_str_new_cstr(mrb, UEFI_LIB_VERSION));

    ARENA_SAVE();
    mrb_init_uefi_boot_service(mrb, mrb_uefi);
    ARENA_RESTORE(); ARENA_SAVE();
    mrb_gc_arena_restore(mrb, arena);
    ARENA_RESTORE(); ARENA_SAVE();
    mrb_init_uefi_runtime_service(mrb, mrb_uefi);
    ARENA_RESTORE(); ARENA_SAVE();
    mrb_init_uefi_gop(mrb, mrb_uefi);
    ARENA_RESTORE(); ARENA_SAVE();
    mrb_init_uefi_low_level(mrb, mrb_uefi);
    ARENA_RESTORE(); ARENA_SAVE();
    mrb_init_uefi_status(mrb, mrb_uefi);
    ARENA_RESTORE(); ARENA_SAVE();
    mrb_init_uefi_handle(mrb, mrb_uefi);
    ARENA_RESTORE(); ARENA_SAVE();
    mrb_init_uefi_pointer(mrb, mrb_uefi);
    ARENA_RESTORE(); ARENA_SAVE();
    mrb_init_uefi_protocol(mrb, mrb_uefi);
    ARENA_RESTORE();

    init_uefi_mrblib(mrb);
}

