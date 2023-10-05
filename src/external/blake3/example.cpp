#include "blake3.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include<array>

std::array<uint8_t,32> blake2b_32(const void *in, size_t inlen )
{
    std::array<uint8_t,32> out;
  blake3_hasher hasher;
  blake3_hasher_init(&hasher);

  blake3_hasher_update(&hasher, in, inlen);
  static_assert(BLAKE3_OUT_LEN==32);
  blake3_hasher_finalize(&hasher, out.data(), BLAKE3_OUT_LEN);
  return out;
}

int main(void) {
  return 0;
}
