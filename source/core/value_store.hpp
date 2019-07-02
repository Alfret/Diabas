#ifndef VALUE_STORE_HPP_
#define VALUE_STORE_HPP_

#include <tsl/robin_map.h>
#include <variant>
#include <tuple>
#include <optional>
#include "core/types.hpp"
#include <alflib/memory/raw_memory_writer.hpp>
#include <alflib/memory/raw_memory_reader.hpp>

namespace dib {
class ValueStore
{
public:
  /**
   * How to add a variant type:
   * 1. Add it here and add it to the VariantTag.
   * 2. Make a operator() overload for it in the TagVisitor.
   * 3. Add a case in the ToBytes method.
   * 4. Add a case in the FromBytes method.
   */
  using Variant = std::variant<u64, f64, String>;

  enum class VariantTag : u8
  {
    kU64 = 0,
    kF64,
    kString
  };

  /**
   * Store a value that corresponds to the given key.
   * @pre TValue Must be one of the Variant types.
   * @return If we successfully stored the value.
   */
  template<typename TValue>
  bool Store(String key, TValue value);

  /**
   * Attempt to load the value with the given key.
   *
   * Note that Variant will hold the value, and VariantTag tells us what type
   * it is.
   *
   * @retval std::nullopt On failure to find the key.
   * @retval std::optional<...> On load success.
   */
  std::optional<std::tuple<Variant, VariantTag>> Load(const String& key) const;

  // Serialize //
  bool ToBytes(alflib::RawMemoryWriter& mw) const;
  static ValueStore FromBytes(alflib::RawMemoryReader& mr);

  // Member Variables //
private:
  tsl::robin_map<String, Variant> map_;
};

// ============================================================ //
// title
// ============================================================ //

template<typename TValue>
bool
ValueStore::Store(String key, const TValue value)
{
  const auto [it, ok] = map_.insert({ std::move(key), value });
  return ok;
}
}

#endif // VALUE_STORE_HPP_
