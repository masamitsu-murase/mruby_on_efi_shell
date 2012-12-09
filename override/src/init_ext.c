/*
** init_ext.c - initialize extend libraries
**
** See Copyright Notice in mruby.h
*/

#include "mruby.h"

void
mrb_init_ext(mrb_state *mrb)
{
#ifdef INCLUDE_SOCKET
  extern void mrb_init_socket(mrb_state *mrb);
  mrb_init_socket(mrb);
#endif

#ifdef INCLUDE_UEFI
  extern void mrb_init_uefi(mrb_state *mrb);
  mrb_init_uefi(mrb);
#endif
}
