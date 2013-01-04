
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
      block.call(*buf)
    end
    block.call(*buf) if (buf.size < num)
  end

  def each_slice(num, &block)
    buf = []
    self.each do |val|
      buf.push(val)
      if (buf.size == num)
        block.call(*buf)
        buf.clear
      end
    end
    block.call(*buf) unless (buf.empty?)
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

