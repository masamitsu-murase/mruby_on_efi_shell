
module UEFI
  class Guid
    include Comparable

    attr_reader :data

    def initialize(arg)
      if (arg.bytesize == 16)
        initialize_raw(arg)
      else
        initialize_str(arg)
      end
    end

    def initialize_str(str)
      # 01234567-0123-0123-0123-0123456789AB
      str = str.slice(1 .. -2) if (str.start_with?("{"))
      data = str.split("-")
      raise "Invalid GUID: #{str}" if (data.size != 5)

      data_array = []
      [ data[0], data[1], data[2] ].each do |d|
        chars = []
        d.each_char do |ch|
          chars.push(ch)
        end
        chars.reverse.each_slice(2) do |a, b|
          data_array.push(a.to_i(16) + b.to_i(16) * 16)
        end
      end
      [ data[3], data[4] ].each do |d|
        chars = []
        d.each_char do |ch|
          chars.push(ch)
        end
        chars.each_slice(2) do |a, b|
          data_array.push(a.to_i(16) * 16 + b.to_i(16))
        end
      end

      @data = data_array.pack("C*")
    end

    def initialize_raw(raw)
      @data = raw.clone
    end

    def to_s
      data = [ @data.slice(0, 4).bytes.to_a.reverse,
               @data.slice(4, 2).bytes.to_a.reverse,
               @data.slice(6, 2).bytes.to_a.reverse,
               @data.slice(8, 2).bytes.to_a,
               @data.slice(10, 6).bytes.to_a ]
      str = data.map do |i|
        i.map{ |j| j.to_s(16).rjust(2, '0') }.join("")
      end.join("-")
      return str
    end

    def inspect
      return "<GUID: #{self.to_s}>"
    end

    def <=>(other)
      self.data <=> other.data
    end


    ################################################################
    # Well-known GUID list.
    GLOBAL_VARIABLE = self.new("8be4df61-93ca-11d2-aa0d-00e098032b8c")
  end
end

# a = UEFI::Guid.new("01234567-0123-0123-0123-0123456789AB")
# puts a
# p a
# p a.data

