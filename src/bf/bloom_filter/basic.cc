#include <bf/bloom_filter/basic.h>

#include <cassert>
#include <cmath>

namespace bf {

size_t basic_bloom_filter::m(double fp, size_t capacity)
{
  auto ln2 = std::log(2);
  return std::ceil(-(capacity * std::log(fp) / ln2 / ln2));
}

size_t basic_bloom_filter::k(size_t cells, size_t capacity)
{
  auto frac = static_cast<double>(cells) / static_cast<double>(capacity);
  return std::ceil(frac * std::log(2));
}

basic_bloom_filter::basic_bloom_filter(hasher h, size_t cells, bool partition)
  : hasher_(std::move(h)),
    bits_(cells),
    partition_(partition)
{
}

basic_bloom_filter::basic_bloom_filter(double fp, size_t capacity, size_t seed,
                                       bool double_hashing, bool partition,
                                       bf_hash_kind hash_kind)
  : partition_(partition)
{
  auto required_cells = m(fp, capacity);
  auto optimal_k = k(required_cells, capacity);
  bits_.resize(required_cells);
  hasher_ = make_hasher(optimal_k, seed, double_hashing, hash_kind);
  k_ = optimal_k;
}

basic_bloom_filter::basic_bloom_filter(basic_bloom_filter&& other)
  : hasher_(std::move(other.hasher_)),
    bits_(std::move(other.bits_))
{
}

void basic_bloom_filter::add(object const& o)
{
  auto digests = hasher_(o);
  assert(bits_.size() % digests.size() == 0);
  if (partition_)
  {
    auto parts = bits_.size() / digests.size();
    for (size_t i = 0; i < digests.size(); ++i)
      bits_.set(i * parts + (digests[i] % parts));
  }
  else
  {
    for (auto d : digests)
      bits_.set(d % bits_.size());
  }
}

void basic_bloom_filter::prefetch(object const& o)
{
  auto digests = hasher_(o);
  assert(bits_.size() % digests.size() == 0);
  if (partition_)
  {
    auto parts = bits_.size() / digests.size();
    for (size_t i = 0; i < digests.size(); ++i)
      bits_.prefetch(i * parts + (digests[i] % parts));

  }
  else
  {
    for (auto d : digests)
      bits_.prefetch(d % bits_.size());
  }
}

size_t basic_bloom_filter::lookup(object const& o) const
{
  auto digests = hasher_(o);
  assert(bits_.size() % digests.size() == 0);
  if (partition_)
  {
    auto parts = bits_.size() / digests.size();
    for (size_t i = 0; i < digests.size(); ++i)
      if (! bits_[i * parts + (digests[i] % parts)])
        return 0;
  }
  else
  {
    for (auto d : digests)
      if (! bits_[d % bits_.size()])
        return 0;
  }

  return 1;
}

size_t basic_bloom_filter::lookup_and_add (object const& o)
{
    bool return_value = 1;
    auto digests = hasher_(o);
    assert(bits_.size() % digests.size() == 0);
    if (partition_)
    {
      auto parts = bits_.size() / digests.size();
      for (size_t i = 0; i < digests.size(); ++i)
        return_value &= bits_.set_getold(i * parts + (digests[i] % parts));
    }
    else
    {
      for (auto d : digests)
        return_value &= bits_.set_getold(d % bits_.size());
    }

    return return_value;
}

void basic_bloom_filter::clear()
{
  bits_.reset();
}

void basic_bloom_filter::remove(object const& o)
{
  for (auto d : hasher_(o))
    bits_.reset(d % bits_.size());
}

void basic_bloom_filter::swap(basic_bloom_filter& other)
{
  using std::swap;
  swap(hasher_, other.hasher_);
  swap(bits_, other.bits_);
}

bitvector const& basic_bloom_filter::storage() const
{
  return bits_;
}

/// Returns the number of functions for this Bloom filter.
/// @return The number of functions for this Bloom filter.
unsigned int basic_bloom_filter::k() const{
  return k_;
}

// / Returns the Returns the number of cells of this Bloom filter.
// / @return The number of cells of this Bloom filter.
size_t basic_bloom_filter::m() const{
  return bits_.size();
}

} // namespace bf
