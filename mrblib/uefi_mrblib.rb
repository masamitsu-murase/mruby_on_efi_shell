
# Extend Enumerable
# This is a too easy implementation,
# so it might be quite different from the original one.
# For example, StopIteration is not implemented.

module Enumerable
  def each_cons(num, &block)
    buf = []
    self.each do |val|
      if (buf.size < num)
        buf.push(val)
        next if (buf.size < num)
      else
        buf.push(val)
        buf.shift
      end
      block.call(buf)
    end
    block.call(buf) if (buf.size < num)
  end

  def each_slice(num, &block)
    buf = []
    self.each do |val|
      buf.push(val)
      if (buf.size == num)
        block.call(buf)
        buf.clear
      end
    end
    block.call(buf) if (!(buf.empty?))  # Workaround
  end

  def zip(*list, &block)
    if (list.all?{ |i| i.respond_to?(:to_ary) })
      array_list = list.map{ |i| i.to_ary }
    else
      array_list = list.map{ |i| i.to_a }
    end
    self.each_with_index do |val, index|
      buf = [ val ]
      list.each do |item|
        buf.push(item[index])
      end
      block.call(buf)
    end
  end
end


# Extend String

##
# String
#
class String
  def start_with?(str)
    return self.slice(0 .. str.size) == str
  end

  def end_with?(str)
    return self.slice(-str.size .. -1) == str
  end

  def ljust(num, ch)
    if (self.size >= num)
      return self
    else
      return self + ch * (num - self.size)
    end
  end

  def rjust(num, ch)
    if (self.size >= num)
      return self
    else
      return ch * (num - self.size) + self
    end
  end
end


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


module UEFI
  class Protocol
    TYPES = [ :p, :u8, :u16, :u32, :u64, :i8, :i16, :i32, :i64, :h, :e, :b ]
    LONG_NAME_TYPES = [ :pointer, :uint8, :uint16, :uint32, :uint64,
                        :int8, :int16, :int32, :int64, :handle,
                        :efi_status, :boolean ]

    def self.inherited(cls)
      cls.instance_eval do
        # members should be
        #  [ { :name, :ret_type, :arg_type, :offset, :size }, ... ]
        @members = []
      end
    end

    def self.check_arg(ret_type, arg_type = nil)
      type_list = TYPES + LONG_NAME_TYPES

      unless (type_list.include?(ret_type))
        raise "ret_type should be one of #{type_list}"
      end

      if (arg_type)
        invalid_type = arg_type.find do |item|
          next !(type_list.include?(item))
        end
        raise "arg_type should be one of #{type_list}" if (invalid_type)
      end
    end

    def self.normalize_arg_type(type)
      index = LONG_NAME_TYPES.index(type)
      return index ? TYPES[index] : type
    end

    def self.calculate_next_offset(type)
      return 0 if (@members.empty?)

      align = ALIGN_SIZE_INFO[type][:align]
      last_offset = @members.last[:offset]
      last_size = @members.last[:size]

      return ((last_offset + last_size + align - 1) / align).to_i * align
    end

    def self.define_function(name, ret_type, arg_type, option = {})
      check_arg(ret_type, arg_type)

      arg_type = arg_type.map{ |i| normalize_arg_type(i) }
      ret_type = normalize_arg_type(ret_type)

      if (option[:offset])
        offset = option[:offset]
      else
        offset = calculate_next_offset(:p)
      end

      @members.push({ name: name, function: true,
                      ret_type: ret_type, arg_type: arg_type,
                      offset: offset, size: FUNCTION_POINTER_SIZE })

      define_method(name) do |*args|
        check_arg(args, arg_type)

        call_raw_function(args, arg_type, ret_type, offset)
      end
    end

    def self.define_member(name, type, option = {})
      check_arg(type)

      type = normalize_arg_type(type)

      if (option[:offset])
        offset = option[:offset]
      else
        offset = calculate_next_offset(type)
      end

      @members.push({ name: name, function: false,
                      type: type,
                      offset: offset, size: ALIGN_SIZE_INFO[type][:size] })

      define_method(name) do
        get_raw_value(type, offset)
      end
    end


    def initialize(pointer)
      raise TypeError, "argument should be UEFI::Pointer." unless (pointer.kind_of?(Pointer))
      @pointer = pointer
    end

    def check_arg(args, arg_type)
      raise "wrong argument count" unless (args.size == arg_type.size)

      args.zip(arg_type) do |arg, type|
        case(type)
        when :p, :pointer
          unless (arg.kind_of?(String) || arg.kind_of?(Pointer))
            raise TypeError, "#{arg} should be String or Pointer"
          end
        when :u8, :u16, :u32, :u64, :i8, :i16, :i32, :i64,
          :uint8, :uint16, :uint32, :uint64, :int8, :int16, :int32, :int64
          raise TypeError, "#{arg} should be Fixnum" unless (arg.kind_of?(Fixnum))
        when :e, :efi_status
          raise TypeError, "#{arg} should be UEFI::Status" unless (arg.kind_of?(Status))
        when :b, :boolean
          raise TypeError, "#{arg} should be Boolean" unless (arg.kind_of?(Boolean))
        else
          raise "May be a bug of mruby on EFI Shell."
        end
      end
    end
  end
end

