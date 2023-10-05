project('TripleX', ['c','cpp'],
  version : '0.1',
  default_options : ['warning_level=3',
                     'cpp_std=c++20'])

inc=include_directories([
  './external/sha3',
  './external/blake2',
  './external/blake3'
  ])

exe = executable('triplex', 
  [ 'triplex.cpp',
    './hash_algos.cpp',
    './external/blake2/blake2b-ref.c',
    './external/blake3/blake3.c',
    './external/blake3/blake3_sse2.c',
    './external/blake3/blake3_portable.c',
    './external/blake3/blake3_dispatch.c',
    './external/sha3/sha3.cpp',
    ],

  c_args: ['-DBLAKE3_NO_AVX512', '-DBLAKE3_NO_SSE41', '-DBLAKE3_NO_AVX2 '],
  include_directories: inc,
  install : true)

test('basic', exe)
