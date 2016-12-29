#include <bf/hash.h>

#include <cassert>
#include<iostream>
using namespace std;
namespace bf {

default_hash_function::default_hash_function(size_t seed)
  : h3_(seed)
{
}

size_t default_hash_function::operator()(object const& o) const
{
  // FIXME: fall back to a generic universal hash function (e.g., HMAC/MD5) for
  // too large objects.
  if (o.size() > max_obj_size)
    throw std::runtime_error("object too large");
  return o.size() == 0 ? 0 : h3_(o.data(), o.size());
}

murmur3_hash_function::murmur3_hash_function(size_t seed)
  : murmur3_(seed)
{
}

size_t murmur3_hash_function::operator()(object const& o) const
{
  return murmur3_ (o.data(), o.size());
}

murmur3_64bit_hash_function::murmur3_64bit_hash_function(size_t seed)
  : murmur3_64bit_(seed)
{
}

size_t murmur3_64bit_hash_function::operator()(object const& o) const
{
  return murmur3_64bit_ (o.data());
}

default_hasher::default_hasher(std::vector<hash_function> fns)
  : fns_(std::move(fns))
{
}

std::vector<digest> default_hasher::operator()(object const& o) const
{
  std::vector<digest> d(fns_.size());
  for (size_t i = 0; i < fns_.size(); ++i)
    d[i] = fns_[i](o);
  return d;
}


double_hasher::double_hasher(size_t k, hash_function h1, hash_function h2)
  : k_(k),
    h1_(std::move(h1)),
    h2_(std::move(h2))
{
}

std::vector<digest> double_hasher::operator()(object const& o) const
{
  auto d1 = h1_(o);
  auto d2 = h2_(o);
  std::vector<digest> d(k_);
  for (size_t i = 0; i < d.size(); ++i)
    d[i] = d1 + i * d2;
  return d;
}

hasher make_hasher(size_t k, size_t seed, bool double_hashing, bf_hash_kind hash_kind)
{
  assert(k > 0);
  std::minstd_rand0 prng(seed);
  if (double_hashing)
  {
    // Use H3 hash function with random seeds
    // auto h1 = default_hash_function(prng());
    // auto h2 = default_hash_function(prng());
    hash_function h1, h2;
    switch (hash_kind) {
        case HASH_KIND_MURMUR3_64BIT:
            h1 = murmur3_64bit_hash_function(0);
            h2 = murmur3_64bit_hash_function(1);
            break;
        case HASH_KIND_MURMUR3:
            h1 = murmur3_hash_function(0);
            h2 = murmur3_hash_function(1);
            break;
        case HASH_KIND_H3:
        default:
            h1 = default_hash_function(prng());
            h2 = default_hash_function(prng());

    }
    return double_hasher(k, std::move(h1), std::move(h2));
  }
  else
  {
    std::vector<hash_function> fns(k);
    for (size_t i = 0; i < k; ++i) {
        // fns[i] = default_hash_function(prng());
        switch (hash_kind) {
            case HASH_KIND_MURMUR3_64BIT:
                fns[i] = murmur3_64bit_hash_function(i);
                break;
            case HASH_KIND_MURMUR3:
                fns[i] = murmur3_hash_function(i);
                break;
            case HASH_KIND_H3:
            default:
                fns[i] = default_hash_function(prng());
        }
    }
    return default_hasher(std::move(fns));
  }
}

} // namespace bf
