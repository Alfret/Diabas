#ifndef SIDE_HPP_
#define SIDE_HPP_

#include <string>

namespace dib {

enum class Side : bool
{
  kServer = false,
  kClient = true
};

#ifdef DIB_IS_SERVER
constexpr Side kSide = Side::kServer;
#else
constexpr Side kSide = Side::kClient;
#endif

std::string
SideToString(const Side side);
}

#endif // SIDE_HPP_
