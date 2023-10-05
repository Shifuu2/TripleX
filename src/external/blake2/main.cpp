#include"blake2.h"
#include<cassert>
#include<array>
/* inlen, at least, should be uint64_t. Others can be size_t. */
std::array<uint8_t,32> blake2b_32(const void *in, size_t inlen )
{
    std::array<uint8_t,32> out;
  blake2b_state S[1];

  /* Verify parameters */
  assert( NULL != in && inlen > 0 );
  assert( blake2b_init( S, 32 ) == 0 );
  blake2b_update( S, ( const uint8_t * )in, inlen );
  blake2b_final( S, out.data(), 32 );
  return out;
}

int main()
{
    blake2b_32("asdf",4);
}
