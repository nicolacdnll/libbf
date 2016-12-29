#ifndef BF_MURMUR3_H
#define BF_MURMUR3_H

namespace bf {

/// An implementation of the MURMUR3 hash function.
template <typename T>
class murmur3
{

public:
  murmur3 (T seed = 0)
  {
    seed_ = seed;
  }

  T operator()(void const* key, size_t len) const
  {
    T result = 0;
    auto * data = static_cast<const uint64_t*>(key);
    auto * end  = data + (len/8);

    const uint64_t  m = 0xc6a4a7935bd1e995;
    const int       r = 47;
    uint64_t        h = seed_ ^ (len * m);

    while (data != end) {
        uint64_t k = *data++;

        k *= m;
        k ^= k >> r;
        k *= m;

        h ^= k;
        h *= m;
    }

    const unsigned char * data2 = (const unsigned char*)data;

    switch(len & 7) {
        case 7: h ^= uint64_t(data2[6]) << 48;
        case 6: h ^= uint64_t(data2[5]) << 40;
        case 5: h ^= uint64_t(data2[4]) << 32;
        case 4: h ^= uint64_t(data2[3]) << 24;
        case 3: h ^= uint64_t(data2[2]) << 16;
        case 2: h ^= uint64_t(data2[1]) << 8;
        case 1: h ^= uint64_t(data2[0]);
                h *= m;
    };

    h ^= h >> r;
    h *= m;
    h ^= h >> r;

    result = h;
    return result;
  }

private:
    T seed_;
};

} // namespace bf

#endif
