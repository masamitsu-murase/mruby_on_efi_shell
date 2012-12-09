
# Extend Enumerable

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
end

