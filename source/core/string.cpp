#include "core/string.hpp"
#include "core/assert.hpp"
#include "string.hpp"
#include "alflib/alf_unicode.h"

// ========================================================================== //
// String Implementation
// ========================================================================== //

namespace dib
{



String::String(const char* string)
  : mString(string) {}

String::String(const u16* wstring)
{ 
  u32 numBytes;
  AlfBool success = alfUTF16ToUTF8(wstring, &numBytes, nullptr);
  DIB_ASSERT(success, "Failed to convert UTF-16 to UTF-8");
  char* buffer = new char[numBytes + 1ull];
  buffer[numBytes] = 0;
  success = alfUTF16ToUTF8(wstring, &numBytes, buffer);
  DIB_ASSERT(success, "Failed to convert UTF-16 to UTF-8");
  mString = buffer;
  delete[] buffer;
}

String::String()
  : mString()
{}


}