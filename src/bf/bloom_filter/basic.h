#ifndef BF_BLOOM_FILTER_BASIC_H
#define BF_BLOOM_FILTER_BASIC_H

#include <random>
#include <bf/bitvector.h>
#include <bf/bloom_filter.h>
#include <bf/hash.h>

namespace bf {

/// The basic Bloom filter.
///
/// @note This Bloom filter does not use partitioning because it results in
/// slightly worse performance because partitioned Bloom filters tend to have
/// more 1s than non-partitioned filters.
class basic_bloom_filter : public bloom_filter
{
public:
  /// Computes the number of cells based on a false-positive rate and capacity.
  ///
  /// @param fp The desired false-positive rate
  ///
  /// @param capacity The maximum number of items.
  ///
  /// @return The number of cells to use that guarantee *fp* for *capacity*
  /// elements.
  static size_t m(double fp, size_t capacity);

  /// Computes @f$k^*@f$, the optimal number of hash functions for a given
  /// Bloom filter size (in terms of cells) and capacity.
  ///
  /// @param cells The number of cells in the Bloom filter (aka. *m*)
  ///
  /// @param capacity The maximum number of elements that can guarantee *fp*.
  ///
  /// @return The optimal number of hash functions for *cells* and *capacity*.
  static size_t k(size_t cells, size_t capacity);

  /// Constructs a basic Bloom filter.
  /// @param hasher The hasher to use.
  /// @param cells The number of cells in the bit vector.
  /// @param partition Whether to partition the bit vector per hash function.
  basic_bloom_filter(hasher h, size_t cells, bool partition = false);

  /// Constructs a basic Bloom filter by given a desired false-positive
  /// probability and an expected number of elements. The implementation
  /// computes the optimal number of hash function and required space.
  ///
  /// @param fp The desired false-positive probability.
  ///
  /// @param capacity The desired false-positive probability.
  ///
  /// @param seed The initial seed used to construct the hash functions.
  ///
  /// @param double_hashing Flag indicating whether to use default or double
  /// hashing.
  ///
  /// @param partition Whether to partition the bit vector per hash function.
  basic_bloom_filter(double fp, size_t capacity, size_t seed = 0,
                     bool double_hashing = true, bool partition = true,
                     bf_hash_kind hash_kind = HASH_KIND_H3);

  basic_bloom_filter(basic_bloom_filter&&);

  using bloom_filter::add;
  using bloom_filter::prefetch;
  using bloom_filter::lookup;
  using bloom_filter::lookup_and_add;

  virtual void add(object const& o) override;
  virtual void prefetch(object const& o) override;
  virtual size_t lookup(object const& o) const override;
  virtual size_t lookup_and_add(object const& o) override;
  virtual void clear() override;

  /// Removes an object from the Bloom filter.
  /// May introduce false negatives because the bitvector indices of the object
  /// to remove may be shared with other objects.
  ///
  /// @param o The object to remove.
  void remove(object const& o);

  /// Swaps two basic Bloom filters.
  /// @param other The other basic Bloom filter.
  void swap(basic_bloom_filter& other);

  /// Returns the underlying storage of the Bloom filter.
  bitvector const& storage() const;

  /// Returns the number of functions for this Bloom filter.
  /// @return The number of functions for this Bloom filter.
  unsigned int k() const;

  /// Returns the Returns the number of cells of this Bloom filter.
  /// @return The number of cells of this Bloom filter.
  size_t m() const;

private:
  unsigned int k_;
  hasher hasher_;
  bitvector bits_;
  bool partition_;
};

} // namespace bf

#endif
