
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

    def self.check_arg(ret_type, arg_type)
      type_list = TYPES + LONG_NAME_TYPES

      unless (type_list.include?(ret_type))
        raise "ret_type should be one of #{type_list}"
      end

      invalid_type = arg_type.find do |item|
        next !(type_list.include?(item))
      end
      raise "arg_type should be one of #{type_list}" if (invalid_type)
    end

    def self.define_function(name, ret_type, arg_type, option = {})
      check_arg(ret_type, arg_type)

      # Convert to short name
      short_name = TYPES  # Workaround
      long_name = LONG_NAME_TYPES  # Workaround
      arg_type = arg_type.map do |type|
        index = long_name.index(type)
        next index ? short_name[index] : type
      end
      index = long_name.index(ret_type)
      ret_type = short_name[index] if (index)

      if (option[:offset])
        offset = option[:offset]
      elsif (@members.empty?)
        offset = 0
      else
        last = @members.last
        offset = last[:offset] + last[:size]
      end

      size = (option[:size] || FUNCTION_POINTER_SIZE)

      @members.push({ name: name, ret_type: ret_type,
                      arg_type: arg_type, offset: offset, size: size })

      define_method(name) do |*args|
        check_arg(args, arg_type)

        call_raw_function(args, arg_type, ret_type, offset)
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

