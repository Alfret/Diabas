#ifndef SIDE_HPP_
#define SIDE_HPP_

#include <string>

namespace dib {

enum class Side : bool
{
  kServer = false,
  kClient = true
};

std::string
SideToString(const Side side);
}

#endif // SIDE_HPP_
