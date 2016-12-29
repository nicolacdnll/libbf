#ifndef BF_MURMUR3_64BIT_H
#define BF_MURMUR3_64BIT_H

namespace bf {

/// An implementation of the MURMUR3 hash function tuned for 64-bit keys.
template <typename T>
class murmur3_64bit
{

public:
  murmur3_64bit (T seed = 0)
  {
    h_ = seed ^ (len * m);
  }

  T operator()(void const* key) const
  {
    T result = 0;
    auto * data = static_cast<const uint64_t*>(key);
    // auto * end  = data + (len/8);

    uint64_t h = h_;

    // With 64-bit keys only one loop is needed
    uint64_t k = *data++;

    k *= m;
    k ^= k >> r;
    k *= m;

    h ^= k;
    h *= m;

    // The switch is removed because 8&7 makes zero and nothing is made here

    h ^= h >> r;
    h *= m;
    h ^= h >> r;

    result = h;
    return result;
  }

private:
    const uint64_t  m = 0xc6a4a7935bd1e995;
    const int       r = 47;
    const size_t    len = 8;
    uint64_t  h_;
};

} // namespace bf

#endif
