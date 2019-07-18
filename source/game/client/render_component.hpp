#pragma once

// ========================================================================== //
// Headers
// ========================================================================== //

#include <alflib/memory/raw_memory_reader.hpp>
#include <alflib/memory/raw_memory_writer.hpp>

#include "graphics/texture.hpp"

// ========================================================================== //
// RenderComponent Declaration
// ========================================================================== //

namespace dib::game {

/** Component to be attached to entities that can be rendered **/
struct RenderComponent
{
  std::shared_ptr<graphics::Texture> texture;

  /** Construct render component from a texture **/
  explicit RenderComponent(std::shared_ptr<graphics::Texture> texture)
    : texture(std::move(texture))
  {}

  /** Write the component to a memory writer **/
  bool ToBytes(alflib::RawMemoryWriter& writer) const
  {
    // TODO(Filip Björklund): Find name from registry
    String textureName = "";
    return writer.Write(textureName);
  }

  /** Read component from memory reader **/
  static RenderComponent FromBytes(alflib::RawMemoryReader& reader)
  {
    // TODO(Filip Björklund): Read texture by looking up name
    String textureName = reader.Read<String>();
    std::shared_ptr<graphics::Texture> texture = nullptr;
    RenderComponent component{ texture };
    return component;
  }
};

}
