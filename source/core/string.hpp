#pragma once

#include <string>
#include "core/types.hpp"

// ========================================================================== //
// String Declaration
// ========================================================================== //

namespace dib {
class String
{
private:
  std::string mString;

public:
  String(const char* string);

  String(const u16* wstring);

  String();

  const char* CString() const { return mString.c_str(); }


};
}