
def assert(str)
  begin
    if (yield)
      print "."
      $ok += 1
    else
      print "F"
      $ko += 1
      $ko_list.push(str)
    end
  rescue
    print "E"
    $error += 1
    $error_list.push(str)
  end
end

def assert_equal(expected, value, str)
  assert("Expected: #{expected}, Value: #{value}, #{str}") do
    expected == value
  end
end

def prepare
  $ok = 0
  $ko = 0
  $ko_list = []
  $error = 0
  $error_list = []
end

def report
  puts ""
  puts "================================"
  puts "OK: #{$ok}"
  puts "KO: #{$ko}"
  puts "ERROR: #{$error}"
  puts "================================"

  unless ($ko_list.empty?)
    puts "KO:"
    $ko_list.each do |text|
      print text
      puts ""
      puts ""
    end
  end

  unless ($error_list.empty?)
    puts "ERROR:"
    $error_list.each do |text|
      print text
      puts ""
      puts ""
    end
  end
end

#------------------------------------------
prepare

#------------------------------------------
# UEFI
assert_equal("1.0.0", UEFI::VERSION, "UEFI module version")

#------------------------------------------
# LowLevel IO
CMOS_INDEX = 0x70
CMOS_DATA = 0x71
CMOS_RTC_MIN = 0x02

UEFI::LowLevel.io_write8(CMOS_INDEX, CMOS_RTC_MIN)
min = UEFI::LowLevel.io_read8(CMOS_DATA)

# min is stored as BCD.
assert("UEFI::LowLevel io_read8/io_write8 RTC") do
  bcd_range = (0..5).to_a.map{ |i| (0..9).to_a.map{ |j| i*16+j } }.flatten

  bcd_range.include?(min)
end


#==========================================
# Boot Service
#==========================================

#------------------------------------------
# Protocol
block_io_protocol_guid = UEFI::Guid.new("964e5b21-6459-11d2-8e39-00a0c969723b")
protocol = UEFI::BootService.locate_protocol(block_io_protocol_guid)
assert("UEFI::BootService.locate_protocol") do
  protocol
end


#==========================================
# Runtime Service
#==========================================

#------------------------------------------
# Variable
a = UEFI::Guid.new("01234567-0123-0123-0123-0123456789AB")
data = [1, 2, 3, 4].pack("C*")
assert("UEFI::RuntimeService.set_variable") do
  UEFI::RuntimeService.set_variable("test_var", a, 6, data)
end
assert("UEFI::RuntimeService.get_variable") do
  UEFI::RuntimeService.get_variable("test_var", a) == data
end
assert("UEFI::RuntimeService.get_variable (unknown_var)") do
  UEFI::RuntimeService.get_variable("unknown_var", a).nil?
end

#------------------------------------------
# Reset System
assert("UEFI::RuntimeService.reset_system") do
  UEFI::RuntimeService.respond_to?(:reset_system)
end


#==========================================
# Other
#==========================================

#------------------------------------------
# Guid
a = UEFI::Guid.new("01234567-0123-0123-0123-0123456789AB")
b = UEFI::Guid.new("01234567-0123-0123-0123-0123456789AB")
c = UEFI::Guid.new("00000000-0123-0123-0123-0123456789AB")
assert("UEFI::Guid data") do
  data = [ 0x67, 0x45, 0x23, 0x01, 0x23, 0x01, 0x23, 0x01, 0x01, 0x23, 0x01, 0x23, 0x45, 0x67, 0x89, 0xab ]

  a.data.bytes == data
end
assert("UEFI::Guid <=>") do
  a == b
end
assert("UEFI::Guid <=>") do
  a > c
end

#------------------------------------------
# EFI_STATUS
a = UEFI::Status::SUCCESS
b = UEFI::Status::BUFFER_TOO_SMALL
assert_equal(true, a == UEFI::Status::SUCCESS, "UEFI::Status#==")
assert_equal(false, a == b, "UEFI::Status#==")
assert_equal(true, a != b, "UEFI::Status#==")
assert_equal(true, a.success?, "UEFI::Status#success?")
assert_equal(false, a.error?, "UEFI::Status#error?")
assert_equal(false, b.success?, "UEFI::Status#success?")
assert_equal(true, b.error?, "UEFI::Status#error?")
assert_equal("EFI_SUCCESS", a.to_s, "UEFI::Status#to_s")
assert_equal("EFI_BUFFER_TOO_SMALL", b.to_s, "UEFI::Status#to_s")
assert_equal("<EFI_SUCCESS (0x0000000000000000)>", a.inspect, "UEFI::Status#inspect")
assert_equal("<EFI_BUFFER_TOO_SMALL (0x8000000000000005)>", b.inspect, "UEFI::Status#inspect")
assert_equal("\x00\x00\x00\x00\x00\x00\x00\x00", a.value, "UEFI::Status#value")
assert_equal("\x05\x00\x00\x00\x00\x00\x00\x80", b.value, "UEFI::Status#value")

#------------------------------------------
# EFI_HANDLE
a = UEFI::Handle::NULL
assert_equal(true, a == UEFI::Handle::NULL, "UEFI::Handle#==")
assert_equal("0x0000000000000000", a.to_s, "UEFI::Handle#to_s")
assert_equal("<Handle 0x0000000000000000>", a.inspect, "UEFI::Handle#inspect")
assert_equal("\x00\x00\x00\x00\x00\x00\x00\x00", a.value, "UEFI::Handle#value")

#------------------------------------------
# Pointer
a = UEFI::Pointer::NULL
assert_equal(true, a == UEFI::Pointer::NULL, "UEFI::Pointer#==")
assert_equal(false, a > UEFI::Pointer::NULL, "UEFI::Pointer#<=>")
assert_equal("0x0000000000000000", a.to_s, "UEFI::Pointer#to_s")
assert_equal("<Pointer 0x0000000000000000>", a.inspect, "UEFI::Pointer#inspect")
assert_equal("\x00\x00\x00\x00\x00\x00\x00\x00", a.value, "UEFI::Pointer#value")


#------------------------------------------
report

