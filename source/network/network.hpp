#ifndef NETWORK_HPP_
#define NETWORK_HPP_
#include "core/types.hpp"
#include <steam/isteamnetworkingutils.h>
#include <steam/steamnetworkingsockets.h>
#include <variant>

namespace dib {

enum class NetworkMode : bool
{
  kClient,
  kServer
};

class Network
{
public:
  /**
   * Call once at startup.
   */
  static bool InitNetwork();

  /**
   * Call once before closing program.
   */
  static void ShutdownNetwork();

private:
};

}

#endif // NETWORK_HPP_
