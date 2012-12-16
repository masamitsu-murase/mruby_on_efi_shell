
MRBC = File.expand_path("../../mruby/bin/mrbc", __FILE__)
OUTPUT = File.expand_path("../uefi_mrblib.rb", __FILE__)
OUTPUT_C = File.expand_path("../uefi_mrblib.c", __FILE__)
raise "#{MRBC} is not found." unless (File.exist?(MRBC))

dir = File.expand_path("..", __FILE__)
files = Dir.glob("#{dir}/*.rb").sort - [ File.expand_path(__FILE__), OUTPUT ]

system("cat #{files.join(' ')} > #{OUTPUT}")
system("#{MRBC} -Buefi_mrblib_irep #{OUTPUT}")

File.open(OUTPUT_C, "a") do |file|
str = <<'EOS'
#include "mruby.h"
#include "mruby/irep.h"
#include "mruby/dump.h"
#include "mruby/string.h"
#include "mruby/proc.h"

void
init_uefi_mrblib(mrb_state *mrb)
{
    mrb_load_irep(mrb, uefi_mrblib_irep);
}
EOS

  file.puts str
end


