project(
  'datastream',
  'cpp',
  meson_version: '>= 1.3.0',
  default_options: ['warning_level=3', 'cpp_std=c++23'] + (get_option('buildtype') == 'release' ? ['optimization=3'] : ['optimization=0']),
  version: files('version.txt')
)

fs = import('fs')
pkg = import('pkgconfig')

dependencies = []

librarySources = [
  'src/datastream.cpp'
]

libraryHeaders = [
  'include/datastream.hpp'
]

lib = shared_library(
  'datastream',
  librarySources,
  dependencies: dependencies,
  include_directories: include_directories('include'),
  cpp_args: '-DVERSION="' + fs.read('version.txt').strip() + '"',
  install: true
)

install_headers(
  libraryHeaders,
  subdir: 'datastream'
)

pkg.generate(
  lib,
  description: 'Stream parsing library with Unicode support for C++'
)

tests = [
  'tests/utf8.cpp'
]

testsuite = executable(
  'dumbtest',
  tests,
  dependencies: [ dependency('dumbtest') ],
  link_with: lib
)

test('testsuite', testsuite)
