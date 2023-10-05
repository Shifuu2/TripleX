#pragma once
#include "crypto/hash.hpp"
#include<span>

[[nodiscard]] std::array<uint8_t, 32> sha3(std::span<const uint8_t> s);
[[nodiscard]] std::array<uint8_t, 32> blake2b_32(std::span<const uint8_t> s);
[[nodiscard]] std::array<uint8_t, 32> blake3(std::span<const uint8_t> s);
