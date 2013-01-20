
# Sample of BLOCK_IO_PROTOCOL

def print_binary_data(data)
  data.bytes.each_slice(16) do |data|
    # print hex.
    data.each do |i|
      print i.to_s(16).rjust(2, "0").upcase
      print " "
    end
    print "   "
    # print ascii.
    data.each do |i|
      print (0x20 <= i && i <= 0x7F) ? i.chr : "."
    end
    puts ""
  end
  puts ""
end

class BlockIoProtocol < UEFI::Protocol
  GUID = UEFI::Guid.new("964e5b21-6459-11d2-8e39-00a0c969723b")

  define_variable(:revision, :u64)
  define_variable(:media, :p)
  define_function(:reset, :e, [:p, :b])
  define_function(:read_blocks, :e, [:p, :u32, :u64, :u64, :p])
  #...
end

# I will rename UEFI::Protocol to UEFI::Structure or any other name...
class Media < UEFI::Protocol
  define_variable(:media_id, :uint32)
  #...
end

# See http://wiki.phoenix.com/wiki/index.php/EFI_DISK_IO_PROTOCOL
class DiskIoProtocol < UEFI::Protocol
  GUID = UEFI::Guid.new("CE345171-BA0B-11d2-8e4F-00a0c969723b")

  define_variable(:revision, :u64)
  define_function(:read_disk, :efi_status, [:p, :u32, :u64, :u64, :p])
  #...
end


# Find first disk.
handles = UEFI::BootService.locate_handle_buffer(BlockIoProtocol::GUID)
handle = handles.first
puts "handle: #{handle}"

# Locate BlockIoProtocol to get the pointer of media.
ptr = UEFI::BootService.handle_protocol(handle, BlockIoProtocol::GUID)
bp = BlockIoProtocol.new(ptr)
# Get media.
media = Media.new(bp.media)
puts "media_id: #{media.media_id}"

# Find DiskIoProtocol related to bp.
ptr = UEFI::BootService.handle_protocol(handle, DiskIoProtocol::GUID)
dp = DiskIoProtocol.new(ptr)

buf = " " * 512  # Buffer to be filled with returned data.
st = dp.read_disk(ptr, media.media_id, 0, buf.size, buf)
if (st.success?)
  print_binary_data(buf)
else
  puts "ERROR: #{st}"
end
