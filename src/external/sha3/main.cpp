#include "sha3.h"
#include <array>
#include <cassert>
#include <cstring>

/* inlen, at least, should be uint64_t. Others can be size_t. */
std::array<uint8_t, 32> sha3(const void *in, size_t inlen) {
  sha3_context c;

  sha3_Init256(&c);
  sha3_Update(&c, in, inlen);
  auto buf{sha3_Finalize(&c)};
  std::array<uint8_t, 32> out;
  memcpy(out.data(), buf, 32);
  return out;
}

int main() { sha3("asdf", 4); }
