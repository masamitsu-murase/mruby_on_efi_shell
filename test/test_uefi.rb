
def assert(str)
  begin
    if (yield)
      print("OK: #{str}\n")
    else
      print("Fail: #{str}\n")
    end
  rescue => e
    print("Error: #{str}: #{e}\n")
  end
end

def assert_equal(expected, value, str)
  assert("Expected: #{expected}, Value: #{value}, #{str}") do
    expected == value
  end
end

#------------------------------------------
# UEFI
assert_equal("0.0.1", UEFI::VERSION, "UEFI module version")

#------------------------------------------
# LowLevel
CMOS_INDEX = 0x70
CMOS_DATA = 0x71
CMOS_RTC_MIN = 0x02

orig_index = UEFI::LowLevel.io_read8(CMOS_INDEX)
UEFI::LowLevel.io_write8(CMOS_INDEX, CMOS_RTC_MIN)
assert_equal(CMOS_RTC_MIN, UEFI::LowLevel.io_read8(CMOS_INDEX),
             "UEFI::LowLevel io_read8/io_write8")

min = UEFI::LowLevel.io_read8(CMOS_DATA)
UEFI::LowLevel.io_write8(CMOS_INDEX, orig_index)

# min is stored as BCD.
assert("UEFI::LowLevel io_read8/io_write8 RTC") do
  range = (0x00 .. 0x09).to_a + (0x10 .. 0x19).to_a +
    (0x20 .. 0x29).to_a + (0x30 .. 0x39).to_a +
    (0x40 .. 0x49).to_a + (0x50 .. 0x59).to_a
  next range.include?(min)
end


#------------------------------------------
# Guid
a = UEFI::Guid.new("01234567-0123-0123-0123-0123456789AB")
b = UEFI::Guid.new("01234567-0123-0123-0123-0123456789AB")
c = UEFI::Guid.new("00000000-0123-0123-0123-0123456789AB")
assert_equal([ 0x67, 0x45, 0x23, 0x01, 0x23, 0x01, 0x23, 0x01, 0x01, 0x23, 0x01, 0x23, 0x45, 0x67, 0x89, 0xab ],
             a.data.bytes.to_a, "UEFI::Guid data")
assert_equal(a, b, "UEFI::Guid <=>")
assert_equal(true, a > c, "UEFI::Guid <=>")

#------------------------------------------
# Variable
a = UEFI::Guid.new("01234567-0123-0123-0123-0123456789AB")
data = [1, 2, 3, 4]
assert_equal(true, UEFI::RuntimeService.set_variable("test_var", a, 6, data), "UEFI::RuntimeService.set_variable")
assert_equal(data, UEFI::RuntimeService.get_variable("test_var", a), "UEFI::RuntimeService.get_variable")
assert_equal(nil, UEFI::RuntimeService.get_variable("unknown_var", a), "UEFI::RuntimeService.get_variable (unknown_var)")

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

