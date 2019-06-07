/**
 * MIT License
 *
 * Copyright (c) 2019 Christoffer Gustafsson
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef MOD_INFO_HPP_
#define MOD_INFO_HPP_

#include <core/string.hpp>
#include <core/types.hpp>
#include <vector>

namespace dib
{
struct ModVersion
{
  s32 version_major;
  s32 version_minor;
  s32 version_patch;
};

struct ModDependency
{
  String name;
  ModVersion version;
};

struct ModInfo
{
  String name;
  std::vector<String> authors;
  ModVersion version;
  std::vector<ModDependency> dependencies;
};
}

#endif//MOD_INFO_HPP_
