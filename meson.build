project(
  'datastream',
  'cpp',
  meson_version: '>= 1.3.0',
  default_options: ['warning_level=3', 'cpp_std=c++23'] + (get_option('buildtype') == 'release' ? ['optimization=3'] : ['optimization=0']),
  version: files('version.txt')
)

fs = import('fs')

dependencies = []

librarySources = [
  'src/datastream.cpp'
]

libraryHeaders = [
  'include/datastream.hpp'
]

tests = run_command('sh', '-c', 'ls -1 $MESON_SOURCE_ROOT/testing/tests | sed \'s/^/testing\\/tests\\//\'', check: true).stdout().strip().split('\n')

testsuiteSources = [
  'testing/runner.cpp',
  'testing/suite.cpp',
  'testing/suite.hpp',
  'testing/util.cpp',
  'testing/util.hpp',
  tests
]

lib = static_library(
  'datastream',
  librarySources,
  dependencies: dependencies,
  include_directories: include_directories('include'),
  cpp_args: '-DVERSION="' + fs.read('version.txt').strip() + '"'
)

testsuite = executable(
  'dumbtest',
  testsuiteSources,
  link_with: lib
)

test('testsuite', testsuite)
