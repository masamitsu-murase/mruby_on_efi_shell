# mruby on EFI Shell

Please refer to the [**project page**](http://masamitsu-murase.github.com/mruby_on_efi_shell/).

## Overview

This is a [mruby](https://github.com/mruby/mruby) porting on [EFI Shell](http://en.wikipedia.org/wiki/Unified_Extensible_Firmware_Interface).

## Build Environment

1. Install and setup 32bit Ubuntu.  
   I use Ubuntu 9.10 Karmic Koala.
2. Clone [edk2\_for\_mruby](https://github.com/masamitsu-murase/edk2_for_mruby).  
   I assume that the directory is 'edk2\_for\_mruby'.
3. Clone [mruby\_on\_efi\_shell](https://github.com/masamitsu-murase/mruby_on_efi_shell) to 'edk2\_for\_mruby/AppPkg/Applications/mruby'.
4. Run 'ruby edk2\_for\_mruby/AppPkg/Applications/mruby/override/override.rb' to patch mruby source code.
5. Move to 'edk2\_for\_mruby' then run following commands to prepare to build edk module.
 1. Run '. edksetup.sh'.
 2. Run 'make -C BaseTools'.
6. Run 'build'.


## TODO

* Support calling UEFI functions from ruby source code.  
  For example,
 * Call GetVariable, SetVariable.

