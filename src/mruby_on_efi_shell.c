
#include  <stdio.h>

int mruby_main(int argc, char **argv);

int EFIAPI main (
  IN int Argc,
  IN char **Argv
  )
{
    return mruby_main(Argc, Argv);
}
