

block_io_protocol_guid = UEFI::Guid.new("964e5b21-6459-11d2-8e39-00a0c969723b")
protocol = UEFI::BootService.locate_protocol(block_io_protocol_guid)
p protocol

class BlockIoProtocol < UEFI::Protocol
  define_function(:read_blocks, :e, [:p, :u32, :u64, :u64, :p], offset: 24)
end

bp = BlockIoProtocol.new(protocol)
buf = " " * 512
p bp.read_blocks(protocol, 1, 0, buf.size, buf)
p buf

