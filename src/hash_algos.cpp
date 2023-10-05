#include "hash_algos.hpp"
#include "external/sha3/sha3.h"
#include "external/blake2/blake2.h"
#include "external/blake2/blake2-impl.h"
#include "external/blake3/blake3.h"
#include "external/blake3/blake3_impl.h"

std::array<uint8_t, 32> sha3(std::span<const uint8_t> s) {
  sha3_context c;

  sha3_Init256(&c);
  sha3_Update(&c, s.data(),s.size());
  auto buf{sha3_Finalize(&c)};
  std::array<uint8_t, 32> out;
  memcpy(out.data(), buf, 32);
  return out;
}

std::array<uint8_t,32> blake2b_32(std::span<const uint8_t> s)
{
    std::array<uint8_t,32> out;
  blake2b_state S[1];

  /* Verify parameters */
  assert( blake2b_init( S, 32 ) == 0 );
  blake2b_update( S, s.data(), s.size() );
  blake2b_final( S, out.data(), 32 );
  return out;
}

std::array<uint8_t,32> blake3(std::span<const uint8_t> s)
{
    std::array<uint8_t,32> out;
  blake3_hasher hasher;
  blake3_hasher_init(&hasher);

  blake3_hasher_update(&hasher, s.data(), s.size());
  static_assert(BLAKE3_OUT_LEN==32);
  blake3_hasher_finalize(&hasher, out.data(), BLAKE3_OUT_LEN);
  return out;
}
