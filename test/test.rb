

# SetVariable, GetVariable

data = [ 0, 1, 2, 3, 4 ]
p data

guid = UEFI::Guid.new("01234567-0123-0123-0123-0123456789AB")
p guid

p UEFI::RuntimeService.set_variable("test_var", guid, 0x06, data)

data2 = UEFI::RuntimeService.get_variable("test_var", guid)
p data2

vars = UEFI::RuntimeService.get_all_variable_names
vars.each do |var|
  if (var[:guid].data == UEFI::Guid::GLOBAL_VARIABLE)
    p var
  end
end


