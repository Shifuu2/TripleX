#include "block/header/difficulty.hpp"
#include "hash_algos.hpp"
#include <iostream>
#include <random>

class DoubleSampler
{
public:
    DoubleSampler (double lower, double upper):unif(lower,upper){};
    double sample(){
        return unif(re);
    }
private:
   std::uniform_real_distribution<double> unif;
   std::default_random_engine re;
};


#define PROJECT_NAME "TripleX"

class HashExponentialDigest {
  friend struct Target;

public:
  uint32_t negExp{0}; // negative exponent of 2
  uint32_t data{0x80000000};

  HashExponentialDigest(){};
  void digest(const Hash &);
};

void HashExponentialDigest::digest(const Hash &h) {
  negExp += 1; // we are considering hashes as number in (0,1), padded with
               // infinite amount of trailing 1's
  size_t i = 0;
  for (; i < h.size(); ++i) {
    if (h[i] != 0)
      break;
    negExp += 8;
  }
  uint64_t tmpData{0};
  for (size_t j = 0;; ++j) {
    if (i < h.size())
      tmpData |= h[i++];
    else
      tmpData |= 0xFFu; // "infinite amount of trailing 1's"

    if (j >= 3)
      break;
    tmpData <<= 8;
  }
  size_t shifts = 0;
  while ((tmpData & 0x80000000ul) == 0) {
    shifts += 1;
    negExp += 1;
    tmpData <<= 1;
  }
  assert(shifts < 8);
  assert((tmpData >> 32) == 0);
  tmpData *= uint64_t(data);
  if (tmpData >= uint64_t(1) << 63) {
    tmpData >>= 1;
    negExp -= 1;
  }
  tmpData >>= 31;
  assert(tmpData < uint64_t(1) << 32);
  assert(tmpData >= uint64_t(1) << 31);
  data = tmpData;
};

inline bool Target::compatible(const HashExponentialDigest &digest) const {
  auto zerosTarget{zeros10()};
  assert(digest.negExp > 0);
  auto zerosDigest{digest.negExp - 1};
  if (zerosTarget < zerosDigest)
    return true;
  if (zerosTarget > zerosDigest)
    return false;
  auto bits32{bits22() << 10};
  return digest.data < bits32;
}

using namespace std;

int main(int argc, char **argv) {
  std::string s{"Hello"};
  std::span<const uint8_t> sp{reinterpret_cast<const uint8_t *>(s.data()),
                              s.length()};
  HashExponentialDigest hed;
  hed.digest(sha3(sp));
  cout << "exp: " << hed.negExp << " data: " << hed.data << endl;
  hed.digest(blake3(sp));
  cout << "exp: " << hed.negExp << " data: " << hed.data << endl;
  hed.digest(blake2b_32(sp));
  cout << "exp: " << hed.negExp << " data: " << hed.data << endl;
  cout<<"Testing target...\n";

  DoubleSampler ds(1e200,2e200);
  DoubleSampler scaler(0.5,2.0);
  double maxdiff{0};
  double argmax;
  for (size_t i=0; i < 100; ++i){
      auto s{scaler.sample()};
      auto d{ds.sample()};
      Target t(d);
      uint32_t f1(10000);
      uint32_t f2(s*10000);
      t.scale(f1,f2);
      double f{double(f2)/double(f1)};
      auto out{t.difficulty()};
      auto compare{d*f};
      // auto compare{d};
      auto diff{1.0-compare/out};
      if (diff>maxdiff) {
          maxdiff= diff;
          argmax=d;
      }
      cout<<d<< "* "<<f<<"="<<compare<<" | "<< out<<" ("<<diff<<")\n";
  }
  cout<<"Max diff: "<<maxdiff<<" at input "<<argmax<<endl;

  if (argc != 1) {
    std::cout << argv[0] << "takes no arguments.\n";
    return 1;
  }
  std::cout << "This is project " << PROJECT_NAME << ".\n";
  return 0;
}
