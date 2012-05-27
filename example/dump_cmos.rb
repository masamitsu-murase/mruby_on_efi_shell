
################################################3
# Example
#  CMOS dump.

class String
  def rjust(width, padding=' ')
    str = ""
    if (self.length < width)
      str = padding * (width - self.length)
    end
    return str + self
  end
end

CMOS_INDEX = 0x70
CMOS_DATA = 0x71

orig_index = UEFI::LowLevel.io_read8(CMOS_INDEX)
(0..0x7F).to_a.each_with_index do |i, index|
  UEFI::LowLevel.io_write8(CMOS_INDEX, i)
  v = UEFI::LowLevel.io_read8(CMOS_DATA)
  print "#{v.to_s(16).rjust(2, '0')} "
  print "\n" if (index % 16 == 15)
end
UEFI::LowLevel.io_write8(CMOS_INDEX, orig_index)

