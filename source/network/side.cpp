#include "side.hpp"

namespace dib
{
std::string
SideToString(const Side side)
{
  switch (side) {
    case Side::kServer:
      return "server";
    case Side::kClient:
    default:
      return "client";
  }
}
}
