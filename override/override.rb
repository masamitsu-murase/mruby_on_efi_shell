
require("fileutils")

dest = "../mruby/"

Dir.chdir(File.expand_path("../", __FILE__)) do
  files = Dir.glob("**/*").select{ |i| File.file?(i) }
  (files - [ File.basename(__FILE__) ]).each do |file|
    FileUtils.cp(file, dest + file, :verbose => true, :noop => false)
  end
end

