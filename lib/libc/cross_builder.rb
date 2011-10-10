require 'rake/builder'

module Rake
  class CrossBuilder < Builder

       # Programmaing languages that Rake::Builder can handle
      @@known_languages = {
          'c' => {
              :source_file_extension => 'c',
              :compiler => 'arm-none-eabi-gcc',
              :linker => 'arm-none-eabi-gcc',
              :archiver => 'arm-none-eabi-ar',
              :librarian => 'arm-none-eabi-ranlib'

          },
          'c++' => {
              :source_file_extension => 'cpp',
              :compiler => 'arm-none-eabi-g++',
              :linker => 'arm-none-eabi-g++',
              :archiver => 'arm-none-eabi-ar',
              :librarian => 'arm-none-eabi-ranlib'
          },
      }

     def initialize_attributes
        @architecture = ''
        @compiler_data = Compiler::Base.for(:gcc)
        @logger = Logger.new(STDOUT)
        @logger.level = Logger::UNKNOWN
        @logger.formatter = Formatter.new
        @programming_language = 'c'
        @header_file_extension = 'h'
        @objects_path = @rakefile_path.dup
        @library_paths = []
        @library_dependencies = []
        @target_prerequisites = []
        @source_search_paths = [@rakefile_path.dup]
        @header_search_paths = [@rakefile_path.dup]
        @target = 'a.out'
        @generated_files = []
        @compilation_options = ['-mcpu=cortex-m3 -mthumb -mlittle-endian -Os -fomit-frame-pointer -ffunction-sections -fdata-sections -fno-strict-aliasing -Wall -DCORTEX_M3']
        @include_paths = []
     end

  end

end


