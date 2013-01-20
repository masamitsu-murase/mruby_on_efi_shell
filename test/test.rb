
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
  define_variable(:revision, :u64)
  define_variable(:media, :p)
  define_function(:reset, :e, [:p, :b])
  define_function(:read_blocks, :e, [:p, :u32, :u64, :u64, :p])
  # ...
end

# I will rename UEFI::Protocol to UEFI::Structure or any other name...
class Media < UEFI::Protocol
  define_variable(:media_id, :uint32)
  define_variable(:removable_media, :b)
  define_variable(:media_present, :b)
  define_variable(:logical_partition, :b)
  define_variable(:read_only, :b)
  define_variable(:write_caching, :b)
  define_variable(:block_size, :u32)
  define_variable(:io_align, :u32)
  define_variable(:last_block, :u64)
end

# EFI_BLOCK_IO_PROTOCOL_GUID
guid = UEFI::Guid.new("964e5b21-6459-11d2-8e39-00a0c969723b")
ptr = UEFI::BootService.locate_protocol(guid)

bp = BlockIoProtocol.new(ptr)
media = Media.new(bp.media)

puts "revision: #{bp.revision}"
puts "media_id: #{media.media_id}"

buf = " " * 512  # Buffer to be filled with returned data.
st = bp.read_blocks(ptr, media.media_id, 0, buf.size, buf)
if (st.success?)
  print_binary_data(buf)
else
  puts "ERROR: #{st}"
end

