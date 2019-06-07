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

  String(std::string string);

  String();

  const char* CString() const { return mString.c_str(); }

  const std::string& StdString() const { return mString; }

  std::string& StdString() { return mString; }
};
}
