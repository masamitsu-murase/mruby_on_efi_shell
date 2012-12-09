
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

