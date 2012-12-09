
MRBC = ""
OUTPUT = File.expand_path("../uefi_mrblib.rb", __FILE__)
raise "#{MRBC} is not found." unless (File.exist?(MRBC))

dir = File.expand_path("..", __FILE__)
files = Dir.glob("#{dir}/*.rb") - [ File.expand_path(__FILE__), OUTPUT ]

system("cat #{files.join(' ')} > #{OUTPUT}")
system("#{MRBC} -Cinit_uefi_mrblib #{OUTPUT}")

