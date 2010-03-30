#!/usr/bin/env ruby

require 'mkmf'

dir_config('gmp')
dir_config('msieve')

ok = true
unless have_header('gmp.h')
  $stderr.puts "can't find gmp.h, try --with-gmp-include=<path>"
  ok = false
end

unless have_library('gmp', '__gmpz_init')
  $stderr.puts "can't find -lgmp, try --with-gmp-lib=<path>"
  ok = false
end

unless have_library('msieve', 'msieve_obj_new')
  $stderr.puts "can't find -lmsieve, try --with-msieve-lib=<path>"
  ok = false
end

unless have_header('msieve.h')
  $stderr.puts "can't find msieve.h, try --with-msieve-include=<path>"
  ok = false
end

$CFLAGS += ' -Wall -W -O6 -g'
if ok
  create_makefile('msieve')
else
  raise "Unable to build, correct above errors and rerun"
end
