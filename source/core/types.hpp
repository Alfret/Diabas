#pragma once

#include <alflib/file/path.hpp>
#include <alflib/core/string.hpp>
#include <cstdint>
#include <string>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

namespace dib {

using char8 = char;

using s8 = int8_t;
using u8 = uint8_t;
using s16 = int16_t;
using u16 = uint16_t;
using s32 = int32_t;
using u32 = uint32_t;
using s64 = int64_t;
using u64 = uint64_t;

using f32 = float;
using f64 = double;

using Path = alflib::Path;
using String = alflib::String;

using Vector2F = glm::vec2;
using Vector3F = glm::vec3;
using Vector4F = glm::vec4;
using Matrix4F = glm::mat4;

}