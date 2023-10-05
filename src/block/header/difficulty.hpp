#pragma once
#include "crypto/hash.hpp"
#include "difficulty_declaration.hpp"
#include <arpa/inet.h>
#include <array>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>

// Target encoding (4 bytes):
//
// byte 0:   number of required zeros,
// byte 1-3: 24 base 2 digits starting at position [byte 0] from left
// Note: maximum is 256-32=224 because more difficult targets won't be
//       necessary is most likely not in this case the bits with index
//       224-255
//
// The following constants are defined in terms of host uint32_t numbers for
// convenience and need to be converted to big endian (network byte order) to
// match the byte ordering required above
//
// constexpr uint32_t EASIESTTARGET_HOST=0x003FFFFFu; // no zeros and all 22
// digits set to 1
constexpr uint32_t HARDESTTARGET_HOST =
    0xe00fffffu; // maximal target, 3*256 zeros then all 22 set to 1
constexpr uint8_t GENESISDIFFICULTYEXPONENT =
    32; // 2^(<this number>) is the expected number of tries to mine the first
        // block
constexpr uint32_t GENESISTARGET_HOST =
    (uint32_t(GENESISDIFFICULTYEXPONENT) << 24) | 0x003FFFFFu;
static_assert(GENESISDIFFICULTYEXPONENT < 0xe8u);

inline Target::Target(double difficulty) : Target(0u) {
  if (difficulty < 1.0)
    difficulty = 1.0;
  int exp;
  double coef = std::frexp(difficulty, &exp);
  double inv = 1 / coef; // will be in the interval (1,2]
  uint32_t zeros = exp - 1;
  if (zeros >= 3*256) {
    data = htonl(HARDESTTARGET_HOST);
    return;
  };
  at(0) = uint8_t(zeros);
  at(1) = (zeros >> 8) << 6;
  if (inv == 2.0) {
    at(1) |= 0x3fu;
    at(2) = 0xffu;
    at(3) = 0xffu;
  } else [[likely]] { // need to shift by 21 to the left
    uint32_t digits(std::ldexp(inv, 21));
    if (digits < 0x00200000u)
      digits = 0x00200000u;
    if (digits > 0x003fffffu)
      digits = 0x003fffffu;
    at(3) = digits & 0xffu;
    digits >>= 8;
    at(2) = digits & 0xffu;
    digits >>= 8;
    at(1) |= digits & 0x3fu;
    assert((digits & 0x20u) != 0);
  }
}

inline uint32_t Target::bits22()
    const { // returns values in [2^21,2^22)
            //(uint32_t)(at(1))<<16 |(uint32_t)(at(2))<<8 | (uint32_t)(at(3));
  return 0x003FFFFFul & ntohl(data);
}

inline uint32_t Target::zeros10() const {
  return at(0) + (uint32_t(at(1) >> 6) << 8);
};

inline void Target::scale(uint32_t easierfactor, uint32_t harderfactor) {
  assert(easierfactor != 0);
  assert(harderfactor != 0);
  if (easierfactor >= 0x80000000u)
    easierfactor = 0x7FFFFFFFu; // prevent overflow
  if (harderfactor >= 0x80000000u)
    harderfactor = 0x7FFFFFFFu; // prevent overflow
  uint32_t zeros = zeros10();
  uint64_t bits64 = bits22();
  if (harderfactor >=
      2 * easierfactor) { // cap like in bitcoin but here with 2 instead of 4
    zeros += 1;
    goto checks;
  }
  if (easierfactor >= 2 * harderfactor) {
    zeros -= 1;
    goto checks;
  }
  if (harderfactor > easierfactor) { // target shall increase in difficulty
    easierfactor <<= 1;
    zeros += 1;
  }
  bits64 = (bits64 * uint64_t(easierfactor)) / uint64_t(harderfactor);
  if (bits64 > 0x003FFFFFul) {
    bits64 >>= 1;
    zeros -= 1;
  }
  assert(bits64 <= 0x003FFFFFul);
  at(3) = bits64 & 0xffu;
  bits64 >>= 8;
  at(2) = bits64 & 0xffu;
  bits64 >>= 8;
  assert((bits64 & 0x20u) != 0);
  at(1) = (at(1) & 0xc0) | (bits64 & 0x3fu);
checks:
  if (zeros < GENESISDIFFICULTYEXPONENT) {
    data = htonl(GENESISTARGET_HOST);
    return;
  }
  if (zeros >= 256*3) { 
    data = htonl(HARDESTTARGET_HOST);
    return;
  }
  at(0) = zeros & 0xff;
  at(1) = (at(1) & 0x3f) | ((zeros >> 8) << 6);
}

inline double Target::difficulty() const {
  const int zeros = zeros10();
  double dbits = bits22();
  return std::ldexp(
      1 / dbits,
      zeros + 22); // first digit  of ((uint8_t*)(&encodedDifficulty))[1] is 1,
                   // compensate for other 23 digts of the 3 byte mantissa
}
inline Target Target::genesis() { return htonl(GENESISTARGET_HOST); }
