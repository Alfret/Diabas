#ifndef WORLD_HPP_
#define WORLD_HPP_

#include "core/assert.hpp"
#include "network/network.hpp"
#include "network/side.hpp"
#include <string_view>

namespace dib {

class World
{
public:
public:
  void Update();

  void OnCommandNetwork(const std::string_view input);

  void OnCommandBroadcast(const std::string_view input);
  
  Network<kSide>& GetNetwork() { return network_; }

private:
  Network<kSide> network_{};
};
}

#endif // WORLD_HPP_
