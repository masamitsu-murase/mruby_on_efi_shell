
def assert(str)
  begin
    if (yield)
      print("OK: #{str}\n")
    else
      print("Fail: #{str}\n")
    end
  rescue => e
    print("Error: #{str}: #{e}\n")
  end
end


assert("UEFI module version") do
  UEFI::VERSION == "0.0.1"
end

