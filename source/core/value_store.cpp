#include "value_store.hpp"

namespace dib {

struct TagVisitor
{
  u8 operator()(const u64)
  {
    return static_cast<u8>(ValueStore::VariantTag::kU64);
  }
  u8 operator()(const f64)
  {
    return static_cast<u8>(ValueStore::VariantTag::kF64);
  }
  u8 operator()(const String&)
  {
    return static_cast<u8>(ValueStore::VariantTag::kString);
  }
};

static u8
VariantToTag(const ValueStore::Variant& variant)
{
  return std::visit(TagVisitor(), variant);
}

// ============================================================ //

std::optional<std::tuple<ValueStore::Variant, ValueStore::VariantTag>>
ValueStore::Load(const String& key) const
{
  if (auto it = map_.find(key); it != map_.end()) {
    return std::tuple<Variant, VariantTag>(
      it->second, static_cast<VariantTag>(VariantToTag(it->second)));
  } else {
    return std::nullopt;
  }
}

bool
ValueStore::ToBytes(alflib::RawMemoryWriter& mw) const
{
  const u32 size = map_.size();
  mw.Write(size);

  bool ok = true;
  for (auto item : map_) {
    mw.Write(item.first);
    const u8 tag = VariantToTag(item.second);
    mw.Write(tag);
    switch (static_cast<VariantTag>(tag)) {
      case VariantTag::kU64:
        ok = mw.Write(std::get<static_cast<u8>(VariantTag::kU64)>(item.second));
        break;

      case VariantTag::kF64:
        ok = mw.Write(std::get<static_cast<u8>(VariantTag::kF64)>(item.second));
        break;

      case VariantTag::kString:
        ok =
          mw.Write(std::get<static_cast<u8>(VariantTag::kString)>(item.second));
        break;
    }
  }
  return ok;
}

ValueStore
ValueStore::FromBytes(alflib::RawMemoryReader& mr)
{
  ValueStore vstore{};
  u32 size = mr.Read<u32>();

  bool ok = true;
  while (size-- > 0) {
    auto key = mr.Read<String>();
    const auto tag = static_cast<VariantTag>(mr.Read<u8>());

    switch (tag) {
      case VariantTag::kU64:
        ok = vstore.Store(std::move(key), mr.Read<u64>());
        break;

      case VariantTag::kF64:
        ok = vstore.Store(std::move(key), mr.Read<f64>());
        break;

      case VariantTag::kString:
        ok = vstore.Store(std::move(key), mr.Read<String>());
    }
    if (!ok) {
      break;
    }
  }
  AlfAssert(ok, "failed to load ValueStore from memory reader");
  return vstore;
}
}
