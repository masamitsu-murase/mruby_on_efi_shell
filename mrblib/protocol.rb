
module UEFI
  class Protocol
    TYPES = [ :p, :u8, :u16, :u32, :u64, :i8, :i16, :i32, :i64, :h, :e, :b ]
    LONG_NAME_TYPES = [ :pointer, :uint8, :uint16, :uint32, :uint64,
                        :int8, :int16, :int32, :int64, :handle,
                        :efi_status, :boolean ]

    def self.inherited(cls)
      cls.instance_eval do
        @members = []
        @pack_alignment = nil
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

    def self.calculate_next_offset(alignment)
      return 0 if (@members.empty?)

      last_offset = @members.last[:offset]
      last_size = @members.last[:size]

      return ((last_offset + last_size + alignment - 1) / alignment).to_i * alignment
    end

    def self.define_function(name, ret_type, arg_type, option = {})
      check_arg(ret_type, arg_type)

      arg_type = arg_type.map{ |i| normalize_arg_type(i) }
      ret_type = normalize_arg_type(ret_type)

      if (@pack_alignment)
        alignment = [ @pack_alignment, TYPE_INFO[:p][:alignment] ].min
      else
        alignment = TYPE_INFO[:p][:alignment]
      end

      if (option[:offset])
        offset = option[:offset]
      else
        offset = calculate_next_offset(alignment)
      end

      @members.push({ name: name, type: :p,
                      alignment: alignment,
                      offset: offset, size: TYPE_INFO[:p][:size],
                      function: {
                        ret_type: ret_type, arg_type: arg_type
                      }
                    })

      define_method(name) do |*args|
        check_arg(args, arg_type)

        call_raw_function(args, arg_type, ret_type, offset)
      end
    end

    def self.define_member(name, type, option = {})
      check_arg(type)

      type = normalize_arg_type(type)

      if (@pack_alignment)
        alignment = [ @pack_alignment, TYPE_INFO[type][:alignment] ].min
      else
        alignment = TYPE_INFO[type][:alignment]
      end

      if (option[:offset])
        offset = option[:offset]
      else
        offset = calculate_next_offset(alignment)
      end

      @members.push({ name: name, type: type,
                      alignment: alignment,
                      offset: offset, size: TYPE_INFO[type][:size] })

      define_method(name) do
        get_raw_value(type, offset)
      end
    end

    def self.pack(alignment = nil)
      alignment_list = [ nil, 1, 2, 4, 8 ]
      raise "Invalid alignment" unless (alignment_list.include?(alignment))

      @pack_alignment = alignment
    end

    def self.size
      @size ||= calculate_next_offset(alignment)
    end

    def self.alignment
      @alignment ||= @members.map{ |i| i[:alignment] }.max
    end


    #================================================================
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

